---
title: Qt moc
categories: 
- cpp
- qt
date: 2023-03-17 14:37:04
tags: qt moc
---

`QObject` 的 `d_ptr` 指针，指向 `QObjectPrivate` ； `QWidget` 的 `d_ptr` 指针，指向 `QWidgetPrivate` 。 

Qt 内部实现上使用了宏，本质上是 pImpl 手法。

指针类型是抽象类 `QObjectData` ，它是所有 QXxxPrivate 类型的基类。

`QScopedPointer<QObjectData> d_ptr` 

元对象系统就定义在 `QObjectData` 中：

```cpp
// 摘自 qobject.h  版本 Qt 5.12.12
class Q_CORE_EXPORT QObjectData {
public:
    virtual ~QObjectData() = 0;
    QObject *q_ptr;
    QObject *parent;
    QObjectList children;

    uint isWidget : 1;
    uint blockSig : 1;
    uint wasDeleted : 1;
    uint isDeletingChildren : 1;
    uint sendChildEvents : 1;
    uint receiveChildEvents : 1;
    uint isWindow : 1; //for QWindow
    uint deleteLaterCalled : 1;
    uint unused : 24;
    int postedEvents;
    QDynamicMetaObjectData *metaObject;
    QMetaObject *dynamicMetaObject() const;
};

```

要理解 `QObject` ，先看 `QMetaObject`

> The `QMetaObject` class contains meta-information about Qt objects.
>
> The `QMetaMethod` class provides meta-data about a member function.

`struct QMetaObject` 实际只有一个（语法上）公有的成员变量 `d` 但在语义上却是 private data （见 `qobjectdefs.h` 头文件）。为什么如此处理？

文件 `moc_qobject.cpp` 去哪里查看呢？

<!-- more -->

之后无符号整型指针 `d.data` 又是按照 `QMetaObjectPrivate` 解析的，后者的定义在哪？找到这个约定我们就能理解 moc_xxx.cpp 中 `const uint qt_meta_data_CObject[]` 一堆的数字是什么意思了。

```cpp
// 5.12.12\Src\qtbase\src\corelib\kernel\qmetaobject_p.h
// 无符号整型数值强转有符号整型数值，存在风险。qt 如何避免的？
struct QMetaObjectPrivate
{
    // revision 7 is Qt 5.0 everything lower is not supported
    // revision 8 is Qt 5.12: It adds the enum name to QMetaEnum
    enum { OutputRevision = 8 }; // Used by moc, qmetaobjectbuilder and qdbus

    int revision;
    int className;
    int classInfoCount, classInfoData;
    int methodCount, methodData;
    int propertyCount, propertyData;
    int enumeratorCount, enumeratorData;
    int constructorCount, constructorData;
    int flags;
    int signalCount;
    // ...
}
```

关于 `methodCount()` 在帮助手册中的描述前后有差异（推断后一段存在笔误）：

> `method()` and `methodCount()` provide information about a class's meta-methods (signals, slots and other **invokable** member functions).
> 
> Returns the number of methods in this class, including the number of methods provided by each base class. These include signals and slots as well as **normal** member functions.

帮助手册针对 `Q_INVOKABLE` 的解释，很明显 invokableMethod() 和 normalMethod() 是不同的！前者表示：

> Apply this macro to declarations of member functions to allow them to be invoked via the meta-object system.

在尝试追踪 `connect` 如何工作之前，我们先看一下怎么使用 `QMetaObject` 执行 `QObject` 中的方法？

```cpp
// 子类的对象元信息定义父类对象元信息时直接使用后者的 staticMetaObject
QT_INIT_METAOBJECT const QMetaObject DObject::staticMetaObject = { {
    &CObject::staticMetaObject,
    qt_meta_stringdata_DObject.data,
    qt_meta_data_DObject,
    qt_static_metacall,
    nullptr,
    nullptr
} };

// 获取对象元信息时的分支是什么情况？ d 指针是什么？
const QMetaObject *DObject::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}
```

# 源文件内部类

我们可以在 .cpp 文件中定义只用于当前文件的 C++ 类，但 `QObject` 子类不支持这种做法。

因为链接时无法看到 moc 扩展的定义，报错 LNK2001: unresolved external symbol

推测可能需要某种技巧才能满足。

[Define a QObject derived class inside an anonymous namespace?][1]

[1]:https://stackoverflow.com/questions/41614016/define-a-qobject-derived-class-inside-an-anonymous-namespace