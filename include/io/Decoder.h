//
// Created by HuyN on 25/12/2025.
//

#ifndef ENGINE_DECODER_H
#define ENGINE_DECODER_H

#pragma once

#include <string>

#include "engine/Frame.h"
#include "libavutil/pixfmt.h"

struct AVFormatContext;
struct AVCodecContext;
struct AVFrame;
struct AVPacket;
struct SwsContext;

namespace engine::io {
    class Decoder {
    public:
        Decoder();

        ~Decoder();

        void open(const std::string &filepath);

        void close();

        bool readFrame(engine::Frame &outFrame, AVPixelFormat PixelFormat);

        // True if a Frame was read from video | False if end of File
        bool readFrame_RGB24(engine::Frame &outFrame); // True if a Frame was read from video | False if end of File
        bool readFrame_RGBA32(engine::Frame &outFrame); // True if a Frame was read from video | False if end of File

        int getWidth() const;

        int getHeight() const;

        static void printVideoInfo(const std::string &filepath);

    private:
        AVFormatContext *formatCtx = nullptr; // The File
        AVCodecContext *codecCtx = nullptr; // The Codec (H.264, etc.)
        AVFrame *avFrame = nullptr; // The Raw Frame (YUV format)
        AVPacket *avPacket = nullptr; // The Compressed Data

        SwsContext *swsCtx = nullptr;

        int videoStreamIndex = -1;
    };
}

#endif //ENGINE_DECODER_H
