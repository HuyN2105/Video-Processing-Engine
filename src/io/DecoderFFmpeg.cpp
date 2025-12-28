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
        }
        if (avPacket) {
            av_packet_free(&avPacket);
        }
        if (swsCtx) {
            sws_freeContext(swsCtx);
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

        const AVCodec *codec = nullptr;
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
        // Keep reading until found a video packet that decodes into a full frame
        while (av_read_frame(formatCtx, avPacket) >= 0) {
            // Is this a video packet?
            if (avPacket->stream_index == videoStreamIndex) {
                // Send packet to decoder
                if (avcodec_send_packet(codecCtx, avPacket) < 0) {
                    logger::error("Decoder::readFrame: Could not send packet");
                    av_packet_unref(avPacket);
                    continue;
                }

                // Receive frame from decoder
                // (One packet might generate 0, 1, or more frames)
                int response = avcodec_receive_frame(codecCtx, avFrame);
                if (response == AVERROR(EAGAIN) || response == AVERROR_EOF) {
                    // Not enough data yet, or end of stream
                    av_packet_unref(avPacket);
                    continue;
                }
                if (response < 0) {
                    logger::error("Decoder::readFrame: Error receiving Frame from Decoder");
                    av_packet_unref(avPacket);
                    return false;
                }

                // =========================================================
                // CONVERSION TIME: YUV -> RGB
                // =========================================================

                // Initialize the Scaler (SwsContext) if it hasn't been created
                // or if the Dimensions changed
                // (Re)initialize the Scaler (SwsContext) using sws_getCachedContext
                // so it is updated if the dimensions or pixel format change mid-stream.

                // TODO: make sure it's RGB24

                swsCtx = sws_getCachedContext(
                    swsCtx,
                    codecCtx->width, codecCtx->height, codecCtx->pix_fmt, // Input (video)
                    outFrame.width, outFrame.height, AV_PIX_FMT_RGB24, // Output (Frame)
                    SWS_BILINEAR, nullptr, nullptr, nullptr
                );
                if (!swsCtx) {
                    logger::error("Decoder::readFrame: Could not initialize SwsContext");
                    av_packet_unref(avPacket);
                    return false;
                }

                // Prepare destination pointers for sws_scale
                // Point to row(0) as it's a contiguous block
                uint8_t *dest[4] = {outFrame.row(0), nullptr, nullptr, nullptr};
                const int destLineSize[4] = {outFrame.stride, 0, 0, 0}; // 3 bytes per pixel for RGB24

                // Perform the conversion
                sws_scale(swsCtx,
                          avFrame->data, avFrame->linesize, // Source (YUV)
                          0, codecCtx->height, // Source height
                          dest, destLineSize); // Destination (RGB)

                // Clean up
                av_packet_unref(avPacket);
                return true;
            }

            // Unreferenced packet if it wasn't video
            av_packet_unref(avPacket);
        }

        return false;
    }

    int Decoder::getWidth() const {
        return codecCtx ? codecCtx->width : 0;
    }

    int Decoder::getHeight() const {
        return codecCtx ? codecCtx->height : 0;
    }
}