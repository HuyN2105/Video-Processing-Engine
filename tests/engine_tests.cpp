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

char* openFileDialog() {
    OPENFILENAME ofn;
    char szFile[260] = {0};
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "All Files (*.*)\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

    if (GetOpenFileName(&ofn) == TRUE) {
        return _strdup(ofn.lpstrFile);
    }
    return NULL;
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

    // engine::Frame frame = randomColorFrameCreate_RGB24(1000, 1000);
    //
    // const int bytesPerPixel = frame.bytesPerPixel();
    //
    // for (int y = 200; y < 800; y++) {
    //     uint8_t *row = frame.row(y);
    //     for (int x = 200; x < 800; x++) {
    //         if (x <= 350 || x >= 650) {
    //             row[x * bytesPerPixel + 0] = static_cast<uint8_t>(0);
    //             row[x * bytesPerPixel + 1] = static_cast<uint8_t>(0);
    //             row[x * bytesPerPixel + 2] = static_cast<uint8_t>(0);
    //             // row[x * bytesPerPixel + 3] = static_cast<uint8_t>(150);
    //             continue;
    //         }
    //         if (y >= 450 && y <= 550) {
    //             row[x * bytesPerPixel + 0] = static_cast<uint8_t>(0);
    //             row[x * bytesPerPixel + 1] = static_cast<uint8_t>(0);
    //             row[x * bytesPerPixel + 2] = static_cast<uint8_t>(0);
    //             // row[x * bytesPerPixel + 3] = static_cast<uint8_t>(150);
    //         }
    //     }
    // }
    //
    // engine::Engine::toGrayScale(frame);
    //
    // engine::Engine::savePPM(frame, "test_ppm.ppm");
    //
    // const std::string filepath = openFileDialog();
    //
    // engine::io::Decoder::printVideoInfo(filepath);

    system("cls");

    logger::info("this is info");
    logger::success("this is success");
    logger::warn("this is warn");
    logger::error("this is error");

    return 0;
}