
Qt [官方下载页面](https://www.qt.io/offline-installers)  离线安装包只支持 5.12.12 ，后续版本不再提供离线安装包。

使用在线安装工具，默认只展示最新的三个 Qt 版本供用户安装。右侧筛选项，**勾选 Archive** 后筛选就会列出旧版本的 Qt 。

# 从源码构建 Qt

如果要在不联网的设备上使用新版本 Qt ，还是需要掌握如何编译 Qt 源码！
之前想断点进 Qt 源码调试一直没成功，借这次机会一并实现。

从源码开始构建 Qt https://doc.qt.io/qt-5/windows-building.html

按部就班做就可以，Step 3 的 ICU 和 ANGLE 三方库并不是必要项目，其他中文教程中常见的 perl/python 也不需要。

- SSL 可能也不需要，但我电脑之前其他项目需要已经配置了，未作关注

Step 4 提供的配置命令太保守，我们可以更激进一些，将不需要的目录全部跳过

configure -debug -nomake examples -nomake tests -opensource -skip qt3d -skip qtactiveqt -skip qtandroidextras -skip qtcharts -skip qtconnectivity -skip qtdatavis3d -skip qtdeclarative -skip qtdoc -skip qtgamepad -skip qtgraphicaleffects -skip qtimageformats -skip qtlocation -skip qtlottie -skip qtmacextras -skip qtmultimedia -skip qtnetworkauth -skip qtpurchasing -skip qtquick3d -skip qtquickcontrols -skip qtquickcontrols2 -skip qtquicktimeline -skip qtremoteobjects -skip qtscript -skip qtscxml -skip qtsensors -skip qtserialbus -skip qtserialport -skip qtspeech -skip qtsvg -skip qttools -skip qttranslations -skip qtvirtualkeyboard -skip qtwayland -skip qtwebchannel -skip qtwebengine -skip qtwebglplugin -skip qtwebsockets -skip qtwebview -skip qtwinextras -skip qtx11extras 
？-no-dbus -no-opengl 

即便如此，在 x240 上通过 nmake 构建依然在一小时以上（报错中止了）

改用 jom ，半小时构建完毕。

> jom is a clone of nmake to support the execution of multiple independent commands in parallel. 

暂时不构建文档 Step 5

# 不用 CDB 调试

使用 Qt Creator 调试 msvc 构建的程序时，必须安装 CDB （没有单独的安装包，要把 WDK 全部装上，占用 2.1G (lll￢ω￢)

那我只用 Qt Creator 借用 gdb 调试

我改用 MSVC 调试 qt 程序就不用 cdb 了吧？

# cannot run 'rc.exe'

在安装 Qt 5.12.12 + MSVC 2015 之后，再次安装 MSVC2019 ，可能会碰到 LNK1158: cannot run 'rc.exe' 错误造成编译失败。

社区关于此问题的 [讨论和 workaround](https://forum.qt.io/topic/90839/lnk1158-cannot-run-rc-exe) 

排查发现 `echo %WindowsSdkDir%` 环境变量指向的 `C:\Program Files (x86)\Windows Kits\10\` 子目录内下没有 rc.exe 

刨根问底 [rc.exe no longer found in VS 2015 Command Prompt][1]

[1]:https://stackoverflow.com/questions/43847542/rc-exe-no-longer-found-in-vs-2015-command-prompt