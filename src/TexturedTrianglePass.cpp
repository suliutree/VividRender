#include "TexturedTrianglePass.h"
#include "Pipeline.h"
#include "OpenGLVertexBuffer.h"
#include "OpenGLCommandBuffer.h"
#include "Texture2D.h"
#include "Camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>


TexturedTrianglePass::TexturedTrianglePass(std::shared_ptr<PipelineState> p,
                                           std::shared_ptr<IVertexBuffer> vb,
                                           std::shared_ptr<Texture2D> tex,
                                           Camera* cam,
                                           float* aspecPtr)
      : pipeline(std::move(p))
      , vb(std::move(vb))
      , texture(std::move(tex))
      , camera(cam) 
      , aspect(aspecPtr)
      {}

const std::string& TexturedTrianglePass::getName() const {
    static std::string n = "TexturedTrianglePass";
    return n;
}

void TexturedTrianglePass::execute(IDevice* device, ICommandBuffer* cmd) {
    GLuint prog = pipeline->getProgramID();
    cmd->bindPipeline(prog);

    GLint locModel = pipeline->getUniformLocation("uModel");
    GLint locView  = pipeline->getUniformLocation("uView");
    GLint locProj  = pipeline->getUniformLocation("uProj");

    glm::mat4 model(1.0f);
    glm::mat4 view = camera->getView();
    glm::mat4 proj = camera->getProj(*aspect);

    cmd->setUniformMat4(locModel, glm::value_ptr(model));
    cmd->setUniformMat4(locView,  glm::value_ptr(view));
    cmd->setUniformMat4(locProj,  glm::value_ptr(proj));

    GLint locTex   = pipeline->getUniformLocation("uTex");
    cmd->bindTexture(GL_TEXTURE0, texture->getID());
    cmd->setUniform1i(locTex, 0);

    auto glVB = static_cast<OpenGLVertexBuffer*>(vb.get());
    cmd->bindVertexArray(glVB->getVAO());

    cmd->draw(3);

    cmd->unbindPipeline();
}