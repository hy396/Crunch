# 导入 Flask 框架中的 Flask 类、request（处理请求）、jsonify（返回 JSON 响应）
from flask import Flask, request, jsonify
import subprocess
from consts import SESSION_NAME_KEY, SESSION_SEARCH_ID_KEY, PORT_KEY
import re

# 创建 Flask 应用
app = Flask(__name__)

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


# 定义路由，当客户端以 POST 请求访问 /Sessions 时触发该函数
@app.route('/Sessions', methods=['POST'])
def CreateServer():
    # 打印请求头信息（调试用，可以看到客户端传过来的数据）
    print(dict(request.headers))
    # 获取请求体中的会话名称和搜索 ID
    sessionName = request.get_json().get(SESSION_NAME_KEY)
    sessionSearchId = request.get_json().get(SESSION_SEARCH_ID_KEY)
    # 创建服务器并获取分配的端口号
    port = CreateServerLocalTest(sessionName, sessionSearchId)

    # 返回 JSON 响应，其中包含状态（success）和分配的端口号
    # 状态码 200 表示请求成功
    return jsonify({"status": "success", PORT_KEY: port}), 200


# 启动 Flask Web 服务
# host="0.0.0.0" 表示允许外部访问
# port=80 表示监听 80 端口（标准 HTTP 端口）
if __name__ == "__main__":
    app.run(host="0.0.0.0", port=80)