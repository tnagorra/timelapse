#include "lib/cli.h"
#include "lib/framerate.h"

int main()  try {


    /// YOU HAVE TO GIVE THIS PART AS INPUT
    DataObject data("../../Resources/outputfilename.avi",800,600);
    data.audioFilename("../../Resources/swan.ogg");

    {
        Group g;
        g.fps(11);
        g.crop(2304,1300);
        g.pan(0,10);

        for(int i=0; i < 7004-6844+1; i++) {
            char* filename = new char[  255];
            sprintf(filename,"../../Resources/One/DSC_%04d.JPG",i+6844);
            g.addFileName(filename);
            delete filename;
        }
        data.addGroup(g);
    }
    ///-------------------------------------



    // Initialize libavformat and register all the muxers, demuxers and protocols.
    av_register_all();

    const int vbitrate = 9000000;
    const int abitrate = 64000;

    /// WRITE MODE - AUDIO & VIDEO
    OFormatContext format_context(data.outputFilename());
    VideoCodecContext vcodec_context(format_context,data.width(),data.height(),vbitrate);
    AudioCodecContext acodec_context(format_context,abitrate);
    format_context.openFile();


    /// READ MODE - AUDIO
    IFormatContext kformat_context(data.audioFilename());
    AudioCodecContext kcodec_context(kformat_context);
    AudioFIFO fifo(acodec_context);
    ResampleContext resample_ctx(kcodec_context,acodec_context);

    IFormatContext* jformat_context = NULL;
    VideoCodecContext* jcodec_context = NULL;
    VideoFrame* jframe = NULL;


    while ( data.number() > 0 ) {

        Group g = data.getGroup();
        FrameNo f(g.fps(),g.number() ,vcodec_context.frameRate());
        bool transition = true;

        // While all the frames aren't read
        while(f >= 0) {

            // Write audio and video frames such that they are in order with time
            if ( acodec_context.stream().time() <= vcodec_context.stream().time() ) {

                AudioFrame kframe(kformat_context,kcodec_context,acodec_context,resample_ctx,fifo);
                format_context.writeFrame(acodec_context,kframe);

            } else if (  vcodec_context.stream().time() < acodec_context.stream().time() ) {

                int valid = true;
                // If there is a change in the frame load a new frame
                // This is the case if no transitions are applied and pictures are only loaded once.
                if(f > f-1 ) {

                    // Leave those files those aren't needed
                    for(int i=0; i< f.value() - f.value(-1) -1 ; i++)
                        g.popOne();

                    Chary filename = g.getFilename();
                    std::cout << f <<" "<<" " << filename <<std::endl;

                    try {
                        jformat_context = new IFormatContext(filename);
                        jcodec_context = new VideoCodecContext(*jformat_context);
                        jframe = new VideoFrame(*jformat_context,*jcodec_context,vcodec_context,g.cropWidth(),g.cropHeight());
                    } catch(const Error& e) {
                        std::cout << 'X';
                        if(jformat_context) {
                            delete jformat_context;
                            jformat_context = NULL;
                        }
                        ++f;
                        while(f == (f-1) ) {
                            f++;
                        }
                        valid = false;
                    }

                }

                // Write Frame if it is valid
                if(valid) {
                    // Create a copy of video frame and apply changes to it if there are transitions involved
                    VideoFrame* dummy =  transition ? new VideoFrame(*jframe) : jframe;
                    //dummy->strip(100,0);
                    dummy->pan(g.panX(),g.panY());
                    dummy->effect(g.effect());
                    dummy->brightness(g.brightness());
                    dummy->saturation(g.saturation());

                    if(format_context.writeFrame(vcodec_context, *dummy) >= 0);
                    /* Can always do something here*/
                    f++;
                    // Delete copied frame
                    if(transition)
                        delete dummy;
                }
            }

            // If there is a change in the frame or this is the end of all the frames in the list
            // To flush the encoder, decoder and the write function
            if(f > f-1 || f==-1 ) {
                if(jframe) {
                    delete jframe;
                    jframe = NULL;
                }
                if(jformat_context && jcodec_context) {
                    while(!jformat_context->videoEOS()) {
                        VideoFrame vf(*jformat_context,*jcodec_context,vcodec_context,0,0);
                        format_context.writeFrame(vcodec_context,vf);
                    }
                }
                if(jcodec_context) {
                    delete jcodec_context;
                    jcodec_context = NULL;
                }
                if(format_context) {
                    delete jformat_context;
                    jformat_context = NULL;
                }
            }
        }
    }

    // This is required as silence frames should also be flushed.
    // As we don't know the end of the silence frames, they must be flushed here.
    AudioFrame kframe;
    format_context.writeFrame(acodec_context,kframe);

    return 0;

} catch(const Empty& emp) {
    std::cout << emp <<std::endl;
} catch(const Error& err) {
    std::cout << err <<std::endl;
} catch(...) {
    std::cout << "Unknown exception." <<std::endl;
}
