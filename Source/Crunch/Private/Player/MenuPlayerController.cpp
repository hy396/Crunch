// 幻雨喜欢小猫咪


#include "MenuPlayerController.h"

#include "Blueprint/UserWidget.h"

void AMenuPlayerController::BeginPlay()
{
	Super::BeginPlay();
	// 设置为仅UI输入模式并显示鼠标
	SetInputMode(FInputModeUIOnly());
	SetShowMouseCursor(true);

	// 具有服务器权限而且是本地玩家控制器时生成菜单UI
	if (HasAuthority() && IsLocalPlayerController())
	{
		SpawnWidget();
	}
}

void AMenuPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	if (IsLocalPlayerController())
	{
		SpawnWidget();
	}
}

void AMenuPlayerController::SpawnWidget()
{
	if (MenuWidgetClass)
	{
		MenuWidget = CreateWidget<UUserWidget>(this, MenuWidgetClass);
		if (MenuWidget)
		{
			MenuWidget->AddToViewport();
		}
	}
}
