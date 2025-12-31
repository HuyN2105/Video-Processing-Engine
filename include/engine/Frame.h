//
// Created by HuyN on 25/12/2025.
//

#ifndef ENGINE_FRAME_H
#define ENGINE_FRAME_H

#pragma once

#include <cstdint>
#include <vector>
#include "PixelFormat.h"

namespace engine {
    struct Frame {
        int width = 0;
        int height = 0;
        PixelFormat pixelFormat = PixelFormat::UNKNOWN;

        // Bytes per row (important for alignment)
        int stride = 0;

        // Owned pixel data
        std::vector<uint8_t> data;

        // Optional metadata
        int64_t pts = 0; // presentation timestamp

        Frame() = default;

        Frame(const int width, const int height, const PixelFormat pixelFormat) : width(width), height(height), pixelFormat(pixelFormat) {
            stride = bytesPerPixel() * width;
            data.resize(stride * height);
        }

        [[nodiscard]] int bytesPerPixel() const {
            switch (pixelFormat) {
                case PixelFormat::RGB24: return 3;
                case PixelFormat::RGBA32: return 4;
                case PixelFormat::GRAY8: return 1;
                default: return 0;
            }
        }

        uint8_t *row(const int y) {
            return data.data() + y * stride;
        }

        [[nodiscard]] const uint8_t *row(const int y) const {
            return data.data() + y * stride;
        }
    };
}

#endif //ENGINE_FRAME_H
