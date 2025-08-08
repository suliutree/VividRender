#include "ModelPass.h"
#include "Pipeline.h"
#include "Model.h"
#include "Texture2D.h"
#include "Camera.h"
#include "OpenGLCommandBuffer.h"
#include "OpenGLVertexBuffer.h"
#include "OpenGLIndexBuffer.h"
#include <iostream>

#define CHECK_UNIFORM(loc, name)                                               \
    if ((loc) == -1) {                                                         \
        std::cerr << "[ModelPass] Uniform '" << (name)                         \
                  << "' NOT found (location=-1)\n";                            \
    }

ModelPass::ModelPass(std::shared_ptr<PipelineState> p,
                     std::shared_ptr<Model> m,
                     Camera* cam, float* a)
    : pipeline(std::move(p))
    , model(std::move(m))
    , camera(cam)
    , aspect(a)
{
    name = "ModelPass";
}

const std::string& ModelPass::getName() const {
    return name;
}


void ModelPass::execute(IDevice* device, ICommandBuffer* cmd)
{
    cmd->bindPipeline(pipeline->getProgramID());

    // std::cout << "program id: " << pipeline->getProgramID() << std::endl;

    glm::mat4 modelMat = glm::translate(glm::mat4(1.0f), -model->getCenter());
    modelMat = glm::scale(modelMat, glm::vec3(0.1f));

    // ─────────────── 摄像机&投影 ───────────────
    glm::mat4 view = camera->getView();
    glm::mat4 proj = camera->getProj(*aspect);
    // glm::mat4 projMat = glm::perspective(glm::radians(camera->getZoom()), *aspect, 0.1f, 1000.0f);


    GLint locModel = pipeline->getUniformLocation("uModel");
    GLint locView = pipeline->getUniformLocation("uView");
    GLint locProj = pipeline->getUniformLocation("uProj");
    cmd->setUniformMat4(locModel, &modelMat[0][0]);
    cmd->setUniformMat4(locView, &view[0][0]);
    cmd->setUniformMat4(locProj, &proj[0][0]);

    // ─────────────── 光照 & 观察者 ───────────────
    glm::vec3 lightDir = glm::normalize(glm::vec3(0.5f, -1.f, 0.3f));

    GLint locLight = pipeline->getUniformLocation("uLightDir");
    GLint locViewP = pipeline->getUniformLocation("uViewPos");

    cmd->setUniform3f(locLight,
                      lightDir.x, lightDir.y, lightDir.z);
    cmd->setUniform3f(locViewP,
                      camera->Position.x, camera->Position.y, camera->Position.z);

    // ─────────────── 纹理 ───────────────
    // albedo->bind(GL_TEXTURE0); 直接这样调用是有问题的，相当于在非 OpenGL Contex 下调用了 GL 函数，一定要通过 Commond Buffer

    GLint locTex   = pipeline->getUniformLocation("uAlbedo");
    cmd->setUniform1i(locTex, 0);

    // ─────────────── Draw 所有 SubMesh ───────────────

    // for (std::size_t idx = 0; idx < model->getSubMeshes().size(); ++idx) {
    //     const auto& sm = model->getSubMeshes()[idx];

    //     if (sm.indexCount == 0) {
    //         std::cerr << "[ModelPass] SubMesh #" << idx << " has 0 indices, skip\n";
    //         continue;
    //     }

    //     glm::mat4 modelM(1.0f);          // 先统一 1，再按需 scale
    //     modelM = glm::scale(modelM, glm::vec3(0.2f));

    //     GLint locModel = pipeline->getUniformLocation("uModel");
    //     cmd->setUniformMat4(locModel, &modelM[0][0]);

    //     // 绑定 VAO & EBO
    //     const GLuint vao = static_cast<OpenGLVertexBuffer*>(sm.vb.get())->getVAO();
    //     if (vao == 0) {
    //         std::cerr << "[ModelPass] SubMesh #" << idx << " VAO == 0 (uninit?)\n";
    //         continue;
    //     }
    //     cmd->bindVertexArray(vao);
    //     static_cast<OpenGLIndexBuffer*>(sm.ib.get())->bind();

    //     cmd->draw(sm.indexCount);
    // }

    for (const auto& submesh : model->getSubMeshes()) {
        // 绑定该 SubMesh 的 VAO
        auto gl_vb = std::static_pointer_cast<OpenGLVertexBuffer>(submesh.vb);
        cmd->bindVertexArray(gl_vb->getVAO());

        // 绑定该 SubMesh 的纹理
        if (submesh.albedoTexture) {
            cmd->bindTexture(GL_TEXTURE0, submesh.albedoTexture->getID());
        } else {
            // 可选：如果没纹理，绑定一个1x1的白色纹理
            // cmd->bindTexture(GL_TEXTURE0, defaultWhiteTextureID);
        }

        // 使用索引绘制
        cmd->drawIndexed(submesh.ib, submesh.indexCount);
    }

    cmd->unbindPipeline();
}