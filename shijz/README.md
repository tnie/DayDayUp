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
