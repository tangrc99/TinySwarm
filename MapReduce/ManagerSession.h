//
// Created by 唐仁初 on 2022/8/31.
//

#ifndef TINYSWARM_MANAGERSESSION_H
#define TINYSWARM_MANAGERSESSION_H

#include "Session.h"
#include "ManagerClient.h"

class ManagerSession : public Session {

public:

    friend class ManagerClient;


    void createService(){

        cli_->SessionCallBack();

    }

    void run(){

    }

private:
    ///
    /// \param ptr
    explicit ManagerSession(std::shared_ptr<Session> &ptr): Session(*ptr){
        // FIXME
        ptr = nullptr;
    }


};


#endif //TINYSWARM_MANAGERSESSION_H
