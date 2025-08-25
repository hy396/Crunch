// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/IHttpResponse.h"
#include "Interfaces/IHttpRequest.h"
#include "MGameInstance.generated.h"


/**
 * 登录完成委托
 * 参数：
 *   - bWasSuccessful：登录是否成功
 *   - PlayerNickName：玩家昵称
 *   - ErrorMsg：错误信息
 */
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnLoginCompleted, bool /*bWasSuccessful*/, const FString& /*PlayerNickName*/, const FString& /*ErrorMsg*/);

/**
 * 自定义游戏实例类 - 管理游戏全局状态和在线服务
 * 功能：
 *   - 玩家登录和身份验证
 *   - 会话创建、搜索和加入
 *   - 服务器协调和匹配
 *   - 关卡管理和切换
 */
UCLASS()
class CRUNCH_API UMGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	// 开始匹配（服务器端调用）
	void StartMatch();
	// 游戏实例初始化
	virtual void Init() override;

	/*************************************/
	/*             登录功能              */
	/*************************************/
public:
	// 检查是否已登录
	bool IsLoggedIn() const;
	
	// 检查是否正在登录中
	bool IsLoggingIn() const;
	
	// 客户端通过账户门户登录
	void ClientAccountPortalLogin();
	
	// 登录完成委托
	FOnLoginCompleted OnLoginCompleted;
	
private:
	// 客户端登录实现
	void ClientLogin(const FString& Type, const FString& Id, const FString& Token);
	
	// 登录完成回调
	void LoginCompleted(int32 NumOfLocalPlayer, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);

	// 登录委托句柄
	FDelegateHandle LoggingInDelegateHandle;
	/*************************************/
	/*      客户端会话创建和搜索			 */
	/*************************************/
public:
	// 请求创建并加入新会话
	void RequestCreateAndJoinSession(const FName& NewSessionName);
	
	// 取消会话创建
	void CancelSessionCreation();

private:
	// 会话创建请求完成回调
	void SessionCreationRequestCompleted(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully, FGuid SessionSearchId);
	/*************************************/
	/*           会话服务器功能			 */
	/*************************************/
public:
	// 玩家加入会话（服务器端调用）
	void PlayerJoined(const FUniqueNetIdRepl& UniqueId);
	
	// 玩家离开会话（服务器端调用）
	void PlayerLeft(const FUniqueNetIdRepl& UniqueId);
private:
	// 创建会话
	void CreateSession();
	// 当会话创建完成时触发
	void OnSessionCreated(FName SessionName, bool bWasSuccessful);
	// 当会话结束完成时回调
	void EndSessionCompleted(FName SessionName, bool bWasSuccessful);
	// 服务器会话名称
	FString ServerSessionName;
	// 会话服务器端口
	int32 SessionServerPort;
	// 终止会话服务器
	void TerminateSessionServer();
	
	// 等待玩家加入的超时计时器句柄
	FTimerHandle WaitPlayerJoinTimeoutHandle;

	// 等待玩家加入的超时时间（默认60秒）
	UPROPERTY(EditDefaultsOnly, Category = "Session")
	float WaitPlayerJoinTimeOutDuration = 60.f;
	// 玩家超时未加入时触发
	void WaitPlayerJoinTimeoutReached();
	
	// 玩家记录集合
	TSet<FUniqueNetIdRepl> PlayerRecord;
private:	
	// 主菜单关卡引用
	UPROPERTY(EditDefaultsOnly, Category = "Map")
	TSoftObjectPtr<UWorld> MainMenuLevel;

	// 大厅关卡引用
	UPROPERTY(EditDefaultsOnly, Category = "Map")
	TSoftObjectPtr<UWorld> LobbyLevel;

	// 游戏关卡引用
	UPROPERTY(EditDefaultsOnly, Category = "Map")
	TSoftObjectPtr<UWorld> GameLevel;

	// 加载关卡并监听连接
	void LoadLevelAndListen(TSoftObjectPtr<UWorld> Level);
};
