//
// Created by Swagger on 2021/9/7.
//

#include "rtspserver.h"


void RtspServer::onConnection(const muduo::net::TcpConnectionPtr &conn) {
    if ( conn->connected() ){
        std::shared_ptr<RtspConnection> rtspConnection(new RtspConnection(this, conn));
        rtspConnectionMap_.emplace(conn->name(), rtspConnection);
    }else {
        rtspConnectionMap_.erase(conn->name());
    }
}


void RtspServer::onMessage(const muduo::net::TcpConnectionPtr conn, muduo::net::Buffer *buf, muduo::Timestamp) {
    rtspConnectionMap_[conn->name()]->handleMessage();
}


int RtspServer::addMediaSession(MediaSession *session) {

    std::lock_guard<std::mutex> lockGuard(sessionMutex_);
    /* 对于已经注册的session，直接返回即可 */
    if ( rtspSuffixMap_.find( session->getRtspUrlSuffix()) != rtspSuffixMap_.end()){
        return 0;
    }

    std::shared_ptr<MediaSession> mediaSession(session);
    int sessionID = session->getSessionID();
    rtspSuffixMap_.emplace(session->getRtspUrlSuffix(), sessionID);
    mediaSession_.emplace(sessionID, std::move(mediaSession));

    return sessionID;
}


