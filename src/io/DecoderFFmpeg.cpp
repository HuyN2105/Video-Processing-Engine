//
// Created by HuyN on 25/12/2025.
//

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

#include <stdexcept>

#include "io/Decoder.h"
#include "utils/Logger.h"

namespace logger = engine::utils::Logger;

namespace engine::io {
    Decoder::Decoder() {
        formatCtx = avformat_alloc_context();
        avFrame = av_frame_alloc();
        avPacket = av_packet_alloc();

        if (!formatCtx) {
            logger::error("Decoder: Could not allocate memory for AVFormatContext");
            throw std::runtime_error("Decoder: Could not allocate memory for AVFormatContext");
        }
        if (!avFrame) {
            logger::error("Decoder: Could not allocate memory for AVFrame");
            throw std::runtime_error("Decoder: Could not allocate memory for AVFrame");
        }
        if (!avPacket) {
            logger::error("Decoder: Could not allocate memory for AVPacket");
            throw std::runtime_error("Decoder: Could not allocate memory for AVPacket");
        }
    }

    Decoder::~Decoder() {
        close();
        avformat_free_context(formatCtx);
        if (avFrame) {
            av_frame_free(&avFrame);
            avFrame = nullptr;
        }
        if (avPacket) {
            av_packet_free(&avPacket);
            avPacket = nullptr;
        }
    }

    void Decoder::open(const std::string &filepath) {
        if (avformat_open_input(&formatCtx, filepath.c_str(), nullptr, nullptr) != 0) {
            logger::error("Decoder::open: Could not open file: {}", filepath);
            throw std::runtime_error("Decoder::open: Could not open file: " + filepath);
        }

        if (avformat_find_stream_info(formatCtx, nullptr) < 0) {
            logger::error("Decoder::open: Could not find stream information");
            throw std::runtime_error("Decoder::open: Could not find stream information");
        }

        const AVCodec* codec = nullptr;
        videoStreamIndex = av_find_best_stream(formatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, &codec, 0);

        if (videoStreamIndex < 0) {
            logger::error("Decoder::open: Could not find video stream");
            throw std::runtime_error("Decoder::open: Could not find video stream");
        }

        codecCtx = avcodec_alloc_context3(codec);
        if (!codecCtx) {
            logger::error("Decoder::open: Could not allocate codec context");
            throw std::runtime_error("Decoder::open: Could not allocate codec context");
        }

        if (avcodec_parameters_to_context(codecCtx, formatCtx->streams[videoStreamIndex]->codecpar) < 0) {
            logger::error("Decoder::open: Could not copy codec parameters");
            throw std::runtime_error("Decoder::open: Could not copy codec parameters");
        }

        if (avcodec_open2(codecCtx, codec, nullptr) < 0) {
            logger::error("Decoder::open: Could not open video codec");
            throw std::runtime_error("Decoder::open: Could not open video codec");
        }

    }

    void Decoder::printVideoInfo(const std::string &filepath) {
        AVFormatContext *formatCtx = avformat_alloc_context();
        if (!formatCtx) {
            logger::error("printVideoInfo: Could not allocate memory for AVFormatContext");
            throw std::runtime_error("printVideoInfo: Could not allocate memory for AVFormatContext");
        }

        if (avformat_open_input(&formatCtx, filepath.c_str(), nullptr, nullptr) != 0) {
            logger::error("printVideoInfo: Could not open file: {}", filepath);
            throw std::runtime_error("printVideoInfo: Could not open file" + filepath);
        }

        if (avformat_find_stream_info(formatCtx, nullptr) < 0) {
            logger::error("printVideoInfo: Could not find stream information");
            throw std::runtime_error("printVideoInfo: Could not find stream information");
        }

        av_dump_format(formatCtx, 0, filepath.c_str(), 0);

        avformat_close_input(&formatCtx);
    }

    void Decoder::close() {
        if (codecCtx) {
            avcodec_free_context(&codecCtx);
            codecCtx = nullptr;
        }
        if (formatCtx) {
            avformat_close_input(&formatCtx);
            formatCtx = nullptr;
        }
        videoStreamIndex = -1;
    }

    bool Decoder::readFrame(engine::Frame &outFrame) {
        // TODO: Implement frame reading with pixel format conversion
        // This requires using swscale to convert from YUV to RGB
        return false;
    }

    int Decoder::getWidth() const {
        return codecCtx ? codecCtx->width : 0;
    }

    int Decoder::getHeight() const {
        return codecCtx ? codecCtx->height : 0;
    }
}
