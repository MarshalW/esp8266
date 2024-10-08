# dht11 exporter

实现 dht11 prometheus exporter

## 安装步骤

安装库：

- DHT

创建 config.h 文件：

```bash
# 填写 config.h 变量值
# #define SSID "xxxxxx" - wifi ssid
# #define PASS "xxxxxxxxxx" - wifi password
cp config.h.example config.h
```

使用 arduino ide 部署，通过串口监视器查看 ip 地址：

```bash
.......
Connected to GMY-PRINTER
IP address: 192.168.0.203
CC:50:E3:E0:3E:E7
HTTP server started

```

访问：

```bash
curl http://192.168.0.203:8663/metrics
dhtexp_temperature 28.20
dhtexp_humidity 63.00

```

在 wifi 路由器绑定 mac 地址和 ip 地址。

