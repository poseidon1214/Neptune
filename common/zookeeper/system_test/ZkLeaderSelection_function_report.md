# ZkLeaderSelection功能测试报告
## 作者：foreveryang


## 1. 参数问题
### 1.1 zk_host
* 格式错误

./select_leader_async --host=abc

    返回 -1
    log "Invalid argument"
    done 不变
    on_expired_seesion_event 不变

* zk集群问题

./select_leader_async --host=1.1.1.1:1111

    返回 -1
    log "Fail to wait connected event, host: 1.1.1.1:1111"
    done 不变
    on_expired_seesion_event 不变
  
### 1.2 path
* 格式错误

./select_leader_async --host=10.123.14.27:2181 --path=abc

    返回 ZBADARGUMENTS
    log "Fail to create abc, bad arguments"
    done  不变
    on_expired_seesion_event 不变

* path不存在

 ./select_leader_async --host=10.123.14.27:2181 --path=/fdafsda

    返回 ZNONODE
    log "Fail to create /fdafsda, no node"
    done 不变
    on_expired_seesion_event 不变

* path是一个节点：同格式错误
### 1.3 credential
* 没有权限

./select_leader_async --host=10.123.14.27:2181 --path=/test_auth/n_

    返回 ZNOAUTH
    log "Fail to create /test_auth/n_, not authenticated"
    done 不变
    on_expired_seesion_event 不变


## 2. 同步调用
场景：zk_host,path,credential均正确，done设置为null。select_leader_sync有重试逻辑，选主返回失败，3s后会重新发起选主。

* 成功

./select_leader_sync --host=10.123.14.27:2181 --path=/test_auth/n_ --credential=digest:u1:p1

	返回 ZOK
* 等待-成功

[1]./select_leader_sync --host=10.123.14.27:2181 --path=/test_auth/n_ --credential=digest:u1:p1 &

[2]./select_leader_sync --host=10.123.14.27:2181 --path=/test_auth/n_ --credential=digest:u1:p1 &

    操作步骤：start [1]; start [2]; kill [1]; kill [2]
    [1]  返回 ZOK
    [2]  kill [1] 之后返回 ZOK

* 等待-杀死

[1]./select_leader_sync --host=10.123.14.27:2181 --path=/test_auth/n_ --credential=digest:u1:p1 &

[2]./select_leader_sync --host=10.123.14.27:2181 --path=/test_auth/n_ --credential=digest:u1:p1

    操作步骤：start 1；start 2； kill 2
    [2] kill -2 无法杀死

* 失败-成功

./select_leader_sync --host=10.123.14.27:2181 --path=/test_tmp_node/n_

    操作步骤：启动；创建节点/test_tmp_node
    创建节点之前返回错误
    创建之后返回成功
## 3. 异步调用
场景：zk_host,path,credential均正确，done设置为一个callback函数。select_leader_async有重试逻辑，选主返回失败，3s后会重新发起选主。

* 成功

./select_leader_async --host=10.123.14.27:2181 --path=/test_auth/n_ --credential=digest:u1:p1

    返回 ZOK
    done 返回ZOK

* 等待-成功

[1]./select_leader_async --host=10.123.14.27:2181 --path=/test_auth/n_ --credential=digest:u1:p1 &

[2]./select_leader_async --host=10.123.14.27:2181 --path=/test_auth/n_ --credential=digest:u1:p1 & 

    操作步骤：start 1; start 2; kill 1; kill 2
    [1] 返回ZOK，done 返回 ZOK
    [2] 返回ZOK，kill 1之后done返回ZOK
    
* 等待-杀死

[1]./select_leader_async --host=10.123.14.27:2181 --path=/test_auth/n_ --credential=digest:u1:p1 &

[2]./select_leader_async --host=10.123.14.27:2181 --path=/test_auth/n_ --credential=digest:u1:p1

    操作步骤：start 1；start 2；kill 2
    [2] 正常退出

* 失败-成功

./select_leader_async --host=10.123.14.27:2181 --path=/test_tmp_node/n_

    操作步骤：启动；创建节点/test_tmp_node
    创建节点之前：返回错误，done返回错误，on_expired_seesion_event不变
    创建节点之后：返回ZOK，done返回ZOK

## 4. 异常情况
### 4.1 expired session event
说明 挂起（ctrl+z）再恢复会收到expired session event
#### 4.1.1 同步
场景：on_expired_seesion_event 被设置为null

* 当选之前异常

[1] ./select_leader_sync &

[2] ./select_leader_sync &
    
    操作步骤：start 1；start 2；fg；ctrl+z;fg;kill 1;kill 2
    [1][2]均正常退出
    [2]在收到异常后，返回失败，根据逻辑重试
* 当选之后异常：

./select_leader_sync
    
    操作步骤：start 1; ctrl+z; fg;
    由于没有设置on_expired_seesion_event，打出FATAL，程序coredump
    
#### 4.1.2 异步
场景：on_expired_seesion_event 被设置为一个回调函数

* 当选之前异常

[1] ./select_leader_async &

[2] ./select_leader_async &
    
    操作步骤：start 1；start 2；fg；ctrl+z;fg;kill 1;kill 2
    [1][2]均正常退出
    [2]在收到异常后根据逻辑重试
* 当选之后异常：

./select_leader_async
    
    操作步骤：start ; ctrl+z; fg;
    on_expired_seesion_event被调用，根据逻辑重新参与选主

### 4.2 参与选主的节点被删除
说明：选主的第一步是创建节点，可以手动去zk上面删除此节点
#### 4.2.1 同步
场景：on_expired_seesion_event 被设置为null

* 当选之前删除

[1] ./select_leader_sync &

[2] ./select_leader_sync &
    
    操作步骤：start 1；start 2；fg；delete node 2;kill 1;kill 2
    [1][2]均正常退出
    [2]在检查时发现自己被删除，返回错误，之后按照逻辑重新选主成功

* 当选之后删除

./select_leader_sync

    操作步骤：start；delete node
    由于没有设置on_expired_seesion_event，打出FATAL，程序coredump
#### 4.2.2 异步
场景：on_expired_seesion_event 被设置为一个回调函数

* 当选之前异常

[1] ./select_leader_async &

[2] ./select_leader_async &
    
    操作步骤：start 1；start 2；delete node 2;kill 1;kill 2
    [1][2]均正常退出
    [2]在检查时发现自己被删除，返回错误，之后按照逻辑重新选主成功
* 当选之后异常：

./select_leader_async
    
    操作步骤：start ; delete node
    on_expired_seesion_event被调用，根据逻辑重新参与选主

### 4.3 ZK集群挂了
说明：sh bin/zk_stop

zk handler 会收到connecting状态，这里没有做任何处理，zk恢复后创建的节点还在，这里就当无感知