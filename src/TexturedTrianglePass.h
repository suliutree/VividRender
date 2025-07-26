#pragma once
#include "RenderPass.h"
#include <memory>
class PipelineState;
class IVertexBuffer;
class Texture2D;
class Camera;

class TexturedTrianglePass : public RenderPass {
public:
    TexturedTrianglePass(std::shared_ptr<PipelineState> p,
                         std::shared_ptr<IVertexBuffer> vb,
                         std::shared_ptr<Texture2D> tex,
                         Camera* cam,
                         float* aspecPtr);

    const std::string& getName() const override;
    void execute(IDevice* device, ICommandBuffer* cmd) override;

private:
    std::string name;

    std::shared_ptr<PipelineState> pipeline;
    std::shared_ptr<IVertexBuffer> vb;
    std::shared_ptr<Texture2D>     texture;

    Camera* camera = nullptr;
    float* aspect = nullptr;
};