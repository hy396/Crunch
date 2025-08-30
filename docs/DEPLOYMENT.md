# Crunch 部署指南

## 🚀 部署概览

Crunch支持多种部署方式，从开发测试到生产环境都有相应的解决方案。本指南将详细介绍各种部署选项。

## 🐳 Docker 部署（推荐）

### 环境要求

- Docker Engine 20.10+
- Docker Compose 2.0+
- 至少 4GB 可用内存
- 至少 20GB 可用磁盘空间

### 快速部署

1. **克隆仓库**
```bash
git clone <repository-url>
cd Crunch
```

2. **构建Docker镜像**
```bash
# 构建游戏服务器镜像
cd ServerDeploy/server
docker build -t crunch-server:latest .

# 构建协调器镜像
cd ../coordinator
docker build -t crunch-coordinator:latest .
```

3. **使用Docker Compose启动**
```bash
# 回到项目根目录
cd ../..

# 启动所有服务
docker-compose up -d
```

### Docker Compose 配置

创建 `docker-compose.yml` 文件：

```yaml
version: '3.8'

services:
  coordinator:
    image: crunch-coordinator:latest
    ports:
      - "8080:8080"
    environment:
      - COORDINATOR_PORT=8080
      - MAX_SERVERS=10
    volumes:
      - ./logs:/app/logs
    restart: unless-stopped
    networks:
      - crunch-network

  game-server-1:
    image: crunch-server:latest
    ports:
      - "7777:7777/udp"
      - "7778:7778/tcp"
    environment:
      - SERVER_NAME=CrunchServer1
      - MAX_PLAYERS=16
      - COORDINATOR_URL=http://coordinator:8080
    depends_on:
      - coordinator
    volumes:
      - ./server-logs:/app/logs
    restart: unless-stopped
    networks:
      - crunch-network

  game-server-2:
    image: crunch-server:latest
    ports:
      - "7779:7777/udp"
      - "7780:7778/tcp"
    environment:
      - SERVER_NAME=CrunchServer2
      - MAX_PLAYERS=16
      - COORDINATOR_URL=http://coordinator:8080
    depends_on:
      - coordinator
    volumes:
      - ./server-logs:/app/logs
    restart: unless-stopped
    networks:
      - crunch-network

networks:
  crunch-network:
    driver: bridge

volumes:
  server-logs:
  coordinator-logs:
```

### Dockerfile 详解

#### 游戏服务器 Dockerfile

```dockerfile
# ServerDeploy/server/Dockerfile
FROM ubuntu:22.04

# 安装必要的依赖
RUN apt-get update && apt-get install -y \
    wget \
    unzip \
    libssl3 \
    libcurl4 \
    && rm -rf /var/lib/apt/lists/*

# 创建应用目录
WORKDIR /app

# 复制游戏服务器文件
COPY ./CrunchServer /app/CrunchServer
COPY ./Content /app/Content
COPY ./Config /app/Config

# 设置可执行权限
RUN chmod +x /app/CrunchServer

# 暴露端口
EXPOSE 7777/udp 7778/tcp

# 启动命令
CMD ["./CrunchServer", "-log"]
```

#### 协调器 Dockerfile

```dockerfile
# ServerDeploy/coordinator/Dockerfile
FROM python:3.9-slim

# 设置工作目录
WORKDIR /app

# 复制requirements文件
COPY requirements.txt .

# 安装Python依赖
RUN pip install --no-cache-dir -r requirements.txt

# 复制应用代码
COPY ./coordinator ./coordinator

# 暴露端口
EXPOSE 8080

# 启动命令
CMD ["python", "-m", "coordinator.coordinator"]
```

## 🖥️ 传统服务器部署

### Windows 服务器部署

#### 环境要求
- Windows Server 2019/2022
- Visual C++ Redistributable 2022
- .NET Framework 4.8+
- 至少 8GB RAM
- 至少 50GB 可用磁盘空间

#### 部署步骤

1. **打包游戏**
```bash
# 在开发机器上使用UE编辑器打包
# File -> Package Project -> Windows (64-bit)
# 配置：Shipping
# 目标：Server
```

2. **上传服务器文件**
```powershell
# 创建服务器目录
New-Item -Path "C:\CrunchServer" -ItemType Directory

# 复制打包文件到服务器
Copy-Item -Path ".\PackagedGame\*" -Destination "C:\CrunchServer\" -Recurse
```

3. **配置防火墙**
```powershell
# 开放游戏端口
New-NetFirewallRule -DisplayName "Crunch Game Port" -Direction Inbound -Protocol UDP -LocalPort 7777
New-NetFirewallRule -DisplayName "Crunch Query Port" -Direction Inbound -Protocol TCP -LocalPort 7778
```

4. **创建启动脚本**
```batch
@echo off
cd /d "C:\CrunchServer"
CrunchServer.exe -log -stdout
pause
```

5. **创建Windows服务**
```powershell
# 使用NSSM创建服务
nssm install CrunchGameServer "C:\CrunchServer\CrunchServer.exe"
nssm set CrunchGameServer Parameters "-log -stdout"
nssm set CrunchGameServer Start SERVICE_AUTO_START
```

### Linux 服务器部署

#### 环境要求
- Ubuntu 20.04+ / CentOS 8+
- 至少 4GB RAM
- 至少 20GB 可用磁盘空间

#### 部署步骤

1. **准备服务器环境**
```bash
# 更新系统
sudo apt update && sudo apt upgrade -y

# 安装必要依赖
sudo apt install -y wget unzip screen htop
```

2. **创建游戏用户**
```bash
# 创建专用用户
sudo useradd -m -s /bin/bash crunchgame
sudo mkdir /opt/crunchserver
sudo chown crunchgame:crunchgame /opt/crunchserver
```

3. **上传并解压游戏文件**
```bash
# 切换到游戏用户
sudo su - crunchgame

# 解压游戏文件
cd /opt/crunchserver
unzip CrunchServer-Linux.zip
chmod +x CrunchServer
```

4. **创建systemd服务**
```bash
# 创建服务文件
sudo tee /etc/systemd/system/crunchserver.service > /dev/null <<EOF
[Unit]
Description=Crunch Game Server
After=network.target

[Service]
Type=simple
User=crunchgame
WorkingDirectory=/opt/crunchserver
ExecStart=/opt/crunchserver/CrunchServer -log
Restart=always
RestartSec=10

[Install]
WantedBy=multi-user.target
EOF

# 启用并启动服务
sudo systemctl daemon-reload
sudo systemctl enable crunchserver
sudo systemctl start crunchserver
```

5. **配置防火墙**
```bash
# UFW配置
sudo ufw allow 7777/udp
sudo ufw allow 7778/tcp
sudo ufw reload

# 或者使用iptables
sudo iptables -A INPUT -p udp --dport 7777 -j ACCEPT
sudo iptables -A INPUT -p tcp --dport 7778 -j ACCEPT
```

## ☁️ 云平台部署

### AWS 部署

#### EC2 实例配置

1. **创建EC2实例**
```bash
# 使用AWS CLI创建实例
aws ec2 run-instances \
    --image-id ami-0c02fb55956c7d316 \
    --count 1 \
    --instance-type t3.medium \
    --key-name your-key-pair \
    --security-group-ids sg-xxxxxxxxx \
    --subnet-id subnet-xxxxxxxxx \
    --user-data file://user-data.sh
```

2. **用户数据脚本**
```bash
#!/bin/bash
# user-data.sh
yum update -y
yum install -y docker
systemctl start docker
systemctl enable docker

# 安装Docker Compose
curl -L "https://github.com/docker/compose/releases/download/2.20.0/docker-compose-$(uname -s)-$(uname -m)" -o /usr/local/bin/docker-compose
chmod +x /usr/local/bin/docker-compose

# 克隆项目并启动
git clone <your-repo> /opt/crunch
cd /opt/crunch
docker-compose up -d
```

#### ECS 部署

1. **创建任务定义**
```json
{
  "family": "crunch-server",
  "networkMode": "awsvpc",
  "requiresCompatibilities": ["FARGATE"],
  "cpu": "1024",
  "memory": "2048",
  "executionRoleArn": "arn:aws:iam::account:role/ecsTaskExecutionRole",
  "containerDefinitions": [
    {
      "name": "crunch-server",
      "image": "your-ecr-repo/crunch-server:latest",
      "portMappings": [
        {
          "containerPort": 7777,
          "protocol": "udp"
        },
        {
          "containerPort": 7778,
          "protocol": "tcp"
        }
      ],
      "logConfiguration": {
        "logDriver": "awslogs",
        "options": {
          "awslogs-group": "/ecs/crunch-server",
          "awslogs-region": "us-west-2",
          "awslogs-stream-prefix": "ecs"
        }
      }
    }
  ]
}
```

### Google Cloud Platform 部署

#### GKE 部署

1. **创建Kubernetes配置**
```yaml
# k8s-deployment.yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: crunch-server
spec:
  replicas: 2
  selector:
    matchLabels:
      app: crunch-server
  template:
    metadata:
      labels:
        app: crunch-server
    spec:
      containers:
      - name: crunch-server
        image: gcr.io/your-project/crunch-server:latest
        ports:
        - containerPort: 7777
          protocol: UDP
        - containerPort: 7778
          protocol: TCP
        resources:
          requests:
            memory: "1Gi"
            cpu: "500m"
          limits:
            memory: "2Gi"
            cpu: "1000m"
---
apiVersion: v1
kind: Service
metadata:
  name: crunch-server-service
spec:
  type: LoadBalancer
  ports:
  - port: 7777
    targetPort: 7777
    protocol: UDP
    name: game
  - port: 7778
    targetPort: 7778
    protocol: TCP
    name: query
  selector:
    app: crunch-server
```

## 📊 监控与日志

### 应用监控

#### Prometheus + Grafana

1. **添加监控到Docker Compose**
```yaml
# 在docker-compose.yml中添加
  prometheus:
    image: prom/prometheus:latest
    ports:
      - "9090:9090"
    volumes:
      - ./monitoring/prometheus.yml:/etc/prometheus/prometheus.yml
    networks:
      - crunch-network

  grafana:
    image: grafana/grafana:latest
    ports:
      - "3000:3000"
    environment:
      - GF_SECURITY_ADMIN_PASSWORD=admin
    volumes:
      - grafana-storage:/var/lib/grafana
    networks:
      - crunch-network
```

2. **Prometheus配置**
```yaml
# monitoring/prometheus.yml
global:
  scrape_interval: 15s

scrape_configs:
  - job_name: 'crunch-coordinator'
    static_configs:
      - targets: ['coordinator:8080']
  
  - job_name: 'crunch-servers'
    static_configs:
      - targets: ['game-server-1:7778', 'game-server-2:7778']
```

### 日志管理

#### ELK Stack

1. **添加到Docker Compose**
```yaml
  elasticsearch:
    image: docker.elastic.co/elasticsearch/elasticsearch:8.8.0
    environment:
      - discovery.type=single-node
      - "ES_JAVA_OPTS=-Xms512m -Xmx512m"
    ports:
      - "9200:9200"
    networks:
      - crunch-network

  kibana:
    image: docker.elastic.co/kibana/kibana:8.8.0
    ports:
      - "5601:5601"
    environment:
      - ELASTICSEARCH_HOSTS=http://elasticsearch:9200
    depends_on:
      - elasticsearch
    networks:
      - crunch-network

  logstash:
    image: docker.elastic.co/logstash/logstash:8.8.0
    volumes:
      - ./monitoring/logstash.conf:/usr/share/logstash/pipeline/logstash.conf
    depends_on:
      - elasticsearch
    networks:
      - crunch-network
```

## 🔧 故障排除

### 常见问题

#### 服务器无法启动
```bash
# 检查端口是否被占用
netstat -tulpn | grep :7777

# 检查防火墙状态
sudo ufw status

# 查看服务器日志
sudo journalctl -u crunchserver -f
```

#### 客户端连接失败
```bash
# 测试端口连通性
telnet server-ip 7778

# 检查网络配置
ping server-ip

# 查看服务器负载
htop
```

#### 性能问题
```bash
# 监控系统资源
top
iostat -x 1
free -h

# 检查网络延迟
ping -c 10 client-ip
```

### 日志位置

- **Windows**: `C:\CrunchServer\Logs\`
- **Linux**: `/opt/crunchserver/Logs/`
- **Docker**: 容器内 `/app/logs/`

### 性能调优

#### 服务器配置优化
```ini
# Config/DefaultEngine.ini
[/Script/Engine.GameSession]
MaxPlayers=16

[/Script/OnlineSubsystemUtils.IpNetDriver]
NetServerMaxTickRate=60
LanServerMaxTickRate=120

[/Script/Engine.Player]
ConfiguredInternetSpeed=25000
ConfiguredLanSpeed=50000
```

#### 系统级优化
```bash
# Linux系统优化
echo 'net.core.rmem_max = 134217728' >> /etc/sysctl.conf
echo 'net.core.wmem_max = 134217728' >> /etc/sysctl.conf
echo 'net.ipv4.udp_mem = 102400 873800 16777216' >> /etc/sysctl.conf
sysctl -p
```

## 🔄 自动化部署

### CI/CD Pipeline

#### GitHub Actions 示例
```yaml
# .github/workflows/deploy.yml
name: Deploy Crunch Server

on:
  push:
    branches: [ main ]

jobs:
  build-and-deploy:
    runs-on: ubuntu-latest
    
    steps:
    - uses: actions/checkout@v3
    
    - name: Build Docker images
      run: |
        docker build -t crunch-server:${{ github.sha }} ./ServerDeploy/server
        docker build -t crunch-coordinator:${{ github.sha }} ./ServerDeploy/coordinator
    
    - name: Deploy to production
      run: |
        # 部署脚本
        ./scripts/deploy.sh ${{ github.sha }}
```

这个部署指南涵盖了从开发环境到生产环境的完整部署流程，为不同规模和需求的部署提供了详细的指导。