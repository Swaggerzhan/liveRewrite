//
// Created by Swagger on 2021/9/5.
//

#ifndef LIVEREWRITE_RTSPCONNECTION_H
#define LIVEREWRITE_RTSPCONNECTION_H

#include <memory>



class Media;
class RtspRequest;
class RtspRespond;

class RtspConnection{
public:

    RtspConnection();
    ~RtspConnection();


    // 处理describe请求
    void handleCmdDescribe();



private:
    std::unique_ptr<RtspRequest> rtspRequest_;
    //std::unique_ptr<RtspRespond> rtspRespond_;
    std::unique_ptr<Media> media_;

};


#endif //LIVEREWRITE_RTSPCONNECTION_H

