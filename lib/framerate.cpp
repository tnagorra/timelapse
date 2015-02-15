#include "framerate.h"
#include<cmath>


FrameNo::FrameNo(float i_f, float i_n,float o_f):i_FrameNo(i_f),o_FrameNo(o_f),i_frameno(i_n) {

    /// FrameNo of the output video ie CONSTANT
    /// Total no. of frames to be extracted from the image sequence
    /// to maintain the output video FrameNo
    total_time = i_frameno/i_FrameNo;
    o_frameno = o_FrameNo*total_time;

    /// Load required frames and skipover/repeat some frames
    /// to maintain the output frame rate
    /// t(n) = a + n*d
    /// d = (b-a)/(n-1)

    n = o_frameno;
    a = 0;					/// First image
    b = (i_frameno-1);		/// Last image - If there are 400 images then 399 is the last image
    d = ( b - a )/(n - 1.0); /// I have changed here, If wrong look here;
    i = 0;

}
