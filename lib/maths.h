#ifndef MATHS_H
#define MATHS_H

#include <cmath>
namespace Maths {

/// Returns the smaller between the two numbers;
/// x,y are two numbers to compare
inline float smaller(float x, float y) {
    return (x < y) ? x : y;
}

/// Returns the smaller between the two numbers;
/// x,y are two numbers to compare
inline float larger(float x, float y) {
    return (x < y) ? y : x;
}

/// Returns the number between the two limiting values
/// return value is in between high and low
inline float limit(float value,float high,float low) {
    if(value >= high)
        return high;
    else if( value <= low)
        return low;
    else
        return value;
}

/// Returns the absolute value of the number
inline float absolute(float value) {
    if(value < 0)
        return -value;
    else
        return value;
}

/// Returns the nearest even number to the given value
inline int evenize(int value,int align_c=2) {
    // int align_c = 2;
    // int align_c = 16*3; for better results but changes the value alot
    return value/align_c*align_c;
}

/// Clips the value if it is negative and returns the default value
inline float clip(float value, float def) {
    if(value <= 0)
        return def;
    else
        return value;
}

}



#endif // MATHS_H
