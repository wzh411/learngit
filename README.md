# my_pkg —— RoboMaster 作业二：ROS2 电机模拟器

> 环境：WSL2 / Ubuntu 24.04 / **ROS 2 Jazzy**，工作空间 `/home/wangzihao/RMCS/rmcs_ws`
> 依赖消息包：`motor_sim_msgs`（`rmcs_ws/src/motor_sim_msgs`，`MotorState.msg` = 转速 + 角度）
> 原有节点 `publisher_sin` / `subscriber_sin` 保持不动，与本次作业的两个新节点共存。

## 0. 代码风格（按你的信息奥赛习惯写）

- `#include <bits/stdc++.h>` 万能头 + `using namespace std;` + `using namespace std::chrono_literals;`
- 全局变量、短命名：`J` `B` `TL` `Tm` `th`(角度) `w`(角速度) `u`(力矩) `t` `dt` `fp/fc`(频率) `kc`(分频比)
- 制表符缩进、`int main(){` 同行大括号、过程式写法（不套类）
- ROS2 独有 API 照官方写：`rclcpp::init` / `make_shared<rclcpp::Node>` / `create_publisher` /
  `create_subscription` / `create_wall_timer` / `publish` / `RCLCPP_INFO` / `rclcpp::spin` / `rclcpp::shutdown`
- ROS2 头文件用双引号：`#include "rclcpp/rclcpp.hpp"`

## 1. 模型

电气部分用「电流环近似」隐藏，只保留机械运动部分（牛顿第二定律旋转形式）：

```
J * dω/dt = Te - TL - B * ω
dθ/dt     = ω
```

| 符号 | 含义 | 单位 |
| --- | --- | --- |
| `θ` | 转子角度（零点自取） | rad |
| `ω` | 转子角速度，`ω = dθ/dt` | rad/s |
| `J` | 转动惯量 | kg·m² |
| `B` | 粘性阻尼系数 | N·m·s/rad |
| `Te` | 控制力矩（**控制输入**） | N·m |
| `TL` | 负载 / 外部扰动力矩 | N·m |

θ 是 ω 的积分，因此是**二阶系统**。积分默认用**定步长四阶龙格-库塔（RK4）**，另有 `integrator:=euler`
可切成前向欧拉做对比。

## 2. 节点与话题

| 节点 | 可执行文件 | 作用 |
| --- | --- | --- |
| `dianjikongzhi` | `ros2 run my_pkg dianjikongzhi` | **电机模拟器（被控对象）** |
| `motor_test` | `ros2 run my_pkg motor_test` | **输入源 / 测试信号（含可选闭环 PI）** |
| `publisher_sin` / `subscriber_sin` | 原作业一 | 保持原样 |

```
/motor/torque_cmd   std_msgs/Float64              控制力矩 Te [N*m]        motor_test -> dianjikongzhi
/motor/state        motor_sim_msgs/MotorState     转速 [rad/s] + 角度 [rad] dianjikongzhi -> motor_test
/motor/velocity_rpm std_msgs/Float64              转速 [rpm]（附带，方便直接看 rpm）
```

**启动握手**：模拟器在收到第一条力矩指令前只广播零状态、不推进仿真；`motor_test` 等收到第一帧状态
再开始发信号。这样两个节点的时间轴自动对齐（与启动先后无关），阶跃自检才准确。

## 3. 编译与运行

```bash
cd /home/wangzihao/RMCS/rmcs_ws
source /opt/ros/jazzy/setup.bash
colcon build --packages-select motor_sim_msgs my_pkg
source install/setup.bash

# 一键跑 18 s 自动测试序列（阶跃/正弦/方波/卸载/脉冲），数据写入 motor_sim_log.csv
ros2 launch my_pkg motor_sim.launch.py

# 出图（作业报告用）
cd ~/rm_homework
python3 ~/RMCS/rmcs_ws/src/my_pkg/scripts/plot_motor_state.py --csv motor_sim_log.csv --out motor_sim_result.png
```

常用覆盖：

```bash
ros2 launch my_pkg motor_sim.launch.py waveform:=step   amplitude:=0.5 duration:=8.0
ros2 launch my_pkg motor_sim.launch.py waveform:=sine   frequency:=0.5
ros2 launch my_pkg motor_sim.launch.py waveform:=square frequency:=0.25
ros2 launch my_pkg motor_sim.launch.py mode:=velocity_pid target_velocity:=20.0
ros2 launch my_pkg motor_sim.launch.py mode:=position_pid target_angle:=1.5708
ros2 launch my_pkg motor_sim.launch.py integrator:=euler physics_rate:=20.0   # 看欧拉法误差
ros2 launch my_pkg motor_sim.launch.py integrator:=euler physics_rate:=0.4    # 看数值发散
```

手动两个终端也行：

```bash
ros2 run my_pkg dianjikongzhi
ros2 run my_pkg motor_test --ros-args -p waveform:=step -p amplitude:=0.5
ros2 topic echo /motor/state
```

## 4. 参数

**dianjikongzhi**

| 参数 | 默认 | 含义 |
| --- | --- | --- |
| `motor.inertia` | 0.01 | 转动惯量 J [kg·m²] |
| `motor.damping` | 0.01 | 粘性阻尼 B [N·m·s/rad] |
| `motor.max_torque` | 3.0 | 峰值力矩限幅 [N·m] |
| `load_torque` | 0.0 | 恒定负载力矩 TL [N·m] |
| `physics_rate` | 1000.0 | **状态更新（积分）频率 [Hz]** |
| `control_rate` | 100.0 | **控制频率 [Hz]**（每 kc 步执行一次） |
| `integrator` | rk4 | 积分方法 `rk4` / `euler` |
| `csv_path` | motor_sim_log.csv | 数据记录文件（相对当前工作目录） |

**motor_test**

| 参数 | 默认 | 含义 |
| --- | --- | --- |
| `mode` | open_loop | `open_loop` / `velocity_pid` / `position_pid` |
| `waveform` | sequence | `constant`/`step`/`sine`/`square`/`chirp`/`impulse`/`sequence` |
| `amplitude` / `frequency` / `phase` / `offset` | 0.5 / 0.5 / 0 / 0 | 波形参数 |
| `impulse_width` / `start_delay` / `duration` | 0.01 / 0.0 / 18.0 | 脉冲宽度 / 启动延时 / 信号时长 [s] |
| `publish_rate` | 100.0 | 发布频率（= 控制频率） |
| `pid.kp` / `pid.ki` | 0.06 / 0.12 | 速度环 PI |
| `pid.torque_limit` / `pid.integral_limit` | 3.0 / 50.0 | 力矩限幅 / 积分限幅 |
| `target.velocity_rad_s` | 20.0 | 速度环目标 [rad/s] |
| `target.angle_rad` | π/2 | 位置环目标 [rad] |
| `position.kp` / `position.velocity_limit` | 3.0 / 30.0 | 位置环外环 P / 速度指令限幅 |

## 5. 作业提交要点

### (1) 代码仓库链接

本目录（`rmcs_ws/src/my_pkg`）即为完整代码。核心文件：`src/dianjikongzhi.cpp`（模拟器）、
`src/motor_test.cpp`（输入源）。

### (2) 状态更新频率与控制频率

**状态更新（积分）1000 Hz（步长 1 ms），控制 100 Hz（每 10 步更新一次指令）**，状态发布跟控制同频
（100 Hz，降采样，省 DDS 带宽）。

- 实现：只用一个 1 kHz 定时器，`kc = fp/fc = 10`，`cnt%kc==0` 时才锁存指令（零阶保持 ZOH）——
  10 个物理步共用同一个力矩，模拟真实控制器/DAC「每个控制周期才更新一次」，节点每秒打印实测频率。
- 为什么积分频率远高于控制频率（实测，阶跃 0.5 N·m、4 s 后角速度，解析真值 49.0842 rad/s）：

  | physics_rate | 步长 h | RK4 误差 | 前向欧拉误差 |
  | --- | --- | --- | --- |
  | 1000 Hz | 1 ms | −0.0000 rad/s | +0.0018 rad/s |
  | 100 Hz | 10 ms | −0.0000 rad/s | +0.0183 rad/s |
  | 20 Hz | 50 ms | −0.0000 rad/s | +0.0900 rad/s |
  | 1 Hz | 1 s | −0.0730 rad/s | +0.9158 rad/s |
  | 0.4 Hz | 2.5 s | −20.11 rad/s（失真） | **发散**（h > 2J/B = 2 s） |

  RK4 对这种慢机械动态极准，**真正逼着选 1 kHz 的是快动态与数值稳定性**：一旦接入电流环/电气时间常数
  （如 `τe = 0.5 ms`），RK4 稳定域要求 `h ≲ 2.78·τe ≈ 1.4 ms`，实测 **200 Hz（h = 5 ms）就发散**；
  1 kHz 也和真实 FOC 电流环（常 1~10 kHz）对齐。

### (3) 输入测试方案与结果

默认 `waveform:=sequence` 一次跑完 6 项测试（共 18 s，时间轴即仿真时间），数据全部进 CSV：

| # | 时间 | 激励 | 预期 / 实测 |
| --- | --- | --- | --- |
| 1 | 0–1 s | 零力矩静置 | ω = 0，θ = 0（零点自取） |
| 2 | 1–5 s | 阶跃 `Te = 0.5 N·m` | `ω(t)=ω∞(1−e^{−t/τ})`，`τ=J/B=1 s`，`ω∞=50 rad/s≈477.5 rpm`；实测 1 s: 31.606、2 s: 43.233、3 s: 47.511、4 s: **49.084 rad/s（468.7 rpm）**，与解析解吻合 |
| 3 | 5–9 s | 正弦 `0.5 N·m @0.5 Hz` | 幅值 `A/√(B²+(Jω)²)=15.166 rad/s≈144.8 rpm`（实测 15.17），相位滞后 `atan(Jω/B)=72.3°`（≈0.40 s） |
| 4 | 9–13 s | 方波 `0.5 N·m @0.5 Hz` | 指数充放电波形，换向连续无跳变 |
| 5 | 13–15 s | 卸载（0 N·m） | 按 `e^{−t/τ}` 自由衰减回 0 |
| 6 | 15–15.1 s | 脉冲 `2.5 N·m, 0.1 s` | 冲量 `Δω≈23.79 rad/s≈227 rpm`（理论 `250(1−e^{−0.1})=23.79`），随后衰减回 0 |

> 想演示**外部扰动 TL**：加 `load_torque:=0.05`，阶跃稳态降为 `(Te−TL)/B=45 rad/s`，且零力矩时会被
> 恒值负载反向拖动，终值 `−TL/B=−5 rad/s`（这是恒值负载转矩的正确物理行为）。

闭环补充（PI 已整定）：

- **速度环**（`mode:=velocity_pid`，目标 20 rad/s = 191 rpm，`kp=0.06, ki=0.12`）：
  超调 **6.3%**，`t63=0.14 s`，`t98=0.36 s`，稳态误差 ≈ 0，峰值力矩 1.22 N·m。
- **位置环**（`mode:=position_pid`，目标 90°，外环 `kp=3.0` + 内环速度 PI）：
  超调 **4.3%**，4 s 时 1.5705 rad、5 s 时 1.5707 rad（目标 1.5708），峰值速度 3.34 rad/s。

实跑时 `motor_test` 每秒打印自检，阶跃段同时打印理论值 `w_ss(1−e^{−t/τ})` 与误差（实测略小
0.02~0.2 rad/s，来自状态反馈 10 ms 延迟 + 控制零阶保持）。

**输出数据图片**：

```bash
python3 src/my_pkg/scripts/plot_motor_state.py --csv motor_sim_log.csv --out motor_sim_result.png
```

得到 4 联图（力矩输入 / 角速度 rad·s⁻¹ / 转速 rpm / 角度 rad），直接贴进报告；脚本同时打印每路 min/max/mean。

## 6. 目录

```
my_pkg/
├── CMakeLists.txt          # 四个可执行文件 + launch/scripts 安装
├── package.xml             # 依赖 rclcpp / std_msgs / motor_sim_msgs
├── README.md               # 本文档
├── launch/motor_sim.launch.py
├── scripts/plot_motor_state.py
└── src/
    ├── publisher_sin.cpp   # 原有
    ├── subscriber_sin.cpp  # 原有
    ├── dianjikongzhi.cpp   # 作业二：电机模拟器
    └── motor_test.cpp      # 作业二：输入源
```

## 7. 如果 IDE 又提示“找不到头文件 / 找不到 motor_sim_msgs”

那是 clangd 的编译库（`rmcs_ws/build/compile_commands.json`）里没有新包的条目，它会去“猜”别的包的
编译命令。重新生成并合并即可（已在本次修复中做过）：

```bash
cd ~/RMCS/rmcs_ws
source /opt/ros/jazzy/setup.bash
colcon build --packages-select motor_sim_msgs my_pkg --cmake-args -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
python3 - << 'EOF'
import json,os
D='/home/wangzihao/RMCS/rmcs_ws/build'
db=json.load(open(D+'/compile_commands.json'))
for p in ['my_pkg','motor_sim_msgs']:
    db+=json.load(open(D+'/'+p+'/compile_commands.json'))
db=list({e['file']:e for e in db}.values())
json.dump(db,open(D+'/compile_commands.json','w'),indent=1)
print('entries =',len(db))
EOF
```

然后在 VSCode 里 `Ctrl+Shift+P` → `clangd: Restart language server`。
