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

const std::string& ModelPass::getName() const { return name; }

void ModelPass::execute(IDevice* device, ICommandBuffer* cmd)
{
    cmd->bindPipeline(pipeline->getProgramID());

    // 可按需居中/缩放
    glm::mat4 globalModel = glm::mat4(1.0f);
    // globalModel = glm::translate(globalModel, -model->getCenter());
    globalModel = glm::scale(globalModel, glm::vec3(0.1f));

    glm::mat4 view = camera->getView();
    glm::mat4 proj = camera->getProj(*aspect);

    GLint locModel = pipeline->getUniformLocation("uModel");
    GLint locView  = pipeline->getUniformLocation("uView");
    GLint locProj  = pipeline->getUniformLocation("uProj");
    CHECK_UNIFORM(locModel, "uModel");
    CHECK_UNIFORM(locView,  "uView");
    CHECK_UNIFORM(locProj,  "uProj");

    cmd->setUniformMat4(locView, &view[0][0]);
    cmd->setUniformMat4(locProj, &proj[0][0]);

    // 光照
    glm::vec3 lightDir = glm::normalize(glm::vec3(0.5f, -1.f, 0.3f));
    GLint locLight = pipeline->getUniformLocation("uLightDir");
    GLint locViewP = pipeline->getUniformLocation("uViewPos");
    if (locLight != -1) cmd->setUniform3f(locLight, lightDir.x, lightDir.y, lightDir.z);
    if (locViewP != -1) cmd->setUniform3f(locViewP, camera->Position.x, camera->Position.y, camera->Position.z);

    // 采样器
    GLint locTex = pipeline->getUniformLocation("uAlbedo");
    if (locTex != -1) cmd->setUniform1i(locTex, 0);

    // ★（可选）法线矩阵，如果着色器支持 uNormalMat（mat3）
    GLint locNormalMat = pipeline->getUniformLocation("uNormalMat");

    for (const auto& submesh : model->getSubMeshes()) {
        // ★ 每个 submesh 的最终模型矩阵
        glm::mat4 modelMat = globalModel * submesh.localTransform;
        cmd->setUniformMat4(locModel, &modelMat[0][0]);

        if (locNormalMat != -1) {
            glm::mat3 normalMat = glm::transpose(glm::inverse(glm::mat3(modelMat)));
            cmd->setUniformMat3(locNormalMat, &normalMat[0][0]);
        }

        // 绑定 VAO
        auto gl_vb = std::static_pointer_cast<OpenGLVertexBuffer>(submesh.vb);
        cmd->bindVertexArray(gl_vb->getVAO());

        // 绑定纹理
        if (submesh.albedoTexture) {
            cmd->bindTexture(GL_TEXTURE0, submesh.albedoTexture->getID());
        } else {
            // 没有纹理可绑定白贴图，避免采样到历史绑定
            // cmd->bindTexture(GL_TEXTURE0, defaultWhiteTextureID);
        }

        // 绘制
        cmd->drawIndexed(submesh.ib, submesh.indexCount);
    }

    cmd->unbindPipeline();
}
