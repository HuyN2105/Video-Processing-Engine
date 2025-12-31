//
// Created by HuyN on 25/12/2025.
//
#ifndef ENGINE_ENGINE_H
#define ENGINE_ENGINE_H

#pragma once

#include <string>

#include "Frame.h"

namespace engine {
    class Engine {
    public:
        static void process(const std::string &input, const std::string &output);

        static void savePPM(const engine::Frame &Frame, const std::string &output);

        static void savePAM(const engine::Frame &frame, const std::string &output);

        static void savePGM(const engine::Frame &frame, const std::string &output);

        static void toGrayScale(engine::Frame &frame);

        static void convertRGB24toRGBA32(const engine::Frame &src, engine::Frame &dest);
    };
}

#endif //ENGINE_ENGINE_H
