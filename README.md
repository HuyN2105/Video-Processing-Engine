# Video Processing Engine (VPE)

**A high-performance, modern C++ video processing library built from scratch.**

> **Note:** This is a personal project developed to explore the intricacies of video codecs, memory management, and low-level media processing pipelines. It is designed to be a lightweight alternative to heavier frameworks, focusing on transparency and control.

---

## 🚀 Features

* **Modern C++20 Base:** Utilizes modern language features while maintaining low-level control.
* **Custom Frame Management:** Manual handling of pixel buffers (`RGB24`, `RGBA32`, `GRAY8`) with stride alignment.
* **FFmpeg Integration:** Direct linking with FFmpeg (`libavcodec`, `libavformat`) for demuxing and decoding.
* **Format Agnostic:** Reads standard video containers (MP4, MKV) and exports to raw image formats (PPM, PGM, PAM) for debugging.
* **Modular Architecture:** Clean separation between the core Engine, I/O handling, and Data structures.

## 🛠 Tech Stack

* **Language:** C++20
* **Build System:** CMake (3.20+)
* **Dependencies:**
* [FFmpeg](https://ffmpeg.org/) (Core video decoding/encoding)
* Windows API (for file dialogs in tests)


* **Development Environment:** CLion / MinGW (Windows)

## 📦 Installation & Setup

Because this project links manually against FFmpeg libraries to ensure portability and control, you must set up the dependencies before building.

### 1. Prerequisites

* CMake installed.
* A C++ Compiler (MinGW, MSVC, or GCC).

### 2. Setting up FFmpeg

This project requires the **Dev** (headers/libs) and **Shared** (DLLs) builds of FFmpeg.

1. Download the **Release Full Dev** and **Release Full Shared** builds from [gyan.dev](https://www.gyan.dev/ffmpeg/builds/).
2. Create a folder named `ffmpeg` in the project root.
3. Extract the contents so your structure looks like this:
```text
VideoProcessing/
├── CMakeLists.txt
├── src/
└── ffmpeg/
    ├── include/   (contains libavcodec, libavformat...)
    ├── lib/       (contains .lib or .a files)

```



### 3. Building the Project

```bash
mkdir build
cd build
cmake ..
cmake --build .

```

### 4. Running (Windows)

**Crucial Step:** You must copy the FFmpeg `.dll` files (from the `bin` folder of the *Shared* download) into the directory where the executable is built (e.g., `cmake-build-debug/` or `build/`).

Without these DLLs (`avcodec-61.dll`, `avutil-59.dll`, etc.), the application will exit with error `0xC0000135`.

## 💻 Usage Example

```cpp
#include "engine/Engine.h"
#include "io/Decoder.h"

int main() {
    // 1. Initialize the Engine
    engine::Engine myEngine;

    // 2. Load a video file
    std::string videoPath = "path/to/video.mp4";
    
    // 3. Inspect Metadata (using FFmpeg backend)
    engine::io::Decoder::printVideoInfo(videoPath);

    // 4. Create and manipulate a manual frame
    engine::Frame frame(1920, 1080, engine::PixelFormat::RGB24);
    
    // Convert to Grayscale
    engine::Engine::toGrayscale(frame);
    
    // Save debug output
    engine::Engine::savePPM(frame, "output.ppm");

    return 0;
}

```

## 🗺 Roadmap

* [x] **Core:** Basic memory management and Frame struct.
* [x] **IO:** FFmpeg linking and metadata extraction.
* [ ] **Decoding:** Full packet-to-frame decoding loop.
* [ ] **Processing:** Resize, Crop, and Color conversion filters.
* [ ] **Encoding:** Saving processed frames back to MP4.
* [ ] **Audio:** Basic audio pass-through support.

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](https://github.com/HuyN2105/Video-Processing-Engine/blob/main/LICENSE) file for details.

---

*Built with passion and C++ by HuyN.*