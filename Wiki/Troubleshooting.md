# Troubleshooting

[返回首页](Home.md)

## 编译问题

### VRM4U assimp 库缺失

**错误**: `LNK1181: cannot open input file 'assimp-vc141-mt.lib'`

**原因**: VRM4U 插件的 assimp 第三方库因文件体积未纳入 Git，`ThirdParty/assimp/lib/` 下只有 `dummy.txt` 占位文件。

**解决**:

1. 克隆 VRM4U 的 assimp 分支：`git clone https://github.com/ruyo/assimp.git`
2. 用 CMake + MSVC 编译 Release 共享库（加 `-DASSIMP_WARNINGS_AS_ERRORS=OFF -DCMAKE_CXX_FLAGS="/w"` 避免新版编译器报错）
3. 将 `.lib` 复制到 `Plugins/VRM4U/ThirdParty/assimp/lib/x64/Release/assimp-vc141-mt.lib`
4. 将 `.dll` 复制到 `Plugins/VRM4U/ThirdParty/assimp/bin/x64/assimp-vc141-mt.dll`

注意：编译产物可能命名为 `assimp-vc14x-mt`，需重命名为 `assimp-vc141-mt`（VRM4ULoader.Build.cs 硬编码了这个名字）。

---

### Live Coding 阻塞外部编译

**错误**: `Unable to build while Live Coding is active`

**原因**: UE 编辑器正在运行，Live Coding 锁定了编译。

**解决**:
- 方案 1: 关闭编辑器后再编译
- 方案 2: 在编辑器中按 `Ctrl+Alt+F11` 触发 Live Coding 编译

---

### AsyncLoadingScreen API 不匹配

**错误**: `'StopSeamlessLoadingScreen': is not a member of 'UAsyncLoadingScreenLibrary'`

**原因**: 不同版本的 AsyncLoadingScreen 插件 API 不同。某些版本使用 `StopSeamlessLoadingScreen()`，当前安装的版本只有 `StopLoadingScreen()`。

**解决**: 检查安装的插件版本，使用对应的函数名。查看 `AsyncLoadingScreenLibrary.h` 确认可用方法。

---

### 新版 MSVC 对旧代码报 warnings-as-errors

**错误**: `error C2220: the following warning is treated as an error`（编译第三方库时）

**原因**: VS 2025+ 的 MSVC 对 C++17 兼容性检查更严格，旧代码的 signed/unsigned 比较等会被当作错误。

**解决**: 编译第三方库时在 CMake 中加：
```bash
-DASSIMP_WARNINGS_AS_ERRORS=OFF -DCMAKE_CXX_FLAGS="/w"
```

---

### UHT 找不到新添加的文件

**现象**: 新建的 UCLASS 在编辑器中不可见，或编译时找不到 `.generated.h`

**原因**: UBT 需要重新扫描源文件树。

**解决**:
1. 确认文件放在 `Source/Crunch/Private/` 的子目录下
2. 右键 `Crunch.uproject` -> Generate Visual Studio project files
3. 如仍无效，删除 `Intermediate/` 目录后重新生成

---

### GASDebugHelper API 不兼容

**错误**: `GetCooldownTimeRemainingAndDuration` 或 `GetAllAttributeSets` 不是 ASC 的成员

**原因**: 这些方法在 UE5.4 的 `UAbilitySystemComponent` 中不存在或签名不同。

**替代方案**:

冷却查询:
```cpp
// 替代 GetCooldownTimeRemainingAndDuration
FGameplayEffectQuery Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(*CooldownTags);
TArray<float> Remaining = ASC->GetActiveEffectsTimeRemaining(Query);
TArray<float> Durations = ASC->GetActiveEffectsDuration(Query);
```

属性集获取:
```cpp
// 替代 GetAllAttributeSets
const TArray<UAttributeSet*>& Sets = ASC->GetSpawnedAttributes();
```

AbilityEndedCallbacks 签名:
```cpp
// UE5.4 中 FAbilityEnded 的签名是 OneParam(UGameplayAbility*)
// 不是 OneParam(const FAbilityEndedData&)
ASC->AbilityEndedCallbacks.AddLambda([](UGameplayAbility* Ability) { ... });
```

---

### CMake 找不到 Visual Studio

**错误**: `Generator "Visual Studio 17 2022" could not find any instance of Visual Studio`

**原因**: 安装的是 VS 2025 (版本号 18)，CMake 不识别。

**解决**: 使用 NMake 或 Ninja 生成器：
```bash
# 先启动 VS 开发者命令行环境
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat"
cmake .. -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release
nmake
```

## 网络问题

### 属性不同步

**现象**: 客户端看不到属性变化

**排查**:
1. 检查 `GetLifetimeReplicatedProps` 中是否注册了该属性
2. 检查 `COND_` 条件是否正确（`COND_OwnerOnly` 的属性其他客户端看不到）
3. Meta 属性（AttackDamage/MagicDamage/TrueDamage）设计上就不同步

### RPC 不触发

**排查**:
1. Server RPC 确认有 `WithValidation`
2. 检查 `_Validate` 函数是否返回 `true`
3. 确认调用方是客户端（Server RPC 只能从客户端调用）

## 调试工具

```cpp
// 打印完整 ASC 状态
UGASDebugHelper::PrintASCState(ASC, true);

// 注册监控回调
UGASDebugHelper::RegisterPredictionFailureMonitor(ASC);
```

控制台命令：
```
showdebug abilitysystem    GAS 调试 HUD
stat net                   网络统计
p.NetShowCorrections 1     显示位置校正
```
