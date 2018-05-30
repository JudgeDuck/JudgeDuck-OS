## 主题

2018年操作系统课程设计-应用程序稳态测试系统

## 基础

基于MIT操作系统（6.828 Operating System Engineering）的JOS实验进行开发。

## 环境

需要32位gcc工具链（`as`,`gcc`,`ld`,`make`等），和`qemu-system-i386`。

## 运行

在根目录下，运行`make qemu`以在QEMU中测试。关闭QEMU窗口或在终端Ctrl-C来退出。

运行`make qemu-nox`直接在终端进行测试，此时使用Ctrl-A X退出。

运行`make jos-grub`生成可以在真机上运行的系统。

## 真机测试

制作一个真机的启动盘（U盘），将生成的`jos-grub`放到启动盘中。

将启动盘插入机器中，并从启动盘引导开机。

进入grub界面，使用命令`multiboot /jos-grub /jos-grub`设置。

使用`boot`引导，可以看到成功进入了系统。

## 运行程序

进入系统后，在命令行下直接可以运行程序，例如：

可以调用Arbiter进行测试，命令为 `arbiter quick 100000 1024 1 1000000`

即可较为准确地测出快速排序的运行时间。