#!/bin/bash

# 定义本地目录和容器内的目录
LOCAL_DIR="./"   # 这里替换为你希望映射的本地目录
CONTAINER_DIR="/app/cppFix"          # 这里是容器内的目录
CONTAINER_NAME="cppFix"

# 你的公钥路径（假设你在本地有一个公钥文件 id_rsa.pub）
PUBLIC_KEY_PATH="$HOME/.ssh/id_rsa.pub"
PUBLIC_KEY_CONTENT=$(cat "$PUBLIC_KEY_PATH")

# 检查容器是否已经存在
if docker ps -a --format '{{.Names}}' | grep -Eq "^${CONTAINER_NAME}$"; then
  echo "Container '$CONTAINER_NAME' already exists. Entering the container..."

  # 如果容器存在，直接进入容器的 bash shell
  docker exec -u root -it "$CONTAINER_NAME" /bin/bash
else
  echo "Container '$CONTAINER_NAME' does not exist. Creating and starting the container..."

  # 创建并启动容器
  docker run -d \
    --name "$CONTAINER_NAME" \
    -v "$LOCAL_DIR":"$CONTAINER_DIR" \
    -p 666:22 \
    debian:11 \
    /bin/bash -c "tail -f /dev/null"

  echo "Container '$CONTAINER_NAME' created and started successfully!"

  # 等待容器启动并安装 build-essential 和 cmake
  echo "Installing 'build-essential', 'cmake', 'tmux', 'openssh-server'..."

  # 在容器内安装 build-essential 和 cmake
  docker exec -u root "$CONTAINER_NAME" /bin/bash -c "\
    apt-get update && \
    apt-get install -y build-essential cmake tmux openssh-server vim rsync && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*"

  echo "'build-essential', 'cmake', 'rsync', 'vim', 'tmux', 'openssh-server' installed successfully!"

  # 确保容器内有 .ssh 目录并且设置正确权限
  docker exec -u root "$CONTAINER_NAME" /bin/bash -c "\
    mkdir -p /root/.ssh && \
    chmod 700 /root/.ssh"

  # 将本地的公钥复制到容器内的 authorized_keys
  echo "Adding public key to the container..."

  docker exec -u root "$CONTAINER_NAME" /bin/bash -c "\
    echo '$PUBLIC_KEY_CONTENT' > /root/.ssh/authorized_keys"

  # 设置权限
  docker exec -u root "$CONTAINER_NAME" /bin/bash -c "\
    chmod 600 /root/.ssh/authorized_keys && \
    chown root:root /root/.ssh/authorized_keys"

  echo "Public key added to '/root/.ssh/authorized_keys' inside the container!"

  # 启动 SSH 服务
  echo "Starting SSH service in the container..."

  docker exec -u root "$CONTAINER_NAME" /bin/bash -c "\
    service ssh start"

  echo "SSH service started successfully!"

  # 自动进入容器的 bash
  docker exec -u root -it "$CONTAINER_NAME" /bin/bash
fi
