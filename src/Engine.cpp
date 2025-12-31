//
// Created by HuyN on 25/12/2025.
//

#include <fstream>
#include <stdexcept>

#include "engine/Engine.h"
#include "engine/Frame.h"
#include "utils/Logger.h"

namespace logger = engine::utils::Logger;

namespace engine {
    void Engine::process(const std::string &input, const std::string &output) {
    }

    void Engine::savePPM(const engine::Frame &frame, const std::string &output) {
        if (frame.pixelFormat != engine::PixelFormat::RGB24) {
            logger::error("savePPM: PPM is only for RGB24");
            throw std::runtime_error("savePPM: PPM is only for RGB24");
        }

        std::ofstream file(output, std::ios::binary);
        if (!file) {
            logger::error("savePPM: could not open file for writing: {}", output);
            throw std::runtime_error("savePPM: could not open file for writing");
        }

        file << "P6\n";
        file << frame.width << " " << frame.height << "\n";
        file << "255\n";

        for (int y = 0; y < frame.height; y++) {
            const uint8_t *row = frame.row(y);
            file.write(reinterpret_cast<const char *>(row), frame.stride);
        }
    }

    void Engine::savePAM(const engine::Frame &frame, const std::string &output) {
        if (frame.pixelFormat != engine::PixelFormat::RGBA32) {
            logger::error("savePAM: PAM is only for RGBA32");
            throw std::runtime_error("savePAM: PAM is only for RGBA32");
        }

        std::ofstream file(output, std::ios::binary);
        if (!file) {
            logger::error("savePAM: could not open file for writing: {}", output);
            throw std::runtime_error("savePAM: could not open file for writing");
        }

        file << "P7\n";
        file << "WIDTH " << frame.width << "\n";
        file << "HEIGHT " << frame.height << "\n";
        file << "DEPTH " << frame.bytesPerPixel() << "\n";
        file << "MAXVAL 255\n";
        file << "TUPLETYPE RGB_ALPHA\n"; // Standard name for RGBA
        file << "ENDHDR\n";

        for (int y = 0; y < frame.height; y++) {
            const uint8_t *row = frame.row(y);
            file.write(reinterpret_cast<const char *>(row), frame.stride);
        }
    }

    void Engine::savePGM(const engine::Frame &frame, const std::string &output) {
        std::ofstream file(output, std::ios::binary);
        if (!file) {
            logger::error("savePGM: could not open file for writing: {}", output);
            throw std::runtime_error("savePGM: could not open file for writing");
        }

        file << "P5\n";
        file << frame.width << " " << frame.height << "\n";
        file << "255\n";

        if (frame.pixelFormat == engine::PixelFormat::GRAY8) {
            // Already grayscale - write directly
            for (int y = 0; y < frame.height; y++) {
                const uint8_t *row = frame.row(y);
                file.write(reinterpret_cast<const char *>(row), frame.width);
            }
        } else {
            // Convert to grayscale on-the-fly and write only the luminance
            logger::warn("savePGM: Converting frame to grayscale for PGM output");
            const int bytesPerPixel = frame.bytesPerPixel();
            for (int y = 0; y < frame.height; y++) {
                const uint8_t *row = frame.row(y);
                for (int x = 0; x < frame.width; x++) {
                    const int i = x * bytesPerPixel;
                    const uint8_t r = row[i + 0];
                    const uint8_t g = row[i + 1];
                    const uint8_t b = row[i + 2];
                    const auto gray = static_cast<uint8_t>(0.299 * r + 0.587 * g + 0.114 * b);
                    file.write(reinterpret_cast<const char *>(&gray), 1);
                }
            }
        }
    }

    void Engine::toGrayScale(engine::Frame &frame) {
        if (frame.pixelFormat == engine::PixelFormat::GRAY8) {
            logger::warn("toGrayScale: frame is already GRAY8");
            return;
        }

        const int bytesPerPixel = frame.bytesPerPixel();

        for (int y = 0; y < frame.height; y++) {
            uint8_t *row = frame.row(y);
            for (int x = 0; x < frame.width; x++) {
                const int i = x * bytesPerPixel;

                const uint8_t r = row[i + 0];
                const uint8_t g = row[i + 1];
                const uint8_t b = row[i + 2];

                const auto YLinear = static_cast<uint8_t>(0.299 * r + 0.587 * g + 0.114 * b);

                row[i + 0] = YLinear;
                row[i + 1] = YLinear;
                row[i + 2] = YLinear;
            }
        }
    }

    void Engine::convertRGB24toRGBA32(const engine::Frame &src, engine::Frame &dest) {
        // Safety checks
        if (src.width != dest.width || src.height != dest.height) {
            logger::error("convertRGB24toRGBA32: dimension mismatch between src and dest frame");
            throw std::runtime_error("convertRGB24toRGBA32: dimension mismatch between src and dest frame");
        }
        if (src.pixelFormat != PixelFormat::RGB24) {
            logger::error("convertRGB24toRGBA32: src frame must be in RGB24 format");
            throw std::runtime_error("convertRGB24toRGBA32: src frame must be in RGB24 format");
        }
        if (dest.pixelFormat != PixelFormat::RGBA32) {
            logger::error("convertRGB24toRGBA32: dest frame must be in RGBA32 format");
            throw std::runtime_error("convertRGB24toRGBA32: dest frame must be in RGBA32 format");
        }

        const int width = src.width;
        const int height = src.height;

        for (int y = 0; y < height; ++y) {
            // Get pointers to the start of the row
            const uint8_t *srcRow = src.row(y);
            uint8_t *destRow = dest.row(y);

            for (int x = 0; x < width; ++x) {
                // Calculate index for the current pixel
                // RGB24 stride is 3, RGBA32 stride is 4
                const int srcIndex = x * src.bytesPerPixel();
                const int destIndex = x * dest.bytesPerPixel();

                // Copy Colors
                destRow[destIndex + 0] = srcRow[srcIndex + 0]; // R
                destRow[destIndex + 1] = srcRow[srcIndex + 1]; // G
                destRow[destIndex + 2] = srcRow[srcIndex + 2]; // B

                // Add Alpha (255 = Opaque, 0 = Transparent)
                destRow[destIndex + 3] = 255;
            }
        }
    }
}
