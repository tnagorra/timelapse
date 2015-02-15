#ifndef PACKET_H
#define PACKET_H

#include "common.h"

class Packet {
private:
    AVPacket pkt;

public:
    Packet(const Packet&) = delete;

    /// Allocates packet to NULL data and initializes the default parameters
    Packet();

    /// Destroys the packet
    ~Packet();

    /// Sets the parameters of Packet
    void set(int pts,int dts,int duration,int stream_index);

    /// Sets the data allocation of Packet
    void set(int size,uint8_t* data, int stream_index,int flags);

    /// Returns the stream index of the packet
    int streamIndex() const;

    /// Returns the pts of the packet
    int pts() const;

    /// Returns the dts of the packet
    int dts() const;

    /// Returns the total duration of the packet
    int duration() const;

    operator AVPacket*();

    operator int() const;
};


inline int Packet::pts() const {
    return pkt.pts;
}

inline int Packet::dts() const {
    return pkt.dts;
}

inline int Packet::duration() const {
    return pkt.duration;
}


#endif // PACKET_H
