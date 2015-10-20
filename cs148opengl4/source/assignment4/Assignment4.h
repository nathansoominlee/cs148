#pragma once

#ifndef __ASSIGNMENT_4__
#define __ASSIGNMENT_4__

#include "common/Application.h"
#include "common/core.h" // <-- haha.

class Assignment4: public Application
{
public:
    Assignment4(std::shared_ptr<class Scene> inputScene, std::shared_ptr<class Camera> inputCamera);

    static std::unique_ptr<Application> CreateApplication(std::shared_ptr<class Scene> scene, std::shared_ptr<class Camera> camera);
    static std::shared_ptr<class Camera> CreateCamera();

    virtual glm::vec2 GetWindowSize() const;

    virtual void HandleInput(SDL_Keysym key, Uint32 state, Uint8 repeat, double timestamp, double deltaTime);
    virtual void Tick(double deltaTime);
protected:
    virtual void HandleWindowResize(float x, float y);

private:

    // for scanline image
    virtual void SetupScene();
    virtual void SetupFinalScene();
    virtual void AddPLight();    // key press 'p'
    virtual void AddDLight();    // key press 'd'
    virtual void AddHLight();    // key press 'h'

    // helper functions
    std::shared_ptr<EpicShader> MakeEpicShader(float metallic,
                                               float roughness,
                                               float specular,
                                               std::string texture);

    std::shared_ptr<class SceneObject> LoadObj(std::shared_ptr<ShaderProgram> shader, std::string obj);



    // for reference
    virtual void SetupDummy();
    virtual void SetupExample1();
    virtual void SetupExample1Epic();
    virtual void SetupExample2();
    virtual void SetupCamera();

    // shared
    std::shared_ptr<class Light> sunLight;
    std::shared_ptr<class Light> hemisphereLight;
    std::shared_ptr<class Light> pointLight;
    std::vector<std::shared_ptr<class SceneObject>> sphereDance;
    bool pLightIsOn;
    bool dLightIsOn;
    bool hLightIsOn;
};

#endif
