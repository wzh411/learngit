#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
绘制电机模拟器输出的 CSV 数据（作业提交要求：附输出数据的图片）。

用法:
    python3 plot_motor_state.py --csv motor_sim_log.csv --out motor_sim_result.png
    python3 plot_motor_state.py --csv motor_sim_log.csv --show

依赖: matplotlib (pip install matplotlib)
"""

import argparse
import csv
import os
import sys

import matplotlib.pyplot as plt


def load_csv(path):
    if not os.path.isfile(path):
        sys.exit("找不到 CSV 文件: %s" % path)
    with open(path, "r", newline="") as f:
        reader = csv.DictReader(f)
        cols = {name: [] for name in (reader.fieldnames or [])}
        for row in reader:
            for name in cols:
                try:
                    cols[name].append(float(row[name]))
                except (TypeError, ValueError):
                    cols[name].append(float("nan"))
    if not cols:
        sys.exit("CSV 文件为空: %s" % path)
    return cols


def fmt_stats(name, values):
    if not values:
        return "%s: 无数据" % name
    return "%s: min=%+9.4f  max=%+9.4f  mean=%+9.4f" % (
        name, min(values), max(values), sum(values) / len(values))


def main():
    parser = argparse.ArgumentParser(description="绘制电机模拟器输出曲线")
    parser.add_argument("--csv", default="motor_sim_log.csv", help="输入 CSV 路径")
    parser.add_argument("--out", default="motor_sim_result.png", help="输出 PNG 路径")
    parser.add_argument("--show", action="store_true", help="交互式显示（否则只保存图片）")
    parser.add_argument("--title", default="", help="图标题")
    args = parser.parse_args()

    data = load_csv(args.csv)
    t = data["sim_time_s"]

    fig, axes = plt.subplots(4, 1, figsize=(11, 12), sharex=True)

    axes[0].plot(t, data["torque_cmd_Nm"], color="tab:red", label="Te (torque cmd)")
    axes[0].plot(t, data["load_torque_Nm"], color="tab:gray", linestyle="--",
                 label="TL (load torque)")
    axes[0].set_ylabel("Torque [N*m]")
    axes[0].set_title("Motor simulator: torque input / velocity output / angle output")
    axes[0].legend(loc="upper right")
    axes[0].grid(True, alpha=0.3)

    axes[1].plot(t, data["velocity_rad_s"], color="tab:blue", label="omega [rad/s]")
    axes[1].set_ylabel("Velocity [rad/s]")
    axes[1].legend(loc="upper right")
    axes[1].grid(True, alpha=0.3)

    axes[2].plot(t, data["velocity_rpm"], color="tab:green", label="n [rpm]")
    axes[2].set_ylabel("Speed [rpm]")
    axes[2].legend(loc="upper right")
    axes[2].grid(True, alpha=0.3)

    axes[3].plot(t, data["angle_rad"], color="tab:purple", label="theta [rad]")
    axes[3].set_ylabel("Angle [rad]")
    axes[3].set_xlabel("sim time [s]")
    axes[3].legend(loc="upper left")
    axes[3].grid(True, alpha=0.3)

    if args.title:
        fig.suptitle(args.title)

    fig.tight_layout()
    fig.savefig(args.out, dpi=150)
    print("图片已保存: %s" % os.path.abspath(args.out))

    print("-" * 72)
    print("数据统计 (%d 个采样点, 时长 %.3f s)" % (len(t), t[-1] if t else 0.0))
    print(fmt_stats("Te      ", data["torque_cmd_Nm"]))
    print(fmt_stats("omega   ", data["velocity_rad_s"]))
    print(fmt_stats("n [rpm] ", data["velocity_rpm"]))
    print(fmt_stats("theta   ", data["angle_rad"]))
    print("-" * 72)

    if args.show:
        plt.show()


if __name__ == "__main__":
    main()
