# timelapse
A timelapse appplication

## Getting ffmpeg
```
# Getting dependencies
sudo apt-get update
sudo apt-get -y --force-yes install autoconf automake build-essential libass-dev libfreetype6-dev libgpac-dev \
  libsdl1.2-dev libtheora-dev libtool libva-dev libvdpau-dev libvorbis-dev libxcb1-dev libxcb-shm0-dev \
  libxcb-xfixes0-dev pkg-config texi2html zlib1g-dev

# Get ffmpeg using following or get to official site
mkdir ~/ffmpeg_build
mkdir ~/ffmpeg_sources && cd ~/ffmpeg_sources
wget http://ffmpeg.org/releases/ffmpeg-snapshot.tar.bz2 && tar xjvf ffmpeg-snapshot.tar.bz2
cd ffmpeg
./configure --prefix="$HOME/ffmpeg_build" --libdir="$HOME/ffmpeg_build/lib"\
--shlibdir="$HOME/ffmpeg_build/sharedlib" \
--incdir="$HOME/ffmpeg_build/include"  --bindir="$HOME/bin" \
--disable-static --enable-shared --enable-small --enable-optimizations \
--enable-gpl --enable-libass  --enable-libtheora --enable-libvorbis\
--enable-nonfree --enable-x11grab \
--disable-programs --disable-doc --disable-htmlpages  --disable-manpages\
--disable-podpages  --disable-txtpages \
 --extra-cflags="-I$HOME/ffmpeg_build/include"\
 --extra-ldflags="-L$HOME/ffmpeg_build/sharedlib"  --extra-libs="-ldl"
make
make install
make distclean
hash -r


# Use our library folder instead of /lib folder using "-rlink LD_LIBRARY_PATH"
# Must see ! I have no idea abou this now
# --extra-ldflags=ELDFLAGS


# Compile program using following
g++ main.cpp -o main -Wl,-rpath,$HOME/ffmpeg_build/sharedlib -I $HOME/ffmpeg_build/include
-L $HOME/ffmpeg_build/sharedlib -lavdevice -lavformat -lavfilter -lavcodec -lswscale -lavutil
-lm -lz -lva -lpthread -D__STDC_CONSTANT_MACROS
```
