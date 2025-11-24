
# 隐式共享

又称为“写时拷贝”。

关联类型： `QSharedData` 引用计数类， `QSharedDataPointer` 按需 clone 。 

我认为隐式共享的隐患太大，特别容易错用、埋雷。守规矩用指针、智能指针或引用，更不容易犯错。

举个例子： `void QPixmap::detach()` 函数描述

> Detaches the pixmap from shared pixmap data.

> A pixmap is automatically detached by Qt whenever its contents are about to change. This is done in almost all QPixmap member functions that modify the pixmap (`fill()`, `fromImage()`, `load()`, etc.), and in `QPainter::begin()` on a pixmap.

重点关注末尾：写时拷贝是放到调用者 QPainter 中执行的。

> There are two **exceptions** in which `detach()` must be called explicitly, that is when calling the `handle()` or the `x11PictureHandle()` function (only available on X11). Otherwise, any modifications done using system calls, will be performed on the shared data.

还有更多的例外！

如果没有完善的描述手册，或者使用者没有关注这些注意事项……

我一直不太喜欢 Qt ，它的确做了好多脏活累活将接口变得简洁易用，但时不时就会被某个接口绊倒：有额外的注意事项，和你的直观想象不一致。

这要求我们用 Qt 的每个接口前，都要关注其接口描述（至少要简单浏览）。这是种思想负担。

再多吐槽一句，我总有种感觉，信号槽是糟粕，是垃圾，Qt 是个糟糕的通用库。
