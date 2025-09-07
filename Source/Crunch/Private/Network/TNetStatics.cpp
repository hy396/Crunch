// 幻雨喜欢小猫咪


#include "TNetStatics.h"
#include "OnlineSubsystemUtils.h"

FOnlineSessionSettings UTNetStatics::GenerateOnlineSessionSettings(const FName& SessionName,
	const FString& SessionSearchId, int32 Port)
{
	// 创建一个会话设置对象
	FOnlineSessionSettings OnlineSessionSettings{};

	// 是否是局域网对战（false 表示在线匹配）
	OnlineSessionSettings.bIsLANMatch = false;

	// 可加入的玩家数，这里取「每队人数 * 2」
	OnlineSessionSettings.NumPublicConnections = GetPlayerCountPerTeam() * 2;

	// 是否向在线服务公开这个会话（可被搜索到）
	OnlineSessionSettings.bShouldAdvertise = true;

	// 是否使用在线状态（Presence，如好友在线显示）功能,(这是服务器没有登录用户)
	OnlineSessionSettings.bUsesPresence = false;
	
	// 是否允许通过在线状态直接加入
	OnlineSessionSettings.bAllowJoinViaPresence = false;

	// 是否仅允许好友通过在线状态加入
	OnlineSessionSettings.bAllowJoinViaPresenceFriendsOnly = false;

	// 是否允许玩家通过邀请加入
	OnlineSessionSettings.bAllowInvites = true;

	// 是否允许游戏进行中途加入（false = 只能在开始前加入）
	OnlineSessionSettings.bAllowJoinInProgress = false;

	// 是否在可用时使用 Lobbies 功能
	OnlineSessionSettings.bUseLobbiesIfAvailable = false;

	// 是否在 Lobbies 中启用语音聊天
	OnlineSessionSettings.bUseLobbiesVoiceChatIfAvailable = false;

	// 是否启用统计系统（用于战绩、匹配等统计）
	OnlineSessionSettings.bUsesStats = true;

	// 设置自定义键值对（附加会话元数据，用于搜索和识别）
	OnlineSessionSettings.Set(GetSessionNameKey(), SessionName.ToString(), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	OnlineSessionSettings.Set(GetSessionSearchIdKey(), SessionSearchId, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	OnlineSessionSettings.Set(GetPortKey(), Port, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	OnlineSessionSettings.Set(GetPlayerCountKey(), 0, EOnlineDataAdvertisementType::ViaOnlineService);

	return OnlineSessionSettings;
}

IOnlineSessionPtr UTNetStatics::GetSessionPtr()
{
	IOnlineSubsystem* OnlineSubSystem = IOnlineSubsystem::Get();
	if (OnlineSubSystem)
	{
		// 返回会话接口（IOnlineSession）
		return OnlineSubSystem->GetSessionInterface();
	}
	return nullptr;
}

IOnlineIdentityPtr UTNetStatics::GetIdentityPtr()
{
	IOnlineSubsystem* OnlineSubSystem = IOnlineSubsystem::Get();
	if (OnlineSubSystem)
	{
		// 返回身份接口（IOnlineIdentity）
		return OnlineSubSystem->GetIdentityInterface();
	}
	return nullptr;
}

IOnlineSessionPtr UTNetStatics::GetSessionPtr(const UObject* WorldContextObject)
{
	if (!WorldContextObject || !WorldContextObject->GetWorld())
	{
		return nullptr;
	}
	// 返回会话接口（IOnlineSession）
	IOnlineSubsystem* OnlineSubSystem = Online::GetSubsystem(WorldContextObject->GetWorld());
	return OnlineSubSystem ? OnlineSubSystem->GetSessionInterface() : nullptr;
}

IOnlineIdentityPtr UTNetStatics::GetIdentityPtr(const UObject* WorldContextObject)
{
	if (!WorldContextObject || !WorldContextObject->GetWorld())
	{
		return nullptr;
	}
	IOnlineSubsystem* OnlineSubSystem = Online::GetSubsystem(WorldContextObject->GetWorld());
	return OnlineSubSystem ? OnlineSubSystem->GetIdentityInterface() : nullptr;
}

uint8 UTNetStatics::GetPlayerCountPerTeam()
{
	return 5;
}

bool UTNetStatics::IsSessionServer(const UObject* WorldContextObject)
{
	// 是否为服务器模式
	return WorldContextObject->GetWorld()->GetNetMode() == ENetMode::NM_DedicatedServer;
}

FString UTNetStatics::GetSessionNameStr()
{
	return GetCommandlineArgAsString(GetSessionNameKey());
}

FName UTNetStatics::GetSessionNameKey()
{
	return FName("SESSION_NAME");
}

FString UTNetStatics::GetSessionSearchIdStr()
{
	return GetCommandlineArgAsString(GetSessionSearchIdKey());
}

FName UTNetStatics::GetSessionSearchIdKey()
{
	return FName("SESSION_SEARCH_ID");
}

int UTNetStatics::GetSessionPort()
{
	return GetCommandlineArgAsInt(GetPortKey());
}

FName UTNetStatics::GetPortKey()
{
	return FName("PORT");
}

int UTNetStatics::GetPlayerCount()
{
	return GetCommandlineArgAsInt(GetPlayerCountKey());
}

FName UTNetStatics::GetPlayerCountKey()
{
	return FName("PLAYER_COUNT");
}

FName UTNetStatics::GetCoordinatorURLKey()
{
	// 返回用于解析命令行参数的键值（这里是固定字符串 "COORDINATOR_URL"）
	return FName("COORDINATOR_URL");
}

FString UTNetStatics::GetCoordinatorURL()
{
	// 优先从命令行中获取 Coordinator URL
	FString CoordinatorURL = GetCommandlineArgAsString(GetCoordinatorURLKey());
	if (CoordinatorURL != "")
	{
		// 如果命令行中有值，则直接返回
		return CoordinatorURL;
	}

	// 如果命令行参数为空，则使用配置文件中的默认值
	return GetDefaultCoordinatorURL();
}

FString UTNetStatics::GetDefaultCoordinatorURL()
{
	FString CoordinatorURL = "";

	// TODO:更改项目名称的时候这里要记得更改，此乃CSDN的54期
	// 从配置文件 [Crunch.Net] 节点中读取键 "CoordinatorURL"
	// 目标配置文件为 DefaultGame.ini
	GConfig->GetString(TEXT("Crunch.Net"), TEXT("CoordinatorURL"), CoordinatorURL, GGameIni);

	// 打印日志，方便调试，输出获取到的默认 URL
	UE_LOG(LogTemp, Warning, TEXT("Getting Default Coordinator URL as: %s"), *CoordinatorURL)

	// 返回默认配置中的 URL
	return CoordinatorURL;
}

FString UTNetStatics::GetCommandlineArgAsString(const FName& ParamName)
{
	FString OutVal = "";
	FString CommandLineArg = FString::Printf(TEXT("%s="), *(ParamName.ToString()));
	FParse::Value(FCommandLine::Get(), *CommandLineArg, OutVal);
	return OutVal;
}

int32 UTNetStatics::GetCommandlineArgAsInt(const FName& ParamName)
{
	int32 OutVal = 0;
	FString CommandLineArg = FString::Printf(TEXT("%s="), *(ParamName.ToString()));
	FParse::Value(FCommandLine::Get(), *CommandLineArg, OutVal);
	return OutVal;
}

FString UTNetStatics::GetTestingURL()
{
	FString TestURL = GetCommandlineArgAsString(GetTestingURLKey());
	UE_LOG(LogTemp, Warning, TEXT("获取测试的 URL: %s"), *TestURL)
	return TestURL;
}

FName UTNetStatics::GetTestingURLKey()
{
	return FName("TESTING_URL");
}

void UTNetStatics::ReplacePort(FString& OutURLStr, int NewPort)
{
	// 用现有的字符串构造一个 FURL 对象
	// FURL 是 UE 内部的 “统一资源定位符”，专门用于网络连接/关卡跳转
	FURL URL(nullptr, *OutURLStr, ETravelType::TRAVEL_Absolute);

	// 把 FURL 里的端口号改掉
	URL.Port = NewPort;

	// 再转回字符串，覆盖原来的 URL
	OutURLStr = URL.ToString();
}
