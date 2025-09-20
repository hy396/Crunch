// 幻雨喜欢小猫咪


#include "MGameInstance.h"

#include "HttpModule.h"
#include "Network/TNetStatics.h"
#include "OnlineSubsystemTypes.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Interfaces/OnlineIdentityInterface.h"

void UMGameInstance::StartMatch()
{
	// 检查是否为专用服务器或监听服务器, 在服务器中切地图
	if (GetWorld()->GetNetMode() == ENetMode::NM_DedicatedServer || GetWorld()->GetNetMode() == ENetMode::NM_ListenServer)
	{
		// 加载游戏关卡并监听
		LoadLevelAndListen(GameLevel);
	}
}

void UMGameInstance::Init()
{
	Super::Init();
	// 编辑器环境下不执行
	if (GetWorld()->IsEditorWorld()) return;

	// 如果是会话服务器，则创建会话
	if (UTNetStatics::IsSessionServer(this))
	{
		CreateSession();
	}
}

bool UMGameInstance::IsLoggedIn() const
{
	if (IOnlineIdentityPtr IdentityPtr = UTNetStatics::GetIdentityPtr())
	{
		// 查询本地玩家0的登录状态，是否为已登录
		return IdentityPtr->GetLoginStatus(0) == ELoginStatus::LoggedIn;
	}
	// 如果IdentityPtr无效，则默认未登录
	return false;
}

bool UMGameInstance::IsLoggingIn() const
{
	// 如果登录委托句柄有效，说明还在等待登录回调
	return LoggingInDelegateHandle.IsValid();
}

void UMGameInstance::ClientAccountPortalLogin()
{
	// 调用统一的ClientLogin接口，使用AccountPortal方式
	ClientLogin("AccountPortal", "", "");
}

void UMGameInstance::ClientLogin(const FString& Type, const FString& Id, const FString& Token)
{
	if (IOnlineIdentityPtr IdentityPtr = UTNetStatics::GetIdentityPtr())
	{
		// 如果已经有一个登录委托在监听，先移除它，避免重复绑定
		if (LoggingInDelegateHandle.IsValid())
		{
			IdentityPtr->OnLoginCompleteDelegates->Remove(LoggingInDelegateHandle);
			LoggingInDelegateHandle.Reset();
		}
		// 绑定登录完成回调
		LoggingInDelegateHandle = IdentityPtr->OnLoginCompleteDelegates->AddUObject(this, &UMGameInstance::LoginCompleted);
		
		// 调用OnlineSubsystem的登录函数（异步）
		if (!IdentityPtr->Login(0,FOnlineAccountCredentials(Type, Id, Token)))
		{
			UE_LOG(LogTemp, Warning, TEXT("登录失败!"))

			if (LoggingInDelegateHandle.IsValid())
			{
				IdentityPtr->OnLoginCompleteDelegates->Remove(LoggingInDelegateHandle);
				LoggingInDelegateHandle.Reset();
			}
			// 通知外部,登录失败
			OnLoginCompleted.Broadcast(false, "", TEXT("登录失败!"));
		}
	}
}

void UMGameInstance::LoginCompleted(int32 NumOfLocalPlayer, bool bWasSuccessful, const FUniqueNetId& UserId,
	const FString& Error)
{
	if (IOnlineIdentityPtr IdentityPtr = UTNetStatics::GetIdentityPtr())
	{
		// 移除登录完成委托
		if (LoggingInDelegateHandle.IsValid())
		{
			IdentityPtr->OnLoginCompleteDelegates->Remove(LoggingInDelegateHandle);
			LoggingInDelegateHandle.Reset();
		}

		FString PlayerNickname = "";
		if (bWasSuccessful)
		{
			// 获取玩家昵称
			PlayerNickname = IdentityPtr->GetPlayerNickname(UserId);
			UE_LOG(LogTemp, Warning, TEXT("登录成功: %s"), *(PlayerNickname))
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("登录失败: %s"), *(Error))
		}
		
		OnLoginCompleted.Broadcast(bWasSuccessful, PlayerNickname, Error);
	}else
	{
		OnLoginCompleted.Broadcast(false, "", TEXT("无法找到身份指针"));
	}
}

void UMGameInstance::RequestCreateAndJoinSession(const FName& NewSessionName)
{
	UE_LOG(LogTemp, Warning, TEXT("请求创建并加入会话: %s"), *(NewSessionName.ToString()))
	
	// 创建HTTP请求对象，准备向协调器发起会话创建请求
	FHttpRequestRef Request = FHttpModule::Get().CreateRequest();
	// 生成唯一会话搜索ID用于后续查询
	FGuid SessionSearchId = FGuid::NewGuid();
	// 获取协调器服务的基础URL地址
	FString CoordinatorURL  = UTNetStatics::GetCoordinatorURL();

	// 拼接目标 API 地址：<CoordinatorURL>/Sessions  
	FString URL = FString::Printf(TEXT("%s/Sessions"), *CoordinatorURL);
	UE_LOG(LogTemp, Warning, TEXT("发送会话创建请求到 URL：%s"), *URL)
	
	// 配置 HTTP 请求：目标地址 + 请求方式 POST
	Request->SetURL(URL);
	Request->SetVerb("POST");

	// 设置 HTTP 请求头，指定请求体为 JSON 格式
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	// 构造 JSON 请求体，包含 SessionName 和 SessionSearchId
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetStringField(UTNetStatics::GetSessionNameKey().ToString(), NewSessionName.ToString());
	JsonObject->SetStringField(UTNetStatics::GetSessionSearchIdKey().ToString(), SessionSearchId.ToString());

	// 将 JSON 对象转换为字符串
	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	// 设置 HTTP 请求体
	Request->SetContentAsString(RequestBody);
	// 绑定会话创建完成回调
	Request->OnProcessRequestComplete().BindUObject(this, &UMGameInstance::SessionCreationRequestCompleted, SessionSearchId);

	// 发送 HTTP 请求,测试中在python中编写代码接收请求信息
	if (!Request->ProcessRequest())
	{
		UE_LOG(LogTemp, Warning, TEXT("会话创建请求失败"))
	}
}

void UMGameInstance::CancelSessionCreation()
{
	UE_LOG(LogTemp, Warning, TEXT("取消会话创建"))
	// 停止所有会话查找
	StopAllSessionFindings();

	// 清理会话委托
	if (IOnlineSessionPtr SessionPtr = UTNetStatics::GetSessionPtr())
	{
		SessionPtr->OnFindSessionsCompleteDelegates.RemoveAll(this);
		SessionPtr->OnJoinSessionCompleteDelegates.RemoveAll(this);
	}

	// 重新开始全局会话搜索
	StartGlobalSessionSearch();
}

void UMGameInstance::StartGlobalSessionSearch()
{
	UE_LOG(LogTemp, Warning, TEXT("开始全局会话搜索"))
	
	// 设置定时器定期搜索会话
	GetWorld()->GetTimerManager().SetTimer(
		GlobalSessionSearchTimerHandle, 
		this, 
		&UMGameInstance::FindGlobalSessions, 
		GlobalSessionSearchInterval, 
		true, 
		0.f
	);
}

/**
 * 尝试通过 SessionId 加入会话
 * @param SessionIdStr 目标会话的唯一 SessionId 字符串
 * @return 是否成功找到对应的会话并发起加入
 */
bool UMGameInstance::JoinSessionWithId(const FString& SessionIdStr)
{
	// 确认当前是否已经有一次有效会话搜索结果（SessionSearch 保存了上次搜索的数据）
	if (SessionSearch.IsValid())
	{
		// 在搜索结果中查找是否存在与传入的 SessionIdStr 匹配的会话
		const FOnlineSessionSearchResult* SessionSearchResult = SessionSearch->SearchResults.FindByPredicate(
			[=](const FOnlineSessionSearchResult& Result)
			{
				// 比较搜索结果中的 SessionId 与目标 Id 是否相同
				return Result.GetSessionIdStr() == SessionIdStr;
			}
		);

		// 如果找到了匹配的会话
		if (SessionSearchResult)
		{
			// 调用已有的函数，使用搜索结果尝试加入该会话
			JoinSessionWithSearchResult(*SessionSearchResult);
			return true; // 返回 true 表示找到了并已开始加入流程
		}
	}

	// 如果搜索无效，或者没找到对应 SessionId 的会话，则返回 false
	return false;
}

void UMGameInstance::SessionCreationRequestCompleted(FHttpRequestPtr Request, FHttpResponsePtr Response,
	bool bConnectedSuccessfully, FGuid SessionSearchId)
{
	if (!bConnectedSuccessfully)
	{
		UE_LOG(LogTemp, Warning, TEXT("连接协调服务器失败，网络连接未成功!"))
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("连接协调服务器成功!"))

	// 获取 HTTP 响应状态码
	int32 ResponseCode = Response->GetResponseCode();
	if (ResponseCode != 200)
	{
		UE_LOG(LogTemp, Warning, TEXT("会话创建失败，服务器返回错误的状态码: %d"), ResponseCode)
		return;
	}
	// 获取 HTTP 响应内容
	FString ResponseContent = Response->GetContentAsString();

	// 解析响应内容(JSON)
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseContent);
	int32 Port = 0;
	// 如果成功解析，则获取端口号
	if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
	{
		// 获取端口号字段（Key 从 UCNetStatics 获取，确保与服务端一致）
		Port = JsonObject->GetIntegerField(*(UTNetStatics::GetPortKey().ToString()));
		// if (JsonObject->TryGetNumberField(UTNetStatics::GetPortKey().ToString(), Port))
		// {
		// 	UE_LOG(LogTemp, Warning, TEXT("连接协调服务器成功，新创建的会话端口为: %d"), Port)
		// }else
		// {
		// 	UE_LOG(LogTemp, Warning, TEXT("会话创建成功，但未找到端口号字段"))
		// }
	}
	UE_LOG(LogTemp, Warning, TEXT("连接协调服务器成功，新创建的会话端口为: %d"), Port)
	// 开始查找并加入刚刚创建的会话
	StartFindingCreatedSession(SessionSearchId);
}

void UMGameInstance::StartFindingCreatedSession(const FGuid& SessionSearchId)
{
	if (!SessionSearchId.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("会话搜索ID无效，无法开始查找！"))
		return;
	}
	// 停止所有查找
	StopAllSessionFindings();

	UE_LOG(LogTemp, Warning, TEXT("开始查找新创建的会话，ID: %s"), *(SessionSearchId.ToString()))

	// 创建一个定时器，用于定期查找已创建的会话
	GetWorld()->GetTimerManager().SetTimer(
		FindCreatedSessionTimerHandle,
		FTimerDelegate::CreateUObject(this, &UMGameInstance::FindCreatedSession, SessionSearchId),
		FindCreatedSessionSearchInterval,
		true, 0.f
		);

	// 超时定时器
	GetWorld()->GetTimerManager().SetTimer(
		FindCreatedSessionTimeoutTimerHandle,
		this,
		&UMGameInstance::FindCreatedSessionTimeout,
		FindCreatedSessionTimeoutDuration
		);
}

void UMGameInstance::StopAllSessionFindings()
{
	UE_LOG(LogTemp, Warning, TEXT("停止所有会话查找"))
	StopFindingCreatedSession();
	StopGlobalSessionSearch();
}

void UMGameInstance::StopFindingCreatedSession()
{
	UE_LOG(LogTemp, Warning, TEXT("停止查找已创建的会话"))

	GetWorld()->GetTimerManager().ClearTimer(FindCreatedSessionTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(FindCreatedSessionTimeoutTimerHandle);

	// 清理会话委托
	if (IOnlineSessionPtr SessionPtr = UTNetStatics::GetSessionPtr())
	{
		// 移除查找会话完成委托
		SessionPtr->OnFindSessionsCompleteDelegates.RemoveAll(this);
		// 移除加入会话完成委托
		SessionPtr->OnJoinSessionCompleteDelegates.RemoveAll(this);
	}
}

void UMGameInstance::StopGlobalSessionSearch()
{
	UE_LOG(LogTemp, Warning, TEXT("停止全局会话查找"))
	// 停止全局会话查找定时器
	if (GlobalSessionSearchTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(GlobalSessionSearchTimerHandle);
	}

	// 清理会话搜索完成的回调委托
	if (IOnlineSessionPtr SessionPtr = UTNetStatics::GetSessionPtr())
	{
		SessionPtr->OnFindSessionsCompleteDelegates.RemoveAll(this);
	}
}

void UMGameInstance::FindGlobalSessions()
{
	UE_LOG(LogTemp, Warning, TEXT("----- 重试全局会话查找 -----"))
	IOnlineSessionPtr SessionPtr = UTNetStatics::GetSessionPtr();
	if (!SessionPtr)
	{
		UE_LOG(LogTemp, Warning, TEXT("无法找到Session接口，等待下一次全局会话查找"))
		return;
	}

	// 创建会话搜索对象
	SessionSearch = MakeShareable(new FOnlineSessionSearch);
	SessionSearch->bIsLanQuery = false;		// 设置为非局域网查询
	SessionSearch->MaxSearchResults = 20;	// 最多搜索20个结果

	// 重新添加会话搜索完成委托
	SessionPtr->OnFindSessionsCompleteDelegates.RemoveAll(this);
	SessionPtr->OnFindSessionsCompleteDelegates.AddUObject(
		this, 
		&UMGameInstance::GlobalSessionSearchCompleted
	);
	// 搜索会话
	if (!SessionPtr->FindSessions(0, SessionSearch.ToSharedRef()))
	{
		UE_LOG(LogTemp, Warning, TEXT("全局会话搜索失败！"))
		SessionPtr->OnFindSessionsCompleteDelegates.RemoveAll(this);
	}
}

void UMGameInstance::GlobalSessionSearchCompleted(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		// 广播会话搜索结果
		OnGlobalSessionSearchCompleted.Broadcast(SessionSearch->SearchResults);
		// 遍历搜索会话名称
		for (const FOnlineSessionSearchResult& OnlineSessionSearchResult : SessionSearch->SearchResults)
		{
			FString SessionName = "Name_None";
			OnlineSessionSearchResult.Session.SessionSettings.Get<FString>(UTNetStatics::GetSessionNameKey(), SessionName);
        	// 总人数（可公开连接数）
        	int32 MaxPlayers = OnlineSessionSearchResult.Session.SessionSettings.NumPublicConnections;

        	// 当前已加入人数
        	int32 CurrentPlayers = MaxPlayers - OnlineSessionSearchResult.Session.NumOpenPublicConnections;
			// 获取玩家数量
			OnlineSessionSearchResult.Session.SessionSettings.Get<int>(UTNetStatics::GetPlayerCountKey(), CurrentPlayers);
        	// 输出日志
        	UE_LOG(LogTemp, Warning, TEXT("发现会话[%s] 人数: %d / %d(全局搜索结果)"),
            *SessionName, CurrentPlayers, MaxPlayers);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("全局会话搜索失败！"))
	}
	
	// 搜索完成移除会话搜索完成委托
	if (IOnlineSessionPtr SessionPtr = UTNetStatics::GetSessionPtr())
	{
		SessionPtr->OnFindSessionsCompleteDelegates.RemoveAll(this);
	}
}

void UMGameInstance::FindCreatedSession(FGuid SessionSearchId)
{
	UE_LOG(LogTemp, Warning, TEXT("尝试查找已创建的会话"))
	
	// 获取在线子系统的 Session 接口指针
	IOnlineSessionPtr SessionPtr = UTNetStatics::GetSessionPtr();
	if (!SessionPtr.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("会话接口无效，无法查找已创建的会话"))
		return;
	}
	// 创建会话搜索对象
	SessionSearch = MakeShareable(new FOnlineSessionSearch);
	if (!SessionSearch.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("无法创建会话搜索对象，取消查找"))
		return;
	}
	// 配置搜索参数
	SessionSearch->bIsLanQuery = false;		// 设置为非局域网查询(搜索在线会话)
	SessionSearch->MaxSearchResults = 1;	// 只搜索一个结果
	// 设置搜索条件: 匹配 SessionSearchId
	SessionSearch->QuerySettings.Set(
		UTNetStatics::GetSessionSearchIdKey(), // 键名（和创建时保持一致）
		SessionSearchId.ToString(),            // 转换成字符串存储
		EOnlineComparisonOp::Equals            // 等于匹配
	);

	// 清理并重新绑定会话搜索结果回调
	SessionPtr->OnFindSessionsCompleteDelegates.RemoveAll(this);
	SessionPtr->OnFindSessionsCompleteDelegates.AddUObject(this,
		&UMGameInstance::FindCreateSessionCompleted);

	// 开始查找会话
	if (!SessionPtr->FindSessions(0, SessionSearch.ToSharedRef()))
	{
		UE_LOG(LogTemp, Warning, TEXT("查找已创建的会话失败"))
		// 移除回调
		SessionPtr->OnFindSessionsCompleteDelegates.RemoveAll(this);
	}
}

void UMGameInstance::FindCreatedSessionTimeout()
{
	UE_LOG(LogTemp, Warning, TEXT("查找已创建会话超时"))
	StopFindingCreatedSession();
}

void UMGameInstance::FindCreateSessionCompleted(bool bWasSuccessful)
{
	if (!bWasSuccessful || SessionSearch->SearchResults.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("未找到已创建的会话"))
		return;
	}
	// 停止查找
	StopFindingCreatedSession();
	// 加入已创建的会话
	JoinSessionWithSearchResult(SessionSearch->SearchResults[0]);
}

void UMGameInstance::JoinSessionWithSearchResult(const class FOnlineSessionSearchResult& SearchResult)
{
	UE_LOG(LogTemp, Warning, TEXT("尝试加入会话..."))

	// 获取会话接口
	IOnlineSessionPtr SessionPtr = UTNetStatics::GetSessionPtr();
	if (!SessionPtr.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("会话接口无效，无法加入会话"))
		return;
	}

	// 从搜索结果中获取会话名称
	FString SessionName = "";
	SearchResult.Session.SessionSettings.Get<FString>(UTNetStatics::GetSessionNameKey(), SessionName);

	// 从搜索结果中提取端口号(端口号默认为:7777)
	const FOnlineSessionSetting* PortSetting = SearchResult.Session.SessionSettings.Settings.Find(UTNetStatics::GetPortKey());
	int64 Port = 7777;
	if (PortSetting)
	{
		PortSetting->Data.GetValue(Port);
	}
	UE_LOG(LogTemp, Warning, TEXT("尝试加入会话: %s，端口: %lld"), *(SessionName), Port)

	// 清理旧加入会话委托
	SessionPtr->OnJoinSessionCompleteDelegates.RemoveAll(this);
	SessionPtr->OnJoinSessionCompleteDelegates.AddUObject(this, &UMGameInstance::JoinSessionCompleted, Port);

	// 尝试正式加入会话
	if (!SessionPtr->JoinSession(0, FName(SessionName), SearchResult))
	{
		// 如果加入会话失败，打印错误并广播失败事件
		UE_LOG(LogTemp, Warning, TEXT("加入会话失败！"))
		SessionPtr->OnJoinSessionCompleteDelegates.RemoveAll(this);
		OnJoinSessionFailed.Broadcast();
	}
}

void UMGameInstance::JoinSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type JoinResult, int64 Port)
{
	IOnlineSessionPtr SessionPtr = UTNetStatics::GetSessionPtr();
	if (!SessionPtr)
	{
		UE_LOG(LogTemp, Warning, TEXT("加入会话完成,但未找到会话接口"))
		OnJoinSessionFailed.Broadcast();
		return;
	}
	// 加入会话的结果类型是否为加入成功的类型
	if (JoinResult == EOnJoinSessionCompleteResult::Success)
	{
		// 停止所有查找
		StopAllSessionFindings();
		UE_LOG(LogTemp, Warning, TEXT("成功加入会话: %s,端口: %lld"), *(SessionName.ToString()), Port)

		// 获取服务器连接字符串
		FString TravelURL = "";
		SessionPtr->GetResolvedConnectString(SessionName, TravelURL);

// #if WITH_EDITOR
		// 在编辑器模式下，允许测试用 URL 覆盖
		FString TestingURL = UTNetStatics::GetTestingURL();
		if (!TestingURL.IsEmpty())
		{
			TravelURL = TestingURL;
			UE_LOG(LogTemp, Warning, TEXT("使用测试URL覆盖: %s | Using testing URL override: %s"), *TravelURL, *TravelURL);
		}
// #endif

		// 实际的 URL
		UTNetStatics::ReplacePort(TravelURL, Port);

		UE_LOG(LogTemp, Warning, TEXT("跳转到会话地址: %s"), *TravelURL)

		// 客户端执行跳转，进入目标会话地图
		GetFirstLocalPlayerController(GetWorld())->ClientTravel(TravelURL, ETravelType::TRAVEL_Absolute);

	}else
	{
		// 广播失败事件
		OnJoinSessionFailed.Broadcast();
	}

	SessionPtr->OnJoinSessionCompleteDelegates.RemoveAll(this);
}

void UMGameInstance::PlayerJoined(const FUniqueNetIdRepl& UniqueId)
{
	// 清除等待超时定时器
	if (WaitPlayerJoinTimeoutHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(WaitPlayerJoinTimeoutHandle);
	}
	// 记录玩家
	PlayerRecord.Add(UniqueId);
	
	UE_LOG(LogTemp, Warning, TEXT("[服务器] 玩家加入，当前玩家数：%d"), PlayerRecord.Num());
	
	// 更新会话设置以同步当前玩家数量到在线子系统
	UpdateSessionSettings();
	
	// 注意：不再直接调用Server_PushSessionUpdate()，因为UpdateSessionSettings()会在更新成功后自动调用
}

void UMGameInstance::PlayerLeft(const FUniqueNetIdRepl& UniqueId)
{
	// 移除玩家记录
	PlayerRecord.Remove(UniqueId);
	
	UE_LOG(LogTemp, Warning, TEXT("[服务器] 玩家离开，当前玩家数：%d"), PlayerRecord.Num());
	
	// 更新会话设置以同步当前玩家数量到在线子系统
	UpdateSessionSettings();
	
	// 注意：不再直接调用Server_PushSessionUpdate()，因为UpdateSessionSettings()会在更新成功后自动调用
	
	// 如果所有玩家都离开，终止会话服务器
	if (PlayerRecord.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("所有玩家都离开了会话, 结束服务器"))
		TerminateSessionServer();
	}
}

void UMGameInstance::UpdateSessionSettings()
{
	// 获取会话接口和当前会话
	IOnlineSessionPtr SessionPtr = UTNetStatics::GetSessionPtr(this);
	if (!SessionPtr.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("无法获取会话接口"));
		return;
	}

	FNamedOnlineSession* CurrentSession = SessionPtr->GetNamedSession(FName(ServerSessionName));
	if (!CurrentSession)
	{
		UE_LOG(LogTemp, Error, TEXT("无法获取当前会话"));
		return;
	}

	// 记录更新前的状态
	UE_LOG(LogTemp, Warning, TEXT("[服务器] 更新前状态 - 公共位置:%d, 私有位置:%d"), 
		CurrentSession->NumOpenPublicConnections, CurrentSession->NumOpenPrivateConnections);

	// 计算当前玩家数和可用连接数
	int32 CurrentPlayers = PlayerRecord.Num();
	int32 MaxPublicConnections = CurrentSession->SessionSettings.NumPublicConnections;
	int32 MaxPrivateConnections = CurrentSession->SessionSettings.NumPrivateConnections;
	int32 MaxPlayers = MaxPublicConnections + MaxPrivateConnections;
	int32 AvailableConnections = FMath::Max(0, MaxPlayers - CurrentPlayers);
	// 设置会话玩家数量
	CurrentSession->SessionSettings.Set(UTNetStatics::GetPlayerCountKey(), 
										CurrentPlayers, 
										EOnlineDataAdvertisementType::ViaOnlineService);
	UE_LOG(LogTemp, Warning, TEXT("[服务器] 计算参数 - 最大玩家数:%d, 当前玩家数:%d, 可用连接数:%d"), 
		MaxPlayers, CurrentPlayers, AvailableConnections);

	// 更新NumOpenPublicConnections和NumOpenPrivateConnections
	if (MaxPublicConnections > 0)
	{
		// 优先填充公共连接
		CurrentSession->NumOpenPublicConnections = FMath::Min(AvailableConnections, MaxPublicConnections);
		CurrentSession->NumOpenPrivateConnections = FMath::Max(0, AvailableConnections - MaxPublicConnections);
		UE_LOG(LogTemp, Warning, TEXT("[服务器] 更新策略 - 公共连接优先, 公共位置:%d, 私有位置:%d"), 
			CurrentSession->NumOpenPublicConnections, CurrentSession->NumOpenPrivateConnections);
	}
	else
	{
		// 如果没有公共连接，则全部为私有连接
		CurrentSession->NumOpenPrivateConnections = AvailableConnections;
		CurrentSession->NumOpenPublicConnections = 0;
		UE_LOG(LogTemp, Warning, TEXT("[服务器] 更新策略 - 私有连接优先, 公共位置:%d, 私有位置:%d"), 
			CurrentSession->NumOpenPublicConnections, CurrentSession->NumOpenPrivateConnections);
	}
	// 调用UpdateSession更新在线子系统
	UE_LOG(LogTemp, Warning, TEXT("[服务器] 调用UpdateSession - 会话名:%s, 强制更新:true"), *ServerSessionName);
	bool bUpdateResult = SessionPtr->UpdateSession(FName(ServerSessionName), CurrentSession->SessionSettings, true);

	UE_LOG(LogTemp, Warning, TEXT("[服务器] 更新会话状态：最大玩家数=%d，当前玩家数=%d，可用连接数=%d (原值=%d)"), 
		MaxPlayers, CurrentPlayers, AvailableConnections, MaxPlayers);
	
	UE_LOG(LogTemp, Warning, TEXT("[服务器] 更新后 - 公共位置:%d, 私有位置:%d, 更新调用结果:%s"), 
		CurrentSession->NumOpenPublicConnections, CurrentSession->NumOpenPrivateConnections, 
		bUpdateResult ? TEXT("成功") : TEXT("失败"));
	
}

void UMGameInstance::CreateSession()
{
	// 获取 Session 接口（这里用全局版，单机/单实例下没问题；多人 PIE 推荐上下文版）
	//IOnlineSessionPtr SessionPtr = UTNetStatics::GetSessionPtr();
	IOnlineSessionPtr SessionPtr = UTNetStatics::GetSessionPtr();
	if (SessionPtr)
	{
		// 获取会话名称、搜索ID、会话端口
		ServerSessionName = UTNetStatics::GetSessionNameStr();
		FString SessionSearchId = UTNetStatics::GetSessionSearchIdStr();
		SessionServerPort = UTNetStatics::GetSessionPort();

		UE_LOG(LogTemp, Warning, TEXT("#### 创建会话 With Name: %s, ID: %s, Port: %d"), *(ServerSessionName), *(SessionSearchId), SessionServerPort)

		// 生成会话设置
		FOnlineSessionSettings OnlineSessionSettings = UTNetStatics::GenerateOnlineSessionSettings(FName(ServerSessionName), SessionSearchId, SessionServerPort);
		// 先移除旧的委托，避免重复绑定
		SessionPtr->OnCreateSessionCompleteDelegates.RemoveAll(this);

		// 绑定本实例的回调函数（异步完成时调用）
		SessionPtr->OnCreateSessionCompleteDelegates.AddUObject(this, &UMGameInstance::OnSessionCreated);
		// 发起会话创建（参数：本地用户索引=0，会话名，会话设置）
		if (!SessionPtr->CreateSession(0, FName(ServerSessionName), OnlineSessionSettings))
		{
			UE_LOG(LogTemp, Warning, TEXT("错误：会话创建立即失败!!!!"))
			SessionPtr->OnCreateSessionCompleteDelegates.RemoveAll(this);
			TerminateSessionServer();
		}
	}
	else
	{
		// 无法获取 Session 接口（通常是 OnlineSubsystem 没初始化）
		UE_LOG(LogTemp, Warning, TEXT("错误：无法获取会话接口，正在终止服务"));
		TerminateSessionServer();
	}
}

void UMGameInstance::OnSessionCreated(FName SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("------------- 会话创建成功！ -------------"));

		// 设置一个定时器：如果在指定时间内没有玩家加入，则自动关闭服务器
		GetWorld()->GetTimerManager().SetTimer(
			WaitPlayerJoinTimeoutHandle, 
			this, 
			&UMGameInstance::WaitPlayerJoinTimeoutReached, 
			WaitPlayerJoinTimeOutDuration
		);

		// 会话创建成功后，加载大厅关卡并开启监听（等待客户端连接）
		LoadLevelAndListen(LobbyLevel);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("------------ 会话创建失败 ------------"));
		// 如果会话创建失败，立刻关闭服务器
		TerminateSessionServer();
	}

	// 无论结果如何，解绑 OnCreateSessionComplete 委托，避免重复绑定
	if (IOnlineSessionPtr SessionPtr = UTNetStatics::GetSessionPtr())
	{
		SessionPtr->OnCreateSessionCompleteDelegates.RemoveAll(this);
	}
}

void UMGameInstance::EndSessionCompleted(FName SessionName, bool bWasSuccessful)
{
	// 直接请求退出游戏进程
	FGenericPlatformMisc::RequestExit(false);
}

void UMGameInstance::TerminateSessionServer()
{
	if (IOnlineSessionPtr SessionPtr = UTNetStatics::GetSessionPtr())
	{
		// 确保委托不重复绑定
		SessionPtr->OnEndSessionCompleteDelegates.RemoveAll(this);
		SessionPtr->OnEndSessionCompleteDelegates.AddUObject(this, &UMGameInstance::EndSessionCompleted);

		// 尝试结束会话，如果失败则直接退出
		if (!SessionPtr->EndSession(FName{ ServerSessionName }))
		{
			FGenericPlatformMisc::RequestExit(false);
		}
	}
	else
	{
		// 如果拿不到 SessionPtr，说明 OnlineSubsystem 不可用，直接退出
		FGenericPlatformMisc::RequestExit(false);
	}
}

void UMGameInstance::WaitPlayerJoinTimeoutReached()
{
	UE_LOG(LogTemp, Warning, TEXT("会话服务器在等待%f秒后后无玩家加入，现已关闭"), WaitPlayerJoinTimeOutDuration);
	// 超时无人加入，关闭服务器
	TerminateSessionServer();
}

void UMGameInstance::LoadLevelAndListen(TSoftObjectPtr<UWorld> Level)
{
	// 从软引用的 UWorld 获取包路径（比如 /Game/Maps/MyMap.MyMap）
	const FName LevelURL = FName(*FPackageName::ObjectPathToPackageName(Level.ToString()));
	if (LevelURL != "")
	{
		// 构造带有监听和端口参数的 URL
		FString TravelStr = FString::Printf(TEXT("%s?listen?port=%d"), *LevelURL.ToString(), SessionServerPort);
		UE_LOG(LogTemp, Warning, TEXT("服务器地图切换至 %s"), *(TravelStr));
		// 开启 ServerTravel（切换关卡并开启监听）
		GetWorld()->ServerTravel(TravelStr);

		// 切换到指定关卡，并加上 "?listen" 参数
		// GetWorld()->ServerTravel(LevelURL.ToString() + "?listen");
	}
}

