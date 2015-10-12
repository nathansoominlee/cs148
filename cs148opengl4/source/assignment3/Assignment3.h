#pragma once

#ifndef __ASSIGNMENT_3__
#define __ASSIGNMENT_3__

#include "common/Application.h"

class Assignment3: public Application
{
public:
    Assignment3(std::shared_ptr<class Scene> inputScene, std::shared_ptr<class Camera> inputCamera);

    static std::unique_ptr<Application> CreateApplication(std::shared_ptr<class Scene> scene, std::shared_ptr<class Camera> camera);
    static std::shared_ptr<class Camera> CreateCamera();

    virtual glm::vec2 GetWindowSize() const;

    virtual void HandleInput(SDL_Keysym key, Uint32 state, Uint8 repeat, double timestamp, double deltaTime);
    virtual void Tick(double deltaTime);
protected:
    virtual void HandleWindowResize(float x, float y);

private:
    
    // relevant to the demo.
    virtual void SetupScene();
    virtual void SetupCamera();
    virtual void SetupDummy();
    virtual void AddPLight();    // key press 'p'
    virtual void AddDLight();    // key press 'd'
    virtual void AddHLight();    // key press 'h'
    
    // for reference
    virtual void SetupEpic();
    virtual void GenericSetupExample(std::shared_ptr<class ShaderProgram> shader, std::shared_ptr<ShaderProgram> groundShader);

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
