# 大斧战士

[返回英雄列表](Heroes.md) | [返回首页](Home.md)

## 定位

近战物理输出。技能偏向 AOE 和硬控，擅长冲入敌阵制造混乱。

## 技能

### ��吼

**按键**: Q | **类型**: AOE 减益 | **C++ 类**: `UGA_WarCry`

以自身为中心释放战吼冲击波。

- 播放战吼蒙太奇，等待动画事件
- 使用 `ATargetActor_Around`（附着角色）检测范围内敌人
- 对所有命中敌人造成伤害
- 施加减甲 + 减速 Debuff
- 轻微击退（从角色位置向外推）

---

### 断头台

**按键**: E | **类型**: 锥形伤害 | **C++ 类**: `UGA_Guillotine`

向前方锥形范围发出强力劈砍。

- 播放蓄力+劈砍蒙太奇
- 使用 `ATargetActor_Line`（短距离宽半径模拟锥形）检测前方敌人
- 对命中敌人造成基础伤害
- **斩杀机制**：如果目标当前生命值 <= 30%，额外造成加成伤害
- 对所有命中目标施加流血 DOT

斩杀判断通过读取目标 `UCAttributeSet` 的 Health/MaxHealth 实现。

---

### 掷斧回旋

**按键**: F | **类型**: 投射物 | **C++ 类**: `UGA_AxeThrow`

投掷飞斧，飞斧到达最大距离后自动返回。

1. 播放投掷蒙太奇，等待释放事件
2. 生成 `ABoomerangAxeProjectile`
3. 飞斧向前飞行，对路径上敌人造成伤害（去程）
4. 到达最大距离后反转方向飞回施法者
5. 返程再次对路径上敌人造成伤害
6. 飞斧接近施法者后发送返回事件，播放接斧蒙太奇

**配套 Actor**: `ABoomerangAxeProjectile`（继承 `AProjectileActor`，去程/回程分别记录命中目标，同一目标每段最多命中一次）

---

### 地裂

**按键**: R（大招） | **类型**: AOE 控制 | **C++ 类**: `UGA_Earthquake`

施法期间获得无敌和控制免疫，连续三次砸地。

1. 附加无敌 + 控制免疫标签
2. 播放砸地蒙太奇（包含 3 个动画事件：Slam1/Slam2/Slam3）
3. 每次砸地使用 `ATargetActor_Around`（附着角色）检测范围，范围逐渐扩大
4. **Slam1/Slam2**: 造成伤害 + 眩晕
5. **Slam3**: 造成最高伤害 + 击飞 + 击退（从角色位置向外推）+ 镜头震动

## 蓝图配置

详细的蓝图配置步骤见 [GAS_Warrior_Abilities.md](../docs/GAS_Warrior_Abilities.md)。

## 源文件

| 文件 | 说明 |
| ---- | ---- |
| `Source/Crunch/Private/GAS/Abilities/GA_WarCry.h/.cpp` | 战吼技能 |
| `Source/Crunch/Private/GAS/Abilities/GA_Guillotine.h/.cpp` | 断头台技能 |
| `Source/Crunch/Private/GAS/Abilities/GA_AxeThrow.h/.cpp` | 掷斧回旋技能 |
| `Source/Crunch/Private/GAS/Abilities/GA_Earthquake.h/.cpp` | 地裂技能 |
| `Source/Crunch/Private/Actor/BoomerangAxeProjectile.h/.cpp` | 回旋飞斧投射物 |
| `docs/GAS_Warrior_Abilities.md` | 蓝图配置指南 |
