//
// Created by Swagger on 2021/9/5.
//

#ifndef LIVEREWRITE_RTSPREQUEST_H
#define LIVEREWRITE_RTSPREQUEST_H


#include <unordered_map>
#include "rtp.h"
#include "muduo/net/Buffer.h"

class RtspRequest{
public:

    typedef std::pair<std::string, uint32_t> HeaderValue;

    enum Method {
        Option = 0,
        Desc,
        Setup,
        Play,
        Teardown,
        None,
    };

    enum RtspRequestParseState{
        ParseRequestLine = 0,
        ParseRequestHeader,
        Done
    };

    RtspRequest();
    ~RtspRequest();


    /**
     * 这里才是RtspRequest入口
     * @param buffer
     * @return
     */
    bool parseRequest(muduo::net::Buffer* buffer);

    /* 重置rtspRequest信息 */
    void reset();

    /* 将header中的CSeq转为int32_t类型 */
    int32_t getCSeq();


    ///////// 一下基本为创建RTSP请求响应相关函数
    /**
     *
     * @param data 空缓冲区
     * @param len 缓冲区长度
     * @return 字符串长度
     */
    int buildOptionRes(char* data, int len);

    /**
     *
     * @param data 空缓冲区
     * @param len 缓冲区长度
     * @param sdp 需要生成的sdp数据，
     * 这个数据一般需要从外部传进来，需要有session的信息
     *
     * @return 字符串长度
     */
    int buildDescribeRes(char* data, int len, const char* sdp);

private:

    ///////////// 内置的解析RTSP协议的函数

    /**
     * 解析请求行
     * @param start
     * @param end
     * @return success for true and fail for false
     */
    bool parseRequestLine(const char* start, const char* end);

    /**
     * 解析请求头
     * @param start
     * @param end
     * @return success for true and fail for false
     */
    bool parseHeader(const char* start, const char* end);



    bool parseCSeq(std::string& message); /* 所有请求都需要解析 */
    bool parseAccept(std::string& message);

    /**
     * 用于解析Setup请求中的Transport字段，获取对应的频道
     * @param message
     * @return
     */
    bool parseTransport(std::string& message);
    bool parseSessionID(std::string& message);



private:

    Method method_;     // 请求方法
    TransportType transportType_;   // 传输方法
    std::string url_;
    std::string version_;
    RtspRequestParseState state_;   // 目前状态

    /* header 和对应字段，int数据则使用int32_t存储，std::string将被弃用 */
    std::unordered_map<std::string, std::pair<std::string, int32_t>> headers_;


};





#endif //LIVEREWRITE_RTSPREQUEST_H
