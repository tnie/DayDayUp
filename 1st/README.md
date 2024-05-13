通过最简单的 Qt 程序探索 Qt 的内部实现。

```cpp
#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    return a.exec();
}
```

我们知道肯定有 event loop ，虽然写成 `a.exec()` 但其实是静态成员函数 `a::exec()`

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

通过 `QEventLoop::WaitForMoreEvents` 标志位和 `MsgWaitForMultipleObjectsEx()` Win32 API 实现的：

推测是 IOCP 或者其前几代技术。

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

类 `QThreadData` 的概念和作用？=线程id （并且管理着事件循环类），这个 `threadData` 成员变量在 `QObjectPrivate` 基类中。

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
```

去了解 Win32 API 还是转头去看 linux 平台的实现呢？
