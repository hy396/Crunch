关于GitHub Cli的安装参考https://github.com/cli/cli#installation

明白了！以下是**精简后的笔记版本**，仅保留你成功执行的操作，并按照你之前使用的 Markdown 笔记结构规范整理：

---

# Git 初始化与 `.gitignore` 创建记录

## 步骤 1: 初始化 Git 仓库

```powershell
git init
```


**结果：**
成功初始化空 Git 仓库：

```
Initialized empty Git repository in D:/ue_texiao/Crunch/.git/
```


## 步骤 2: 创建 `.gitignore` 文件

```powershell
New-Item .gitignore
```


**结果：**
创建了一个空的 `.gitignore` 文件。

```
    目录: D:\ue_texiao\Crunch

Mode                 LastWriteTime         Length Name
----                 -------------         ------ ----
-a----         2025/6/29     22:45              0 .gitignore
```


## 步骤 3: 使用 JetBrains Rider 打开 `.gitignore`

```powershell
rider .gitignore
```


**结果：**
成功调用 JetBrains Rider 并打开 `.gitignore` 文件进行编辑。

> 提示：如果你尚未配置 Rider 的命令行支持，可进入 `Tools > Create Command-line Launcher` 启用后重试。

---

### 注意事项

- `.gitignore` 应放在项目根目录下。
- 如果某些文件已经提交但后来被加入 `.gitignore`，仍会被 Git 跟踪，需使用以下命令取消跟踪：
  ```bash
  git rm --cached <file-name>
  ```


---
