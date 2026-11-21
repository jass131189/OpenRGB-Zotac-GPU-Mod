# OpenRGB 1.1 ZOTAC RTX 5070 Ti Plugin (ZOTAC GAMING GeForce RTX 5070 Ti SOLID OC)

## Target Hardware Profile
- PCI Vendor / Device: 0x10DE:0x2C05 (NVIDIA GeForce RTX 5070 Ti, GB203)
- PCI Subsystem: 0x19DA:0x2765 (ZOTAC)
- I2C Address: 0x49
- Zones: Side Lightbar, SPECTRA Emblem

## Option A — Cloud Build the .dll via GitHub Actions (No Local Visual Studio / Qt Required)
1. Push the extracted files (including `.github/workflows/build-plugin.yml`) to a GitHub repository.
2. Open the repository's **Actions** tab and download the compiled `OpenRGBZotac5070TiPlugin.dll` artifact.
3. In OpenRGB 1.1 (running as Administrator, with FireStorm exited), open **Settings -> Plugins -> Install Plugin** and select the downloaded `.dll`.

## Option B — Local Build (Windows Qt 6 / MSVC)
1. Clone OpenRGB alongside this folder:
   git clone https://gitlab.com/CalcProgrammer1/OpenRGB.git
2. Configure & build with CMake:
   cmake -B build -DOPENRGB_SRC_DIR=../OpenRGB
   cmake --build build --config Release
3. Copy the compiled library into your OpenRGB plugins directory:
   - Windows: %APPDATA%\OpenRGB\plugins\OpenRGBZotac5070TiPlugin.dll
   - Linux:   ~/.config/OpenRGB/plugins/libOpenRGBZotac5070TiPlugin.so

## Option C — Zero-Compile Python SDK Bridge (Windows)
1. Enable OpenRGB 1.1's **SDK Server** tab (**Start Server**).
2. Close FireStorm, then run: `pip install openrgb-python && python zotac_openrgb_sync.py` (as Administrator).
