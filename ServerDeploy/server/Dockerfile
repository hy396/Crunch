# 使用一个小巧的 Linux 镜像，不用 root 用户
# FROM gcr.io/distroless/cc-debian10:nonroot
# FROM gcr.io/distroless/cc-debian12:nonroot
FROM swr.cn-north-4.myhuaweicloud.com/ddn-k8s/gcr.io/distroless/cc-debian12:nonroot

# 把本地 server 文件夹复制到镜像里
COPY --chown=nonroot:nonroot ./server /home/nonroot/server

# 容器启动时运行 CrunchServer 程序
ENTRYPOINT [ "/home/nonroot/server/Crunch/Binaries/Linux/CrunchServer", "Crunch"]
