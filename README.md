# sirius 中文文档查重服务

## 简介

中文文档(比如论文)，查重服务(根据 title 和 content )，虽然程序代码仓促简陋，但是也算是五脏俱全吧。

## 特性

+ 使用 [CppJieba] 作为中文分词组件.
+ 支持 `utf-8` 编码.

## 用法

```
mkdir build
cd build
cmake ..
make
```

### 启动服务

```
./bin/SiriusServer ../conf/sirius_server.conf
```

### HTTP 访问测试( PUT 方法)

```
curl -d @- "http://127.0.0.1:11200/" < ../test/testdata/curl_post_data.utf8
```


## 联系

wuyanyi09@foxmail.com

[CppJieba]:https://github.com/aszxqw/cppjieba
