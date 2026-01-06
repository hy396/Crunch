// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "OptionsDataRegistry.generated.h"

class UListDataObject_Base;
class UListDataObject_Collection;

/**
 * 选项数据注册表（Options Data Registry）
 *
 * 负责集中管理游戏设置界面（如画面、音频、控制等）的所有可配置选项数据。
 * 将选项按“标签页（Tab）”组织为多个 Collection，并支持：
 * - 按 Tab ID 动态获取扁平化选项列表
 * - 初始化各模块的默认选项结构
 * - 与本地玩家上下文绑定（用于输入映射等）
 *
 * 生命周期：由选项主界面（Frontend Options Screen）在打开时创建，
 *            并在关闭时销毁。不进行持久化（标记为 transient）。
 */
UCLASS()
class CRUNCH_API UOptionsDataRegistry : public UObject
{
	GENERATED_BODY()
public:
	/**
	 * 初始化整个选项数据注册表。
	 *
	 * 此函数应在对象构造完成后、UI 绑定前调用一次。
	 * 它会：
	 *   1. 保存所属本地玩家引用
	 *   2. 调用各模块初始化函数（画面、音频、控制等）
	 *   3. 构建完整的选项数据树
	 *
	 * @param InOwningLocalPlayer 所属的本地玩家，用于获取玩家特定的配置上下文（如键位绑定）
	 */
	void InitOptionsDataRegistry(ULocalPlayer* InOwningLocalPlayer);

	/**
	 * 获取所有已注册的选项标签页集合（每个集合对应一个 UI Tab）。
	 *
	 * 返回顺序通常与 UI 中的 Tab 显示顺序一致。
	 *
	 * @return 所有 UListDataObject_Collection 的引用数组（只读）
	 */	
	const TArray<UListDataObject_Collection*>& GetRegisteredOptionsTabCollections() const {return RegisteredOptionsTabCollections;}

	/**
	 * 根据选中的 Tab ID，获取该标签页下所有可显示的选项项（扁平化列表）。
	 *
	 * 此函数会递归遍历指定 Tab 的 Collection 及其所有子项，
	 * 返回一个线性列表，供 ListView 或 ScrollView 直接绑定使用。
	 *
	 * @param InSelectedTabID 当前激活的 Tab 标识符（如 "Video", "Audio", "Controls"）
	 * @return 该 Tab 下所有有效的 UListDataObject_Base 子项（包含嵌套子项）
	 */
	TArray<UListDataObject_Base*> GetListSourceItemsBySelectedTabID(const FName& InSelectedTabID) const;

	
private:
	/** 初始化“游戏性”选项集合（例如难度、HUD 缩放、字幕等） */
	void InitGameplayCollectionTab();

	/** 初始化“音频”选项集合（主音量、音乐、音效、语音等） */
	void InitAudioCollectionTab();

	/** 初始化“画面”选项集合（分辨率、画质预设、帧率限制、VSync 等） */
	void InitVideoCollectionTab();

	/** 初始化“控制”选项集合（键位绑定、手柄灵敏度、反转视角等） */
	void InitControlCollectionTab(ULocalPlayer* InOwningLocalPlayer);

	/** 所有已注册的选项 Tab 集合（每个元素是一个 Collection 根节点） */
	UPROPERTY(transient)
	TArray<UListDataObject_Collection*> RegisteredOptionsTabCollections;

	/**
	* 递归遍历选项数据树，收集所有子项。
	*
	* 用于将树形结构（支持分组、折叠）转换为 UI 列表所需的线性数据源。
	*
	* @param InParentData 当前遍历的父节点（可能是 Collection 或 Group）
	* @param OutFoundChildListData [out] 输出收集到的所有子项（深度优先顺序）
	*/
	void FindChildListDataRecursively(UListDataObject_Base* InParentData, TArray<UListDataObject_Base*>& OutFoundChildListData) const;
};
