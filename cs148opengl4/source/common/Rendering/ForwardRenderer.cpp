#include "common/Rendering/ForwardRenderer.h"
#include "common/Scene/Scene.h"
#include "common/Scene/SceneObject.h"
#include "common/Scene/Camera/Camera.h"
#include "common/Rendering/RenderingObject.h"
#include "common/Rendering/Shaders/ShaderProgram.h"

#include <algorithm>

ForwardRenderer::ForwardRenderer(std::shared_ptr<class Scene> inputScene, std::shared_ptr<class Camera> inputCamera):
    Renderer(std::move(inputScene), std::move(inputCamera))
{
}

ForwardRenderer::~ForwardRenderer()
{
}

void ForwardRenderer::Initialize()
{
    std::unordered_map<GLenum, std::string> shaderSpec = {
        { GL_VERTEX_SHADER, "required/pass/pass.vert" },
        { GL_FRAGMENT_SHADER, "required/pass/pass.frag"}
    };
    depthPrepassShader = std::make_shared<ShaderProgram>(shaderSpec);
}

void ForwardRenderer::Render()
{
    auto totalObjects = scene->GetTotalObjects();
    auto totalLights = scene->GetTotalLights();
    auto totalRenderingPasses = totalLights + 2;

    // Render passes:
    //   0 - Depth Prepass
    //   1 - Global Light for the Objects (Ambient, Emissive)
    //   Rest - Diffuse/Specular/etc Lighting for Objects
    for (decltype(totalRenderingPasses) p = 0; p < totalRenderingPasses; ++p) {
        OGL_CALL(glDepthMask((p == 0) ? GL_TRUE : GL_FALSE));
        if (p == 0) {
            OGL_CALL(glDisable(GL_POLYGON_OFFSET_FILL));
        } else {
            // Need this to prevent z-fighting with the depth prepass!
            OGL_CALL(glEnable(GL_POLYGON_OFFSET_FILL));
            OGL_CALL(glPolygonOffset(-1.f, -1.f));
        }

        for (decltype(totalObjects) i = 0; i < totalObjects; ++i) {
            const SceneObject& sceneObject = scene->GetSceneObject(i); 

            auto totalRenderObjects = sceneObject.GetTotalRenderObjects();
            for (decltype(totalRenderObjects) r = 0; r < totalRenderObjects; ++r) {
                const RenderingObject* renderObject = sceneObject.GetRenderObject(r);
                if (!renderObject) {
                    continue;
                }

                const ShaderProgram* shaderToUse = (p == 0) ? depthPrepassShader.get() : renderObject->GetShaderProgramRaw();
                assert(shaderToUse);

                shaderToUse->StartUseShader();

                renderObject->BeginRender();

                // Make the scene object setup its shader with any data its shader might need.
                // Start from the end because the very 'last' light handles the global lighting for the object.
                // A light that is a nullptr will cause the shader to perform default behavior, whatever that may be.
                const Light* lightObject = scene->GetLightObject(totalRenderingPasses - p - 1);
                sceneObject.PrepareShaderForRendering(shaderToUse, camera.get(), lightObject);

                renderObject->Render();
                renderObject->EndRender();

                shaderToUse->StopUseShader();
            }
        }
    }
    OGL_CALL(glDepthMask(GL_TRUE));
}
