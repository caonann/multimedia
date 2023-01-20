include(ExternalProject)

set(ROOT          ${CMAKE_BINARY_DIR}/thirdparty/ffmpeg)
set(LIB_DIR       ${ROOT}/lib)
set(INCLUDE_DIR   ${ROOT}/include)

set(URL           https://github.com/FFmpeg/FFmpeg/archive/refs/heads/release/4.4.zip)
set(CONFIGURE     cd ${ROOT}/src/ffmpeg-4.4 && ./configure  --enable-pic --disable-debug  --disable-decoders --disable-encoders --disable-parsers --disable-demuxers --disable-muxers --disable-filters --disable-htmlpages --disable-manpages --disable-podpages --disable-txtpages --disable-programs  --enable-doc --disable-bzlib --disable-error-resilience --disable-iconv --disable-lzo --disable-network --disable-schannel --disable-sdl2 --disable-symver --disable-xlib --disable-zlib --disable-securetransport --disable-faan --disable-alsa --disable-autodetect --enable-demuxer=flv --enable-demuxer=h264 --enable-demuxer=aac --enable-muxer=flv --enable-muxer=h264 --enable-muxer=opus --enable-parser=aac --enable-parser=opus  --enable-parser=h264 --enable-parser=vp8 --enable-decoder=h264 --enable-decoder=aac --enable-encoder=opus --enable-decoder=opus --prefix=${ROOT})
set(MAKE          cd ${ROOT}/src/ffmpeg-4.4 && make -j16)
set(INSTALL       cd ${ROOT}/src/ffmpeg-4.4 && make install)

ExternalProject_Add(ffmpeg-4.4
        URL                   ${URL}
        DOWNLOAD_NAME         4.4.zip
        PREFIX                ${ROOT}
        CONFIGURE_COMMAND     ${CONFIGURE}
        BUILD_COMMAND         ${MAKE}
        INSTALL_COMMAND       ${INSTALL}
)

include_directories(${INCLUDE_DIR})
link_directories(${LIB_DIR})