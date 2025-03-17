#include "Bloom.hpp"
#include "FrameEngine.hpp"

using namespace FrameEngine;

class BloomDemo : public Engine {
private:
  Object *glowingSphere = nullptr;
  Object *nonGlowingCube = nullptr;

  PointLight *redLight = nullptr;

  BloomEffect *bloomEffect = nullptr;

  float timeAccumulator = 0.0f;

public:
  void on_start() override {
    Shader *basicShader = new Shader("shaders/basic.vs", "shaders/basic.fs");

    // --- Base Plane ---
    Mesh *planeMesh = MeshGenerator::createPlane(20.f, 20.f);
    Object *plane = new Object(planeMesh);
    plane->setMaterial(new BasicMaterial(basicShader, Vector3(1.f, 1.f, 1.f),
                                         Vector3(1.0f, 1.0f, 1.0f), 10.0f));
    plane->transform->position = Vector3(0, -2, 0);

    // --- Glowing Sphere ---
    Mesh *sphereMesh = MeshGenerator::createSphere(32, 32);
    glowingSphere = new Object(sphereMesh);

    BasicMaterial *glowMat =
        new BasicMaterial(basicShader, Vector3(8.0f, 8.0f, 1.0f),
                          Vector3(1.0f, 1.0f, 1.0f), 64.0f);
    glowMat->setEmissiveEnabled(true);
    glowMat->setEmissiveColor(Vector3(1.0f, 1.0f, 0.0f));
    glowingSphere->setMaterial(glowMat);

    glowingSphere->transform->scale = Vector3(2.0f, 2.0f, 2.0f);
    glowingSphere->transform->position = Vector3(-2.0f, 0.0f, 0.0f);

    // --- Non-glowing Cube ---
    Mesh *cubeMesh = MeshGenerator::createCube();
    nonGlowingCube = new Object(cubeMesh);

    nonGlowingCube->setMaterial(
        new BasicMaterial(basicShader, Vector3(0.7f, 0.7f, 0.7f),
                          Vector3(1.0f, 1.0f, 1.0f), 32.0f));
    nonGlowingCube->transform->position = Vector3(1.1f, 0.0f, 0.0f);

    // --- Yellow Point Light ---
    redLight = new PointLight(Vector3(0.0f, 3.0f, -3.0f),
                              Vector3(1.0f, 1.0f, 0.0f), .7f);

    // --- Camera Setup ---
    Camera &cam = renderer.getCamera();
    cam.transform->position = Vector3(0.0f, 2.0f, -10.0f);
    cam.lookAt(Vector3(0.0f, 0.0f, 0.0f));
    cam.setProjection(60.0f, 16.0f / 9.0f, 0.1f, 100.0f);

    // --- Post-Processing Bloom Setup ---
    renderer.clearPostProcessingEffects();
    bloomEffect = new BloomEffect();
    bloomEffect->setBloomIntensity(1.f);
    bloomEffect->setThreshold(0.6f);
    bloomEffect->setSoftThreshold(0.2f);
    renderer.addPostProcessingEffect(bloomEffect);
  }

  void fixed_update(float dt) override {
    timeAccumulator += dt;
    glowingSphere->transform->position.x = -2.0f + sin(timeAccumulator);
    redLight->transform->position = glowingSphere->transform->position;
    nonGlowingCube->rotate(Vector3(0, 1, .6), 30.f * dt);
  }
};

int main() {
  BloomDemo demo;
  demo.init();
  demo.run();
  return 0;
}
