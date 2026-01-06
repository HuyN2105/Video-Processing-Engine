//
// Created by HuyN on 25/12/2025.
//

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <vector>

#include <windows.h>
#include <shlobj.h>
#include <commdlg.h>
#include <iostream>

#include "engine/Engine.h"
#include "engine/Frame.h"
#include "io/Decoder.h"
#include "utils/Logger.h"

namespace io = engine::io;

char *openFileDialog() {
    OPENFILENAME ofn;
    char szFile[260] = {0};
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = nullptr;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "All Files (*.*)\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = nullptr;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = nullptr;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

    if (GetOpenFileName(&ofn) == TRUE) {
        return _strdup(ofn.lpstrFile);
    }
    return nullptr;
}


engine::Frame randomColorFrameCreate_RGB24(const int width, const int height) {
    engine::Frame frame(width, height, engine::PixelFormat::RGB24);
    int bytesPerPixel = frame.bytesPerPixel();

    for (int y = 0; y < frame.height; y++) {
        uint8_t *row = frame.row(y);
        for (int x = 0; x < frame.width; x++) {
            row[x * bytesPerPixel + 0] = static_cast<uint8_t>((x * 255) / frame.width);
            row[x * bytesPerPixel + 1] = static_cast<uint8_t>((y * 255) / frame.height);
            row[x * bytesPerPixel + 2] = static_cast<uint8_t>(128);
        }
    }

    return frame;
}

engine::Frame randomColorFrameCreate_RGBA32(const int width, const int height) {
    engine::Frame frame(width, height, engine::PixelFormat::RGBA32);
    int bytesPerPixel = frame.bytesPerPixel();

    for (int y = 0; y < frame.height; y++) {
        uint8_t *row = frame.row(y);
        for (int x = 0; x < frame.width; x++) {
            row[x * bytesPerPixel + 0] = static_cast<uint8_t>((x * 255) / frame.width);
            row[x * bytesPerPixel + 1] = static_cast<uint8_t>((y * 255) / frame.height);
            row[x * bytesPerPixel + 2] = static_cast<uint8_t>(128);
            row[x * bytesPerPixel + 3] = static_cast<uint8_t>(255);
        }
    }

    return frame;
}

/*
62
#     #              #     #
#     # #    # #   # ##    #
#     # #    #  # #  # #   #
####### #    #   #   #  #  #
#     # #    #   #   #   # #
#     # #    #   #   #    ##
#     #  ####    #   #     # 125

 */

// Add blueMark
void addBlueMark(engine::Frame &frame) {
    if (frame.pixelFormat != engine::PixelFormat::RGBA32) {
        engine::utils::Logger::error("addBlueMark: pixelFormat not supported.");
        return;
    }

    const std::vector<std::vector<int> > bitmap = {
        {1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1},
        {1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1},
        {1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1},
        {1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1}
    };

    const int bytePerPixel = frame.bytesPerPixel();

    for (int y = 102; y < 977; y++) {
        uint8_t *row = frame.row(y);
        for (int x = 92; x < 1828; x++) {
            if (bitmap[(y - 102) / 125][(x - 92) / 62] == 1) {
                row[x * bytePerPixel + 3] = static_cast<uint8_t>(100);
            }
        }
    }
}

int main() {
    using engine::utils::Logger::error;
    using engine::utils::Logger::info;
    using engine::utils::Logger::success;
    using engine::utils::Logger::warn;

    const char *filepath = openFileDialog();

    if (!filepath) {
        error("No file was selected.");
        return 1;
    }

    io::Decoder decoder;

    decoder.open(filepath);

    const double fps = decoder.getFPS(filepath);

    std::cout << "FPS: " << fps << "\n";

    // try {
    //     io::Decoder decoder;
    //     decoder.open(filepath);
    //
    //     engine::Frame frame(decoder.getWidth(), decoder.getHeight(), engine::PixelFormat::RGBA32);
    //
    //     int frameCount = 0;
    //
    //     while (decoder.readFrame_RGBA32(frame)) {
    //         frameCount++;
    //         if (frameCount % 30 == 0) {
    //             info("Decoded frame {}", frameCount);
    //         }
    //
    //         if (frameCount % 100 == 0) {
    //             addBlueMark(frame);
    //             engine::Engine::savePAM(frame, "./test/debug_frame" + std::to_string(frameCount) + ".pam");
    //             success("Snapshot of frame {} has been successfully saved.", frameCount);
    //         }
    //     }
    //
    //     success("Finished! Total Frames: {}", frameCount);
    // } catch (std::exception &e) {
    //     error("Error: {}", e.what());
    // }

    return 0;
}
