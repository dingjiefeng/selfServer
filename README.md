# selfServer
[![license](https://img.shields.io/github/license/mashape/apistatus.svg)](https://opensource.org/licenses/MIT)
## 简介
本项目计划用Cpp 11编写linux平台的web服务器，项目计划由浅入深，由完善功能到逐步提高性能

## 工程环境
本项目系统环境为deepin 15,在Clion下进行代码编写的工作,目前上传的也是Clion工程的版本,集合了单元测试等工具

## 基本组件
- ThreadPool.h : C++ 11 实现的线程池


## 目录结构

.
|
├── CMakeLists.txt \
├── lib \
│   └── googletest \
│       ├── CMakeLists.txt \
│       ├── googlemock \
│       └── googletest \
├── LICENSE \
├── main.cpp \
├── problems.md  \
├── push2master.sh  \
├── README.md  \
├── server  \
│   └── base  \
│       ├── base.h \
│       ├── CountDownLatch.cpp  \
│       ├── CountDownLatch.h    \
│       ├── FileUtil.cpp    \
│       ├── FileUtil.h  \
│       ├── LogFile.cpp \
│       ├── LogFile.h   \
│       └── ThreadPool.h    \
├── Tests   \
│   ├── outputFile.txt  \
│   ├── test_CountDownLatch.cpp \
│   ├── test.cpp    \
│   └── test_FileUtil.cpp   \
└── 更新记录.md




