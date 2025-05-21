#ifndef CANVAS_HPP
#define CANVAS_HPP

#include <vector>
#include <limits>
#include <fstream>
#include <algorithm>
#include <cmath>

class Color {
public:
    int r, g, b;
    Color(int r = 0, int g = 0, int b = 0) : r(r), g(g), b(b) {}
};

class Point {
public:
    float x, y;
    Point(float x = 0, float y = 0) : x(x), y(y) {}
};

class Canvas {
private:
    int width, height;
    std::vector<std::vector<Color>> pixels;
    std::vector<std::vector<float>> depthBuffer;
    Color background;
    const float INITIAL_DEPTH = std::numeric_limits<float>::max();

public:
    Canvas(int w, int h, Color bg = Color(0, 0, 0))
        : width(w), height(h), background(bg) {
        pixels.resize(height, std::vector<Color>(width, background));
        depthBuffer.resize(height, std::vector<float>(width, INITIAL_DEPTH));
    }

    void setPixel(int x, int y, float z, const Color& color) {
        if (x >= 0 && x < width && y >= 0 && y < height) {
            if (z < depthBuffer[y][x]) {
                pixels[y][x] = color;
                depthBuffer[y][x] = z;
            }
        }
    }

    void clear() {
        for (auto& row : pixels) {
            std::fill(row.begin(), row.end(), background);
        }
        for (auto& row : depthBuffer) {
            std::fill(row.begin(), row.end(), INITIAL_DEPTH);
        }
    }

    void writePPM(const std::string& filename) {
        std::ofstream file(filename);
        file << "P3\n" << width << " " << height << "\n255\n";
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                file << pixels[y][x].r << " "
                     << pixels[y][x].g << " "
                     << pixels[y][x].b << " ";
            }
            file << "\n";
        }
        file.close();
    }
};

std::vector<float> Interpolate(int i0, float d0, int i1, float d1) {
    std::vector<float> values;
    if (i0 == i1) {
        values.push_back(d0);
        return values;
    }
    float a = (d1 - d0) / (i1 - i0);
    float d = d0;
    for (int i = i0; i <= i1; i++) {
        values.push_back(d);
        d += a;
    }
    return values;
}

// Interpolate de profundidad
void DrawLine(Canvas& canvas, const Point& P0, float z0, const Point& P1, float z1, const Color& color) {
    int x0 = static_cast<int>(P0.x);
    int y0 = static_cast<int>(P0.y);
    int x1 = static_cast<int>(P1.x);
    int y1 = static_cast<int>(P1.y);

    if (std::abs(x1 - x0) > std::abs(y1 - y0)) {
        if (x0 > x1) {
            std::swap(x0, x1); std::swap(y0, y1); std::swap(z0, z1);
        }
        auto ys = Interpolate(x0, static_cast<float>(y0), x1, static_cast<float>(y1));
        auto zs = Interpolate(x0, z0, x1, z1);
        for (int x = x0; x <= x1; x++) {
            canvas.setPixel(x, static_cast<int>(ys[x - x0]), zs[x - x0], color);
        }
    } else {
        if (y0 > y1) {
            std::swap(x0, x1); std::swap(y0, y1); std::swap(z0, z1);
        }
        auto xs = Interpolate(y0, static_cast<float>(x0), y1, static_cast<float>(x1));
        auto zs = Interpolate(y0, z0, y1, z1);
        for (int y = y0; y <= y1; y++) {
            canvas.setPixel(static_cast<int>(xs[y - y0]), y, zs[y - y0], color);
        }
    }
}

// Triangle mode wireframe
void DrawTriangle(Canvas& canvas,
                  const Point& P0, float z0,
                  const Point& P1, float z1,
                  const Point& P2, float z2,
                  const Color& color) {
    DrawLine(canvas, P0, z0, P1, z1, color);
    DrawLine(canvas, P1, z1, P2, z2, color);
    DrawLine(canvas, P2, z2, P0, z0, color);
}

// Tringle con interpolación de z
void DrawFilledTriangle(Canvas& canvas,
                        const Point& P0, float z0,
                        const Point& P1, float z1,
                        const Point& P2, float z2,
                        const Color& color) {
    int x0 = static_cast<int>(P0.x);
    int y0 = static_cast<int>(P0.y);
    int x1 = static_cast<int>(P1.x);
    int y1 = static_cast<int>(P1.y);
    int x2 = static_cast<int>(P2.x);
    int y2 = static_cast<int>(P2.y);

    if (y1 < y0) { std::swap(x0, x1); std::swap(y0, y1); std::swap(z0, z1); }
    if (y2 < y0) { std::swap(x0, x2); std::swap(y0, y2); std::swap(z0, z2); }
    if (y2 < y1) { std::swap(x1, x2); std::swap(y1, y2); std::swap(z1, z2); }

    auto x01 = Interpolate(y0, static_cast<float>(x0), y1, static_cast<float>(x1));
    auto z01 = Interpolate(y0, z0, y1, z1);
    auto x12 = Interpolate(y1, static_cast<float>(x1), y2, static_cast<float>(x2));
    auto z12 = Interpolate(y1, z1, y2, z2);
    auto x02 = Interpolate(y0, static_cast<float>(x0), y2, static_cast<float>(x2));
    auto z02 = Interpolate(y0, z0, y2, z2);

    x01.pop_back(); z01.pop_back();
    auto x012 = x01; x012.insert(x012.end(), x12.begin(), x12.end());
    auto z012 = z01; z012.insert(z012.end(), z12.begin(), z12.end());

    int m = x012.size() / 2;
    std::vector<float> x_left, x_right, z_left, z_right;

    if (x02[m] < x012[m]) {
        x_left = x02; z_left = z02;
        x_right = x012; z_right = z012;
    } else {
        x_left = x012; z_left = z012;
        x_right = x02; z_right = z02;
    }

    for (int y = y0; y <= y2; y++) {
        int y_idx = y - y0;
        int x_l = static_cast<int>(x_left[y_idx]);
        int x_r = static_cast<int>(x_right[y_idx]);
        auto z_segment = Interpolate(x_l, z_left[y_idx], x_r, z_right[y_idx]);
        for (int x = x_l; x <= x_r; x++) {
            float z = z_segment[x - x_l];
            canvas.setPixel(x, y, z, color);
        }
    }
}

#endif 
