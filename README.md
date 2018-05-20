# 6.828

MIT操作系统（6.828 Operating System Engineering）的JOS实验。

## 环境

需要32位gcc工具链（`as`,`gcc`,`ld`,`make`等），和`qemu-system-i386`。

## 运行

在根目录下，运行`make qemu`y以在QEMU中测试。关闭QEMU窗口或在终端Ctrl-C来退出。

运行`make qemu-nox`直接在终端进行测试，此时使用Ctrl-A X退出。

运行`make grade`运行评分脚本。

可以使用`git checkout labX`（1≤X≤6）切换到某个实验刚做完的状态。
