//
// Created by 唐仁初 on 2022/7/20.
//

#ifndef RPCSERVER_CLOSURE_H
#define RPCSERVER_CLOSURE_H

#include <google/protobuf/stubs/callback.h>

class ClosureImpl : public google::protobuf::Closure {
public:

    void Run() override{


    }


};


#endif //RPCSERVER_CLOSURE_H
