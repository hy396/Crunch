# 法师技能 —— 蓝图配置指南

## 你需要完成的步骤

---

## 技能一览（先看全貌）

| 键位 | 技能蓝图 | 父类C++ | 需要的GE | 需要的蒙太奇 | 需要的Actor蓝图 |
|------|----------|---------|----------|-------------|----------------|
| Q | `BP_GA_ArcaneOrb` | `GA_ArcaneOrb` | 2个 | 1个 | `BP_ArcaneOrbProjectile` |
| E | `BP_GA_PhaseShift` | `GA_PhaseShift` | 2个 | 1个 | `BP_ArcaneTrapActor` |
| F | `BP_GA_MeteorStrike` | `GA_MeteorStrike` | 3个 | 2个 | `BP_BurningGroundActor` |
| R | `BP_GA_ArcaneStorm` | `GA_ArcaneStorm` | 6个 | 2个 | 无（复用现有TA） |

---

### 1. 创建 GameplayEffect 蓝图

在编辑器中 Content Browser → 右键 → Blueprint Class → GameplayEffect

#### Q技能 奥术弹 → 配置到 `BP_GA_ArcaneOrb`

| GE蓝图名 | 用途 | 关键配置 | 配到蓝图哪个属性 |
|-----------|------|----------|-----------------|
| `GE_ArcaneOrb_Damage` | 奥术弹伤害 | Instant, Execution: ECC_AttackDamage, DamageType: MagicDamage | `DamageEffect` → `DamageEffect` 字段 |
| `GE_ArcaneOrb_ManaRestore` | 命中回蓝 | Instant, Modifier: Mana +15 (Additive) | `ManaRestoreEffect` |

#### E技能 相位转移 → 配置到 `BP_GA_PhaseShift`

| GE蓝图名 | 用途 | 关键配置 | 配到蓝图哪个属性 |
|-----------|------|----------|-----------------|
| `GE_PhaseShift_TrapDamage` | 陷阱触发伤害 | Instant, Execution: ECC_AttackDamage, DamageType: MagicDamage | `TrapDamageEffect` |
| `GE_PhaseShift_Root` | 陷阱定身 | HasDuration(1.5秒), GrantedTag: Status.Root | `RootEffect` |

#### F技能 陨石打击 → 配置到 `BP_GA_MeteorStrike`

| GE蓝图名 | 用途 | 关键配置 | 配到蓝图哪个属性 |
|-----------|------|----------|-----------------|
| `GE_MeteorStrike_Impact` | 落地爆炸伤害 | Instant, Execution: ECC_AttackDamage, 高伤害 | `ImpactDamageEffect` → `DamageEffect` 字段 |
| `GE_MeteorStrike_BurnDOT` | 燃烧地面DOT | Instant, 少量魔法伤害（每0.5秒触发一次） | `BurnDamageEffect` |
| `GE_MeteorStrike_Aim` | 瞄准状态 | Infinite, GrantedTag: Stats.Aim（同BlackHole的AimEffect） | `AimEffect` |

#### R技能 奥术风暴 → 配置到 `BP_GA_ArcaneStorm`

| GE蓝图名 | 用途 | 关键配置 | 配到蓝图哪个属性 |
|-----------|------|----------|-----------------|
| `GE_ArcaneStorm_TickDamage` | 每0.5秒Tick伤害 | Instant, DamageType: MagicDamage | `TickDamageEffect` → `DamageEffect` 字段 |
| `GE_ArcaneStorm_Slow` | Tick减速 | HasDuration(1秒), Modifier: MoveSpeed ×0.5, GrantedTag: Status.Slow | `SlowEffect` |
| `GE_ArcaneStorm_BurstDamage` | 最终爆发伤害 | Instant, 高伤害 | `FinalBurstDamageEffect` → `DamageEffect` 字段 |
| `GE_ArcaneStorm_Silence` | 最终沉默 | HasDuration(2秒), GrantedTag: Status.Silence | `SilenceEffect` |
| `GE_ArcaneStorm_SelfSlow` | 引导期间自身减速 | Infinite（手动移除）, Modifier: MoveSpeed ×0.6 | `SelfSlowEffect` |
| `GE_ArcaneStorm_Aim` | 瞄准状态 | Infinite, GrantedTag: Stats.Aim | `AimEffect` |

---

### 2. 创建 AnimMontage + AnimNotify

#### Q技能 奥术弹 → 蒙太奇配到 `BP_GA_ArcaneOrb` 的 `CastMontage`

| 蒙太奇 | AnimNotify Tag | 触发时刻 | 配到蓝图哪个属性 |
|--------|----------------|----------|-----------------|
| `AM_ArcaneOrb_Cast` | `Ability.ArcaneOrb.Cast` | 能量球离手的那一帧 | `CastMontage` |

#### E技能 相位转移 → 蒙太奇配到 `BP_GA_PhaseShift` 的 `PhaseShiftMontage`

| 蒙太奇 | AnimNotify Tag | 触发时刻 | 配到蓝图哪个属性 |
|--------|----------------|----------|-----------------|
| `AM_PhaseShift` | `Ability.PhaseShift.Shift` | 角色闪烁/消失的那一帧 | `PhaseShiftMontage` |

#### F技能 陨石打击 → 两个蒙太奇分别配到 `BP_GA_MeteorStrike`

| 蒙太奇 | AnimNotify Tag | 触发时刻 | 配到蓝图哪个属性 |
|--------|----------------|----------|-----------------|
| `AM_MeteorStrike_Targeting` | 无（循环播放，选目标阶段） | 手持法杖指向地面循环 | `TargetingMontage` |
| `AM_MeteorStrike_Cast` | 无（施法动作） | 手举向天空召唤陨石 | `CastMontage` |

> **注意：** 参考 `GA_BlackHole` 的两阶段模式。`TargetingMontage` 在选择地面位置时播放，玩家确认后切换到 `CastMontage`。

#### R技能 奥术风暴 → 两个蒙太奇分别配到 `BP_GA_ArcaneStorm`

| 蒙太奇 | AnimNotify Tag | 触发时刻 | 配到蓝图哪个属性 |
|--------|----------------|----------|-----------------|
| `AM_ArcaneStorm_Targeting` | 无（循环播放，选目标阶段） | 法师蓄力/选择区域循环 | `TargetingMontage` |
| `AM_ArcaneStorm_Channel` | 无（引导循环动画） | 法师双手维持风暴的姿势 | `ChannelMontage` |

> **注意：** 同陨石打击，`TargetingMontage` 在选择阶段播放，确认后切换到 `ChannelMontage` 持续引导。

**添加 AnimNotify 步骤：**
1. 打开蒙太奇编辑器
2. 在时间线上右键 → Add Notify → `AnimNotify_GameplayEvent`
3. 在 Details 面板中设置 `EventTag` 为上表中对应的Tag字符串
4. 没有 AnimNotify Tag 的蒙太奇不需要添加Notify，它们是纯视觉循环动画

---

### 3. 创建技能蓝图（每个属性该填什么）

在 Content Browser 中：右键 → Blueprint Class → 搜索对应C++父类名

#### `BP_GA_ArcaneOrb`（父类：`GA_ArcaneOrb`）

| 属性名（Details面板） | 填什么 | 说明 |
|----------------------|--------|------|
| `CastMontage` | `AM_ArcaneOrb_Cast` | 施法动画 |
| `OrbProjectileClass` | `BP_ArcaneOrbProjectile` | 穿透弹蓝图类 |
| `OrbSpeed` | 1200 | 投射物速度（故意慢） |
| `OrbMaxDistance` | 3500 | 最大飞行距离 |
| `DamageEffect` | 展开：`DamageEffect` = `GE_ArcaneOrb_Damage`，`DamageTypeDefinitions` 配置MagicDamage | 伤害定义 |
| `ManaRestoreEffect` | `GE_ArcaneOrb_ManaRestore` | 命中回蓝GE |
| `DistanceDamageMultiplier` | 1.5 | 最远处伤害倍率 |
| `CastSocketName` | `Hand_R`（或你的手部Socket名） | 发射点 |
| `ArcaneOrbCueTag` | `GameplayCue.Ability.ArcaneOrb` | 视觉特效 |
| `CooldownDuration` | 4 | 冷却时间（秒） |

#### `BP_GA_PhaseShift`（父类：`GA_PhaseShift`）

| 属性名 | 填什么 | 说明 |
|--------|--------|------|
| `PhaseShiftMontage` | `AM_PhaseShift` | 闪烁动画 |
| `BackwardPushSpeed` | 1500 | 后退速度 |
| `TrapActorClass` | `BP_ArcaneTrapActor` | 陷阱蓝图类 |
| `TrapRadius` | 200 | 陷阱检测半径 |
| `TrapDuration` | 4 | 陷阱存在时间 |
| `TrapArmDelay` | 0.5 | 激活延迟 |
| `TrapDamageEffect` | `GE_PhaseShift_TrapDamage` | 陷阱伤害 |
| `RootEffect` | `GE_PhaseShift_Root` | 定身效果 |
| `PhaseShiftCueTag` | `GameplayCue.Ability.PhaseShift` | 视觉特效 |
| `CooldownDuration` | 12 | 冷却时间 |

#### `BP_GA_MeteorStrike`（父类：`GA_MeteorStrike`）

| 属性名 | 填什么 | 说明 |
|--------|--------|------|
| `TargetingMontage` | `AM_MeteorStrike_Targeting` | 瞄准阶段动画 |
| `CastMontage` | `AM_MeteorStrike_Cast` | 施法动画 |
| `TargetActorClass` | 现有的 `TargetActor_GroundPick` 蓝图 | 地面选点 |
| `ImpactTargetActorClass` | 现有的 `TargetActor_Around` 蓝图 | 落点范围检测 |
| `TargetAreaRadius` | 500 | 爆炸范围 |
| `CastRange` | 2500 | 施法距离 |
| `ImpactDamageEffect` | 展开配置：`DamageEffect`=`GE_MeteorStrike_Impact` | 落地伤害 |
| `ImpactPushSpeed` | 3000 | 爆炸击退 |
| `AimEffect` | `GE_MeteorStrike_Aim` | 瞄准状态 |
| `BurningGroundActorClass` | `BP_BurningGroundActor` | 燃烧地面蓝图 |
| `BurnZoneDuration` | 5 | 燃烧持续时间 |
| `BurnTickInterval` | 0.5 | 燃烧间隔 |
| `BurnDamageEffect` | `GE_MeteorStrike_BurnDOT` | 燃烧伤害 |
| `MeteorFallDelay` | 1.0 | 陨石下落延迟 |
| `MeteorImpactCueTag` | `GameplayCue.Ability.MeteorStrike` | 视觉特效 |
| `CooldownDuration` | 16 | 冷却时间 |

#### `BP_GA_ArcaneStorm`（父类：`GA_ArcaneStorm`）

| 属性名 | 填什么 | 说明 |
|--------|--------|------|
| `TargetingMontage` | `AM_ArcaneStorm_Targeting` | 瞄准阶段动画 |
| `ChannelMontage` | `AM_ArcaneStorm_Channel` | 引导循环动画 |
| `TargetActorClass` | 现有的 `TargetActor_GroundPick` 蓝图 | 地面选点 |
| `StormTargetActorClass` | 现有的 `TargetActor_BlackHole` 蓝图 | 区域周期检测（PullSpeed=0由C++设置） |
| `StormRadius` | 800 | 风暴半径 |
| `CastRange` | 2000 | 施法距离 |
| `TickInterval` | 0.5 | Tick间隔 |
| `StormDuration` | 5 | 风暴持续时间 |
| `TickDamageEffect` | 展开配置：`DamageEffect`=`GE_ArcaneStorm_TickDamage` | 每Tick伤害 |
| `FinalBurstDamageEffect` | 展开配置：`DamageEffect`=`GE_ArcaneStorm_BurstDamage` | 最终爆发伤害 |
| `SlowEffect` | `GE_ArcaneStorm_Slow` | Tick减速 |
| `SilenceEffect` | `GE_ArcaneStorm_Silence` | 最终沉默 |
| `SelfSlowEffect` | `GE_ArcaneStorm_SelfSlow` | 自身引导减速 |
| `AimEffect` | `GE_ArcaneStorm_Aim` | 瞄准状态 |
| `StormCueTag` | `GameplayCue.Ability.ArcaneStorm` | 风暴持续VFX |
| `StormExplosionCueTag` | `GameplayCue.Ability.ArcaneStorm.Explosion` | 爆发VFX |
| `CooldownDuration` | 80 | 冷却时间 |

---

### 4. 创建投射物/Actor蓝图

| 蓝图名 | 父类 | 关键设置 | 配到哪个技能蓝图 |
|--------|------|----------|----------------|
| `BP_ArcaneOrbProjectile` | `ArcaneOrbProjectile` | **bAutoDestroyOnHit = false**（必须！），添加球体碰撞+Niagara特效 | `BP_GA_ArcaneOrb` → `OrbProjectileClass` |
| `BP_ArcaneTrapActor` | `ArcaneTrapActor` | 添加 Decal 或 Niagara 法阵特效 | `BP_GA_PhaseShift` → `TrapActorClass` |
| `BP_BurningGroundActor` | `BurningGroundActor` | 添加 Niagara 火焰地面特效 | `BP_GA_MeteorStrike` → `BurningGroundActorClass` |

**ArcaneOrbProjectile 蓝图步骤：**
1. 添加 `SphereComponent`（半径~50cm）
2. 设置碰撞预设 `OverlapAllDynamic`
3. 添加 `NiagaraComponent` 发光能量球特效
4. **Details → `bAutoDestroyOnHit = false`**（不勾选=穿透）
5. **Details → `bCorrectDirectionByCamera = true`**

**ArcaneTrapActor 蓝图步骤：**
1. 添加 `DecalComponent` 或 `NiagaraComponent` 地面法阵特效
2. 特效大小建议和 `TrapRadius`(200cm) 匹配
3. 检测球体半径由C++代码 `InitializeTrap` 设置，不需要手动调

**BurningGroundActor 蓝图步骤：**
1. 添加 `NiagaraComponent` 火焰地面循环特效
2. 特效范围需要和 `TargetAreaRadius`(500cm) 匹配

---

### 5. TargetActor（复用现有，无需新建）

| TargetActor | 已有蓝图 | 被哪个技能使用 | 配到哪个属性 |
|-------------|---------|---------------|-------------|
| `TargetActor_GroundPick` | 已有 | MeteorStrike, ArcaneStorm | `TargetActorClass` |
| `TargetActor_Around` | 已有 | MeteorStrike（落地检测） | `ImpactTargetActorClass` |
| `TargetActor_BlackHole` | 已有 | ArcaneStorm（周期检测，PullSpeed由C++设0） | `StormTargetActorClass` |

---

### 6. 创建 GameplayCue

| Cue 蓝图名 | Tag | 效果 | 触发者 |
|------------|-----|------|--------|
| `GC_ArcaneOrb` | `GameplayCue.Ability.ArcaneOrb` | 能量球飞行拖尾 | `BP_GA_ArcaneOrb` |
| `GC_PhaseShift` | `GameplayCue.Ability.PhaseShift` | 闪烁/消失特效 | `BP_GA_PhaseShift` |
| `GC_MeteorStrike` | `GameplayCue.Ability.MeteorStrike` | 陨石下落+爆炸VFX | `BP_GA_MeteorStrike` |
| `GC_ArcaneStorm` | `GameplayCue.Ability.ArcaneStorm` | 风暴持续VFX | `BP_GA_ArcaneStorm` |
| `GC_ArcaneStorm_Explosion` | `GameplayCue.Ability.ArcaneStorm.Explosion` | 风暴终结爆发VFX | `BP_GA_ArcaneStorm` |

---

### 7. 注册到角色

在法师角色的 `CAbilitySystemComponent` 蓝图中：
- `Abilities` TMap 添加：
  - `AbilityQ` → `BP_GA_ArcaneOrb`
  - `AbilityE` → `BP_GA_PhaseShift`
  - `AbilityF` → `BP_GA_MeteorStrike`
  - `AbilityR` → `BP_GA_ArcaneStorm`

---

### 8. 配置冷却GE

每个技能需要一个冷却GE，配置方式：
1. 创建GE蓝图（如 `GE_ArcaneOrb_Cooldown`）
2. Duration Policy = `HasDuration`
3. Duration Magnitude = 对应冷却时间（或引用技能的 `CooldownDuration`）
4. Gameplay Tag → 添加对应Tag

| 冷却GE | Tag | 配到哪个技能蓝图 |
|--------|-----|----------------|
| `GE_ArcaneOrb_Cooldown` | `Ability.ArcaneOrb.Cooldown` | `BP_GA_ArcaneOrb` → `CooldownGameplayEffectClass` |
| `GE_PhaseShift_Cooldown` | `Ability.PhaseShift.Cooldown` | `BP_GA_PhaseShift` → `CooldownGameplayEffectClass` |
| `GE_MeteorStrike_Cooldown` | `Ability.MeteorStrike.Cooldown` | `BP_GA_MeteorStrike` → `CooldownGameplayEffectClass` |
| `GE_ArcaneStorm_Cooldown` | `Ability.ArcaneStorm.Cooldown` | `BP_GA_ArcaneStorm` → `CooldownGameplayEffectClass` |

---

## Debug 调试

- 技能蓝图 Details 中 `bShouldDrawDebug = true` → 开启TargetActor调试绘制
- `UGASDebugHelper::PrintASCState(ASC, true)` → 打印完整GAS状态
- `UGASDebugHelper::RegisterPredictionFailureMonitor(ASC)` → 监控预测失败
- 控制台 `showdebug abilitysystem` → 实时调试面板
- 控制台 `log LogCrunchGAS Verbose` → 显示详细GAS日志

---

## 完成后就可以了！

C++ 代码已全部就绪，你只需要完成上述蓝图层的配置即可在游戏中释放技能。
