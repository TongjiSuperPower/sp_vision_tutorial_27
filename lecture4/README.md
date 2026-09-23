# Hello ROS2 · 第四讲（导航方向）命令速查

SuperPower 战队算法组培训第四讲配套命令手册，对应课件《Lecture4_nav Hello ROS2》。按课件的三部分组织：**ROS2 基础 → 四大通信机制 → ROS 常用工具**。

- ROS 发行版：**ROS 2 Humble**
- 构建工具：colcon
- 语言：C++（`rclcpp`）与 Python（`rclpy`）混用
- 演示源码：本仓库 `src/` 下的 `ep1` ~ `ep3`

---

## 0. 环境准备

```bash
source /opt/ros/humble/setup.bash   # 若 ~/.bashrc 已写入则可省略
printenv ROS_DISTRO                  # 应输出 humble
which colcon                        # 应输出 /usr/bin/colcon
```

每次改完代码后，标准三步走：

```bash
cd nav_lecture4
colcon build               # 编译
source install/setup.bash  # 让当前终端能找到编译好的可执行文件
```

---

## 1. ROS2 基础

### 1.1 工作空间与功能包

工作空间（workspace）是存放项目代码的文件夹，含 `src/ install/ build/ log/` 四个子目录；功能包（package）是 `src/` 下实现单个功能的文件夹。

| 目录 | 作用 |
|------|------|
| `src/` | 源码空间，手写代码放这里 |
| `build/` | 编译空间，中间产物 |
| `install/` | 安装空间，编译后的成品，运行时用它 |
| `log/` | 日志空间 |

创建功能包（C++ 用 ament_cmake，Python 用 ament_python）：

```bash
ros2 pkg create --build-type ament_cmake learning_pkg_c
ros2 pkg create --build-type ament_python learning_pkg_python
```

> 判断一个文件夹算不算功能包：看它有没有 `package.xml`（colcon 只认含 `package.xml` 的目录）。

编译工作空间：

```bash
cd <工作空间>
colcon build
```

### 1.2 节点（Node）

节点只负责一个单独的模块化功能。查找并运行节点的命令：

```bash
colcon build                        # 编译当前工作空间
colcon list                         # 查看工作空间有哪些功能包
ros2 pkg executables package_name   # 查看某功能包有哪些可执行文件
ros2 run package_name executables_name   # 运行一个节点
ros2 node list                      # 查看正在运行的节点（需另开终端）
```

最小节点示例：

```bash
ros2 run learning_node hello_world_node
```

预期输出：

```
[INFO] [hello_world_node]: hello, world
[INFO] [hello_world_node]: hello, world
...
```

### 1.3 命令速查（第一部分小结）

```bash
# 创建
ros2 pkg create --build-type ament_cmake learning_pkg_c
ros2 pkg create --build-type ament_python learning_pkg_py
# 查找
colcon list
ros2 pkg executables package_name
# 运行
ros2 run learning_node hello_world_node
ros2 node list   # 另开终端
```

---

## 2. 四大通信机制

四大通信机制：**话题（topics）、服务（services）、参数（parameters）、动作（action）**。

### 2.1 话题（Topic）—— 节点传输数据的桥梁

发布/订阅模型，异步通信，发布者和订阅者彼此无要求，用 `.msg` 定义数据类型。

```bash
ros2 topic pub /chatter std_msgs/String 'data: Hello World'   # 命令行直接发布
ros2 topic list                                              # 查看所有话题
ros2 topic echo /topic_learning                              # 打印话题内容
```

演示话题通信（先开订阅者，再开发布者）：

```bash
# 终端 A：订阅者
ros2 run topic_learning_sub subscriber_node
# 终端 B：发布者
ros2 run topic_learning_pub publisher_node
```

订阅者终端持续打印：

```
[INFO] [subscriber_node]: Received message: "I'm publisher: 0"
[INFO] [subscriber_node]: Received message: "I'm publisher: 1"
...
```

### 2.2 QoS（Quality of Service）

QoS 是发布/订阅的传输策略，**发布者和订阅者的 QoS 必须兼容才能通信**。

运行带 QoS 配置的示例：

```bash
ros2 pkg executables topic_learning_pub      # 查看包内可执行文件
ros2 run topic_learning_pub qos_publisher    # 终端 B
ros2 run topic_learning_sub qos_subscriber   # 终端 A
```

可配置项：

| 配置 | 选项 | 含义 |
|------|------|------|
| History | `Keep last` | 只缓存最新 N 个，N 由 Depth 决定 |
| | `Keep all` | 缓存所有数据，受 DDS 底层资源限制 |
| Depth / Queue size | 整数 | 仅 History=Keep last 时有效 |
| Reliability | `Best effort` | 尽力传送，网络不稳可能丢数据 |
| | `Reliable` | 确保送达，可能重传导致延时 |

> 常见坑：`reliable` 对 `reliable` 或 `best_effort` 对 `best_effort` 才匹配，配错则消息收不到。

### 2.3 服务（Service）—— 节点的你问我答

服务/客户端模型，**同步**通信，服务端唯一、客户端不唯一，用 `.srv` 定义请求和应答。

```bash
source install/setup.bash
ros2 run <package_name> <node_name>
# 演示：先启动服务端，再另开终端启动客户端
ros2 run srv_learning add_two_ints_service
ros2 run srv_learning add_two_ints_client   # 带参数，例如 2 3
```

### 2.4 动作（Action）—— 完整的行为流程管理

三大组成部分：**目标（goal）、反馈（feedback）、结果（result）**。用 `.action` 定义。

```bash
ros2 action -h
ros2 action list -t
ros2 interface show turtlesim/action/RotateAbsolute
ros2 run turtlesim turtlesim_node
ros2 action send_goal /turtle1/rotate_absolute turtlesim/action/RotateAbsolute '{theta: 90}'
```

> 注意：`RotateAbsolute` 的 `theta` 单位是**弧度**，90° 应写 `1.57`（课件里写 90 仅为示意）。

### 2.5 参数（Parameter）—— 机器人的全局字典

参数是节点的配置值，可理解为节点的设置。ROS2 自带的小乌龟节点是练习参数的好例子：

```bash
ros2 run turtlesim turtlesim_node       # 终端 A
ros2 run turtlesim turtle_teleop_key    # 终端 B（键盘控制）
ros2 param set turtlesim background_b 155   # 改背景蓝色分量
```

---

## 3. ROS 常用工具

### 3.1 接口（Interface）

接口定义了消息/服务/动作的数据结构。

```bash
ros2 interface -h
ros2 interface show std_msgs/msg/String
```

> 复制课件里的 `ros2 topic pub /chatter std_msgs/msg/String 'data: hello,world'` 到终端跑不了，通常是**中文引号**问题，记得改成英文单引号 `'`。

### 3.2 从零跑通一个 ROS 包（目录结构）

```
my_pkg/
├── config/            # 参数文件
│   └── config.yaml
├── include/           # .hpp 头文件
│   └── xxx.hpp
├── src/               # .cpp 源文件
│   └── xxx.cpp
├── CMakeLists.txt     # 编译说明书（重要！）
└── package.xml        # 包的信息 + 依赖清单（重要！）
```

两种方式使用参数文件：

```bash
# 方式一：代码内用 yaml-cpp 读取（略）
# 方式二：运行时传入参数文件
ros2 run <pkg> <node> --ros-args --params-file ./config/params.yaml
# 或直接传单个参数
ros2 run <pkg> <node> --ros-args -p param:=4.0
```

### 3.3 package.xml —— 包的“身份证”

依赖清单 5 类标签：

| 标签 | 什么时候需要 |
|------|-------------|
| `<buildtool_depend>` | 只有编译时 · 构建工具 |
| `<depend>` | 编译 + 运行 |
| `<build_depend>` | 只有编译时 · 头文件/消息 |
| `<exec_depend>` | 只有运行时 |
| `<test_depend>` | 只有测试时 · lint/gtest |

### 3.4 CMakeLists.txt —— 编译说明书

关键点（详见课件第 25 页）：
- `find_package` 声明依赖、`ament_target_dependencies` 链接依赖
- 添加可执行文件 `add_executable`
- 安装 launch 目录（下面 launch 一节有）

### 3.5 C++ 代码固定三段式

```cpp
int main(int argc, char **argv) {
    rclcpp::init(argc, argv);           // ① 初始化 ROS 环境
    auto node = std::make_shared<TurtleCircle>();  // 建节点
    rclcpp::spin(node);                 // ② 自旋：交出控制权，跑回调
    rclcpp::shutdown();                 // ③ 退出，释放资源
    return 0;
}
```

常用写法：`Xxx::SharedPtr` 就是 `std::shared_ptr<Xxx>` 的别名；`create_publisher` / `create_subscription` / `create_wall_timer` 都返回 SharedPtr，回调用 `std::bind` 或 lambda 注册。

### 3.6 colcon build —— 编译

colcon 干的事：`src/` 里的包 → 按依赖拓扑排序 → 逐包调 CMake → 产出到 `install/`。

| 命令 | 用途 |
|------|------|
| `colcon build` | 编译工作空间里所有包 |
| `colcon build --packages-select ros2_pkg` | 只编这个包 |
| `colcon build --packages-up-to ros2_pkg` | 连它的依赖一起编 |
| `colcon build --symlink-install` | launch/config 改动免重编 |
| `colcon build --cmake-clean-cache` | 改过 CMakeLists 后清缓存 |
| `colcon list` | 看工作空间里有哪些包 |

> colcon 只认有 `package.xml` 的目录。包没被编译？先确认它在 `colcon list` 的输出里。

### 3.7 ros2 run —— 运行

`build` 只是把东西放进 `install/`，`source` 才让它生效：

```bash
cd ~/ros2_ws
colcon build --packages-select ros2_pkg
source install/setup.bash
ros2 run ros2_pkg main
```

### 3.8 ros2 launch —— 一次启动多个节点

launch 文件就是一个 Python 脚本，描述「要启动哪些节点、给谁传什么参数」。

```python
# main.launch.py —— 放在包的 launch/ 目录下
from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():   # ← 函数名固定
    return LaunchDescription([
        Node(package='turtlesim', executable='turtlesim_node'),
        Node(package='ros2_pkg', executable='main',
             parameters=[{'linear_speed': 2.0}]),
    ])
```

```bash
# 运行：一次把上面两个节点全拉起来
ros2 launch ros2_pkg main.launch.py
# 看这个 launch 文件能传哪些参数
ros2 launch ros2_pkg main.launch.py --show-args
```

三条注意：

1. `generate_launch_description()` 函数名不能改
2. 改了 launch 文件要重新 `colcon build`（或加 `--symlink-install`）
3. CMakeLists.txt 里别忘了装 launch 目录：
   ```cmake
   install(DIRECTORY launch DESTINATION share/${PROJECT_NAME})
   ```

本仓库示例：

```bash
ros2 launch launch_learning basic.launch.py
```

launch 的价值：一条命令拉起整个系统（仿真 + 定位 + 控制 + 你的节点），不用开一堆终端逐个 `ros2 run`。

### 3.9 rosbag —— 录包与回放

把话题数据录下来，随时回放复现。

| 命令 | 作用 |
|------|------|
| `ros2 bag record /turtle1/cmd_vel -o demo` | 录指定话题到 demo/ |
| `ros2 bag record -a -o demo` | 录所有话题 |
| `ros2 bag info demo` | 看录了多久、多少条消息 |
| `ros2 bag play demo` | 原速回放 |
| `ros2 bag play demo --loop -r 2.0` | 循环 + 2 倍速 |

> - 录下来的是「一个目录」：`metadata.yaml` + `.db3`；存的是**消息流**，不是节点状态——参数、TF 树要单独考虑。
> - **结束录制一定要用 `Ctrl+C` 退出**，直接关终端会缺失 `metadata.yaml`，即使恢复也容易丢部分话题数据。
> - 查看 bag 的 QoS：见 `metadata.yaml` 里每个 topic 的 `offered_qos_profiles`。

---

## 4. 参考文档

- [小鱼动手学 ROS2](https://fishros.com/d2lros2) —— 教程详细
- [rclcpp API 文档](https://docs.ros2.org/latest/api/rclcpp/)
- [通信质量 QoS 配置指南](https://mp.weixin.qq.com/s/J63fO4c_QIseLGQd5W2fAw)
