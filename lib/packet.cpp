#include "common.h"
#include "packet.h"

Packet::Packet() {
    pkt.data = NULL;
    pkt.size = 0;
    av_init_packet(&pkt);
}

Packet::~Packet() {
    av_free_packet(&pkt);
}

void Packet::set(int size,uint8_t * data,int stream_index,int flags) {
    pkt.data = data;
    pkt.size = size;
    pkt.stream_index = stream_index;
    pkt.flags |= flags;
}

void Packet::set(int pts,int dts,int duration,int stream_index) {
    pkt.pts = pts;
    pkt.dts = dts;
    pkt.duration = duration;
    pkt.stream_index = stream_index;
}

Packet::operator AVPacket*() {
    return &pkt;
}

 int Packet::streamIndex() const {
    return pkt.stream_index;
}


Packet::operator int() const {
    return pkt.size;
}
