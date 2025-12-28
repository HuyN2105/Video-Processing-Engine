//
// Created by HuyN on 25/12/2025.
//

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

#include <iostream>
#include <stdexcept>

#include "io/Decoder.h"

namespace engine::io {
    Decoder::Decoder() {
        formatCtx = avformat_alloc_context();
        avFrame = av_frame_alloc();
        avPacket = av_packet_alloc();

        if (!formatCtx) {
            throw std::runtime_error("Decoder: Could not allocate memory for AVFormatContext");
        }
        if (!avFrame) {
            throw std::runtime_error("Decoder: Could not allocate memory for AVFrame");
        }
        if (!avPacket) {
            throw std::runtime_error("Decoder: Could not allocate memory for AVPacket");
        }
    }

    Decoder::~Decoder() {
        close();
        avformat_free_context(formatCtx);
        av_frame_free(&avFrame);
        av_packet_free(&avPacket);
    }

    void Decoder::open(const std::string &filepath) {
        if (avformat_open_input(&formatCtx, filepath.c_str(), nullptr, nullptr) != 0) {
            throw std::runtime_error("Decoder::open: Could not open file" + filepath);
        }

        if (avformat_find_stream_info(formatCtx, nullptr) < 0) {
            throw std::runtime_error("Decoder::open: Could not find stream information");
        }

        const AVCodec* codec = nullptr;
        videoStreamIndex = av_find_best_stream(formatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, &codec, 0);

        if (avcodec_open2(codecCtx, codec, nullptr) < 0) {
            throw std::runtime_error("Decoder::open: Could not open video codec");
        }


    }

    void Decoder::printVideoInfo(const std::string &filepath) {
        AVFormatContext *formatCtx = avformat_alloc_context();
        if (!formatCtx) {
            throw std::runtime_error("printVideoInfo: Could not allocate memory for AVFormatContext");
        }

        if (avformat_open_input(&formatCtx, filepath.c_str(), nullptr, nullptr) != 0) {
            throw std::runtime_error("printVideoInfo: Could not open file" + filepath);
        }

        if (avformat_find_stream_info(formatCtx, nullptr) < 0) {
            throw std::runtime_error("printVideoInfo: Could not find stream information");
        }

        av_dump_format(formatCtx, 0, filepath.c_str(), 0);

        avformat_close_input(&formatCtx);
    }
}
