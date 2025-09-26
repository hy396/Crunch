# 导入 Flask 框架中的 Flask 类、request（处理请求）、jsonify（返回 JSON 响应）
from flask import Flask, request, jsonify
import subprocess
from consts import SESSION_NAME_KEY, SESSION_SEARCH_ID_KEY, PORT_KEY
import re
import socket

# 创建 Flask 应用
app = Flask(__name__)

# 获取当前正在使用的Docker容器端口
def GetUsedPort():
    # 运行docker命令，列出所有容器的端口信息
    result = subprocess.run(['docker', 'ps', '--format', '"{{.Ports}}"'], capture_output=True, text=True)
    output = result.stdout # 获取命令输出
    usedPorts = set()

    # 遍历输出中的每一行
    for line in output.strip().split("\n"):
        # 匹配 0.0.0.0:port-> 格式的端口映射（如 0.0.0.0:8080->80/tcp）
        matches = re.findall(r'0\.0\.0\.0:(\d+)->', line)
        # 将找到的端口号添加到集合中（自动去重）
        usedPorts.update(map(int, matches))
    
    return usedPorts  # 返回已使用的端口集合

# 查找下一个可用的端口号
def FindNextAvailablePort(start = 7777 ,end = 8000):
    usedPort = GetUsedPort()        # 获取当前已使用的端口
    # 在指定范围内查找可用端口
    for port in range(start, end+1):
        if port not in usedPort:    # 如果端口未被占用
            return port             # 返回可用端口
    return 0                        # 无可用端口返回0

# 创建服务器的实现函数（使用Docker）
def CreateServerImpl(sessionName, sessionSearchId):
    port = FindNextAvailablePort()  # 查找可用端口
    # 打印启动信息
    print(f"启动服务器: {sessionName}, ID: {sessionSearchId}, 端口: {port}")

    # 使用subprocess.Popen启动Docker容器
    # 这会创建一个新的进程运行服务器，不会阻塞当前程序
    subprocess.Popen([
        "docker",                                   # Docker命令
        "run",                                      # 运行容器
        "--rm",                                     # 容器退出后自动删除
        "-p", f"{port}:{port}/tcp",                 # 映射TCP端口
        "-p", f"{port}:{port}/udp",                 # 映射UDP端口
        "server",                                   # 使用的Docker镜像名称
        "-server",                                  # 服务器模式参数
        "-log",                                     # 启用日志参数
        '-epicapp="ServerClient"',                  # 应用标识参数
        f'-SESSION_NAME="{sessionName}"',           # 会话名称参数
        f'-SESSION_SEARCH_ID="{sessionSearchId}"',  # 会话ID参数
        f'-PORT={port}'                             # 端口参数
    ])

    return port  # 返回分配的端口号



# TODO: 将来使用 Docker 时移除该变量
# 当前用作测试的可用端口（后续可以根据需求动态分配）
nextAvailablePort = 7777


# 用于本地测试时创建服务器进程
# 参数：
#   sessionName: 会话名称
#   sessionSearchId: 会话搜索ID
def CreateServerLocalTest(sessionName, sessionSearchId):
    # 使用全局变量 nextAvailablePort
    global nextAvailablePort
    # 启动一个新的进程
    subprocess.Popen([
        # UnrealEditor.exe 的路径（指定引擎可执行文件）
        "D:/UnrealSource/UnrealEngine/Engine/Binaries/Win64/UnrealEditor.exe",

        # 工程文件路径（告诉引擎要启动哪个项目）
        "D:/ue_texiao/Crunch/Crunch.uproject" ,

        # 以服务器模式运行（而不是客户端/编辑器模式）
        "-server",

        # 打开日志输出
        "-log",

        # 指定 Epic 应用 ID（可用于标识不同的运行实例）
        '-epicapp="ServerClient"',

        # 传递会话名称参数（作为命令行参数给引擎使用）
        f'-SESSION_NAME="{sessionName}"',

        # 传递会话搜索 ID 参数
        f'-SESSION_SEARCH_ID="{sessionSearchId}"',

        # 指定使用的端口号
        f'-PORT={nextAvailablePort}'
    ])
    # 记录当前使用的端口号
    usedPort = nextAvailablePort
    nextAvailablePort += 1
    # 返回当前使用的端口号
    return usedPort


# from flask import Flask, request, jsonify
# import subprocess
# from consts import SESSION_NAME_KEY, SESSION_SEARCH_ID_KEY, PORT_KEY
# import re
# import socket

# # 创建 Flask 应用
# app = Flask(__name__)

# # 用于记录已分配的端口
# allocated_ports = set()

# # 检查端口是否可用
# def is_port_available(port):
#     try:
#         with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
#             s.bind(('', port))
#             print(f"端口 {port} 绑定成功")
#             return True
#     except OSError as e:
#         print(f"端口 {port} 绑定失败: {e}")
#         return False

# # 获取当前正在使用的Docker容器端口（带错误处理）
# def GetUsedPort():
#     try:
#         # 运行docker命令，列出所有容器的端口信息
#         result = subprocess.run(['docker', 'ps', '--format', '{{.Ports}}'], capture_output=True, text=True, timeout=10)
#         if result.returncode != 0:
#             raise subprocess.CalledProcessError(result.returncode, 'docker ps')
        
#         output = result.stdout # 获取命令输出
#         usedPorts = set()

#         # 遍历输出中的每一行
#         for line in output.strip().split("\n"):
#             if line.strip():  # 确保行不为空
#                 # 匹配 0.0.0.0:port-> 格式的端口映射（如 0.0.0.0:8080->80/tcp）
#                 matches = re.findall(r'0\.0\.0\.0:(\d+)->', line)
#                 # 将找到的端口号添加到集合中（自动去重）
#                 usedPorts.update(map(int, matches))
        
#         print(f"从Docker获取到已使用端口: {usedPorts}")
#         return usedPorts  # 返回已使用的端口集合
#     except (subprocess.TimeoutExpired, FileNotFoundError, subprocess.CalledProcessError, Exception) as e:
#         print(f"获取Docker端口信息失败，使用本地端口检测: {e}")
#         # 如果 Docker 命令失败，返回空集合，使用本地端口检测
#         return set()

# # 查找下一个可用的端口号
# def FindNextAvailablePort(start = 7777 ,end = 8000):
#     print(f"开始查找可用端口，范围: {start}-{end}")

#     try:
#         usedPort = GetUsedPort()        # 获取当前已使用的端口
#     except Exception as e:
#         print(f"获取已使用端口失败: {e}")
#         usedPort = set()
    
#     # 合并已分配的端口
#     usedPort.update(allocated_ports)
#     print(f"合并已分配端口后: {usedPort}")
#     # 在指定范围内查找可用端口
#     for port in range(start, end+1):
#         print(f"检查端口 {port}...")
#         if port not in usedPort:
#             print(f"端口 {port} 不在已使用列表中，检查是否可绑定...")
#             if is_port_available(port):    # 如果端口未被占用且可用
#                 allocated_ports.add(port)  # 记录已分配的端口
#                 print(f"成功分配端口: {port}")
#                 return port             # 返回可用端口
#             else:
#                 print(f"端口 {port} 无法绑定")
#         else:
#             print(f"端口 {port} 已被使用")
    
#     print("在指定范围内未找到可用端口")
#     return 0                        # 无可用端口返回0

# # 创建服务器的实现函数（使用Docker）
# def CreateServerImpl(sessionName, sessionSearchId):
#     port = FindNextAvailablePort()  # 查找可用端口
#     if port == 0:
#         print("无可用端口，创建服务器失败")
#         return 0
    
#     # 打印启动信息
#     print(f"启动服务器: {sessionName}, ID: {sessionSearchId}, 端口: {port}")

#     try:
#         # 使用subprocess.Popen启动Docker容器
#         # 这会创建一个新的进程运行服务器，不会阻塞当前程序
#         process = subprocess.Popen([
#             "docker",                                   # Docker命令
#             "run",                                      # 运行容器
#             "--rm",                                     # 容器退出后自动删除
# #            "-d",                                       # 后台运行
#             "-p", f"{port}:{port}/tcp",                 # 映射TCP端口
#             "-p", f"{port}:{port}/udp",                 # 映射UDP端口
#             "server",                             # 使用的Docker镜像名称
#             "-server",                                  # 服务器模式参数
#             "-log",                                     # 启用日志参数
#             '-epicapp="ServerClient"',                  # 应用标识参数
#             f'-SESSION_NAME="{sessionName}"',           # 会话名称参数
#             f'-SESSION_SEARCH_ID="{sessionSearchId}"',  # 会话ID参数
#             f'-PORT={port}'                             # 端口参数
#         ])
#         print(f"Docker容器启动成功，端口: {port}")
#     except FileNotFoundError:
#         print("Docker命令未找到，请确保Docker已正确安装并且容器有权限访问Docker socket")
#         # 移除分配记录
#         allocated_ports.discard(port)
#         return 0
#     except Exception as e:
#         print(f"启动Docker容器失败: {e}")
#         # 移除分配记录
#         allocated_ports.discard(port)
#         return 0

#     return port  # 返回分配的端口号

# 定义路由，当客户端以 POST 请求访问 /Sessions 时触发该函数
@app.route('/Sessions', methods=['POST'])
def CreateServer():
    # 打印请求头信息（调试用，可以看到客户端传过来的数据）
    print(dict(request.headers))
    # 获取请求体中的会话名称和搜索 ID
    sessionName = request.get_json().get(SESSION_NAME_KEY)
    sessionSearchId = request.get_json().get(SESSION_SEARCH_ID_KEY)
    # 创建服务器并获取分配的端口号
    # port = CreateServerLocalTest(sessionName, sessionSearchId) # 测试用
    port = CreateServerImpl(sessionName, sessionSearchId)        # Docker用

    # 返回 JSON 响应，其中包含状态（success）和分配的端口号
    # 状态码 200 表示请求成功
    return jsonify({"status": "success", PORT_KEY: port}), 200


# 启动 Flask Web 服务
# host="0.0.0.0" 表示允许外部访问
# port=80 表示监听 80 端口（标准 HTTP 端口）
if __name__ == "__main__":
    app.run(host="0.0.0.0", port=80)
    # CreateServerImpl("", "")