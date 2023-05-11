# FTS2.0
文件传输服务器的2.0版本
使用libevent的监听事件，监听socket fd的读操作和写操作

## Reactor
可是设置定时事件，用来销毁remove list 中的 handler和worker

## Handler
1. 一次连接可以有多次请求，每次请求的流程read->work->write

    read后删掉read_event，write完成后加上read_event

    写的时候添加erite_event，写完删掉write_event

2. 设置超时断开连接

    Handler设置超时事件，超过一定时间后，主动断开连接，长时间没有请求

    可能出现的问题：断开连接后，销毁Handler，线程池里面可能有该任务
    
    暂时解决方案：write后添加或者重置超时时间，设置变量，判断是否有任务

## Acceptor
保存下客户端信息，使用hashmap，key: Handler* value: sockaddr_in*

## 存在问题
1. 多线程下有bug，发送文件的大小有错误，导致提前结束
解决方法：worker线程设置write_buff时，导致handler线程的的条件出错，给线程加锁

2. EV_WRITE采用边缘触发时，手动激活无效，导致无法发送文件
问题分析：自己收到激活可写事件时，可以已经处于可写事件中，但是并未写任何内容（空写，此时还未写完，连接后默认会有两次空写，暂时不知道为什么），导致这次激活无效
解决方法：1. 暂时采用水平触发 2. 写的时候再注册，即写缓冲区满的时候，或者第一次写的时候注册

## 进展
已有的测试都已完成