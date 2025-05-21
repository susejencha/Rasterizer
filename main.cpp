#include "canvas.hpp"
#include "geometry.hpp"
#include "model.hpp"
#include "render.hpp"
#include <iostream>
#include <cmath>

int main() {
    const int Cw = 900;
    const int Ch = 600;

    Canvas canvas(Cw, Ch, Color(50, 50, 50));

   
    Model cube;
    cube.vertices = {
        { 1,  1,  1}, {-1,  1,  1}, {-1, -1,  1}, { 1, -1,  1},
        { 1,  1, -1}, {-1,  1, -1}, {-1, -1, -1}, { 1, -1, -1}
    };

    cube.triangles = {
        Triangle({0, 1, 2}, 2), Triangle({0, 2, 3}, 2),
        Triangle({4, 0, 3}, 3), Triangle({4, 3, 7}, 3),
        Triangle({5, 4, 7}, 1), Triangle({5, 7, 6}, 1),
        Triangle({1, 5, 6}, 5), Triangle({1, 6, 2}, 5),
        Triangle({4, 5, 1}, 4), Triangle({4, 1, 0}, 4),
        Triangle({2, 6, 7}, 6), Triangle({2, 7, 3}, 6)
    };

    Scene scene;

    // Original CENTER
    Instance originalCube(cube,
        MakeTranslationMatrix(0, 0, 8.0f),
        BoundingSphere({0, 0, 8}, std::sqrt(3))
    );
    scene.instances.push_back(originalCube);

    // Trasladado LEFT
    Instance translatedCube(cube,
        MakeTranslationMatrix(-3.0f, -1.0f, 7.0f),
        BoundingSphere({-2, -1, 7}, std::sqrt(3))
    );
    scene.instances.push_back(translatedCube);

    // Scaled RIGHT
    auto scale = MakeScalingMatrix(0.5f, 0.5f, 0.5f);
    auto transformScaled = MultiplyMatrices(MakeTranslationMatrix(3.0f, 1.0f, 9.0f), scale);
    Instance scaledCube(cube,
        transformScaled,
        BoundingSphere({3, 1, 9}, std::sqrt(3) * 0.5f)
    );
    scene.instances.push_back(scaledCube);

    Camera camera;
    camera.position = {0, 0, 0};

    RenderScene(canvas, scene, camera, Cw, Ch, true);
    canvas.writePPM("output.ppm");

    std::cout << "Rendered image saved to output.ppm" << std::endl;
    return 0;
}
