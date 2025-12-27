//
// Created by HuyN on 25/12/2025.
//

#ifndef ENGINE_DECODER_H
#define ENGINE_DECODER_H

namespace engine::io {
    class Decoder {
    public:
        virtual ~Decoder() = default;

        static void printVideoInfo(const std::string &filepath);
    };
}

#endif //ENGINE_DECODER_H
