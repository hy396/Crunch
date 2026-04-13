# 聊天系统

## 概述

Crunch 的聊天系统支持多频道文本通信，包含传统消息列表和弹幕滚动两种显示模式。通过 Server/Client RPC 实现网络同步。

## 架构

```text
玩家输入消息
  → UChatWidget::SendChatMessage()
  → IChatInterface::SendChatMessageToServer()
  → ACPlayerController::Server_SendChatMessage()  [Server RPC]
  → 服务器验证后广播
  → ACPlayerController::Client_ReceiveChatMessage()  [Client RPC]
  → IChatInterface::ReceiveChatMessageFromServer()
  → UChatWidget::AddMessageToChat()
```

## 核心类型

### EChatChannelType

```cpp
enum class EChatChannelType : uint8
{
    Team = 0,   // 队友聊天
    All  = 1    // 全体聊天
};
```

### FChatMessage

```cpp
struct FChatMessage
{
    FString SenderName;            // 发送者名称
    FString MessageContent;        // 消息内容
    EChatChannelType ChannelType;  // 频道类型
    FGenericTeamId SenderTeamId;   // 发送者队伍
    FDateTime Timestamp;           // 时间戳
};
```

## 核心类

### IChatInterface

纯虚接口，`ACPlayerController` 和 `ALobbyPlayerController` 实现：

- `SendChatMessageToServer(Message, ChannelType)` — 发送消息到服务器
- `ReceiveChatMessageFromServer(Message)` — 从服务器接收消息

### UChatWidget

主聊天 UI Widget，绑定组件：

- `ChatScrollBox` — 消息滚动区域
- `ChatInputBox` — 输入框
- `SendButton` — 发送按钮
- `ChannelComboBox` — 频道选择下拉框
- `ChatMessagesBorder` — 消息区域边框

关键方法：

| 方法 | 说明 |
| ---- | ---- |
| `ShowChatWidget()` / `HideChatWidget()` | 显示/隐藏聊天界面 |
| `ToggleChatWidget()` | 切换显示状态 |
| `AddMessageToChat(Message, bIsSelf, bIsTeammate)` | 添加消息到历史列表 |
| `ShowTemporaryMessage(Message, bIsSelf, bIsTeammate)` | 弹出临时消息（不展开完整聊天） |
| `FocusChatInput()` / `UnfocusChatInput()` | 控制输入框焦点 |
| `SendChatMessage()` | 从输入框发送消息 |

关键配置：

- `MaxChatMessages = 100` — 最大消息历史数
- `TemporaryMessageDuration = 10.0f` — 临时消息显示时长（秒）
- `bAutoHideAfterSend = true` — 发送后自动隐藏

### UChatMessageItemWidget

单条消息 Widget，支持两种模式：

**列表模式** — `SetChatMessageWithColors(Message, bIsSelf, bIsTeammate)`

根据发送者身份着色：
- 自己 — 绿色
- 队友 — 蓝色
- 敌方 — 红色

频道标识着色：队内聊天 / 全体聊天使用不同颜色前缀。

消息格式：`[时间] [频道] 发送者: 内容`，使用 `URichTextBlock` 渲染富文本。

**弹幕模式** — `SetAsBarrageMode(Message, bIsSelf, bIsTeammate, BarragePanel)`

消息从屏幕右侧滚动到左侧：

- `BarrageMoveSpeed = 250.0f` — 滚动速度（像素/秒）
- `BarrageMessageDuration = 8.0f` — 滚动持续时间
- `BarrageVerticalSpacing = 30.0f` — 垂直间距
- `UpdateBarragePosition()` — 每帧更新位置
- `OnBarrageFinished()` — 滚动完成后自动销毁

安全处理：`SanitizeMessageContent()` 转义特殊字符。

## 网络 RPC

### ACPlayerController 中的聊天 RPC

```cpp
// 客户端 → 服务器（带验证）
UFUNCTION(Server, Reliable, WithValidation)
void Server_SendChatMessage(const FString& Message, EChatChannelType ChannelType);

// 服务器 → 客户端
UFUNCTION(Client, Reliable)
void Client_ReceiveChatMessage(const FChatMessage& Message);
```

服务器收到消息后根据频道类型决定转发范围：
- `Team` — 仅转发给同队玩家
- `All` — 转发给所有玩家

## 输入绑定

通过 Enhanced Input System 绑定：
- `ACPlayerController::ToggleChatInputAction` — 切换聊天界面的输入动作
- 聊天输入框获得焦点后，`NativeOnKeyDown` 处理 Enter（发送）和 Escape（关闭）
