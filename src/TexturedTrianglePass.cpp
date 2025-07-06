#include "TexturedTrianglePass.h"
#include "Pipeline.h"
#include "OpenGLVertexBuffer.h"
#include "OpenGLCommandBuffer.h"
#include "Texture2D.h"

#include <iostream>


TexturedTrianglePass::TexturedTrianglePass(std::shared_ptr<PipelineState> p,
                                           std::shared_ptr<IVertexBuffer> vb,
                                           std::shared_ptr<Texture2D> tex)
      : pipeline(std::move(p)), vb(std::move(vb)), texture(std::move(tex)) {}

const std::string& TexturedTrianglePass::getName() const {
    static std::string n = "TexturedTrianglePass";
    return n;
}

void TexturedTrianglePass::execute(IDevice* device, ICommandBuffer* cmd) {
    GLuint prog = pipeline->getProgramID();
    cmd->bindPipeline(prog);

    auto glVB = static_cast<OpenGLVertexBuffer*>(vb.get());
    cmd->bindVertexArray(glVB->getVAO());

    GLint texLoc = pipeline->getUniformLocation("uTexture");
    cmd->bindTexture(GL_TEXTURE0, texture->getID());
    cmd->setUniform1i(texLoc, 0);

    cmd->draw(3);

    cmd->unbindPipeline();
}