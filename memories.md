# 设计备忘录

- TinySwarm 主要针对的是短期job的调度
- 为什么 manager 不直接跟 worker 通信来获取当前的状态呢？因为worker上只存储了最基本的pod信息，而在 pod 基础上派生出的service等概念不会在worker上面体现出来

## 缺点，且不打算进行完善


## 做到一半没有做完的
- manager/Result.h 还没有写完
- manager 部分，将用户rpc与manager节点中加入一个 中间类 ServiceManager 用来处理用户逻辑中的服务概念，并且进行多个rpc的分布式操作

## 比较困难，但是打算进行完善的
- manager 节点处的 proxy 代理
- manager 节点的抽象还不够，需要加入pod - service - service_group 的抽象；一个service可以具有多个pod，一个service_group可以有多个 service

## 未来需要加入的新功能
- 节点内部的各个功能需要进行分块处理，防止某个.cpp文件过大，不方便阅读
- 存活探针机制，如果 worker 节点发现某个进程陷入死循环等，需要主动将其退出，并且报告错误；需要参考k8s的具体实现
- worker 节点需要具备自己的信息，如本地部署服务的信息，版本号；自身 CPU 的利用率；物理分区等情况
- worker 上的服务需要进行状态的区分，就绪，正在运行，掉线等状态 --- 就绪是发起创建，但没有fork 只要在 manager 处更新就可以了
- worker 节点远程拉取服务文件
- 错误文件信息可以放入到数据库中存储，然后定时删除
- k8s 内部具有占有硬盘空间的限制，这个还没有加入，应该定期清理占用的硬盘空间

## 当前还没有解决掉的问题
- gossip 节点的传播策略并不是最优化的
- Manager 节点的任务还没有加入定时功能
- 目前fork采用的是同步rpc，启动时间长地会卡顿，但是我们的启动都很快
- worker 节点崩溃重启后，如何处理之前进程留下的文件
- 对于worker节点只会存储节点的启动信息，也就是说不会对节点的输出文件进行管理，如果节点意外退出将会造成无法恢复

## 已经解决掉的问题
- worker 节点需要具备恢复能力，也就是 manager 节点如果在两次check中间，worker实现了掉线-恢复过程，那么应该怎么处理？
    首先，是不能直接进行恢复服务的，尝试在收到manager的check时再对服务进行恢复


## worker 与 manager 节点的机制
- worker 节点会定期清除大于 5s 的失活连接，若manager 失活后，又重新与 worker 通信，那么worker 会通知一个 manager down 错误，让 manager 节点将所有的服务清除。
- manager 节点在收到 manager down 错误后，会将该结点上的所有服务下线，然后再重试本次 RPC
- manager 会定时与 worker 节点进行通信，收集错误信息，并且将对应的节点下线
- manager 内部会定期尝试将下线服务重启，策略为: 若对应 worker 失活，则转移，否则在之前的 worker 上重启

## ServiceRecord
- worker 节点上的持久化机制，主要用来解决worker node 意外崩溃的情况。所以，一旦有进程以任何方式退出，都应该清除掉 ServiceRecord。