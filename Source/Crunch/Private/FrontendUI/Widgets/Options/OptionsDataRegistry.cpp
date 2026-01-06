// 幻雨喜欢小猫咪


#include "OptionsDataRegistry.h"

#include "DataObjects/ListDataObject_Collection.h"

void UOptionsDataRegistry::InitOptionsDataRegistry(ULocalPlayer* InOwningLocalPlayer)
{
	// 清空可能存在的旧数据（防御性编程）
	RegisteredOptionsTabCollections.Empty();

	// 初始化各个功能模块的选项集合
	InitGameplayCollectionTab();
	InitAudioCollectionTab();
	InitVideoCollectionTab();
	InitControlCollectionTab(InOwningLocalPlayer);

	// 注：此处可扩展，例如从配置文件动态加载自定义 Tab
}

TArray<UListDataObject_Base*> UOptionsDataRegistry::GetListSourceItemsBySelectedTabID(
	const FName& InSelectedTabID) const
{
	// 通过传入的Tab ID查找对应的选项集合
	UListDataObject_Collection* const* FoundTabCollectionPtr = RegisteredOptionsTabCollections.FindByPredicate(
		[InSelectedTabID](UListDataObject_Collection* AvailableTabCollection)->bool
		{
			return AvailableTabCollection->GetDataID() == InSelectedTabID;
		}
	);
	// 确保找到了对应的Tab集合，否则抛出断言错误
	checkf(FoundTabCollectionPtr, TEXT("在%s的tab下没有找到ID"), *InSelectedTabID.ToString());

	UListDataObject_Collection* FoundTabCollection = *FoundTabCollectionPtr;
	// return FoundTabCollection->GetAllChildListData();  这种只能获取最顶部的数据，如果有子类则不能展示

	// 创建数组用于存储所有子列表项（包括递归的子项）
	TArray<UListDataObject_Base*> AllChildListItems;
	// 遍历找到的Tab集合中的所有子数据
	for (UListDataObject_Base* ChildListData : FoundTabCollection->GetAllChildListData())
	{
		if (!ChildListData) continue;

		// 添加当前子数据到结果数组
		AllChildListItems.Add(ChildListData);

		// 如果当前子数据还有子数据，则递归查找所有子数据
		if (ChildListData->HasAnyChildListData())
		{
			FindChildListDataRecursively(ChildListData, AllChildListItems);
		}
	}
	return AllChildListItems;
}

void UOptionsDataRegistry::InitGameplayCollectionTab()
{
	// 我暂时不想要这个，我不太需要
}

void UOptionsDataRegistry::InitAudioCollectionTab()
{
	UListDataObject_Collection* AudioTabCollection = NewObject<UListDataObject_Collection>();
	AudioTabCollection->SetDataID(FName("AudioTabCollection"));
	AudioTabCollection->SetDataDisplayName(FText::FromString(TEXT("音频")));
	// Volume Category
	{
		UListDataObject_Collection* VolumeCategoryCollection = NewObject<UListDataObject_Collection>();
		VolumeCategoryCollection->SetDataID(FName("VolumeCategoryCollection"));
		VolumeCategoryCollection->SetDataDisplayName(FText::FromString(TEXT("音量"))); // 音量

		AudioTabCollection->AddChildListData(VolumeCategoryCollection);

		// 全局音量
		{
			
		}
		// 音乐音量
		{
			
		}
		// 音效音量
		{
			
		}
	}
	// 音频类别
	{
		// 允许后台音频
		{
			
		}
		// 使用HDR音频
		{

		}
	}

	RegisteredOptionsTabCollections.Add(AudioTabCollection);
}

void UOptionsDataRegistry::InitVideoCollectionTab()
{
	UListDataObject_Collection* VideoTabCollection = NewObject<UListDataObject_Collection>();
	VideoTabCollection->SetDataID(FName("VideoTabCollection"));
	VideoTabCollection->SetDataDisplayName(FText::FromString(TEXT("画面")));

	RegisteredOptionsTabCollections.Add(VideoTabCollection);
}

void UOptionsDataRegistry::InitControlCollectionTab(ULocalPlayer* InOwningLocalPlayer)
{
	UListDataObject_Collection* ControlTabCollection = NewObject<UListDataObject_Collection>();
	ControlTabCollection->SetDataID(FName("ControlTabCollection"));
	ControlTabCollection->SetDataDisplayName(FText::FromString(TEXT("控制")));

	RegisteredOptionsTabCollections.Add(ControlTabCollection);
}

void UOptionsDataRegistry::FindChildListDataRecursively(UListDataObject_Base* InParentData,
	TArray<UListDataObject_Base*>& OutFoundChildListData) const
{
	// 检查父数据对象是否有效，以及是否包含子数据
	if (!InParentData || !InParentData->HasAnyChildListData()) return;

	// 遍历父对象的所有子数据项
	for (UListDataObject_Base* SubChildListData : InParentData->GetAllChildListData())
	{
		// 跳过空的子数据项
		if (!SubChildListData) continue;

		// 将有效的子数据项添加到输出数组中
		OutFoundChildListData.Add(SubChildListData);
		
		// 如果子数据项还包含更深层的子数据，则递归调用此函数继续查找
		if (SubChildListData->HasAnyChildListData())
		{
			FindChildListDataRecursively(SubChildListData, OutFoundChildListData);
		}
	}
}

