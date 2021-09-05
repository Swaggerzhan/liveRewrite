//
// Created by Swagger on 2021/9/5.
//

#include "rtspconnection.h"
#include "rtsprequest.h"
#include "media.h"

using std::string;

RtspConnection::RtspConnection()
:   rtspRequest_(new RtspRequest),
    media_(new Media("test_file")) // for test
{

}

RtspConnection::~RtspConnection() {

}

void RtspConnection::handleCmdDescribe() {


    char* tmp_test_data = new char[1024]; // 缓冲区
    string sdp = media_->getSdp();
    int len = rtspRequest_->buildDescribeRes(tmp_test_data, 1024, sdp.c_str());


}




