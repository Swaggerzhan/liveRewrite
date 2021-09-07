//
// Created by Swagger on 2021/9/7.
//

#ifndef LIVEREWRITE_MEDIASESSION_H
#define LIVEREWRITE_MEDIASESSION_H

#include "rtp.h"
#include <iostream>
#include <map>

class Media;

class MediaSession {
public:

    static MediaSession* CreateMediaSession();

    ~MediaSession();


    /**
     * 向MediaSession中添加对应的音频源
     * @param channelID video or audio
     * @param mediaSource 对应音频解析类
     * @return
     */
    bool addMediaSource(MediaChannel channelID, Media* mediaSource);


private:

    MediaSession();


private:

    std::string sdp_;



};


#endif //LIVEREWRITE_MEDIASESSION_H
