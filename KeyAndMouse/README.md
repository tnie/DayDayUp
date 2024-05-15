针对键鼠共享软件的两个问题，做针对性的测试：
- 在客户端屏幕上，按键大小写混乱
- 在客户端屏幕上，鼠标移动有效，但点击无效
- 在客户端屏幕上，释放 `Win` 键弹出服务端开始菜单

此项目的目标，是排除客户端主机上业务软件的干扰后复现问题，分析问题。

服务端弹出开始菜单，这个是操作系统层面的行为，并非 synergy 转发事件异常。
释放 `Win` 键弹出开始菜单，有没有开关控制这种行为？
- 在 windows 上据说可以通过 PowerToys 关掉
- 在银河麒麟上呢？

搞清楚几个问题：
1. 除了常规的数字、字母输入键，其他按键的生效机制
2. synergy 项目中 `KeyState::fakeKeyDown()` 等接口是否伪装得一致？


[Modifier key 修饰键](https://zh.wikipedia.org/wiki/%E4%BF%AE%E9%A5%B0%E9%94%AE)

> In computing, a modifier key is a special key (or combination 组合键) on a computer keyboard that temporarily modifies the normal action of another key when pressed together. By themselves, modifier keys usually do nothing; that is, pressing any of the `⇧ Shift`, `Alt`, or `Ctrl` keys alone does not (generally) trigger any action from the computer.

和 Qt 键盘修饰枚举值保持一致，在以下描述 Windows 系统操作时，我们使用 `Win` 键，触发 `Meta` 修饰。

1. `Shift` `Ctrl` `Alt` `Meta`/`Win` ，并不包括 `CapsLock` 大写锁定键
2. 在 `enum Qt::KeyboardModifier` 中除了上述四类，还多一个 `Qt::KeypadModifier` ，指示按键事件来自小键盘。
3. `Shift`/`Ctrl`/`Alt` 修饰在按下时生效，释放时取消，且三个按键同时按下（组合键）时三个修饰同时生效
4. 在 `Windows` 系统（和银河麒麟 v10 桌面版）上， `Meta` 修饰是在释放 `Win` 键时生效：开始菜单并非在按下 `Win` 键时弹出而是在释放 `Win` 键时弹出

    ```shell
    // Meta 修饰是在释放 Meta/Win 键时生效
    [ keyPressEvent "2024-05-15 10:18:05:459" ] QKeyEvent(KeyPress, Key_Meta)
    [ keyReleaseEvent "2024-05-15 10:18:08:151" ] QKeyEvent(KeyRelease, Key_Meta, MetaModifier)
    ```

5. 在基于 Ubuntu 的银河麒麟 v10 中，Shift + Alt 会触发 Meta 修饰。 [Why is shift-alt being mapped to meta?][1]
6. 锁定键的状态保存在操作系统上，新接入键盘时大写指示灯、数字键指示灯会同步亮起。
7. 键鼠共享软件 synergy 并不会将锁定键的操作分发给客户端：大写锁定键、小键盘的数字锁定键、滚动锁定键。

以下代码摘自 synergy-core 项目的 KeyState.h/cpp 文件
```cpp
/*!
Returns \c true if and only if the key should always be ignored.
The default returns \c true only for the toggle keys.
*/
virtual bool        isIgnoredKey(KeyID key, KeyModifierMask mask) const;
{
    switch (key) {
    case kKeyCapsLock:
    case kKeyNumLock:
    case kKeyScrollLock:
        return true;

    default:
        return false;
    }
}
```

关注 `class KeyMap` 类型

# TODO

```shell
# 可执行文件在桌面展示需要三个分辨率，否则以小、中、大图标展示可能出现意外
magick.exe convert icon-16.png icon-32.png icon-256.png icon.ico
```

[1]:https://askubuntu.com/questions/567731/why-is-shift-alt-being-mapped-to-meta
