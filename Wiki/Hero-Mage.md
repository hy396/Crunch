# 法师

[返回英雄列表](Heroes.md) | [返回首��](Home.md)

## 定位

远程魔法输出。技能偏向 AOE 伤害和控制，擅长团战中造成大面积伤害。

## 技能

### 奥术弹

**按键**: Q | **类型**: 技能射击 | **C++ 类**: `UGA_ArcaneOrb`

发射一颗慢速穿透奥术能量球，穿透所有敌人造成魔法伤害。

- 穿透所有命中目标（同一目标只命中一次）
- 飞行距离越远伤害倍率越高（最远 1.5x）
- 命中敌人时回复施法者少量法力

**配套 Actor**: `AArcaneOrbProjectile`（继承 `AProjectileActor`，记录已命中目标防重复）

---

### 相位转移

**按键**: E | **类型**: 位移/控制 | **C++ 类**: `UGA_PhaseShift`

短暂获得无敌和穿透状态，向后方位移，并在原位放置一个奥术陷阱。

- 激活期间附加无敌 + 穿透标签
- 动画事件触发时向后推移角色
- 原地生成陷阱，延迟 0.5 秒后激活
- 陷阱持续 4 秒，首个触碰的敌人受到伤害 + 定身

**配套 Actor**: `AArcaneTrapActor`（球形碰撞检测，队伍识别，延迟激活）

---

### 陨石打击

**按键**: F | **类型**: AOE 伤害 | **C++ 类**: `UGA_MeteorStrike`

两阶段技能：先选择地面目标点，再施法召唤陨石。

1. 播放瞄准动画，使用 `ATargetActor_GroundPick` 选择落点
2. 播放施法动画
3. 延迟 1 秒后陨石落地，使用 `ATargetActor_Around` 检测范围内敌人
4. 造成爆发伤害 + 击退
5. 落点生成燃烧地面，持续 5 秒造成 DOT

**配套 Actor**: `ABurningGroundActor`（周期性灼烧，球形碰撞，队伍识别）

---

### 奥术风暴

**按键**: R（大招） | **类型**: AOE 控制 | **C++ 类**: `UGA_ArcaneStorm`

两阶段技能：选择地面目标点，在该位置召唤持续 5 秒的奥术风暴。

1. 施法期间对自身施加减速
2. 使用 `ATargetActor_GroundPick` 选择区域
3. 风暴持续 5 秒，每 0.5 秒造成一次伤害并减速范围内敌人
4. 风暴结束时爆发：对范围内所有敌人造成高额伤害 + 沉默 + 镜头震动

复用 `ATargetActor_BlackHole`（`PullSpeed=0`）作为纯范围检测器。

## 蓝图配置

详细的蓝图配置步骤见 [GAS_Mage_Abilities.md](../docs/GAS_Mage_Abilities.md)。

## 源文件

| 文件 | 说明 |
| ---- | ---- |
| `Source/Crunch/Private/GAS/Abilities/GA_ArcaneOrb.h/.cpp` | 奥术弹技能 |
| `Source/Crunch/Private/GAS/Abilities/GA_PhaseShift.h/.cpp` | 相位转移技能 |
| `Source/Crunch/Private/GAS/Abilities/GA_MeteorStrike.h/.cpp` | 陨石打击技能 |
| `Source/Crunch/Private/GAS/Abilities/GA_ArcaneStorm.h/.cpp` | 奥术风暴技能 |
| `Source/Crunch/Private/Actor/ArcaneOrbProjectile.h/.cpp` | 穿透投射物 |
| `Source/Crunch/Private/Actor/ArcaneTrapActor.h/.cpp` | 奥术陷阱 |
| `Source/Crunch/Private/Actor/BurningGroundActor.h/.cpp` | 燃烧地面 |
| `docs/GAS_Mage_Abilities.md` | 蓝图配置指南 |
