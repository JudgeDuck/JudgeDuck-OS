## FAQ

Q：什么是评测鸭？

A：见@TODO

Q：可以使用什么编程语言提交？具体有什么限制？

A：

目前仅支持 C 语言，使用 `gcc 5.4.0` 编译为 32 位程序，且开启 `-O2` 优化开关。具体的信息是：
* `gcc version 5.4.0 20160609 (Ubuntu 5.4.0-6ubuntu1~16.04.9)`
* `gcc -O2 -m32 -pipe -nostdinc -MD -fno-builtin -fno-omit-frame-pointer -fno-stack-protector -fno-tree-ch -std=gnu99 -static -Wall -Wno-format -Wno-unused -Werror -gstabs`

目前可以使用的头文件包括：
* `string.h`
* `math.h`
* `stdint.h`
* `stdbool.h`
更多头文件正在路上……

Q：评测机配置如何？

A：

* Intel® Core™ i3-3240 Processor @ 3.40 GHz
* 4GB DDR3 1600 RAM
* JudgeDuck OS，@TODO加个链接

Q：时间和空间使用量是如何计算的？

A：

空间使用量计算为你的函数运行时的总**写入**量，以 4 KB（一页）为单位。评测结束时，将统计评测期间被写入过的页的个数，乘以 4 KB 作为你的空间使用量。

我们的大多数题目保证你在运行时不需要进行任何系统调用（例如读写文件），并且JudgeDuck OS保证评测时不调度其他进程且不产生中断。这种情况下，“内核态时间”为零，且“用户态时间”等于“真实时间”，就是我们测定的时间。目前时间的分度值是 1.28 μs。

交互库会占用微量的时间和空间，例如当你的函数会被多次调用时，交互库会花费微量时间保存你的结果，这部分时间会计入你的时间和空间使用量。然而，生成数据、判定结果正确性等不会计入你的使用量；换句话说，我们会尽可能减少交互库占用的资源。如果交互库占用了任何空间，则题目会明确告知占用的量（即使只有 4 KB）。
