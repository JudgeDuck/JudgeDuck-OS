## 主题

2018年操作系统课程设计-应用程序稳态测试系统

## 基础

基于MIT操作系统（6.828 Operating System Engineering）的JOS实验进行开发。

## 环境

需要32位gcc工具链（`as`,`gcc`,`ld`,`make`等），和`qemu-system-i386`。

## 运行

在根目录下，运行`make qemu`以在QEMU中测试。关闭QEMU窗口或在终端Ctrl-C来退出。

运行`make qemu-nox`直接在终端进行测试，此时使用Ctrl-A X退出。

运行`make jos-grub`