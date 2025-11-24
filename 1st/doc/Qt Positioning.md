

# Qt Positioning

Qt 自带的地理方位的库，其中的类型和接口在项目中完全没用，惊呆我了。

经纬度浮点数值的格式化输出，使用 `QGeoCoordinate` 或自定义其子类会方便很多。

`QGeoPath`  可用于航迹；圆形、矩形、多边形就是训练区；

`QGeoPositionInfo` 和 `QGeoPositionInfoSource`  以及 `QGeoPositionInfoSourceFactory` 可用于 AIS 和雷达。