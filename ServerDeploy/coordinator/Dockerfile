# 使用官方提供的轻量级 Python 3.9 镜像作为基础镜像(因为3.9比较稳定)
# FROM python:3.9-slim
FROM swr.cn-north-4.myhuaweicloud.com/ddn-k8s/docker.io/python:3.9-slim

# 更新 apt 包索引
RUN apt-get update

# 安装 curl 工具，用于后续下载脚本
RUN apt-get install -y curl

# 使用官方 Docker 安装脚本在容器里安装 Docker (dind 场景下可能用)
# RUN curl -fsSL https://get.docker.com | sh
RUN curl -fsSL https://get.docker.com | sh -s docker --mirror Aliyun


# 将当前目录下的所有文件复制到容器的根目录 /
COPY . /

# 设置容器的工作目录为 /coordinator
WORKDIR /coordinator

# 安装 Flask Web 框架 (用于运行 Python Web 服务)
RUN pip3 install flask

# 设置容器启动命令，运行 coordinator.py
ENTRYPOINT [ "python", "coordinator.py"]