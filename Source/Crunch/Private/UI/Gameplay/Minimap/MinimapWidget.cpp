// 幻雨喜欢小猫咪

#include "MinimapWidget.h"
#include "MinimapActor.h"
#include "Components/Image.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "GameplayTagContainer.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Blueprint/WidgetTree.h"
#include "Subsystems/Subsystem.h"
#include "GAS/Core/TGameplayTags.h"

void UMinimapWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 生成小地图Actor
	SpawnMinimapActor();

	// 配置小地图参数
	if (MinimapActor)
	{
		MinimapActor->ConfigureMinimap(MinimapViewRange, MinimapViewHeight);
	}

	// 设置图像控件的材质
	if (MinimapImage && MinimapActor)
	{
		UMaterialInstanceDynamic* DynamicMaterial = MinimapImage->GetDynamicMaterial();
		if (DynamicMaterial)
		{
			DynamicMaterial->SetTextureParameterValue(RenderTargetParamName, MinimapActor->GetRenderTarget());
		}
	}

	// 设置定时器定期更新小地图位置和单位状态
	GetWorld()->GetTimerManager().SetTimer(
		MinimapUpdateTimerHandle,
		this,
		&UMinimapWidget::OnMinimapUpdateTimer,
		0.1f, // 每0.1秒更新一次
		true
	);
}

void UMinimapWidget::NativeDestruct()
{
	// 清除定时器
	if (GetWorld() && MinimapUpdateTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(MinimapUpdateTimerHandle);
	}

	Super::NativeDestruct();
}

void UMinimapWidget::ConfigureMinimap(float ViewRange, float ViewHeight)
{
	MinimapViewRange = ViewRange;
	MinimapViewHeight = ViewHeight;

	if (MinimapActor)
	{
		MinimapActor->ConfigureMinimap(ViewRange, ViewHeight);
	}
}

void UMinimapWidget::SetTargetToFollow(AActor* Target)
{
	TargetToFollow = Target;
}

void UMinimapWidget::AddUnitMarker(AActor* Unit, UTexture2D* Icon, FLinearColor Color)
{
	if (!Unit || !UnitMarkersContainer) return;

	// 检查是否已存在标记
	if (UnitMarkers.Contains(Unit)) return;

	// 创建标记图像
	UImage* MarkerImage = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
	if (!MarkerImage) return;

	// 设置图像和颜色
	if (Icon)
	{
		MarkerImage->SetBrushFromTexture(Icon);
	}
	else
	{
		// 如果没有提供图标，使用默认的圆形图像
		MarkerImage->SetBrushFromMaterial(nullptr); // 使用默认材质
	}
	MarkerImage->SetColorAndOpacity(Color);

	// 添加到容器
	UCanvasPanelSlot* CanvasSlot = UnitMarkersContainer->AddChildToCanvas(MarkerImage);
	if (CanvasSlot)
	{
		// 设置标记大小
		CanvasSlot->SetSize(UnitMarkerSize);

		// 存储标记
		UnitMarkers.Add(Unit, MarkerImage);
		
		// 初始化死亡状态
		UnitDeathStatus.Add(Unit, false);

		// 更新标记位置
		UpdateUnitMarkerPosition(Unit);
	}
}

void UMinimapWidget::RemoveUnitMarker(AActor* Unit)
{
	if (!Unit || !UnitMarkersContainer) return;

	// 查找并移除标记
	if (TObjectPtr<UImage>* MarkerImage = UnitMarkers.Find(Unit))
	{
		if (*MarkerImage)
		{
			UnitMarkersContainer->RemoveChild(*MarkerImage);
		}
		UnitMarkers.Remove(Unit);
	}
	
	// 移除死亡状态记录
	UnitDeathStatus.Remove(Unit);
}

void UMinimapWidget::UpdateUnitMarkerPosition(AActor* Unit)
{
	if (!Unit || !MinimapActor || !UnitMarkersContainer) return;

	// 查找标记
	TObjectPtr<UImage>* MarkerImage = UnitMarkers.Find(Unit);
	if (!MarkerImage || !*MarkerImage) return;

	// 获取CanvasPanelSlot
	UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>((*MarkerImage)->Slot);
	if (!CanvasSlot) return;

	// 将世界坐标转换为小地图UV坐标
	FVector2D UVCoord = MinimapActor->WorldToMinimapUV(Unit->GetActorLocation());

	// 获取小地图图像的实际大小
	FVector2D MinimapSize = MinimapImage->GetCachedGeometry().GetLocalSize();

	// 计算标记位置
	FVector2D MarkerPosition;
	MarkerPosition.X = UVCoord.X * MinimapSize.X - UnitMarkerSize.X * 0.5f;
	MarkerPosition.Y = UVCoord.Y * MinimapSize.Y - UnitMarkerSize.Y * 0.5f;

	// 设置标记位置
	CanvasSlot->SetPosition(MarkerPosition);
}

bool UMinimapWidget::IsUnitDead(AActor* Unit) const
{
	// 检查单位是否实现了能力系统接口
	IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(Unit);
	if (!AbilitySystemInterface)
	{
		return false;
	}

	// 获取能力系统组件
	UAbilitySystemComponent* ASC = AbilitySystemInterface->GetAbilitySystemComponent();
	if (!ASC)
	{
		return false;
	}

	// 检查是否有死亡标签，使用预定义的标签以提高性能
	return ASC->HasMatchingGameplayTag(TGameplayTags::Stats_Dead);
}

void UMinimapWidget::CheckAndUpdateUnitStatus(AActor* Unit)
{
	if (!Unit) return;

	// 检查单位当前是否死亡
	bool bIsDead = IsUnitDead(Unit);
	
	// 获取之前的死亡状态
	bool* bWasDead = UnitDeathStatus.Find(Unit);
	bool bPreviouslyDead = bWasDead ? *bWasDead : false;
	
	// 更新死亡状态记录
	UnitDeathStatus.FindOrAdd(Unit) = bIsDead;
	
	// 如果状态发生了变化
	if (bIsDead != bPreviouslyDead)
	{
		if (bIsDead)
		{
			// 单位死亡，隐藏标记
			if (TObjectPtr<UImage>* MarkerImage = UnitMarkers.Find(Unit))
			{
				if (*MarkerImage)
				{
					(*MarkerImage)->SetVisibility(ESlateVisibility::Hidden);
				}
			}
		}
		else
		{
			// 单位复活，显示标记
			if (TObjectPtr<UImage>* MarkerImage = UnitMarkers.Find(Unit))
			{
				if (*MarkerImage)
				{
					(*MarkerImage)->SetVisibility(ESlateVisibility::HitTestInvisible);
				}
			}
		}
	}
}

FReply UMinimapWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	// 处理鼠标点击事件
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		HandleMinimapClick(InMouseEvent);
		return FReply::Handled();
	}

	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UMinimapWidget::SpawnMinimapActor()
{
	if (!MinimapActorClass) return;

	UWorld* World = GetWorld();
	if (!World) return;

	// 设置生成参数
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// 生成小地图Actor
	MinimapActor = World->SpawnActor<AMinimapActor>(MinimapActorClass, SpawnParams);
}

void UMinimapWidget::OnMinimapUpdateTimer()
{
	UpdateMinimapPosition();

	// 更新所有单位标记位置和状态
	for (auto It = UnitMarkers.CreateIterator(); It; ++It)
	{
		if (It.Key().IsValid())
		{
			AActor* Unit = It.Key().Get();
			
			// 更新标记位置
			UpdateUnitMarkerPosition(Unit);
			
			// 检查并更新单位状态
			CheckAndUpdateUnitStatus(Unit);
		}
		else
		{
			// 移除无效的标记
			It.RemoveCurrent();
		}
	}
}

void UMinimapWidget::UpdateMinimapPosition()
{
	// 更新小地图摄像机位置
	if (MinimapActor && TargetToFollow.Get())
	{
		MinimapActor->UpdateMinimapPosition(TargetToFollow->GetActorLocation());
	}
}

void UMinimapWidget::HandleMinimapClick(const FPointerEvent& InMouseEvent)
{
	if (!MinimapActor || !TargetToFollow.Get()) return;

	// 获取点击位置相对于小地图的坐标
	FVector2D LocalClickPosition = InMouseEvent.GetScreenSpacePosition() - MinimapImage->GetCachedGeometry().GetAbsolutePosition();

	// 获取小地图图像的实际大小
	FVector2D MinimapSize = MinimapImage->GetCachedGeometry().GetLocalSize();

	// 计算UV坐标
	FVector2D UVCoord;
	UVCoord.X = LocalClickPosition.X / MinimapSize.X;
	UVCoord.Y = LocalClickPosition.Y / MinimapSize.Y;

	// 将UV坐标转换为世界坐标
	FVector CameraLocation = MinimapActor->GetActorLocation();
	FVector WorldPosition;
	WorldPosition.X = (UVCoord.X - 0.5f) * MinimapViewRange * 2.0f + CameraLocation.X;
	WorldPosition.Y = (0.5f - UVCoord.Y) * MinimapViewRange * 2.0f + CameraLocation.Y;
	WorldPosition.Z = CameraLocation.Z - MinimapViewHeight; // 设置在地面附近

	// 发送移动命令到玩家控制器
	APlayerController* PlayerController = GetOwningPlayer();
	if (PlayerController)
	{
		// 这里可以发送移动命令到服务器
		// 例如：PlayerController->MoveToLocation(WorldPosition);
		UE_LOG(LogTemp, Log, TEXT("Minimap clicked at world position: %s"), *WorldPosition.ToString());
	}
}