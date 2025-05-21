#ifndef RENDER_HPP
#define RENDER_HPP

#include <vector>
#include <cmath>
#include "canvas.hpp"
#include "geometry.hpp"
#include "model.hpp"

struct Plane {
    float a, b, c, d;
    Plane(float a = 0, float b = 0, float c = 0, float d = 0)
        : a(a), b(b), c(c), d(d) {}
};

float SignedDistance(const Plane& plane, const std::vector<float>& vertex) {
    return plane.a * vertex[0] + plane.b * vertex[1] + plane.c * vertex[2] + plane.d;
}

std::vector<float> IntersectLinePlane(const std::vector<float>& v0, const std::vector<float>& v1, const Plane& plane) {
    float d0 = SignedDistance(plane, v0);
    float d1 = SignedDistance(plane, v1);
    float t = d0 / (d0 - d1);
    return {
        v0[0] + t * (v1[0] - v0[0]),
        v0[1] + t * (v1[1] - v0[1]),
        v0[2] + t * (v1[2] - v0[2])
    };
}

std::vector<Triangle> ClipTriangle(const Triangle& triangle,
                                   std::vector<std::vector<float>>& vertices,
                                   const Plane& plane) {
    const auto& v0 = vertices[triangle.vertices[0]];
    const auto& v1 = vertices[triangle.vertices[1]];
    const auto& v2 = vertices[triangle.vertices[2]];

    float d0 = SignedDistance(plane, v0);
    float d1 = SignedDistance(plane, v1);
    float d2 = SignedDistance(plane, v2);

    bool in0 = d0 >= 0;
    bool in1 = d1 >= 0;
    bool in2 = d2 >= 0;

    int inside = in0 + in1 + in2;
    std::vector<Triangle> result;

    auto add_vertex = [&](const std::vector<float>& v) {
        vertices.push_back(v);
        return static_cast<int>(vertices.size() - 1);
    };

    if (inside == 3) return { triangle };
    if (inside == 0) return {};

    if (inside == 1) {
        int i0 = in0 ? 0 : (in1 ? 1 : 2);
        int i1 = (i0 + 1) % 3;
        int i2 = (i0 + 2) % 3;

        const auto& A = vertices[triangle.vertices[i0]];
        const auto& B = vertices[triangle.vertices[i1]];
        const auto& C = vertices[triangle.vertices[i2]];

        auto P = IntersectLinePlane(A, B, plane);
        auto Q = IntersectLinePlane(A, C, plane);

        int iA = triangle.vertices[i0];
        int iP = add_vertex(P);
        int iQ = add_vertex(Q);

        result.push_back(Triangle({ iA, iP, iQ }, triangle.color_index));
    } else if (inside == 2) {
        int o = !in0 ? 0 : (!in1 ? 1 : 2);
        int i1 = (o + 1) % 3;
        int i2 = (o + 2) % 3;

        const auto& A = vertices[triangle.vertices[i1]];
        const auto& B = vertices[triangle.vertices[i2]];
        const auto& C = vertices[triangle.vertices[o]];

        auto P = IntersectLinePlane(A, C, plane);
        auto Q = IntersectLinePlane(B, C, plane);

        int iA = triangle.vertices[i1];
        int iB = triangle.vertices[i2];
        int iP = add_vertex(P);
        int iQ = add_vertex(Q);

        result.push_back(Triangle({ iA, iB, iP }, triangle.color_index));
        result.push_back(Triangle({ iP, iB, iQ }, triangle.color_index));
    }

    return result;
}

Instance ClipTrianglesAgainstPlane(const Instance& instance, const Plane& plane) {
    Instance result = instance;
    result.model.triangles.clear();

    for (const auto& tri : instance.model.triangles) {
        auto new_tris = ClipTriangle(tri, result.model.vertices, plane);
        for (const auto& t : new_tris)
            result.model.triangles.push_back(t);
    }

    return result;
}

bool ClipInstanceAgainstPlane(const Instance& instance, const Plane& plane, Instance& outClipped) {
    float d = SignedDistance(plane, instance.bounding_sphere.center);
    float r = instance.bounding_sphere.radius;

    if (d > r) {
        outClipped = instance;
        return true;
    }
    if (d < -r) return false;

    outClipped = ClipTrianglesAgainstPlane(instance, plane);
    return true;
}

Instance* ClipInstance(const Instance& original, const std::vector<Plane>& planes) {
    Instance current = original;
    Instance temp;

    for (const auto& plane : planes) {
        if (!ClipInstanceAgainstPlane(current, plane, temp)) return nullptr;
        current = temp;
    }

    return new Instance(current);
}

Scene ClipScene(const Scene& scene, const std::vector<Plane>& planes) {
    Scene result;
    for (const auto& instance : scene.instances) {
        Instance temp = instance;
        Instance* clipped = ClipInstance(temp, planes);
        if (clipped != nullptr) {
            result.instances.push_back(*clipped);
            delete clipped;
        }
    }
    return result;
}

void RenderTriangle(Canvas& canvas,
                    const Triangle& tri,
                    const std::vector<std::vector<float>>& transformed,
                    const std::vector<Point>& projected,
                    bool filled = true) {
    const auto& v0 = transformed[tri.vertices[0]];
    const auto& v1 = transformed[tri.vertices[1]];
    const auto& v2 = transformed[tri.vertices[2]];

    Color color;
    switch (tri.color_index) {
        case 0: color = Color(15, 15, 15); break;     // Obsidian Black
        case 1: color = Color(120, 0, 15); break;     // Blood Crimson
        case 2: color = Color(90, 0, 30); break;      // Deep Maroon
        case 3: color = Color(70, 0, 80); break;      // Phantom Purple
        case 4: color = Color(40, 0, 40); break;      // Midnight Plum
        case 5: color = Color(100, 100, 100); break;  // Ashen Grey
        case 6: color = Color(160, 160, 160); break;  // Veiled Silver
        default: color = Color(255, 255, 255); break;
    }
    
    

    if (filled) {
        DrawFilledTriangle(canvas,
            projected[tri.vertices[0]], v0[2],
            projected[tri.vertices[1]], v1[2],
            projected[tri.vertices[2]], v2[2],
            color);
    } else {
        DrawTriangle(canvas,
            projected[tri.vertices[0]], v0[2],
            projected[tri.vertices[1]], v1[2],
            projected[tri.vertices[2]], v2[2],
            color);
    }
}

void RenderInstance(Canvas& canvas, const Instance& instance,
                    const std::vector<std::vector<float>>& cameraMatrix,
                    int canvasWidth, int canvasHeight, bool filled = true) {
    std::vector<Point> projected;
    std::vector<std::vector<float>> transformed;

    auto M = MultiplyMatrices(cameraMatrix, instance.transform);

    for (const auto& vertex : instance.model.vertices) {
        auto v = TransformVertex(vertex, M);
        transformed.push_back(v);
        projected.push_back(ProjectVertex(v, canvasWidth, canvasHeight));
    }

    for (const auto& tri : instance.model.triangles) {
        RenderTriangle(canvas, tri, transformed, projected, filled);
    }
}

void RenderScene(Canvas& canvas, const Scene& scene, const Camera& camera,
                 int canvasWidth, int canvasHeight, bool filled = true) {
    canvas.clear();
    auto M_camera = MakeCameraMatrix(camera.position, camera.orientation);

    std::vector<Plane> clipping_planes = {
        Plane(0, 0, 1, 1),   // near
        Plane(0, 0, -1, 10), // far
        Plane(1, 0, 0, 5),   // right
        Plane(-1, 0, 0, 5),  // left
        Plane(0, 1, 0, 5),   // top
        Plane(0, -1, 0, 5)   // bottom
    };

    Scene clipped = ClipScene(scene, clipping_planes);

    for (const auto& instance : clipped.instances) {
        RenderInstance(canvas, instance, M_camera, canvasWidth, canvasHeight, filled);
    }
}

#endif 