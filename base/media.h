//
// Created by Swagger on 2021/9/5.
//

#ifndef LIVEREWRITE_MEDIA_H
#define LIVEREWRITE_MEDIA_H

#include <iostream>
#include "rtp.h"

// 这里media只做比较简单的做法，直接从h264中提取nalu单元
class Media{
public:

    Media(std::string filename);
    ~Media();


    /* 封装好的 */
    std::string getSdp();


private:
    /**
     * 此函数暂时作模仿使用
     * @param data 空缓冲区
     * @param len 长度
     * @return 字符串长度
     */
    int getSdp(char* data, int len);

private:
    std::string filename_;
    int fd_;                // 文件指针

    std::string sdp_;       // 缓存

};











#endif //LIVEREWRITE_MEDIA_H
