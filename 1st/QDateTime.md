
在 Qt 中虽然存在 `QTimeZone` 类型，但其官方手册中多次强调了用户**不应**直接使用 `QTimeZone` ，而是结合 `Qt::TimeSpec` 枚举值使用 `QDateTime` 。

> This class is primarily designed for use in QDateTime; most applications will not need to access this class directly and should instead use `QDateTime` with a `Qt::TimeSpec` of `Qt::TimeZone`.

> If you require a `QDateTime` that uses the current system time zone at any given moment then you should use a `Qt::TimeSpec` of `Qt::LocalTime`.

Time Zone Offsets 时差

> The total offset is comprised of two component parts, the standard time offset and the [daylight-saving time][1] offset. 夏时制

[1]:https://en.wikipedia.org/wiki/Daylight_saving_time

# Qt::TimeSpec 时间规格 

You can use `timeSpec()` to find out what time-spec a `QDateTime` object stores its time relative to.

| 枚举值 | `enum Qt::TimeSpec`  意义|
|--|----|
|`Qt::LocalTime`| Locale dependent time (Timezones and Daylight Savings Time). |
| `Qt::UTC` | Coordinated Universal Time, replaces Greenwich Mean Time. |
| `Qt::OffsetFromUTC` | An offset in seconds from Coordinated Universal Time. |
| `Qt::TimeZone` | A named time zone using a specific set of Daylight Savings rules. |

完整的时区概念，远比 UTC+ 复杂：前者还要处理地区、时区名称，以及夏时制等等。

`QDateTime QDateTime::currentDateTime()`

> Returns the current datetime, as reported by the system clock, **in the local time zone**.

`QDateTime QDateTime::currentDateTimeUtc()`

> Returns the current datetime, as reported by the system clock, **in UTC**.

# QDateTime 时间

`QDateTime` 用来抽象时间的概念，它至少有两个成员：

1. timespec 时间规格，类似与选取某个时刻作为参考点（当然远比这个复杂）
2. offset 基于前者的偏移量

有两个和 timespce 有关的接口，注意区分：

- `void setTimeSpec(Qt::TimeSpec spec)` ，只修改 timespec 不改动偏移量，所以就指向另一时刻了。
- `QDateTime toTimeSpec(Qt::TimeSpec spec) const` ，新建对象使用另一参考系指示相同的时刻。

事实上，`toTimeSpec()` 是老接口，估计当时 timespec 还没有 `Qt::OffsetFromUTC `和 `Qt::TimeZone` 。

建议使用新接口：`toUTC()`, `toLocalTime()`, `toOffsetFromUtc(offset)` and `toTimeZone(zone)`

# QDateTimeEdit 控件

`QDateTimeEdit` 控件可以指定 time-spec ，默认是 `Qt::LocalTime` 。
通过 `void setDateTime(const QDateTime &dateTime)` 设置其时间时，**忽略**入参的 time-spec ，延用自身的 time-spec 。

> When setting this property the timespec of the `QDateTimeEdit` remains the same and the timespec of the new `QDateTime` is ignored.

副作用：当两者 timespec 不一致时，getter 返回值和 setter 入参不相等。

所以，它的 `timeSpec` 属性**不参与**更新时间的过程。那有什么用呢？影响时间的呈现方式？

手册没有描述其详细功能！我原本以为其表现和 `void QDateTime::setTimeSpec()` 一致 —— 会生成一个不同的时间。但试验发现两者表现并不一致，反而和 `QDateTime::toTimeSpec()` 表现相仿：

1. 在更新时间时，**保证**控件和入参的 **timespec 一致**
2. 更新时间后，通过修改控件的 timespec （切换 `Qt::LocalTime` 和 `Qt::UTC`）会以不同规格展示**相同**的时间；
3. 但 timespec 改成 `Qt::OffsetFromUTC` 反而不再是同一时间。困惑

Qt 真难用啊，不看手册不敢用（担心有坑/用错），看了手册不会用（各种冲突/不一致），写 Demo 验证，5 个小时过去了……去尼玛的，造轮子都用不了这么久。

**核心需求**：三个 `QDateTimeEdit` 控件，分别展示本地时间、世界时间（UTC）和北京时间，如何通过单一 `QDateTime` 进行赋值且保证三者的 `->dateTime()` 相等？

Qt 如何抽象的呢？本地时间、世界时间和北京时间是同一时刻的不同表现形式？还是同一时刻的不同记录方式？

**结论**： `QDateTimeEdit` 不支持 `Qt::OffsetFromUTC` 和 `Qt::TimeZone` 。这是 Qt 的 bug ，在 6.7 版本中才修复。[问题报告](https://bugreports.qt.io/browse/QTBUG-80417)

## 其他

关于 `void QDateTime::setTimeSpec(Qt::TimeSpec spec)` 手册：

> Sets the time specification used in this datetime to `spec`. The datetime will refer to a **different point in time**. 不再是同一时间
>
>If `spec` is `Qt::OffsetFromUTC` then the `timeSpec()` will be set to `Qt::UTC`, i.e. an effective offset of 0.
> 
> If spec is `Qt::TimeZone` then the spec will be set to `Qt::LocalTime`, i.e. the current system time zone.

# 夏时制

缘由：工业化社会通常会遵循以时间为基础的日常活动行程，而这些行程在一年当中不会发生变化。

如果将某个地区的所有时间进行重设，从而比标准时间提早一小时，这样**遵循年度行程**的人们将会比正常情况提早一个小时醒来；他们将会提早一个小时完成日常工作行程，并且在工作活动之后提供额外的一个小时。