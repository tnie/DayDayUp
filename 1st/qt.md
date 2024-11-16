---
title: Qt 备忘录
categories:
  - cpp
  - qt
date: 2023-03-17 14:37:04
tags:
  - qt
---

事件怎么传播的？在父子组件怎么传播，怎么消失？

重绘的逻辑是什么样子？ 对自己 `resize()` 无效的吗？

绘图时，将逻辑代码直接调用不可以吗？为什么要重写 `paintEvent()` 虚函数？

pushbutton 和 toolbutton 两者区别？

`QTableWidget` 的 currentItem 和 selectedItem 概念有什么区别？

Qt Creator 自动创建的 `.gitignore` 并未包含 `moc_predefs.h` 文件，这个文件有什么特殊吗？

`std::vector` 等容器类和 qt 的容器类，区别和联系？优先使用哪个呢？

widget 页面默认隐藏不释放内存的优点？为什么不及时释放内存资源呢？

<!-- more -->

`QItemSelectionModel` 的选中和 currentIndex 相互独立，后者的使用场景？

> Note the that the current index changes independently from the selection. 

信号槽机制，如何避免调用已释放的对象的槽函数的？

# QT Creator

> A profile configuration is an optimized release build that is delivered with separate debug information. It is best suited for analyzing applications.

运行时的配置：

> When building an application, Qt Creator creates a list of directories where the linker will look for libraries that the application links to. By default, the linked libraries are made visible to the executable that Qt Creator is attempting to run. Usually, you should disable this option only if it causes unwanted side-effects or if you use deployment steps, such as `make install`, and want to make sure that the deployed application will find the libraries also when it is run without Qt Creator.

怎么打包发布项目呢？

# 命令行构建

在 proj/src 目录下包含 untitled.pro 等项目源代码，
采用 shadow 方式编译中间编译产物避免污染源码所在目录

```shell
# 假设当前工作目录是 *.pro Qt 工程文件所在目录
cd ../ && make release debug
cd release && qmake ../src/untitled.pro
# 调试版本
cd ../debug && qmake ../src/untitled.pro CONFIG+=debug
make -j8
# 验证编译结果。在远程终端中一般无法启动窗口程序
```

关于 Debug 模式等更多 qmake 操作，请学习 Qt 帮助手册。

# 窗口

A widget that is not embedded in a parent widget is called a window. 
(Usually, windows have a frame and a title bar, although it is also possible to create windows without such decoration using suitable window flags). 
In Qt, `QMainWindow` and the various subclasses of `QDialog` are the most common window types.

# 定时器

On UNIX (including Linux, macOS, and iOS), Qt will keep millisecond accuracy for `Qt::PreciseTimer`. 
For `Qt::CoarseTimer`, the interval will be adjusted up to 5% to align the timer with other timers that are expected to fire at or around the same time. 
The objective is to make most timers wake up at the same time, thereby reducing CPU wakeups and power consumption.

On Windows, Qt will use Windows's Multimedia timer facility (if available) for `Qt::PreciseTimer` and 
normal Windows timers for `Qt::CoarseTimer` and `Qt::VeryCoarseTimer`.

# 事件驱动

QT 的事件驱动，和 libevent/ asio 等库在性能、易用性、可靠性上是否存在大的差别？ 是不是大同小异，只需要关注接口使用上的差别即可？

## 内存管理

查看 `void QObject::deleteLater()` 接口

什么是 d 指针？查看 `QSharedDataPointer ` 类型和 Implicit Sharing 章节

> However, Qt's container iterators have different behavior than those from the STL. 

Qt 的隐式共享（写时拷贝）带来一些反直觉的现象：

```cpp
// 注意 深拷贝 发生的位置
 QPixmap p1, p2;
 p1.load("image.bmp");
 p2 = p1;                        // p1 and p2 share data

 QPainter paint;
 paint.begin(&p2);               // cuts p2 loose from p1
 paint.drawText(0,50, "Hi");
 paint.end();
```

> The classes listed below automatically detach from common data if an object is about to be changed. The programmer **will not even notice** that the objects are shared. Thus you should treat separate instances of them as **separate objects**. 
> 
> They will always behave as separate objects but with the added benefit of sharing data whenever possible. 
> 
> For this reason, you can pass instances of these classes as arguments to functions by value without concern for the copying overhead. 

只能局限在 Qt 自身封装的类型！

如果开发者自己封装隐式共享类型，还不如多用常量引用来的简单直观。

虽然提高了类库使用者随意赋值、随意新增变量等不良代码的效率，降低了类库使用者的编码负担（即使不理解深浅拷贝等，代码效率也不会低），但也妨碍了开发人员学习提升的机会，纵容（误导）开发人员以不好的习惯使用其他类库（降低效率），增加了开发人员使用其他库、使用自定义类型的负担，某种意义上的围墙，封闭而非开放，将开发者圈在自己的类库中（使用其他库效率怎么就低了呢）。如果按照使用 Qt 类型的习惯（随意传值拷贝）使用 Qt 之外的类型（三方库、或者自己封装的）那就是不良代码。

——我觉得这是有原罪的。应该改善大环境，而不应该为了利己而纵容初学者使大环境更糟糕。

> 在 Qt 中很多类型不允许拷贝，并且接口参数基本都在使用指针，使用 Qt 开发较多人照猫画虎（懒得思考、分辨什么时候传值，什么时候传引用）也在大量使用指针传参，客观事实上传值的使用场景比较少，所以节省这一点开销而带来理解上的负担，是否值得？ 
> 
> 写时拷贝本应该是开发者关注性能的基础原则之一，不要动不动就拷贝一份写烂代码（这也是引用、常量引用的意义），现在 Qt 鼓励大家如此滥用（或者说开发人员滥用后 Qt 不得不祭出隐式共享“擦屁股”？）
> 
> ——Qt 的隐式共享特性，大大增加了只写/习惯写 Qt 的 C++ 开发人员如果没有理解其中的原理（深拷贝、浅拷贝、拷贝构造等等概念），转而编写非 Qt 的代码，写出烂代码的概率。


# 网络编程

`class QIODevice ` 可以同步地处理，也可以异步地处理。查看这个类的接口。

上层抽象：由  `QNetworkAccessManager ` 关联 ` QNetworkRequest` 和 `QNetworkReply` ：

>  The URL specified when a request object is constructed determines the protocol used for a request. Currently HTTP, FTP and local file URLs are supported for uploading and downloading.

底层概念：` QTcpSocket` 结合事件驱动，是异步的。当同步使用时，应该避免在 GUI 线程中使用，防止阻塞。

> Although most of its functions work asynchronously, it's possible to use `QTcpSocket` synchronously.
> Synchronous sockets often lead to code with a simpler flow of control. The main disadvantage of the `waitFor...()` approach is that events won't be processed while a `waitFor...()` function is blocking. If used in the GUI thread, this might freeze the application's user interface. For this reason, we recommend that you use synchronous sockets only in non-GUI threads. When used synchronously, `QTcpSocket` doesn't require an event loop.

UDP 广播主要用于发现协议:

> Broadcasting is often used to implement network discovery protocols, such as finding which host on the network has the most free hard disk space.
> 
> To broadcast a datagram, simply send it to the special address `QHostAddress::Broadcast` (255.255.255.255), or to your local network's broadcast address.

域名解析存在异步接口、同步接口两个，前者可以在 GUI 线程中使用，而后者只能在 non-GUI thread 中使用。

QT 网络模块也是支持代理的，也支持对主机网络接口的开关等操作。

# Action 和 QToolButton

这两个似乎息息相关

# 布局

在 `QVBoxLayout` 中添加自定义 widget 时：同时满足以下两个条件时就会显示不出来

1. 自定义 widget 没有全局的布局
2. 再次 `addWidget()` 的组件的“垂直策略”可以拉伸时

workarount 要么自定义的 widget 增加全局的布局，要么 `addWidget( stretch =1)` 。为什么呢？