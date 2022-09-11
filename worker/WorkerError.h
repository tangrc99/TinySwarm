//
// Created by 唐仁初 on 2022/9/1.
//

#ifndef TINYSWARM_WORKERERROR_H
#define TINYSWARM_WORKERERROR_H

/// The file tells the error in rpc.

#define SUCCESS "OK"

/// 启动阶段可能出现的错误
#define SERV_NOT_FOUND "SERV NOT FOUND" // 服务文件夹不存在
#define SERV_NO_EXE "EXEC NOT EXIT" // 存在服务文件夹，但是无可执行文件
#define DUP_ERROR "DUP ERROR"   // 重定向出现错误


#define SERV_NOT_RUN "SERV NOT RUN" // 当前服务已经停止

#define SAME_SERV "SAME SERV" // 重复创建服务
#define SAME_ALIAS "SAME ALIAS" // 重复创建服务

#define ALIAS_NOT_FOUND "ALIAS_NOT_FOUND"
#define KILL_ERROR "KILL_ERROR"
#define USR_ERROR "USR_ERROR"

#define MANAGER_DOWN "MAN DOWN" // manager 长时间没有连接

#define PF_IN_ERROR "PF_IN_ERROR"
#define PF_OUT_ERROR "PF_OUT_ERROR"

#endif //TINYSWARM_WORKERERROR_H
