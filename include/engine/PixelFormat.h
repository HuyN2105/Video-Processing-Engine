//
// Created by HuyN on 25/12/2025.
//

#ifndef ENGINE_PIXELFORMAT_H
#define ENGINE_PIXELFORMAT_H

#pragma once

namespace engine {
    enum class PixelFormat {
        RGB24, // 3 bytes per pixel
        RGBA32, // 4 bytes per pixel
        GRAY8,
        UNKNOWN
    };
}

#endif //ENGINE_PIXELFORMAT_H
