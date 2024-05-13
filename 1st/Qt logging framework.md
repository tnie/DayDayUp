
在 Qt Creator 如何运行终端程序呢？

在项目的 Build & Run 配置中，勾选 Run in terminal 即可。

选中 Run in terminal 之前，应用程序 Run in 哪里呢？

通过 Windows 任务管理器查看，勾选后作为 qtcreator_process_stub.exe （和 Qt Creator 进程同级别）的子进程运行，没有勾选的话作为 Qt Creator 的子进程运行。

# Qt 的日志框架

最常用的函数： `qDebug(const char *message,  ...)`

Calls the message handler with the debug message _message_. 
- If no message handler has been installed, the message is printed to `stderr`. 
- Under Windows the message is sent to the console, if it is a console application; otherwise, it is sent to the debugger. 
- On QNX, the message is sent to slogger2. 
- This function does nothing if `QT_NO_DEBUG_OUTPUT` was defined during compilation.

上述 Windows 平台的 debugger 概念稍后介绍。

日志框架涉及的几个类型：

```cpp
using QtMessageHandler = void(QtMsgType, const QMessageLogContext &, const QString &);
```

```cpp
// 摘自 qlogging.cpp
static void qDefaultMessageHandler(QtMsgType type, const QMessageLogContext &context,
                                   const QString &message)
{
    bool handledStderr = false;

    // A message sink logs the message to a structured or unstructured destination,
    // optionally formatting the message if the latter, and returns true if the sink
    // handled stderr output as well, which will shortcut our default stderr output.
    // In the future, if we allow multiple/dynamic sinks, this will be iterating
    // a list of sinks.

#if !defined(QT_BOOTSTRAPPED)
# if defined(Q_OS_WIN)
    handledStderr |= win_message_handler(type, context, message);
# elif QT_CONFIG(slog2)
    handledStderr |= slog2_default_handler(type, context, message);
# elif QT_CONFIG(journald)
    handledStderr |= systemd_default_message_handler(type, context, message);
# elif QT_CONFIG(syslog)
    handledStderr |= syslog_default_message_handler(type, context, message);
# elif defined(Q_OS_ANDROID) && !defined(Q_OS_ANDROID_EMBEDDED)
    handledStderr |= android_default_message_handler(type, context, message);
# elif defined(QT_USE_APPLE_UNIFIED_LOGGING)
    handledStderr |= AppleUnifiedLogger::messageHandler(type, context, message);
# elif defined Q_OS_WASM
    handledStderr |= wasm_default_message_handler(type, context, message);
# endif
#endif

    if (!handledStderr)
        stderr_message_handler(type, context, message);
}

// 在 Windows 平台，实际调用 Win32 API OutputDebugString()
static bool win_message_handler(QtMsgType type, const QMessageLogContext &context, const QString &message)
{
    if (shouldLogToStderr())
        return false; // Leave logging up to stderr handler

    QString formattedMessage = qFormatLogMessage(type, context, message);
    formattedMessage.append(QLatin1Char('\n'));
    OutputDebugString(reinterpret_cast<const wchar_t *>(formattedMessage.utf16()));

    return true; // Prevent further output to stderr
}

bool shouldLogToStderr()
{
    static bool forceStderrLogging = qEnvironmentVariableIntValue("QT_FORCE_STDERR_LOGGING");
    return forceStderrLogging || stderrHasConsoleAttached();
}
```

在 Windows 下，在项目的 Build & Run 配置中，如果
- 没有勾选 Run in terminal ，此时 `qDebug()`/ `qWarning()` 等日志就会发送给调试器，在 Qt Creator 的“应用程序输出”窗口打印；
- 勾选了 Run in terminal ， 就会满足 `shouldLogToStderr()` 条件判断，输出到 `stderr` 标准错误， `qDebug()` 就会在终端打印。

关于 `static bool stderrHasConsoleAttached()` 函数：

> Returns true if writing to `stderr` will end up in a console/terminal visible to the user.

如果写入 `stderr` 最终会出现在用户可见的控制台/终端中，则返回 true 。

## Windows 平台

如何理解 `OutputDebugString()` 描述中的 debugger 调试器？ 

> Sends a string to the debugger for display.

-  If the application does not have a debugger, and the filter mask allows it, the **system debugger** displays the string. 
- If the application does not have a debugger and the system debugger is not active, `OutputDebugString` does nothing.

更多细节请到微软网站了解。

## QMessageLogger

> `QMessageLogger` is used to generate messages for the Qt logging framework.

```cpp
// 在 MSVC2015 调试模式下 qDebug() 宏展开
#define qDebug QMessageLogger(__FILE__, __LINE__, __FUNCSIG__).debug
```

上述 `debug()` 成员函数存在多个重载：
- 一类返回 `QDebug()` 对象， `<<` 流式输出日志，在 `~QDebug()` 析构时调用 `qt_message_print()`
- 一类返回 `void`，类似 `printf()` 可变参数函数，内部调用 `qt_message_print()`
