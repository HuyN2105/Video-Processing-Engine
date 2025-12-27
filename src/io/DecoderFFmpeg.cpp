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
    void Decoder::printVideoInfo(const std::string &filepath) {
        // 1. Allocate Context
        AVFormatContext *formatCtx = avformat_alloc_context();
        if (!formatCtx) {
            throw std::runtime_error("printVideoInfo: Could not allocate memory for AVFormatContext");
        }

        // 2. Open the File
        if (avformat_open_input(&formatCtx, filepath.c_str(), nullptr, nullptr) != 0) {
            throw std::runtime_error("printVideoInfo: Could not open file" + filepath);
        }

        // 3. Read Stream Information (Resolution, FPS, etc.)
        if (avformat_find_stream_info(formatCtx, nullptr) < 0) {
            throw std::runtime_error("printVideoInfo: Could not find stream information");
        }

        // 4. Print Details
        av_dump_format(formatCtx, 0, filepath.c_str(), 0);

        // 5. Cleanup
        avformat_close_input(&formatCtx);
    }
}
