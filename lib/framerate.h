#ifndef FRAMERATE_H
#define FRAMERATE_H

#include<cmath>
#include<iostream>


class FrameNo {
private:
    float i_FrameNo,o_FrameNo;
    int i_frameno, o_frameno;
    float total_time;
    int i,a,b,n;
    float d;
public:

    /// Get the frameno object
    /// Inputs are input framerate, input no. of frames and output framerates
    FrameNo(float i_f, float i_n,float o_f=25.0);

    /// Returns the total time
    float totalTime() const;

    /// Returns the total no. of output frames
    int totalState() const;

    /// Returns the current position in the no. of output frames
    int currentState() const ;


    /// Returns the value of the current frame
    int value(int x = 0) const ;

    operator int() const;

    int operator++();

    int operator++(int);

    int operator+(int x) const;

    int operator-(int x) const ;

};

inline float FrameNo::totalTime() const{
    return total_time;
}

inline int FrameNo::totalState() const {
    return n;
}

inline int FrameNo::currentState() const {
    return i;
}


inline int FrameNo::value(int x) const {
    if(i+x < n && i+x >= 0)
        return round(a + (i+x)*d);
    else
        return -1;
}

inline FrameNo::operator int() const {
    return value();
}

inline int FrameNo::operator++() {
    i++;
    return value();
}

inline int FrameNo::operator++(int) {
    int temp = value();
    i++;
    return temp;
}

inline int FrameNo::operator+(int x) const {
    return value(x);
}

inline int FrameNo::operator-(int x) const {
    return value(-x);
}

#endif // FRAMERATE_H
