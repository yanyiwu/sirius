# sirius

## Introduction

Near-Duplicate Checking for Papers(Title and Abstract).

## Feature 

+ use [CppJieba] as chinese word segmenter.
+ support `utf-8` encoding.

## Usage

```
mkdir build
cd build
cmake ..
make
```

### start server

```
./bin/SiriusServer ../conf/sirius_server.conf
```

### client example

```
curl -d @- "http://127.0.0.1:11200/" < ../test/testdata/curl_post_data.utf8
```


## Contact

wuyanyi09@foxmail.com

[CppJieba]:https://github.com/aszxqw/cppjieba
