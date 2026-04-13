# 大斧战士技能 —— 蓝图配置指南

## 你需要完成的步骤

---

## 技能一览（先看全貌）

| 键位 | 技能蓝图 | 父类C++ | 需要的GE | 需要的蒙太奇 | 需要的Actor蓝图 |
|------|----------|---------|----------|-------------|----------------|
| Q | `BP_GA_WarCry` | `GA_WarCry` | 2个 | 1个 | 无 |
| E | `BP_GA_Guillotine` | `GA_Guillotine` | 3个 | 1个 | 无 |
| F | `BP_GA_AxeThrow` | `GA_AxeThrow` | 2个 | 2个 | `BP_BoomerangAxeProjectile` |
| R | `BP_GA_Earthquake` | `GA_Earthquake` | 5个 | 1个 | 无 |

---

### 1. 创建 GameplayEffect 蓝图

在编辑器中 Content Browser → 右键 → Blueprint Class → GameplayEffect

#### Q技能 战吼 → 配置到 `BP_GA_WarCry`

| GE蓝图名 | 用途 | 关键配置 | 配到蓝图哪个属性 |
|-----------|------|----------|-----------------|
| `GE_WarCry_Damage` | 战吼伤害 | Instant, Execution: ECC_AttackDamage, DamageType: AttackDamage | `DamageEffect` → `DamageEffect` 字段 |
| `GE_WarCry_Debuff` | 减甲减速 | HasDuration(3秒), Modifier: Armor -30, MoveSpeed ×0.7 | `DebuffEffect` |

#### E技能 断头台 → 配置到 `BP_GA_Guillotine`

| GE蓝图名 | 用途 | 关键配置 | 配到蓝图哪个属性 |
|-----------|------|----------|-----------------|
| `GE_Guillotine_Damage` | 基础劈砍伤害 | Instant, Execution: ECC_AttackDamage | `DamageEffect` → `DamageEffect` 字段 |
| `GE_Guillotine_ExecuteDamage` | 斩杀额外伤害 | Instant, Execution: ECC_AttackDamage, 伤害更高 | `ExecuteBonusDamage` → `DamageEffect` 字段 |
| `GE_Guillotine_Bleed` | 流血DOT | HasDuration(5秒), Period=1秒, GrantedTag: Status.Bleed | `BleedEffect` |

#### F技能 掷斧回旋 → 配置到 `BP_GA_AxeThrow`

| GE蓝图名 | 用途 | 关键配置 | 配到蓝图哪个属性 |
|-----------|------|----------|-----------------|
| `GE_AxeThrow_Damage` | 掷斧伤害（去回共用） | Instant, Execution: ECC_AttackDamage | `DamageEffect` → `DamageEffect` 字段 |
| `GE_AxeThrow_Slow` | 命中减速 | HasDuration(2秒), Modifier: MoveSpeed ×0.6, GrantedTag: Status.Slow | `SlowEffect` |

#### R技能 地裂 → 配置到 `BP_GA_Earthquake`

| GE蓝图名 | 用途 | 关键配置 | 配到蓝图哪个属性 |
|-----------|------|----------|-----------------|
| `GE_Earthquake_Slam1_Damage` | 第1次砸地伤害 | Instant, Execution: ECC_AttackDamage, 低伤害 | `Slam1DamageEffect` → `DamageEffect` 字段 |
| `GE_Earthquake_Slam2_Damage` | 第2次砸地伤害 | Instant, Execution: ECC_AttackDamage, 中伤害 | `Slam2DamageEffect` → `DamageEffect` 字段 |
| `GE_Earthquake_Slam3_Damage` | 第3次砸地伤害 | Instant, Execution: ECC_AttackDamage, 高伤害 | `Slam3DamageEffect` → `DamageEffect` 字段 |
| `GE_Earthquake_Stun` | 眩晕（第1/2次砸地） | HasDuration(0.5秒), GrantedTag: Status.Stun | `StunEffect` |
| `GE_Earthquake_Knockup` | 击飞（第3次砸地） | HasDuration(0.3秒), GrantedTag: Status.Knockup | `KnockupEffect` |

---

### 2. 创建 AnimMontage + AnimNotify

#### Q技能 战吼 → 蒙太奇配到 `BP_GA_WarCry` 的 `WarCryMontage`

| 蒙太奇 | AnimNotify Tag | 触发时刻 | 配到蓝图哪个属性 |
|--------|----------------|----------|-----------------|
| `AM_WarCry` | `Ability.WarCry.Shout` | 吼叫最大声时 | `WarCryMontage` |

#### E技能 断头台 → 蒙太奇配到 `BP_GA_Guillotine` 的 `GuillotineMontage`

| 蒙太奇 | AnimNotify Tag | 触发时刻 | 配到蓝图哪个属性 |
|--------|----------------|----------|-----------------|
| `AM_Guillotine` | `Ability.Guillotine.Slam` | 斧头砸到地面的那一帧 | `GuillotineMontage` |

#### F技能 掷斧回旋 → 两个蒙太奇分别配到 `BP_GA_AxeThrow`

| 蒙太奇 | AnimNotify Tag | 触发时刻 | 配到蓝图哪个属性 |
|--------|----------------|----------|-----------------|
| `AM_AxeThrow` | `Ability.AxeThrow.Release` | 斧头离手的那一帧 | `ThrowMontage` |
| `AM_AxeCatch` | 无（纯视觉接住动作） | 斧头返回到手中 | `CatchMontage` |

#### R技能 地裂 → 一个蒙太奇配到 `BP_GA_Earthquake` 的 `EarthquakeMontage`

| 蒙太奇 | AnimNotify Tag | 触发时刻 | 配到蓝图哪个属性 |
|--------|----------------|----------|-----------------|
| `AM_Earthquake` | `Ability.Earthquake.Slam1` | 第1次砸到地面 | `EarthquakeMontage` |
|  | `Ability.Earthquake.Slam2` | 第2次砸到地面 | （同一个蒙太奇，3个Notify） |
|  | `Ability.Earthquake.Slam3` | 第3次砸到地面 | |

> **注意：** 地裂只有一个蒙太奇，但里面有3个 AnimNotify 分别对应3次砸地。动画应该是：跳起 → 砸1 → 砸2 → 砸3。

**添加 AnimNotify 步骤：**
1. 打开蒙太奇编辑器
2. 在时间线上右键 → Add Notify → `AnimNotify_GameplayEvent`
3. 在 Details 面板中设置 `EventTag` 为上表对应的Tag字符串
4. 没有 AnimNotify Tag 的蒙太奇（如 `AM_AxeCatch`）不需要添加Notify

---

### 3. 创建技能蓝图（每个属性该填什么）

在 Content Browser 中：右键 → Blueprint Class → 搜索对应C++父类名

#### `BP_GA_WarCry`（父类：`GA_WarCry`）

| 属性名（Details面板） | 填什么 | 说明 |
|----------------------|--------|------|
| `WarCryMontage` | `AM_WarCry` | 战吼动画 |
| `TargetActorClass` | 现有的 `TargetActor_Around` 蓝图 | 圆形范围检测 |
| `DetectionRadius` | 600 | 战吼范围 |
| `DamageEffect` | 展开：`DamageEffect` = `GE_WarCry_Damage`，`DamageTypeDefinitions` 配置AttackDamage | 伤害定义 |
| `DebuffEffect` | `GE_WarCry_Debuff` | 减甲减速GE |
| `PushSpeed` | 500 | 轻微击退 |
| `WarCryCueTag` | `GameplayCue.Ability.WarCry` | 视觉标签 |
| `CooldownDuration` | 6 | 冷却时间（秒） |

#### `BP_GA_Guillotine`（父类：`GA_Guillotine`）

| 属性名 | 填什么 | 说明 |
|--------|--------|------|
| `GuillotineMontage` | `AM_Guillotine` | 劈砍动画 |
| `TargetActorClass` | 现有的 `TargetActor_Line` 蓝图 | 前方线性检测 |
| `CleaveRange` | 400 | 劈砍射程（短距离） |
| `CleaveRadius` | 150 | 劈砍宽度（宽半径模拟扇形） |
| `DamageEffect` | 展开：`DamageEffect` = `GE_Guillotine_Damage`，配置AttackDamage | 基础伤害 |
| `ExecuteBonusDamage` | 展开：`DamageEffect` = `GE_Guillotine_ExecuteDamage`，配置AttackDamage | 斩杀额外伤害 |
| `BleedEffect` | `GE_Guillotine_Bleed` | 流血DOT |
| `ExecuteHealthThreshold` | 0.3 | 目标血量低于30%触发斩杀 |
| `GuillotineCueTag` | `GameplayCue.Ability.Guillotine` | 视觉标签 |
| `CooldownDuration` | 10 | 冷却时间 |

#### `BP_GA_AxeThrow`（父类：`GA_AxeThrow`）

| 属性名 | 填什么 | 说明 |
|--------|--------|------|
| `ThrowMontage` | `AM_AxeThrow` | 投掷动画 |
| `CatchMontage` | `AM_AxeCatch` | 接住动画 |
| `AxeProjectileClass` | `BP_BoomerangAxeProjectile` | 回旋斧蓝图类 |
| `ThrowSpeed` | 2500 | 飞行速度 |
| `MaxThrowDistance` | 2000 | 最大飞行距离 |
| `DamageEffect` | 展开：`DamageEffect` = `GE_AxeThrow_Damage`，配置AttackDamage | 伤害定义 |
| `SlowEffect` | `GE_AxeThrow_Slow` | 命中减速 |
| `ThrowSocketName` | `Hand_R`（或你的手部Socket名） | 发射位置 |
| `AxeThrowCueTag` | `GameplayCue.Ability.AxeThrow` | 视觉标签 |
| `CooldownDuration` | 12 | 冷却时间 |

#### `BP_GA_Earthquake`（父类：`GA_Earthquake`）

| 属性名 | 填什么 | 说明 |
|--------|--------|------|
| `EarthquakeMontage` | `AM_Earthquake` | 三连砸地动画（含3个AnimNotify） |
| `TargetActorClass` | 现有的 `TargetActor_Around` 蓝图 | 圆形范围检测 |
| `Slam1Radius` | 400 | 第1次砸地范围 |
| `Slam2Radius` | 600 | 第2次砸地范围 |
| `Slam3Radius` | 900 | 第3次砸地范围（最大） |
| `Slam1DamageEffect` | 展开：`DamageEffect` = `GE_Earthquake_Slam1_Damage` | 第1段伤害 |
| `Slam2DamageEffect` | 展开：`DamageEffect` = `GE_Earthquake_Slam2_Damage` | 第2段伤害 |
| `Slam3DamageEffect` | 展开：`DamageEffect` = `GE_Earthquake_Slam3_Damage` | 第3段伤害 |
| `StunEffect` | `GE_Earthquake_Stun` | 眩晕GE（第1/2次用） |
| `KnockupEffect` | `GE_Earthquake_Knockup` | 击飞GE（第3次用） |
| `Slam3PushSpeed` | 4000 | 第3次砸地击退力度 |
| `LeapUpSpeed` | 1500 | 起跳高度 |
| `EarthquakeCueTag` | `GameplayCue.Ability.Earthquake` | 视觉标签 |
| `CooldownDuration` | 60 | 冷却时间 |

---

### 4. 创建投射物蓝图

| 蓝图名 | 父类 | 关键设置 | 配到哪个技能蓝图 |
|--------|------|----------|----------------|
| `BP_BoomerangAxeProjectile` | `BoomerangAxeProjectile` | **bAutoDestroyOnHit = false**（必须！） | `BP_GA_AxeThrow` → `AxeProjectileClass` |

**投射物蓝图配置步骤：**
1. 添加 `SphereComponent` 作为碰撞体（半径~30cm）
2. 添加 `StaticMeshComponent` 作为斧头模型
3. 设置碰撞预设为 `OverlapAllDynamic`
4. **Details → `bAutoDestroyOnHit = false`**（不勾选 = 穿透不销毁）
5. 可选：在蓝图Tick中添加旋转（`AddActorLocalRotation`）模拟飞斧旋转
6. 可选：添加 `NiagaraComponent` 拖尾特效

---

### 5. TargetActor（复用现有，无需新建）

| TargetActor | 已有蓝图 | 被哪个技能使用 | 配到哪个属性 |
|-------------|---------|---------------|-------------|
| `TargetActor_Around` | 已有 | WarCry, Earthquake | `TargetActorClass` |
| `TargetActor_Line` | 已有 | Guillotine（短距宽半径） | `TargetActorClass` |

---

### 6. 创建 GameplayCue

| Cue 蓝图名 | Tag | 效果 | 触发者 |
|------------|-----|------|--------|
| `GC_WarCry` | `GameplayCue.Ability.WarCry` | 吼叫音效 + 地面冲击波粒子 | `BP_GA_WarCry` |
| `GC_Guillotine` | `GameplayCue.Ability.Guillotine` | 重劈音效 + 地裂粒子 | `BP_GA_Guillotine` |
| `GC_AxeThrow` | `GameplayCue.Ability.AxeThrow` | 飞斧拖尾粒子 | `BP_GA_AxeThrow` |
| `GC_Earthquake` | `GameplayCue.Ability.Earthquake` | 砸地震动 + 裂缝粒子 | `BP_GA_Earthquake` |

---

### 7. 注册到角色

在大斧战士角色的 `CAbilitySystemComponent` 蓝图中：
- `Abilities` TMap 添加：
  - `AbilityQ` → `BP_GA_WarCry`
  - `AbilityE` → `BP_GA_Guillotine`
  - `AbilityF` → `BP_GA_AxeThrow`
  - `AbilityR` → `BP_GA_Earthquake`

---

### 8. 配置冷却GE

每个技能需要一个冷却GE：
1. 创建GE蓝图
2. Duration Policy = `HasDuration`
3. Duration Magnitude = 对应冷却时间
4. Gameplay Tag → 添加对应Tag

| 冷却GE | Tag | 配到哪个技能蓝图 |
|--------|-----|----------------|
| `GE_WarCry_Cooldown` | `Ability.WarCry.Cooldown` | `BP_GA_WarCry` → `CooldownGameplayEffectClass` |
| `GE_Guillotine_Cooldown` | `Ability.Guillotine.Cooldown` | `BP_GA_Guillotine` → `CooldownGameplayEffectClass` |
| `GE_AxeThrow_Cooldown` | `Ability.AxeThrow.Cooldown` | `BP_GA_AxeThrow` → `CooldownGameplayEffectClass` |
| `GE_Earthquake_Cooldown` | `Ability.Earthquake.Cooldown` | `BP_GA_Earthquake` → `CooldownGameplayEffectClass` |

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
