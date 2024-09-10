通过最简单的 Qt 程序探索 Qt 的内部实现。

```cpp
#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    return a.exec();
}
```

我们知道肯定有 event loop ，虽然写成 `a.exec()` 但其实是静态成员函数 `a::exec()` 。

为什么是静态函数？因为 `QCoreApplication ` 其实是单例类。

关联的 `exit()` 和 `quit()` 槽函数也都是 `static` 的。

> It's good practice to always connect signals to this slot using a `QueuedConnection`.

信号槽的连接 `QueuedConnection` 有什么特殊？

`QXXXApplication` 是单例类，存在私有静态成员 `static QCoreApplication *self` 指针。构造中也做了对应的约束：

```cpp
// 摘自 QCoreApplicationPrivate::init()
    Q_ASSERT_X(!QCoreApplication::self, "QCoreApplication", "there should be only one application object");
    QCoreApplication::self = q;
```

熟悉 ASIO 框架的同学，会有一个疑问：没有事件的时候，循环就退出了。Qt 怎么处理的？

Windows 平台通过 `QEventLoop::WaitForMoreEvents` 标志位和 `MsgWaitForMultipleObjectsEx()` Win32 API 实现的：推测是 IOCP 或者其前几代技术。

Ubuntu 平台通过 GLib 的 `g_main_context_iteration()` 接口实现。

# 断点调试

用过 Visual Studio C++ 的同学，肯定知道 Windows 下调试需要源代码和 pdb 文件。

怎么获取？怎么使用？

[qt5.12.3版本在Qt creator中添加源码调试](https://blog.csdn.net/wanfengnianhua/article/details/110198916)

[Qt5.12.x怎么调试Qt的源码?](https://blog.csdn.net/libaineu2004/article/details/106188325/)

# 基类 QObject 

这里面的东西非常多，我们用到的时候再回来记录。

成对的两个指针：`QScopedPointer<QObjectData> d_ptr` 和 `QObject * q_ptr`

抽象类 `QObjectData` 作为接口，类型 `QObjectPrivate` 继承自前者。

在 `QObjectData` 的子类 `QXXXPrivate` 中通过 `q_func()` 完成 `q_ptr` 的类型提升：

> 参考 QtCore/qglobal.h 中宏 `Q_DECLARE_PUBLIC` 和  `Q_Q`

在 `QObject` 的子类 `QXXX` 中通过 `d_func()` 完成 `d_ptr` 的类型提升：

> 参考 QtCore/qglobal.h 中宏 `Q_DECLARE_PRIVATE` 和  `Q_D`

## 线程亲和性

见 Thread Affinity 特性：

> A `QObject` instance is said to have a thread affinity, or that it lives in a certain thread.
>
> When a `QObject` receives a `queued signal` or a `posted event`, the slot or event handler will run in the thread that the object lives in.

见 `QThread` 类型描述：`run()` 虚函数可以在其子类中重写。

> A `QThread` object manages one thread of control within the program. `QThreads` begin executing in `run()`. By default, `run()` starts the event loop by calling `exec()` and runs a Qt event loop inside the thread.

# 主线程

读代码就是剥洋葱，一层又一层。逻辑清晰明了非常重要。

在 `QCoreApplicationPrivate::QCoreApplicationPrivate()` 会执行以下代码，
表示第一个创建 `QCoreApplication` 对象的线程就是主线程。

```cpp
// 摘自 QThreadData::current()
if (!QCoreApplicationPrivate::theMainThread) {
    QCoreApplicationPrivate::theMainThread = threadData->thread.loadRelaxed();
}
```

类 `QThreadData` 的概念和作用？=线程id （并且 **管理着事件循环类**），这个 `threadData` 成员变量在 `QObjectPrivate` 基类中。

```cpp
// 摘自 QObjectPrivate 类定义
    QAtomicPointer<QThreadData> threadData; // id of the thread that owns the object

// 摘自 QObject() 构造函数
    auto threadData = (parent && !parent->thread()) ? parent->d_func()->threadData.loadRelaxed() : QThreadData::current();
    threadData->ref();
    d->threadData.storeRelaxed(threadData);
```

在 `QThreadData::current()` 中新建对象时，底层调用 Win32 API 完成的赋值： `TlsGetValue()`

```cpp
// 摘自 QThreadData::current()
    QThreadData *threadData = reinterpret_cast<QThreadData *>(TlsGetValue(qt_current_thread_data_tls_index));
```

事件循环类 `QEventLoop` ，作为函数 `QCoreApplication::exec()` 内部的局部变量使用（生命周期同 `QCoreApplication` 对象），那么循环是如何退出的？

> From within the event loop, calling `exit()` will force `exec()` to return.

查看 `QCoreApplication::exit()` 的实现，发现是借 `QThreadData::eventLoops` 退出的，可上述局部变量如何关联到前者的？

```cpp
// 摘自 QCoreApplication::exit()
for (int i = 0; i < data->eventLoops.size(); ++i) {
    QEventLoop *eventLoop = data->eventLoops.at(i);
    eventLoop->exit(returnCode);
}
```

就在局部变量 `eventLoop.exec()` 启动事件循环时：

```cpp
// 摘自 QEventLoop::exec()
    auto threadData = d->threadData.loadRelaxed();
    ++threadData->loopLevel;
    threadData->eventLoops.push(d->q_func());
```

创建 `QCoreApplication` 对象和 `QEventLoop` 对象时都没有设置父对象 `parent` ，都是通过 `QThreadData::current()` 赋值给各自的 `d->threadData` ，两者的构造在同一线程内完成！

事件循环的主体：`QEventLoop::processEvents()`

> This function is simply a wrapper for `QAbstractEventDispatcher::processEvents()`. See the documentation for that function for details.

思考：此处的 thread id 概念很好理解，但是理解代码实现反而有难度。难道这是兼容旧标准，兼容多平台的代价吗？

Qt 第一个版本 Qt1.0 诞生于 1995 年，比 C++ 第一个标准 C++98 还早三年。 C++ 的线程类 `std::thread` 在 2011 年前后的 C++11 标准中引入。

# 终端

想看到熟悉的终端窗口？

在项目的 Build & Run 配置中，勾选 Run in terminal 即可。

此时 `qDebug()`/ `qWarning()` 等日志就不会在应用程序输出窗口打印，而是在终端打印。

选中 Run in terminal 之前，应用程序 Run in 哪里呢？

# 事件

事件循环，我们给 `QCoreApplication` 单例一个事件，看一下怎么分发处理的？

两个发送事件的函数：

- `QCoreApplication::postEvent()` 事件对象应在堆上创建，先放队列，分发处理时用到了后者
- `QCoreApplication::sendEvent()` 事件对象应在栈上创建，直接在当前线程、当前函数执行

用户自定义事件 `a.postEvent(&a, new QEvent(QEvent::User))` 放到了 receiver 线程的队列上，
在 `QObject::customEvent()` 函数加断点，以便了解调用逻辑：
过程似乎用到 Win32 的窗口类以及有关的函数调用，可这只是个 Console 程序而已。为什么？

```cpp
// 摘自 QCoreApplication::postEvent
    data->postEventList.addEvent(QPostEvent(receiver, event, priority));

// 摘自 QEventDispatcherWin32::processEvents() ，先处理 OS 事件（键鼠输入、网络收发）
    haveMessage = PeekMessage(&msg, 0, 0, 0, PM_REMOVE);

// 在 Ubuntu 平台，摘自 QEventDispatcherGlib::processEvents()
    bool result = g_main_context_iteration(d->mainContext, canWait);
```

在 Ubuntu 上，底层使用 GLib 实现事件循环： 

> [GLib][glib] is the low-level core library that forms the basis for projects such as GTK and GNOME. 
It provides data structure handling for C, portability wrappers, and interfaces for such runtime functionality as an event loop, threads, dynamic loading, and an object system.

可以查看 Qt 文档中对 `QAbstractEventDispatcher Class` 的介绍，了解 Qt 是如何封装各平台差异的。

备注1：在 Ubuntu 使用 `.run` 在线安装包部署 Qt 5.15.2 后，发现 `qobject.cpp` 调试信息不太一致，断点进不去。如果追求严格的一致性，估计还是需要源码构建才行。

[glib]:https://gitlab.gnome.org/GNOME/glib

# Ubuntu 20.04.1 LTS (Focal Fossa)

为什么选择这个版本的系统来学习？

公司业务线目前采用的 Kylin V10 桌面版。无论是含掉电保护模式的海军版，还是“物美价廉”的国防版，大厦基石都一致：

> Linux version 5.4.18 ， gcc version 9.3.0

推断两者都是基于 Ubuntu 20.04 LTS ，改用 UKUI 桌面环境的换皮版。 [所见略同][2]。 

以下是不同的版本：联网安装时会自动更新 Linux 内核！！

- ubuntu-20.04  
- ubuntu-20.04.1 ，Linux version 5.4.0  [Ubuntu 旧版本][1]
- ubuntu-20.04.1 ，Linux version 5.8
- ubuntu-20.04.6 ，Linux version 5.15.0

思考：公司产品都是采用 Arm 架构的飞腾主板，我只有 x86 的电脑。底层架构都不一致，争取 Linux kernel 一致性有没有必要？

## Qt 5.12.8

在 ubuntu-20.04.1 系统中使用 `qt-opensource-linux-x64-5.12.8.run` 离线安装包时，勾选 Qt 库及其源代码，缺少 debug info 导致断点调试无法进入 Qt 源代码。

> 如果使用 online 安装包，可以同时勾选、下载 debug info 。但 online 安装包只支持 Qt5.15.1 、 Qt5.15.2 和 Qt6.x 。

调试信息需要另行下载：必须选择较新的 [202004051456qtbase][3] 包！我试过 202003131355qtbase 包，不成功。

Qt [打包的历史仓库][5]中，并没有 arm 架构下 Qt5.x 的离线安装包、库文件、debug info 等。

[Ubuntu下跟踪调试QT源码][4]

```shell
# 安装开发工具，以及之后使用 QtCreator 依赖的库
sudo apt update
sudo apt install openssh-client openssh-server \
 open-vm-tools open-vm-tools-desktop \
 git build-essential vim \
 libxcb-cursor-dev mesa-common-dev libglu1-mesa-dev libxcb-xinerama0 
 
# 不要更新（避免更新 Linux kernal）
reboot 
sudo ./qt-opensource-linux-x64-5.12.8.run  # 9+qDuLth.F@MD#8
wget https://download.qt.io/online/qtsdkrepository/linux_x64/desktop/qt5_5128/qt.qt5.5128.debug_info.gcc_64/5.12.8-0-202004051456qtbase-Linux-RHEL_7_4-GCC-Linux-RHEL_7_4-X86_64-debug-symbols.7z
# 解压后，将 debug info 放到 Qt lib 路径下
sudo mv ./5.12.8/gcc_64/lib/*.debug /opt/Qt5.12.8/5.12.8/gcc_64/lib/
# 在 QtCreator 中进行源码路径映射
# 下载 demo 进行测试
export http_proxy='http://192.168.50.222:10811'
export https_proxy='http://192.168.50.222:10811'
git clone https://github.com/tnie/DayDayUp.git
```

[1]:https://old-releases.ubuntu.com/releases/20.04.1/
[2]:https://dotatong.com/index.php/archives/143/
[3]:https://download.qt.io/online/qtsdkrepository/linux_x64/desktop/qt5_5128/qt.qt5.5128.debug_info.gcc_64/5.12.8-0-202004051456qtbase-Linux-RHEL_7_4-GCC-Linux-RHEL_7_4-X86_64-debug-symbols.7z
[4]:https://blog.csdn.net/fhw_bin_dl/article/details/82909028
[5]:https://download.qt.io/online/qtsdkrepository/linux_arm64/desktop/

# QEMU 模拟器

在 amd64(3400g) + windows + qemu 上模拟 arm 架构安装 Kylin V10 ，延迟 10min+ 响应一次键盘事件。

摸索交叉编译试一试吧
