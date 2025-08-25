// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "TNetStatics.generated.h"

/**
 * 
 */
UCLASS()
class CRUNCH_API UTNetStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	/**
	 * 生成在线会话配置
	 * @param SessionName 会话名称标识符
	 * @param SessionSearchId 会话搜索ID
	 * @param Port 监听端口号
	 * @return 配置好会话设置对象
	 */
	static FOnlineSessionSettings GenerateOnlineSessionSettings(const FName& SessionName, const FString& SessionSearchId, int32 Port);

	/**
	 * 获取全局 Session 接口
	 * 使用 IOnlineSubsystem::Get() 直接获取默认的在线子系统。
	 * 注意：在 PIE 多实例 或 专用服务器环境下可能不安全。
	 */
	static IOnlineSessionPtr GetSessionPtr();

	/**
	 * 获取全局 Identity 接口
	 * 使用 IOnlineSubsystem::Get() 直接获取默认的在线子系统。
	 * 注意：在 PIE 多实例 或 专用服务器环境下可能不安全。
	 */
	static IOnlineIdentityPtr GetIdentityPtr();

	/**
	 * 获取与指定 WorldContextObject 对应的 Session 接口
	 * 推荐使用此版本，能保证在 PIE 多开 / 客户端 / 服务端环境下正确拿到对应的子系统。
	 *
	 * @param WorldContextObject 任何可提供 UWorld 的对象（如 GameInstance、Actor、Widget 等）
	 * @return IOnlineSessionPtr 会话接口（可能为 nullptr）
	 */
	static IOnlineSessionPtr GetSessionPtr(const UObject* WorldContextObject);

	/**
	 * 获取与指定 WorldContextObject 对应的 Identity 接口
	 * 推荐使用此版本，能保证在 PIE 多开 / 客户端 / 服务端环境下正确拿到对应的子系统。
	 *
	 * @param WorldContextObject 任何可提供 UWorld 的对象（如 GameInstance、Actor、Widget 等）
	 * @return IOnlineIdentityPtr 身份接口（可能为 nullptr）
	 */
	static IOnlineIdentityPtr GetIdentityPtr(const UObject* WorldContextObject);

	/**
	 * 获取每支队伍的基础玩家数量
	 * @return 玩家数量（默认值为5）
	 */
	static uint8 GetPlayerCountPerTeam();

	/**
	 * 检查当前上下文是否为会话服务器
	 * @param WorldContextObject 世界上下文对象
	 * @return 是否为服务器实例
	 */
	static bool IsSessionServer(const UObject* WorldContextObject);
	
	/**
	 * 获取会话名称字符串常量
	 * @return 会话名称字符串
	 */
	static FString GetSessionNameStr();
	/**
	 * 获取会话名称键值
	 * @return 会话名称的FName键
	 */
	static FName GetSessionNameKey();
	
	/**
	 * 获取会话搜索ID字符串
	 * @return 会话搜索ID字符串
	 */
	static FString GetSessionSearchIdStr();
	/**
	 * 获取会话搜索ID键值
	 * @return 会话搜索ID的FName键
	 */
	static FName GetSessionSearchIdKey();
	
	/**
	 * 获取会话端口配置
	 * @return 网络监听端口号
	 */
	static int GetSessionPort();
	/**
	 * 获取端口配置键值
	 * @return 端口号配置的FName键
	 */
	static FName GetPortKey();

	/**
	 * 获取协调器URL键值
	 * @return 协调器服务URL的FName键
	 */
	static FName GetCoordinatorURLKey();

	/**
	 * 获取协调器URL地址
	 * @return 协调器服务地址字符串
	 */
	static FString GetCoordinatorURL();

	/**
	 * 获取默认协调器URL
	 * @return 默认的协调器服务地址
	 */
	static FString GetDefaultCoordinatorURL();
	
	/**
	 * 从命令行参数中获取指定名称的字符串值
	 * 
	 * @param ParamName 参数名称（FName类型）
	 * @return 对应的字符串值，如果参数未找到则返回空字符串
	 * 
	 * 示例：
	 * 命令行参数：-loglevel=3
	 * 调用 GetCommandlineArgAsString(TEXT("loglevel")) 将返回 "3"
	 */
	static FString GetCommandlineArgAsString(const FName& ParamName);

	/**
	 * 从命令行参数中获取指定名称的整数值
	 * 
	 * @param ParamName 参数名称（FName类型）
	 * @return 对应的整数值，如果参数未找到或转换失败则返回0
	 * 
	 * 示例：
	 * 命令行参数：-port=8080
	 * 调用 GetCommandlineArgAsInt(TEXT("port")) 将返回 8080
	 */
	static int GetCommandlineArgAsInt(const FName& ParamName);
};
