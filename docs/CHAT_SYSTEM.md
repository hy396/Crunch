# Crunch 聊天系统详细指南

## 📋 概览

Crunch的聊天系统是一个功能丰富的实时多人游戏通信解决方案，支持多频道、发送者类型识别、弹幕效果和网络同步。

## 🎯 核心特性

### 1. 多频道支持
- **队伍聊天**: 仅队友可见的内部交流
- **全体聊天**: 所有玩家可见的公共频道
- **智能频道切换**: 根据游戏阶段自动调整可用频道

### 2. 发送者类型识别
- **自己**: 绿色显示，标识为"[我] 玩家名"
- **队友**: 蓝色显示，标识为"[队友] 玩家名"  
- **对手**: 红色显示，标识为"[对手] 玩家名"

### 3. 显示模式
- **永久聊天记录**: 完整的聊天历史，支持滚动查看
- **临时消息提示**: 10秒自动隐藏的快速显示
- **弹幕模式**: 从右到左滚动的实时弹幕效果

### 4. 富文本支持
- **颜色区分**: 不同类型消息使用不同颜色
- **时间戳**: 每条消息显示发送时间
- **频道标识**: 清晰显示消息所属频道
- **自动换行**: 智能文本换行和显示优化

## 🏗️ 系统架构

### 数据结构

#### FChatMessage
```cpp
USTRUCT(BlueprintType)
struct CRUNCH_API FChatMessage
{
    GENERATED_BODY()

    // 发送者名称
    UPROPERTY(BlueprintReadOnly)
    FString SenderName;

    // 消息内容
    UPROPERTY(BlueprintReadOnly)
    FString MessageContent;

    // 聊天频道类型
    UPROPERTY(BlueprintReadOnly)
    EChatChannelType ChannelType;

    // 发送者团队ID
    UPROPERTY(BlueprintReadOnly)
    FGenericTeamId SenderTeamId;

    // 时间戳
    UPROPERTY(BlueprintReadOnly)
    FDateTime Timestamp;
};
```

#### EChatChannelType
```cpp
UENUM(BlueprintType)
enum class EChatChannelType : uint8
{
    Team    UMETA(DisplayName = "队友聊天"),
    All     UMETA(DisplayName = "全体聊天")
};
```

### 核心组件

#### UChatWidget
主聊天界面控件，负责：
- 消息显示和输入处理
- 频道选择和切换
- 临时模式和正常模式管理
- 自动滚动和焦点控制

#### UChatMessageItemWidget  
单条消息显示控件，负责：
- 富文本格式化和显示
- 弹幕动画效果
- 自适应高度计算
- 特殊字符处理

#### IChatInterface
聊天接口定义，规范：
- 消息收发标准接口
- 网络RPC函数签名
- 消息处理流程

## 🔧 配置指南

### 1. 输入映射配置

在 `Config/DefaultInput.ini` 中添加：

```ini
[/Script/Engine.InputSettings]
+ActionMappings=(ActionName="OpenChat",bShift=False,bCtrl=False,bAlt=False,bCmd=False,Key=Enter)
+ActionMappings=(ActionName="OpenTeamChat",bShift=False,bCtrl=False,bAlt=False,bCmd=False,Key=T)
+ActionMappings=(ActionName="SendMessage",bShift=False,bCtrl=False,bAlt=False,bCmd=False,Key=Enter)
```

### 2. 富文本样式配置

创建富文本样式数据表，包含以下样式：

| 样式名称 | 用途 | 建议颜色 | 字体大小 |
|---------|------|----------|----------|
| `RichTextBlock.TimeStyle` | 时间显示 | #808080 | 12 |
| `RichTextBlock.TeamChatStyle` | 队伍聊天 | #4A90E2 | 14 |
| `RichTextBlock.AllChatStyle` | 全体聊天 | #FFFFFF | 14 |
| `RichTextBlock.SelfChatStyle` | 自己消息 | #7ED321 | 14 |
| `RichTextBlock.EnemyChatStyle` | 敌方消息 | #D0021B | 14 |
| `RichTextBlock.DefaultStyle` | 默认样式 | #FFFFFF | 14 |

### 3. 富文本样式配置

1. **控件绑定**：
   - `ChatScrollBox`: 消息列表容器
   - `ChatInputBox`: 文本输入框
   - `SendButton`: 发送按钮
   - `ChannelComboBox`: 频道选择下拉框

2. **布局配置**：
   - 设置适当的边距和间距
   - 配置滚动框的自动滚动行为
   - 确保输入框获得正确的键盘焦点

3. **动画设置**：
   - 添加显示/隐藏动画
   - 配置临时消息的淡入淡出效果

### ChatMessageItemWidget 配置

1. **富文本设置**：
   - 启用 `Auto Wrap Text`
   - 设置 `Wrap Text At` 为 0.0
   - 绑定富文本样式数据表

2. **弹幕配置**：
   - 移动速度：250 像素/秒
   - 持续时间：8 秒
   - 更新频率：30 FPS

## 🌐 网络配置

### RPC 函数实现

#### 客户端到服务器
```cpp
UFUNCTION(Server, Reliable, WithValidation)
void Server_SendChatMessage(const FString& Message, EChatChannelType ChannelType);
```

#### 服务器到客户端
```cpp
UFUNCTION(Client, Reliable)
void Client_ReceiveChatMessage(const FChatMessage& Message);
```

### 网络优化

1. **消息长度限制**: 最大256字符
2. **频率限制**: 每秒最多5条消息
3. **内容过滤**: 自动过滤特殊字符
4. **可靠传输**: 聊天消息使用可靠RPC确保送达

## 💡 最佳实践

### 1. 性能优化

```cpp
// 使用对象池避免频繁创建销毁
TArray<UChatMessageItemWidget*> MessageWidgetPool;

// 限制聊天历史记录数量
static constexpr int32 MAX_CHAT_MESSAGES = 100;

// 弹幕使用30FPS降低CPU消耗
static constexpr float BARRAGE_UPDATE_RATE = 1.0f / 30.0f;
```

### 2. 用户体验

```cpp
// 自动滚动到最新消息
void UChatWidget::ScrollToBottom()
{
    if (ChatScrollBox)
    {
        ChatScrollBox->ScrollToEnd();
    }
}

// 智能输入焦点管理
void UChatWidget::FocusChatInput()
{
    if (ChatInputBox)
    {
        ChatInputBox->SetKeyboardFocus();
    }
}
```

### 3. 错误处理

```cpp
// 消息内容验证
bool ValidateMessage(const FString& Message)
{
    return !Message.IsEmpty() && 
           Message.Len() <= 256 && 
           !Message.Contains(TEXT("<script>"));
}

// 网络断开处理
void OnNetworkDisconnected()
{
    // 显示离线提示
    // 禁用消息发送
}
```

## 🔍 调试指南

### 日志类别

```cpp
DECLARE_LOG_CATEGORY_EXTERN(LogCrunchChat, Log, All);
DEFINE_LOG_CATEGORY(LogCrunchChat);

// 使用示例
UE_LOG(LogCrunchChat, Log, TEXT("Message sent: %s"), *Message);
UE_LOG(LogCrunchChat, Warning, TEXT("Invalid channel type: %d"), (int32)ChannelType);
```

### 调试命令

在控制台中使用以下命令进行调试：

```
# 显示聊天统计信息
chat.showstats

# 测试发送消息
chat.test "测试消息" team

# 切换聊天日志
chat.enablelog true

# 清空聊天历史
chat.clear
```

### 常见问题排查

1. **消息不显示**：
   - 检查 `ChatMessageItemClass` 是否正确设置
   - 验证富文本样式数据表是否正确绑定
   - 确认网络连接状态

2. **弹幕不移动**：
   - 检查定时器是否正确启动
   - 验证 `BarragePanel` 是否有效
   - 确认弹幕速度设置

3. **频道切换失效**：
   - 检查游戏模式限制
   - 验证玩家团队ID设置
   - 确认权限检查逻辑

## 📈 扩展功能

### 表情系统
```cpp
// 表情数据结构
USTRUCT(BlueprintType)
struct FEmojiData
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString EmojiCode;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UTexture2D* EmojiIcon;
};
```

### 私聊功能
```cpp
// 私聊消息结构
USTRUCT(BlueprintType)
struct FPrivateMessage : public FChatMessage
{
    GENERATED_BODY()
    
    UPROPERTY(BlueprintReadOnly)
    FString TargetPlayerName;
};
```

### 消息过滤
```cpp
// 敏感词过滤
class CRUNCH_API UChatFilter : public UObject
{
public:
    UFUNCTION(BlueprintCallable)
    FString FilterMessage(const FString& RawMessage);
    
private:
    TArray<FString> BannedWords;
};
```

## 🎯 未来规划

1. **语音聊天集成**: 支持实时语音通信
2. **跨平台兼容**: 支持PC、移动端聊天互通
3. **AI聊天助手**: 智能客服和游戏提示
4. **国际化支持**: 多语言聊天界面
5. **云端聊天记录**: 跨设备同步聊天历史

---

> 🎮 **Crunch聊天系统** - 让每一次交流都更加精彩！
> 
> 📝 文档持续更新中，欢迎反馈和建议！