通过自定义 delegate 格式化输出经纬度

使用 Model/View 的限制：（也可能是我还没学到）

- 列的序号=魔术数字；
- 如何调换列的顺序；
- 修改内容就出错

使用 `QSqlTableModel::setTable()` 结合 QODBC 打开达梦数据库的表时，报错

```
"QODBCDriver::record: Unable to execute column list" 	Error: ""
```

workaround 的思路有几个，需要分别尝试：

- 放弃 `QSqlTableModel::setTable()` 改用 `QSqlQueryModel` 
- 放弃 QODBC ，直接使用达梦的原生驱动。 How to Write Your Own Database Driver

> **Note**: You should use the native driver, if it is available, instead of the ODBC driver. 
> ODBC support can be used as a fallback for compliant databases if no native driver is available.

# 典型场景

和用户的交互有两种方式：文字、图形。

1. 用户在表格中输入、修改（或者在对话框中）
2. 在表格输入、修改、选中的同时，在图上呈现（只读：用户和图不存在互动）
3. 直接在图上输入、修改、选中的同时，在表格中呈现
4. 图上多个要素叠加怎么处理

每个交互场景下，都会存在多种业务场景。

# sqlite3

在 Qt 的数据库框架中，存在”默认连接“概念：

> The connection defined above will be the *default* connection, because we didn't give a connection name to `addDatabase()`. 

是否支持 `QVariant QSqlQuery::lastInsertId() const` 接口，是由 Qt 封装的
还是底层数据库直接返回的的？在 sqlite 底层支持 Last Insert Rowid 的基础上 Qt 做了封装。

在 sqlite3 设计表时保持什么约定（在什么前提下）才能够返回上述 id 呢？参考其
rowid 概念。简单来说 `rowid` 是个隐藏列，如果用户自行定义了 `INTERGER PRIMARY KEY` 列
（声明一字不差才行）那么这一列只是 rowid 的别名 alias。 rowid 列支持自动生成。

> The Qt SQLite driver will retry to write to a locked resource until it runs into a timeout (see `QSQLITE_BUSY_TIMEOUT` at `QSqlDatabase::setConnectOptions()`).

以下描述说的模棱两可，能不能用？有什么具体的限制？异常时什么现象？

> 	The driver is locked for updates while a select is executed. This may cause problems when using `QSqlTableModel` because Qt's item views fetch data as needed (with ~~`QSqlQuery::fetchMore()`~~  `QSqlQueryModel::fetchMore()` in the case of `QSqlTableModel`).

Detailed Description of `QSqlQueryModel`

> If the database doesn't return the number of selected rows in a query（比如 Sqlite）, the model will fetch rows incrementally. See `fetchMore()` for more information.

数据量很大的时候， QSqlQuryModel 会自动获取更多吗？ 不会！由用户触发，每次追加 `#define QSQL_PREFETCH 255` 条记录。

- 如果会自动获取，总有完毕的时候吧。那被锁的另一进程应该会有写入的机会，实际测试发现另一进程等待 9 分钟后，Sqlite 数据库依然处于锁定的状态。等待 9 分钟后，下拉占用数据库的窗口手动将数据加载完毕，另一进程马上更新库成功。 => 不会自动获取更多的数据，肯定按需加载啊，不然大数据量表直接内存爆炸。
- 如果不会自动获取，反而一直（`stmt` 句柄未释放）锁着 Sqlite 更不合理。√

我对数据库的理解太少了！增删改可以类比成 http 的简单问答；而查询其实更接近 http 的长连接或者 chunk 分块传输，过程中可能会存在多次交互，客户端未拿完数据或未明确关闭，数据库就一直被占用着，并非一次性把数据吐给客户端就释放了。

接口 `bool QSqlQuery::isActive() const` ，活跃的 `SELECT` 查询会造成事务提交或回滚失败。

> An *active* `QSqlQuery` is one that has been `exec()`'d successfully but not yet finished with. 

但是，单一连接下为什么活跃的 `SELECT` 查询不会造成其他写入操作失败呢？

```cpp
// Qt Sqlite Driver Feature
hasFeature( "Transactions" ) true
hasFeature( "QuerySize" ) false
hasFeature( "BLOB" ) true
hasFeature( "Unicode" ) true
hasFeature( "PreparedQueries" ) true
hasFeature( "NamedPlaceholders" ) true
hasFeature( "PositionalPlaceholders" ) true
hasFeature( "LastInsertId" ) true
hasFeature( "BatchOperations" ) false
hasFeature( "SimpleLocking" ) true
hasFeature( "LowPrecisionNumbers" ) true
hasFeature( "EventNotifications" ) true
hasFeature( "FinishQuery" ) true
hasFeature( "MultipleResultSets" ) false
hasFeature( "CancelQuery" ) false
```
Qt 中 sqlite 查询时遇到文件被锁，默认的超时时长经测试大概是 5 秒。在哪里配置的呢？

为什么 `QSqlQueryModel` 会一直锁着 sqlite 呢，另一进程完全没机会更新数据库？分情况：

- 当 sqlite 表数据量少，`QSqlQueryModel` 一次性加载完毕；或者数据量大，下滑滚动条将数据加载完毕后，另一进程可以正常更新数据库。
- 如果数据没有加载完毕，另一进程无法更新数据库。