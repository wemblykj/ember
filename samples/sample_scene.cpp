#include "sample_scene.h"
#include "core/logger.h"

namespace ember::samples {

SampleScene::SampleScene() {
}

SampleScene::~SampleScene() {
    shutdown();
}

bool SampleScene::initialize(graphics::Renderer* renderer) {
    using namespace ember::core;

    renderer_ = renderer;

    // Setup projection matrix (60 degree FOV)
    projectionMatrix_ = perspective(glm::radians(60.0f), 1280.0f / 720.0f, 0.1f, 100.0f);

    // Setup view matrix (camera position)
    viewMatrix_ = lookAt(
        Vec3(0.0f, 3.0f, 5.0f),  // Camera position
        Vec3(0.0f, 0.0f, 0.0f),  // Look at
        Vec3(0.0f, 1.0f, 0.0f)   // Up vector
    );

    // Setup model matrix (identity)
    modelMatrix_ = glm::mat4(1.0f);

    EMBER_LOG_INFO("Sample scene initialized");
    return true;
}

void SampleScene::update(float deltaTime) {
    // Rotate the model
    rotationAngle_ += deltaTime * 1.0f;  // 1 radian per second
    modelMatrix_ = rotate(rotationAngle_, core::Vec3(0.0f, 1.0f, 0.0f));
}

void SampleScene::render() {
    if (!renderer_) {
        return;
    }

    // Begin frame
    renderer_->beginFrame();

    // In a full implementation, we would:
    // 1. Bind pipelines
    // 2. Set render targets
    // 3. Bind descriptor sets with matrices
    // 4. Draw geometry

    renderer_->endFrame();
}

void SampleScene::shutdown() {
    renderer_ = nullptr;
}

}  // namespace ember::samples
