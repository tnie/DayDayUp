---
title: QUdpSocket 的 readyRead 信号失效
categories:
  - cpp
  - qt
date: 2024-04-28 18:47:28
tags:
---

记录在生产环境发现的一个问题，在测试环境上未能复现。

- 生产环境是飞腾主板+银河麒麟 v10 sp1 操作系统，且使用了不知名公司的国产化网卡。
- 测试环境是长城生产的国产化台式机，同样是飞腾主板+银河麒麟 v10 操作系统。
- 虚拟机环境是 x86 Windows10 宿主机，通过 VMware 安装银河麒麟 v10 操作系统。

问题描述：存在 incorrect checksum 报文时，`QUdpSocket` 类型不再 emit `readyRead()` 信号。

<!-- more -->

1. 在软件开发办公室试验，经 TP-LINK 交换机后，飞腾开发机上软件接收数据稳定
2. 在交付检验办公室试验，经设备上交换机后，软件接收数据失败
3. 编译、运行软件的 Qt 版本同为 5.13.2，操作系统大致相同。
4. 同一执行文件从办公室拿到交付空间也会接收失败

重放 bug0319~88.pcap 报文（第 50/110 条 udp 报文的 checksum 是非法的）

在生产环境下，incorrect checksum 的报文会造成 `readyRead` 信号失效：在前 49 条报文正常接收后，不再有 `readyRead` 信号。改用定时器是能够正常接收 109/110 条报文的（非法报文会被 TCP/IP 协议栈过滤掉）

```cpp
//    connect(socket, SIGNAL(readyRead()), this, SLOT(on_readData()));
    connect(timer, SIGNAL(timeout()), this, SLOT(on_readData()));
    timer->start(1000);
```

在测试环境和虚拟机环境上 `readyRead` 信号表现正常，能够收到  109/110 条报文。

- 生产环境 `tcpdump udp[34:2]=0xa841` 能够抓到 110 条
- 测试环境 `tcpdump udp[34:2]=0xa841` 抓到 109 条，非法的过滤掉了

![](https://raw.githubusercontent.com/tnie/MarkdownPhotos/picgo/ebd59081_134031.png)


关闭测试环境的 offload 特性 `ethtool -K enaphyt4i0 rx off` 也能够收到包含 invalid checksum 在内的 110 条报文，通过 `tcpdump -vv` 可以看到 “bad udp cksum” 提示。但是生产环境的 offload 自始至终都是 `on` 启用的，两者对于 invalid checksum 报文的不同表现并不是开关 offload  带来的。

tcpdump 如何过滤 invalid checksum 的报文？

通过这个问题现象，我们大致可以判断 QUdpSocket 是“看到”了 invalid checksum 报文然后出的问题。POSIX `socket()` using `SOCK_DGRAM `是会过滤掉校验和错误的包，难道 QUdpSocket 不是基于 `SOCK_DGRAM`，而是封装的 `SOCK_RAW `吗 ？

['SOCK_RAW' option in 'socket' system call](https://stackoverflow.com/questions/30780082/sock-raw-option-in-socket-system-call)

为了验证判断，试验废掉 checksum 是否能够接收成功呢？
https://stackoverflow.com/questions/41005455/datagrams-seen-in-wireshark-not-received-by-qt-udp-socket
https://forum.qt.io/topic/127199/qudpsocket-doesn-t-trigger-readyread-signal/21
https://forum.qt.io/topic/10956/checksum-setting-in-udp/4
https://wdd.js.org/network/udp-checksum-offload/
socket `SO_NO_CHECK`

# 抓包过滤字节

tcpdump 和 wireshark 过滤字节的语法存在差异：

```
tcpdump udp[34:2]=0xa841
udp.payload[26:2] == A841  # wireshark
tcpdump -i enaphyt4i0  udp[34:2]=0xa841 and dst 224.112.212.11 
```

# 网卡 offload 特性

查看网卡的 offload 特性 `ethtool -k enaphyt4i0`

接收网络报文关闭校验 `ethtool -K enaphyt4i0 rx off`

[How to disable checksums on ethernet card in Windows 10?](https://superuser.com/questions/961617/how-to-disable-checksums-on-ethernet-card-in-windows-10)

> 网卡属性 - 配置 - 高级 - UDP 校验和分载传输（IPv4）


# 数据报 `QNetworkDatagram` 接口有坑

```cpp
QNetworkDatagram datagram(getPayload(), destinationAddress, destinationPort);
if(QString("不要使用以下接口").isEmpty())
{
    const QHostAddress address("192.168.50.221");
    // setSender() 参数和 bind() 绑定的地址及端口必须一致，否则 writeDatagram(const QNetworkDatagram &) 执行失败
    // 建议不要调用 setSender() 此接口：此接口冗余，且容易错用。
    // writeDatagram(const QNetworkDatagram &) 之前的 bind() 操作不会使用 setSender() 的参数
    datagram.setSender(address);
    // 在 Windows 平台下，不能调用 setHopLimit()，因为底层配置 IP_TTL 是非法项造成 writeDatagram(const QNetworkDatagram &) 执行失败
    // https://learn.microsoft.com/en-us/windows/win32/winsock/windows-sockets-error-codes-2
    //  Return code/value: WSAEINVAL
    // https://learn.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-wsasendmsg
    //  Error code: WSAEINVAL
    // https://learn.microsoft.com/en-us/windows/win32/winsock/ipproto-ip-socket-options
    //  Options: IP_TTL
    // https://learn.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-getsockopt
    //  Return value: WSAEFAULT
    datagram.setHopLimit(7);
    // setInterfaceIndex() 和 bind() 冲突似乎没有不良影响。
    // 但如果不是 QNetworkInterface::allInterfaces() 集合内的索引，就会发送失败
    datagram.setInterfaceIndex(119);
    qDebug() << "interfaceIndex is" << datagram.interfaceIndex();
}
```
