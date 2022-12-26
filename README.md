# TinySwarm

TinySwarm 主要是用于简单的分布式调度，实验室具有一个算法评估软件，每次运行该软件的时候，我们都需要手动从数据库里面选择算法运行，然后再将算法的运行结果放到这个软件里面。这样就非常麻烦。tinyswarm 被集成到了评估软件里面，评估软件通过在Nvidia Jetson AGX 嵌入式平台上启动 yolo 服务器，然后再通过一个mapreduce manager 来划分任务，这样就可以实现算法评估的自动化，并且大大节约时间。

目前，TinySwarm 已经实现了如下功能：

- 一次性启动多个用户服务，并自动选择节点进行调度
- 用户服务状态检测，自动转移状态异常的服务实例
- tcp 存活探针检测用户服务的网络状态
- ipvs 与 Nginx 分别实现了传输层和应用层的负载均衡
- 集群访问和单实例访问两种用户服务访问模式

## worker 节点

worker 节点是运行在 AGX 上面的 server，负责创建可以调度的pod，即一个进程，然后对进程进行监控。这一级主要是对进程进行抽象，一个进程是一个pod，并且具有相应的描述符，启动参数等。

worker 节点一共具有以下几个模块，分别负责不同的功能：

- 进程启动模块：负责处理 manager 的 rpc 请求，使用给定的参数来启动进程
- 启动信息持久化模块：记录进程的启动信息，用于进程的重启操作
- 进程输出管理模块：负责重定向进程的输出到不同的文件，清理已经失效的输出文件
- 进程状态检测模块：负责利用 tcp 探针来检测进程的工作状况
- 进程退出信息收集模块：负责收集进程的退出信息，并准备上报给 manager 节点
- 注册模块：负责管理 etcd 的注册功能，用于服务发现

worker 节点的方式如下：

```c++
// 初始化 log4cxx 
auto log_controller = LoggerController::getInstance();
log_controller.init("/properties/log4cxx.properties", "fileappender");

// 指定工作目录，启动地址和 etcd url
worker::WorkerNode node("work_dir","0.0.0.0:10001","127.0.0.1:2379");
node.start();
```

使用 tinyswarm 时，需要提前将二进制文件按照如下格式放入到工作目录中：

```
work_dir
├── service1
│   └── service1
└── service2
    └── service2
```

## manager 节点

manager 节点比较复杂，主要负责处理用户请求，管理进程信息，可以分为以下几个模块：

- 服务抽象层：负责处理用户的请求，与代理模块、进程调度模块、
- 进程调度模块：负责与 worker 节点通信，发送启动、调度等命令。
- 代理模块：负责使用 Nginx 或 ipvs 工具搭建集群的统一访问方式。
- 发布模块：负责处理与其他节点的信息交互
- 存储模块：提供无锁数据结构，用于进程内的信息存储
- 日志管理模块：负责持久化客户端请求

manager 节点的启动命令如下：

```c++
// 初始化 log4cxx 
auto log_controller = LoggerController::getInstance();
log_controller.init("/properties/log4cxx.properties", "fileappender");

// 指定启动地址和 etcd url
manager::ServiceManager manager("0.0.0.0:10000","127.0.0.1:2379");
manager.run();
```

