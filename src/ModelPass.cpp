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
                     std::shared_ptr<Texture2D> tex,
                     Camera* cam, float* a)
    : pipeline(std::move(p)), model(std::move(m)),
      albedo(std::move(tex)), camera(cam), aspect(a) {
    name = "ModelPass";
}

const std::string& ModelPass::getName() const {
    return name;
}

// void ModelPass::execute(IDevice* device, ICommandBuffer* cmd) {
//     cmd->bindPipeline(pipeline->getProgramID());

//     // 常量
//     glm::mat4 view = camera->getView();
//     glm::mat4 proj = camera->getProj(*aspect);
//     cmd->setUniformMat4(pipeline->getUniformLocation("uView"), &view[0][0]);
//     cmd->setUniformMat4(pipeline->getUniformLocation("uProj"), &proj[0][0]);
//     glm::vec3 lightDir = glm::normalize(glm::vec3(0.5, -1, 0.3));
//     cmd->setUniform3f(pipeline->getUniformLocation("uLightDir"),
//                       lightDir.x, lightDir.y, lightDir.z);
//     cmd->setUniform3f(pipeline->getUniformLocation("uViewPos"),
//                       camera->Position.x, camera->Position.y, camera->Position.z);

//     albedo->bind(GL_TEXTURE0);
//     cmd->setUniform1i(pipeline->getUniformLocation("uAlbedo"), 0);

//     // 多 SubMesh
//     for (const auto& sm : model->getSubMeshes()) {
//         glm::mat4 modelM = glm::mat4(0.2f);       // 不做动画
//         cmd->setUniformMat4(pipeline->getUniformLocation("uModel"),
//                             &modelM[0][0]);

//         cmd->bindVertexArray(static_cast<OpenGLVertexBuffer*>(sm.vb.get())->getVAO());
//         static_cast<OpenGLIndexBuffer*>(sm.ib.get())->bind();
//         cmd->draw(sm.indexCount);
//     }

//     cmd->unbindPipeline();
// }


void ModelPass::execute(IDevice* device, ICommandBuffer* cmd)
{
    cmd->bindPipeline(pipeline->getProgramID());

    // std::cout << "program id: " << pipeline->getProgramID() << std::endl;

    // ─────────────── 摄像机&投影 ───────────────
    glm::mat4 view = camera->getView();
    glm::mat4 proj = camera->getProj(*aspect);

    GLint locView = pipeline->getUniformLocation("uView");
    GLint locProj = pipeline->getUniformLocation("uProj");
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
    cmd->bindTexture(GL_TEXTURE0, albedo->getID());
    cmd->setUniform1i(locTex, 0);

    // // ─────────────── Draw 所有 SubMesh ───────────────
    // if (model->getSubMeshes().empty()) {
    //     std::cerr << "[ModelPass] WARNING: Model has no sub-meshes!\n";
    // }

    for (std::size_t idx = 0; idx < model->getSubMeshes().size(); ++idx) {
        const auto& sm = model->getSubMeshes()[idx];

        if (sm.indexCount == 0) {
            std::cerr << "[ModelPass] SubMesh #" << idx << " has 0 indices, skip\n";
            continue;
        }

        glm::mat4 modelM(1.0f);          // 先统一 1，再按需 scale
        modelM = glm::scale(modelM, glm::vec3(0.2f));

        GLint locModel = pipeline->getUniformLocation("uModel");
        cmd->setUniformMat4(locModel, &modelM[0][0]);

        // 绑定 VAO & EBO
        const GLuint vao = static_cast<OpenGLVertexBuffer*>(sm.vb.get())->getVAO();
        if (vao == 0) {
            std::cerr << "[ModelPass] SubMesh #" << idx << " VAO == 0 (uninit?)\n";
            continue;
        }
        cmd->bindVertexArray(vao);
        static_cast<OpenGLIndexBuffer*>(sm.ib.get())->bind();

        cmd->draw(sm.indexCount);
    }

    cmd->unbindPipeline();
}