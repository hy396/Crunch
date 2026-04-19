// 幻雨喜欢小猫咪


#include "ListDataObject_StringResolution.h"

#include "FrontendUI/FrontendSettings/FrontendGameUserSettings.h"
#include "Kismet/KismetSystemLibrary.h"

void UListDataObject_StringResolution::InitResolutionValues()
{
	// 创建可用分辨率数组
	TArray<FIntPoint> AvailableResolutions;

	// 获取支持的全屏分辨率
	UKismetSystemLibrary::GetSupportedFullscreenResolutions(AvailableResolutions);
	
	// 列表从小到大进行排序
	AvailableResolutions.Sort([](const FIntPoint& A, const FIntPoint& B)->bool
	{
		return A.SizeSquared() < B.SizeSquared();
	});
	
	// 遍历所有可用分辨率，将其转换为选项添加到列表中
	for (const FIntPoint& Resolution : AvailableResolutions)
	{
		AddDynamicOption(ResToValueString(Resolution), ResToDisplayText(Resolution));
	}
	
	// 设置最大允许分辨率为最后一个（最大的）分辨率
	MaximumAllowedResolution = ResToValueString(AvailableResolutions.Last());

	// 将最大允许分辨率设置为默认值
	SetDefaultValueFromString(MaximumAllowedResolution);
}

void UListDataObject_StringResolution::OnDataObjectInitialized()
{
	// 调用父类的实现
	Super::OnDataObjectInitialized();
	
	// if (DataDynamicGetter)
	// {
	// Debug::Print(TEXT("动态获取器中获取分辨率：") + DataDynamicGetter->GetValueAsString());
		
	// 如果在有效列表中获取不到分辨率，则去配置文件中获取
	if (!TrySetDisplayTextFromStringValue(CurrentStringValue))
	{
		CurrentDisplayText = ResToDisplayText(UFrontendGameUserSettings::Get()->GetScreenResolution());
	}
	// }
}

FString UListDataObject_StringResolution::ResToValueString(const FIntPoint& InResolution) const
{
	// 格式化为"(X=1920,Y=1080)"形式的字符串
	return FString::Printf(TEXT("(X=%i,Y=%i)"),InResolution.X,InResolution.Y);
}

FText UListDataObject_StringResolution::ResToDisplayText(const FIntPoint& InResolution) const
{
	// 格式化为"1920 x 1080"形式的显示文本
	const FString DisplayString = FString::Printf(TEXT("%i x %i"),InResolution.X,InResolution.Y);

	return FText::FromString(DisplayString);
}
