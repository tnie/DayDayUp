针对最简单的场景（单一线程），提供一个粗略的实现过程：

在 `::connect()` 连接时，将 `receiver::slot` 放到和 `sender::signal` 关联的集合中，那么发射信号就是普通的函数调用。

怎么关联？

存在各自 `sender::metaobject` 中以 `signal` 为索引的集合，实际实现会有非常多的细节和分支处理。

方法论：pIpml 手法+观察者模式。

怎么保证调用槽函数 `slot` 时其所在对象 `receiver` 未释放的呢？这是在 `QObject::~QObject()` 中完成的

> All signals to and from the object are automatically disconnected, and any pending posted events for the object are removed from the event queue.

作为 `sender` 角色析构时清空其 `QObjectPrivate::connectionLists`；作为 `receiver` 角色析构时将其 sender 的 receiver 置空 `node->receiver = 0`

[Qt - 一文理解信号槽机制（万字剖析整理）_JYU_hsy的博客-CSDN博客](https://blog.csdn.net/weixin_40774605/article/details/109342536)

# 声明槽函数

在 Qt 元对象系统内，存在 `Q_INVOKABLE` 宏以便指示某个成员函数使其能够被元对象系统调用。

使用 `QObject::connect()` 链接信号-槽：

- 如果使用 `SLOT` 宏时，要求槽函数必须使用 `slots` 声明以便元对象系统能够识别、调用
- 如果使用函数指针的方式，槽函数无需 `slots` 声明。此场景下，内部实现并不依赖 `Q_INVOKABLE` 概念。