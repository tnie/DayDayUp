字符串哈，串起来什么样的字符呢？

查一下背景：

> Unicode 至今仍在不断增修，每个新版本都加入更多新的字符。目前最新的版本为 2022 年 9 月公布的 15.0.0，已经收录超过 14 万个字符（第十万个字符在 2005 年获采纳）。

`2^16-1=65535`

> Unicode 最普遍的编码格式是和 ASCII 兼容的 UTF-8，以及和 UCS-2 兼容的 UTF-16。

在表示一个 Unicode 的字符时，通常会用“U+”然后紧接着一组十六进制的数字来表示这一个字符。
- 在**基本多文种平面**里的所有字符，要用四个数字（即 2 字节，共 16 位，例如 `U+4AE0` ，共支持六万多个字符）；
- 在**零号平面**以外的字符则需要使用五或六个数字。

> 目前的统一码字符分为 17 组编排，每组称为平面（Plane），每平面有 65536（2^16）点代码，但目前只用了少数平面。

> 基本多文种平面（Basic Multilingual Plane, BMP），或称基本平面或〇号平面（Plane 0），是统一码的一区块编码，从 0000 至 `FFFF` 。

其他平面基本用不到就是了。

The `QChar` class provides a 16-bit Unicode character.

`QString` stores a string of 16-bit `QChars`, where each QChar corresponds to one UTF-16 code unit. (Unicode characters with code values above 65535 are stored using surrogate pairs, i.e., two consecutive QChars.)

要想高效地使用 `QString` ，还需要关注以下类型和宏：

The `QLatin1String` class provides a thin wrapper around an US-ASCII/Latin-1 encoded string literal.

The `QStringLiteral` **macro** generates the data for a QString out of the string literal `str` at **compile time**.

总结：https://www.cnblogs.com/ybqjymy/p/13948899.html

1. 优先使用 `const char *` 或者 `QLatin1String` 入参的接口；
2. 如果只有 `QString` 入参的接口，就尽可能使用 `QStringLiteral` 宏构造。

