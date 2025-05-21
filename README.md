# 🕸️ Gothic Rasterizer — CPU Triangle Clipping & Z-Buffer 🕸️

This project implements a **CPU-based triangle rasterizer** in C++ using a custom software pipeline. It supports full **depth buffering**, **clipping**, and multiple 3D transformations including translation and scaling. The rendered output is exported as a `.ppm` image.

## 🖤 Features 🖤

- Triangle rasterization with filled or wireframe mode
- Z-buffer for hidden surface removal
- Near and far plane clipping
- Support for custom transformation matrices
- Scene with 3 cubes: original, translated, and scaled
- Rendered with gothic-inspired colors

## 🩶 Scene Description 🩶

The rendered image includes:
- Three cubes arranged horizontally:
  - A central cube (original)
  - One to the left (translated)
  - One to the right (scaled )
- Each cube uses a gothic tone like deep maroon, ash grey, or phantom purple
- Output is generated in the `output.ppm` file, viewable with any PPM viewer or convertible to PNG

All geometry and raster operations are processed entirely on the CPU, without OpenGL or external graphics libraries.

## ⚰️ Files ⚰️

- `main.cpp` – main scene setup and renderer call
- `canvas.hpp` – pixel buffer with depth testing
- `geometry.hpp` – transformations and projection logic
- `model.hpp` – triangle, instance, and scene structs
- `render.hpp` – triangle rasterization, clipping, and drawing logic
- `output.ppm` – generated image (after running)

## 🕯️ How to Run 🕯️

```bash
# Compile
g++ -std=c++11 main.cpp -o rasterizer

# Run
./rasterizer

# View output
open output.ppm # on macOS
