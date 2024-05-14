针对键鼠共享软件的两个问题，做针对性的测试：
- 在客户端屏幕上，按键大小写混乱
- 在客户端屏幕上，鼠标移动有效，但点击无效
- 在客户端屏幕上，释放 `Win` 键弹出服务端开始菜单

此项目的目标，是排除客户端主机上业务软件的干扰后复现问题，分析问题。

服务端弹出开始菜单，这个是操作系统层面的行为，并非 synergy 转发事件异常。
释放 `Win` 键弹出开始菜单，有没有开关控制这种行为？

搞清楚几个问题：
1. 除了常规的数字、字母输入键，其他按键的生效机制
2. synergy 项目中 `KeyState::fakeKeyDown()` 等接口是否伪装得一致？


[Modifier key 修饰键](https://zh.wikipedia.org/wiki/%E4%BF%AE%E9%A5%B0%E9%94%AE)

> In computing, a modifier key is a special key (or combination 组合键) on a computer keyboard that temporarily modifies the normal action of another key when pressed together. By themselves, modifier keys usually do nothing; that is, pressing any of the `⇧ Shift`, `Alt`, or `Ctrl` keys alone does not (generally) trigger any action from the computer.

和 Qt 键盘修饰枚举值保持一致，在以下描述 Windows 系统操作时，我们使用 `Win` 键，触发 `Meta` 修饰。

1. `Shift` `Ctrl` `Alt` `Meta`/`Win` ，并不包括 `CapsLock` 大小写转换键
2. 在 `enum Qt::KeyboardModifier` 中除了上述四类，还多一个 `Qt::KeypadModifier` ，是什么功能？
3. `Shift`/`Ctrl`/`Alt` 修饰在按下时生效，释放时取消，且三个按键同时按下（组合键）时三个修饰同时生效
4. 在 `Windows` 系统上， `Meta` 修饰是在释放 `Win` 键时生效：开始菜单并非在按下 `Win` 键时弹出而是在释放 `Win` 键时弹出

```shell
// Meta 修饰是在释放 Meta/Win 键时生效
[ keyPressEvent "2024-05-15 10:18:05:459" ] QKeyEvent(KeyPress, Key_Meta)
[ keyReleaseEvent "2024-05-15 10:18:08:151" ] QKeyEvent(KeyRelease, Key_Meta, MetaModifier)
```