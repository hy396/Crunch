// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "StormCore.generated.h"

class USphereComponent;
// 声明多播委托：当核心到达目标点时触发
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnGoalReachedDelegate, AActor* /*ViewTarget*/, int /*WiningTeam*/);

// 声明多播委托：当队伍影响力计数更新时触发
DECLARE_MULTICAST_DELEGATE_TwoParams(FonTeamInfluncerCountUpdatedDelegate, int /*TeamOneInfluencerCount*/, int /*TeamTwoInfluencerCount*/);

/**
 * 风暴核心类 - 游戏核心机制
 * 
 * 功能：
 * 1. 作为两队争夺的目标点
 * 2. 根据两队影响力权重决定移动方向
 * 3. 实现核心占领机制
 * 4. 管理胜利条件触发
 */
UCLASS()
class CRUNCH_API AStormCore : public ACharacter
{
	GENERATED_BODY()

public:
	// 核心到达目标点时触发的委托
	FOnGoalReachedDelegate OnGoalReachedDelegate;
	
	// 队伍影响力更新时触发的委托
	FonTeamInfluncerCountUpdatedDelegate OnTeamInfluenceCountUpdated;
	// Sets default values for this character's properties
	AStormCore();

	// 网络同步属性设置
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// 获取核心当前进度（0-1范围）
	float GetProgress() const;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// 当控制器接管时调用
	virtual void PossessedBy(AController* NewController) override;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

#if WITH_EDITOR
	// 编辑器属性变更回调
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
private:
	// 扩张动画
	UPROPERTY(EditDefaultsOnly, Category = "Move")
	TObjectPtr<UAnimMontage> ExpandMontage; 
	// 捕获动画
	UPROPERTY(EditDefaultsOnly, Category = "Move")
	TObjectPtr<UAnimMontage> CaptureMontage;
	
	// 影响力半径
	UPROPERTY(EditDefaultsOnly, Category = "Move")
	float InfluenceRadius = 1000.f; 
	// 最大移动速度
	UPROPERTY(EditDefaultsOnly, Category = "Move")
	float MaxMoveSpeed = 500.f; 
	
	// 检测组件
	// 影响力范围检测球体
	UPROPERTY(VisibleDefaultsOnly, Category = "Detection")
	TObjectPtr<USphereComponent> InfluenceRange;
	// 贴花显示范围
	UPROPERTY(VisibleDefaultsOnly, Category = "Detection")
	TObjectPtr<class UDecalComponent> GroundDecalComponent; 
	// 观察视角相机
	UPROPERTY(VisibleDefaultsOnly, Category = "Detection")
	TObjectPtr<class UCameraComponent> ViewCam; 

	// TODO:想让英雄的权重大一点
	// 重叠事件处理
	UFUNCTION()
	void NewInfluencerInRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	
	UFUNCTION()
	void InfluencerLeftRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// 核心逻辑函数
	void UpdateTeamWeight(); // 更新队伍权重
	void UpdateGoal(); // 更新移动目标

	// 游戏目标点
	// 队伍1目标点
	UPROPERTY(EditAnywhere, Category = "Team")
	TObjectPtr<AActor> TeamOneGoal;
	
	// 队伍2目标点
	UPROPERTY(EditAnywhere, Category = "Team")
	TObjectPtr<AActor> TeamTwoGoal; 
	// 队伍1的核心
	UPROPERTY(EditAnywhere, Category = "Team")
	TObjectPtr<AActor> TeamOneCore; 
	// 队伍2的核心
	UPROPERTY(EditAnywhere, Category = "Team")
	TObjectPtr<AActor> TeamTwoCore; 

	// 网络同步属性：当前要捕获的核心
	UPROPERTY(ReplicatedUsing = OnRep_CoreToCapture)
	TObjectPtr<AActor> CoreToCapture;

	float CoreCaptureSpeed = 0.f; // 核心捕获速度
	float TravelLength = 0.f; // 总移动距离

	// 网络同步回调
	UFUNCTION()
	void OnRep_CoreToCapture();
	
	// 胜利条件处理
	void GoalReached(int WiningTeam);
	
	// 核心捕获流程
	void CaptureCore(); // 开始捕获核心
	void ExpandFinished(); // 扩张动画完成回调
	
	// 队伍影响力计数
	int32 TeamOneInfluencerCount = 0; // 队伍1影响力单位数量
	int32 TeamTwoInfluencerCount = 0; // 队伍2影响力单位数量

	// 当前队伍权重（-1到1的范围）
	float TeamWeight = 0.f;

	// 拥有的AI控制器
	UPROPERTY()
	TObjectPtr<class AAIController> OwnerAIC; 
};
