#ifndef GEOMETRY_HPP
#define GEOMETRY_HPP

#include <vector>
#include <cmath>
#include "canvas.hpp"


// Matrix traslacion
std::vector<std::vector<float>> MakeTranslationMatrix(float tx, float ty, float tz) {
    return {
        {1, 0, 0, tx},
        {0, 1, 0, ty},
        {0, 0, 1, tz},
        {0, 0, 0, 1}
    };
}

// Matrix escalado
std::vector<std::vector<float>> MakeScalingMatrix(float sx, float sy, float sz) {
    return {
        {sx, 0,  0,  0},
        {0,  sy, 0,  0},
        {0,  0,  sz, 0},
        {0,  0,  0,  1}
    };
}

// Matrix de rotacion en x
std::vector<std::vector<float>> MakeRotationXMatrix(float angle) {
    return {
        {1, 0, 0, 0},
        {0, std::cos(angle), -std::sin(angle), 0},
        {0, std::sin(angle),  std::cos(angle), 0},
        {0, 0, 0, 1}
    };
}

// Matrix de rotacion en y
std::vector<std::vector<float>> MakeRotationYMatrix(float angle) {
    return {
        { std::cos(angle), 0, std::sin(angle), 0},
        { 0,               1, 0,               0},
        {-std::sin(angle), 0, std::cos(angle), 0},
        { 0,               0, 0,               1}
    };
}

// Matrix de rotacion en z
std::vector<std::vector<float>> MakeRotationZMatrix(float angle) {
    return {
        {std::cos(angle), -std::sin(angle), 0, 0},
        {std::sin(angle),  std::cos(angle), 0, 0},
        {0,                0,               1, 0},
        {0,                0,               0, 1}
    };
}

// look at matrix
std::vector<std::vector<float>> MakeCameraMatrix(const std::vector<float>& position, const std::vector<float>& orientation) {
    return {
        {1, 0, 0, -position[0]},
        {0, 1, 0, -position[1]},
        {0, 0, 1, -position[2]},
        {0, 0, 0, 1}
    };
}

// Multiply 2 4x4 matrices
std::vector<std::vector<float>> MultiplyMatrices(const std::vector<std::vector<float>>& a, const std::vector<std::vector<float>>& b) {
    std::vector<std::vector<float>> result(4, std::vector<float>(4, 0));
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            for (int k = 0; k < 4; ++k) {
                result[i][j] += a[i][k] * b[k][j];
            }
        }
    }
    return result;
}

// Transform vertices 3D w matriz 4x4
std::vector<float> TransformVertex(const std::vector<float>& vertex, const std::vector<std::vector<float>>& transform) {
    std::vector<float> v = {vertex[0], vertex[1], vertex[2], 1.0f};
    std::vector<float> result(4, 0);
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result[i] += transform[i][j] * v[j];
        }
    }
    return {result[0], result[1], result[2]};
}

Point ViewportToCanvas(float x, float y, int canvasWidth, int canvasHeight) {
    const float Vw = 1.0f, Vh = 1.0f;
    return Point(x * canvasWidth / Vw, y * canvasHeight / Vh);
}

// Proyeccion perspectiva normalizacion y desplazamiento al canvas
Point ProjectVertex(const std::vector<float>& v, int canvasWidth, int canvasHeight) {
    const float d = 1.0f;
    Point point = ViewportToCanvas(v[0] * d / v[2], v[1] * d / v[2], canvasWidth, canvasHeight);
    return Point(point.x + canvasWidth / 2, canvasHeight / 2 - point.y);
}

#endif 