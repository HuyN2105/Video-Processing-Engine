//
// Created by HuyN on 25/12/2025.
//

#include <cstdint>
#include <cstdlib>
#include <cstring>

#include <windows.h>
#include <shlobj.h>
#include <commdlg.h>
#include "engine/Engine.h"
#include "engine/Frame.h"
#include "io/Decoder.h"
#include "utils/Logger.h"

namespace logger = engine::utils::Logger;
namespace io = engine::io;

char* openFileDialog() {
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

int main() {

    const std::string filepath = openFileDialog();

    io::Decoder decoder;

    decoder.open(filepath);

    engine::Frame frame(decoder.getWidth(), decoder.getHeight(), engine::PixelFormat::RGB24);

    int frameCount = 0;
    while (decoder.readFrame(frame)) {
        frameCount++;
        if (frameCount % 30 == 0) {
            engine::utils::Logger::info("Decoded frame {}", frameCount);

            engine::Engine::savePPM(frame, "./test/debug_frame" + std::to_string(frameCount) + ".ppm");
        }
    }

    return 0;
}