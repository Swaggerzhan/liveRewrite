//
// Created by Swagger on 2021/9/5.
//

#ifndef LIVEREWRITE_RTP_H
#define LIVEREWRITE_RTP_H

#include <iostream>


/*
 *
 *    0                   1                   2                   3
 *    7 6 5 4 3 2 1 0|7 6 5 4 3 2 1 0|7 6 5 4 3 2 1 0|7 6 5 4 3 2 1 0
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |V=2|P|X|  CC   |M|     PT      |       sequence number         |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                           timestamp                           |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |           synchronization source (SSRC) identifier            |
 *   +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
 *   |            contributing source (CSRC) identifiers             |
 *   :                             ....                              :
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 */
struct RtpHeader{
    /* byte 0 */
    uint8_t csrcLen:4;      // CC
    uint8_t extension:1;    // X
    uint8_t padding:1;      // P
    uint8_t version:2;      // V

    /* byte 1 */
    uint8_t payloadType:7;  // PT
    uint8_t marker:1;       // M

    /* byte 2-3 */
    uint16_t seq;           // sequence number

    /* bytes 4-7 */
    uin32_t timestamp;

    /* bytes 8-11 */
    uint32_t ssrc;
};


struct RtpPacket{
    struct RtpHeader header;
    char* payload;
};






#endif //LIVEREWRITE_RTP_H
