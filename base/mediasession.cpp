//
// Created by Swagger on 2021/9/7.
//

#include "mediasession.h"
#include "media.h"


using std::map;
using std::string;

MediaSession* MediaSession::CreateMediaSession() {
    return new MediaSession;
}

MediaSession::MediaSession()
:   sdp_(),

{

}

MediaSession::~MediaSession() {

}


bool MediaSession::addMediaSource(MediaChannel channelID, Media *mediaSource) {

    mediaSource->setSendFrameCallBack( [this](MediaChannel channelID, RtpPacket pkt){
        std::forward<std::shared_ptr<RtpConnection>> rtpConnList;
        map<string, RtpPacket> packets;
        {
            //TODO: lock_guard
            for (){ // TODO:遍历每个RTP connection

            }
        }


        return true;
    });
}








