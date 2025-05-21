#ifndef MODEL_HPP
#define MODEL_HPP

#include <vector>
#include <cmath>

// vertices + color
struct Triangle {
    std::vector<int> vertices;
    int color_index;

    Triangle(const std::vector<int>& v = {}, int ci = 0)
        : vertices(v), color_index(ci) {}
};

// Model vertices y triangles
struct Model {
    std::vector<std::vector<float>> vertices;
    std::vector<Triangle> triangles;
};

// frustum culling rápido
struct BoundingSphere {
    std::vector<float> center;
    float radius;

    BoundingSphere(const std::vector<float>& c = {0, 0, 0}, float r = 1.0f)
        : center(c), radius(r) {}
};

// modelo + transformación + bounding sphere
struct Instance {
    Model model;
    std::vector<std::vector<float>> transform;
    BoundingSphere bounding_sphere;

    Instance(const Model& m = Model(),
             const std::vector<std::vector<float>>& t = {
                 {1, 0, 0, 0},
                 {0, 1, 0, 0},
                 {0, 0, 1, 0},
                 {0, 0, 0, 1}},
             const BoundingSphere& bs = BoundingSphere())
        : model(m), transform(t), bounding_sphere(bs) {}
};

// Camera position y orientacion
struct Camera {
    std::vector<float> position = {0, 0, 0};
    std::vector<float> orientation = {0, 0, 0}; // no se usa 
};

struct Scene {
    std::vector<Instance> instances;
};

#endif 
