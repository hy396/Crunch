// 幻雨喜欢小猫咪


#include "MGameInstance.h"

#include "HttpModule.h"
#include "Network/TNetStatics.h"
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

	if (!Request->ProcessRequest())
	{
		UE_LOG(LogTemp, Warning, TEXT("会话创建请求失败"))
	}
}

void UMGameInstance::CancelSessionCreation()
{
	UE_LOG(LogTemp, Warning, TEXT("取消会话创建"))
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
}

void UMGameInstance::PlayerLeft(const FUniqueNetIdRepl& UniqueId)
{
	// 移除玩家记录
	PlayerRecord.Remove(UniqueId);
	
	// 如果所有玩家都离开，终止会话服务器
	if (PlayerRecord.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("所有玩家都离开了会话, 结束服务器"))
		TerminateSessionServer();
	}
}

void UMGameInstance::CreateSession()
{
	// 获取 Session 接口（这里用全局版，单机/单实例下没问题；多人 PIE 推荐上下文版）
	//IOnlineSessionPtr SessionPtr = UTNetStatics::GetSessionPtr();
	IOnlineSessionPtr SessionPtr = UTNetStatics::GetSessionPtr(this);
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
