#pragma once
#include "RenderPass.h"
#include <memory>
class PipelineState;
class Model;
class Camera;

class ModelPass : public RenderPass {
public:
    ModelPass(std::shared_ptr<PipelineState> pipeline,
              std::shared_ptr<Model> model,
              Camera* cam,
              float* aspectPtr);

    const std::string& getName() const override;
    void execute(IDevice* device, ICommandBuffer* cmd) override;

private:
    std::string name;
    std::shared_ptr<PipelineState> pipeline;
    std::shared_ptr<Model> model;
    Camera* camera;
    float* aspect;
};
