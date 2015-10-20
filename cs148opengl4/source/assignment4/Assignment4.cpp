#include "assignment4/Assignment4.h"
#include "common/Utility/Mesh/Simple/PrimitiveCreator.h"
#include "common/Utility/Mesh/Loading/MeshLoader.h"
#include "common/Utility/Texture/TextureLoader.h"

Assignment4::Assignment4(std::shared_ptr<class Scene> inputScene, std::shared_ptr<class Camera> inputCamera):
    Application(std::move(inputScene), std::move(inputCamera))
{
}

std::unique_ptr<Application> Assignment4::CreateApplication(std::shared_ptr<class Scene> scene, std::shared_ptr<class Camera> camera)
{
    return make_unique<Assignment4>(std::move(scene), std::move(camera));
}

std::shared_ptr<class Camera> Assignment4::CreateCamera()
{
    // Specify any old aspect ratio for now, we'll update it later once the window gets made!
    // Read more about Field of View: http://rg3.name/201210281829.html!
    // Note that our field of view is the VERTICAL field of view (in degrees).
    return std::make_shared<PerspectiveCamera>(60.f, 1280.f / 720.f);
}

glm::vec2 Assignment4::GetWindowSize() const
{
    return glm::vec2(1280.f, 720.f);
}

void Assignment4::SetupScene()
{
    SetupFinalScene();
}

void Assignment4::SetupCamera()
{
    PerspectiveCamera* pcamera = static_cast<PerspectiveCamera*>(camera.get());
  
    // Set camera field of view and clipping planes
    pcamera->SetFOV(35.9f);
    pcamera->SetZNear(0.1f);
    pcamera->SetZFar(1000.f);
    
    // Set camera position and rotation
    camera->SetPosition(glm::vec3(-0.607f, 40.868f, 115.363f));
    camera->Rotate(glm::vec3(1.f, 0.f, 0.f), -10.745f * PI / 180.f);
    camera->Rotate(glm::vec3(0.f, 1.f, 0.f), -0.089f* PI / 180.f);
    camera->Rotate(glm::vec3(0.f, 0.f, 1.f), 0.f * PI / 180.f);

}

void Assignment4::HandleInput(SDL_Keysym key, Uint32 state, Uint8 repeat, double timestamp, double deltaTime)
{   
    if (state != SDL_KEYDOWN) {
        return;
    }

    switch (key.sym) {
    case SDLK_1:
        if (!repeat && state == SDL_KEYDOWN) {
            SetupExample1();
        }
        break;
    case SDLK_2:
        if (!repeat && state == SDL_KEYDOWN) {
            SetupExample1Epic();
        }
        break;
    case SDLK_3:
        if (!repeat && state == SDL_KEYDOWN) {
            SetupDummy();
        }
        break;
    case SDLK_4:
        if (!repeat && state == SDL_KEYDOWN) {
            SetupExample2();
        }
        break;
    case SDLK_p:
        AddPLight();
        break;
    case SDLK_d:
        AddDLight();
        break;
    case SDLK_h:
        AddHLight();
        break;
    case SDLK_c:
        scene->ClearLights();
        break;
    case SDLK_UP:
        camera->Rotate(glm::vec3(camera->GetRightDirection()), 0.1f);
        break;
    case SDLK_DOWN:
        camera->Rotate(glm::vec3(camera->GetRightDirection()), -0.1f);
        break;
    case SDLK_RIGHT:
        camera->Rotate(glm::vec3(SceneObject::GetWorldUp()), -0.1f);
        break;
    case SDLK_LEFT:
        camera->Rotate(glm::vec3(SceneObject::GetWorldUp()), 0.1f);
        break;
    case SDLK_w:
        camera->Translate(glm::vec3(camera->GetForwardDirection() * 0.3f));
        break;
    case SDLK_a:
        camera->Translate(glm::vec3(camera->GetRightDirection() * -0.3f));
        break;
    case SDLK_s:
        camera->Translate(glm::vec3(camera->GetForwardDirection() * -0.3f));
        break;
    case SDLK_SPACE:
        camera->Translate(glm::vec3(camera->GetUpDirection() * 0.3f));
        break;
    case SDLK_LCTRL:
    case SDLK_RCTRL:
        camera->Translate(glm::vec3(camera->GetUpDirection() * -0.3f));
        break;
    default:
        Application::HandleInput(key, state, repeat, timestamp, deltaTime);
        break;
    }
}

void Assignment4::HandleWindowResize(float x, float y)
{
    Application::HandleWindowResize(x, y);
    std::static_pointer_cast<PerspectiveCamera>(camera)->SetAspectRatio(x / y);
}

// Returns an epic shader with the given parameters
// and texture. Any objects with different parameter/textures
// need different shaders.
std::shared_ptr<EpicShader> Assignment4::MakeEpicShader(float metallic,
                                                          float roughness,
                                                          float specular,
                                                          std::string texture)
{
    std::unordered_map<GLenum, std::string> shaderSpec = {
        { GL_VERTEX_SHADER, "brdf/epicshader/frag/noSubroutine/epicshader.vert" },
        { GL_FRAGMENT_SHADER, "brdf/epicshader/frag/noSubroutine/epicshader.frag"}
    };

    std::shared_ptr<EpicShader> shader = std::make_shared<EpicShader>(shaderSpec, GL_FRAGMENT_SHADER);
    shader->SetMetallic(metallic);
    shader->SetRoughness(roughness);
    shader->SetSpecular(specular);

    if (!texture.empty())
    {
        shader->SetTexture(EpicShader::TextureSlots::DIFFUSE, TextureLoader::LoadTexture(texture));
    }

    return shader;
}

// Loads an .obj file, assigns it a shader, and adds it to the scene.
std::shared_ptr<class SceneObject> Assignment4::LoadObj(std::shared_ptr<ShaderProgram> shader, std::string obj)
{
    std::vector<std::shared_ptr<RenderingObject>> meshTemplate = MeshLoader::LoadMesh(shader, obj);
    if (meshTemplate.empty()) 
    {
        std::cerr << "ERROR: Failed to load the model. Check your paths." << std::endl;
        return nullptr;
    }       

    // Make scene object and add it to the scene
    std::shared_ptr<class SceneObject> sceneObject = std::make_shared<SceneObject>(meshTemplate);
    scene->AddSceneObject(sceneObject);

    // Return handle to scene object so caller can translate/rotate/scale it
    return sceneObject;
}

void Assignment4::SetupFinalScene()
{
    scene->ClearScene();
    
    std::string texture = "dummy/Texture/dummy_wood.jpg";
    float metallic = 0.5f;
    float roughness = 0.f;
    float specular = 0.5f;
    std::shared_ptr<EpicShader> shader = MakeEpicShader(metallic, roughness, specular, texture);
    
    // load the dummy objects
    std::array<std::string, 3> dummy_objects = {"dummy/Model/dummy.obj", 
                                                "dummy/Model/base.obj",
                                                "dummy/Model/support.obj"};

    std::vector<std::shared_ptr<RenderingObject>> meshTemplate;
    for (auto dummy_object : dummy_objects)
    {
        LoadObj(shader, dummy_object);
    }

    // add the frog!
    texture = "frog/Texture/frog_texture.png";
    metallic = 0.5f;
    roughness = 0.f;
    specular = 0.5f;
    shader = MakeEpicShader(metallic, roughness, specular, texture);

    std::string object = "frog/Model/frog.obj";
    std::shared_ptr<class SceneObject> sceneObject = LoadObj(shader, object);

    // place the frog
    sceneObject->SetPosition(glm::vec3(-5.317f, -2.027f, 2.966f));
    sceneObject->Rotate(glm::vec3(1.f, 0.f, 0.f), 8.441f * PI / 180.f);
    sceneObject->Rotate(glm::vec3(0.f, 1.f, 0.f), -7.558f * PI / 180.f);
    sceneObject->Rotate(glm::vec3(0.f, 0.f, 1.f), 1.087f * PI / 180.f);
    sceneObject->MultScale(0.2f);


    // add the blind window!
    texture = "";
    metallic = 0.5f;
    roughness = 0.f;
    specular = 0.5f;
    shader = MakeEpicShader(metallic, roughness, specular, texture);
    
    object = "blind_window/Model/blind_window.obj";
    sceneObject = LoadObj(shader, object);

    // place the blind window
    sceneObject->SetPosition(glm::vec3(23.f, 30.f, -21.4f));

    // add the pearls!
    texture = "";
    metallic = 0.5f;
    roughness = 0.f;
    specular = 0.5f;
    shader = MakeEpicShader(metallic, roughness, specular, texture);
    
    object = "pearl_necklace/Model/pearl_necklace.obj";
    sceneObject = LoadObj(shader, object);

    // place the pearls
    sceneObject->SetPosition(glm::vec3(-6.107f, 26.359f, -2.537f));
    sceneObject->Rotate(glm::vec3(1.f, 0.f, 0.f), 0.f * PI / 180.f);
    sceneObject->Rotate(glm::vec3(0.f, 1.f, 0.f), 90.f * PI / 180.f);
    sceneObject->Rotate(glm::vec3(0.f, 0.f, 1.f), 0.f * PI / 180.f);


    // add the banister!
    texture = "";
    metallic = 0.5f;
    roughness = 0.f;
    specular = 0.5f;
    shader = MakeEpicShader(metallic, roughness, specular, texture);
    
    object = "banister/Model/banister.obj";
    sceneObject = LoadObj(shader, object);

    // add the wall!
    texture = "";
    metallic = 0.5f;
    roughness = 0.f;
    specular = 0.5f;
    shader = MakeEpicShader(metallic, roughness, specular, texture);
    
    object = "wall/Model/wall.obj";
    sceneObject = LoadObj(shader, object);
    sceneObject->SetPosition(glm::vec3(0.f, 0.f, -23.f));


    AddPLight();
}

void Assignment4::SetupDummy()
{
    scene->ClearScene();
    
    std::unordered_map<GLenum, std::string> shaderSpec = {
        { GL_VERTEX_SHADER, "brdf/epicshader/frag/noSubroutine/epicshader.vert" },
        { GL_FRAGMENT_SHADER, "brdf/epicshader/frag/noSubroutine/epicshader.frag"}
    };
    
    std::shared_ptr<EpicShader> shader = std::make_shared<EpicShader>(shaderSpec, GL_FRAGMENT_SHADER);
    shader->SetMetallic(0.5f);
    shader->SetRoughness(0.f);
    shader->SetSpecular(0.5f);
    
    shader->SetTexture(EpicShader::TextureSlots::DIFFUSE, TextureLoader::LoadTexture("dummy/Texture/dummy_wood.jpg"));
    
    // load the dummy objects
    std::array<std::string, 3> dummy_objects = {"dummy/Model/dummy.obj", 
                                                "dummy/Model/base.obj",
                                                "dummy/Model/support.obj"};

    std::vector<std::shared_ptr<RenderingObject>> meshTemplate;
    for (auto dummy_object : dummy_objects)
    {
        meshTemplate = MeshLoader::LoadMesh(shader, dummy_object);
        if (meshTemplate.empty()) 
        {
            std::cerr << "ERROR: Failed to load the model. Check your paths." << std::endl;
            return;
        }       

        std::shared_ptr<class SceneObject> sceneObject = std::make_shared<SceneObject>(meshTemplate);
        scene->AddSceneObject(sceneObject);
    }

    // add the frog!
    std::shared_ptr<EpicShader> frog_shader = std::make_shared<EpicShader>(shaderSpec, GL_FRAGMENT_SHADER);
    frog_shader->SetMetallic(0.5f);
    frog_shader->SetRoughness(0.f);
    frog_shader->SetSpecular(0.5f);

    frog_shader->SetTexture(EpicShader::TextureSlots::DIFFUSE, TextureLoader::LoadTexture("frog/Texture/frog_texture.png"));
    meshTemplate = MeshLoader::LoadMesh(frog_shader, "frog/Model/frog.obj");
    if (meshTemplate.empty()) 
    {
        std::cerr << "ERROR: Failed to load the model. Check your paths." << std::endl;
        return;
    }       
    std::shared_ptr<class SceneObject> sceneObject = std::make_shared<SceneObject>(meshTemplate);
    // place the frog
    sceneObject->SetPosition(glm::vec3(-5.317f, -2.027f, 2.966f));
    sceneObject->Rotate(glm::vec3(1.f, 0.f, 0.f), 8.441f * PI / 180.f);
    sceneObject->Rotate(glm::vec3(0.f, 1.f, 0.f), -7.558f * PI / 180.f);
    sceneObject->Rotate(glm::vec3(0.f, 0.f, 1.f), 1.087f * PI / 180.f);
    sceneObject->MultScale(0.2f);

    scene->AddSceneObject(sceneObject);

    
    AddPLight();
    
}


void Assignment4::AddPLight()
{
    // turn on point light.
    std::unique_ptr<LightProperties> lightProperties = make_unique<LightProperties>();
    lightProperties->color = glm::vec4(2.f, 2.f, 2.f, 1.f);
    lightProperties->radius = 1000.f;
    
    pointLight = std::make_shared<Light>(std::move(lightProperties));
    pointLight->SetPosition(glm::vec3(0.f, 0.f, 10.f));
    scene->AddLight(pointLight);
    pLightIsOn = true;
} 
void Assignment4::AddDLight()
{
    // add a directional light which has a direction instead of position
    std::unique_ptr<LightProperties> lightProperties = make_unique<LightProperties>();
    lightProperties = make_unique<LightProperties>();
    lightProperties->color = glm::vec4(2.f, 2.f, 2.f, 1.f);
    lightProperties->direction = glm::vec4(0.f, -10.f, 0.f, 1.f); // Sunlight shines down (in the -y direction)
    
    sunLight = std::make_shared<Light>(std::move(lightProperties), Light::LightType::DIRECTIONAL);
    scene->AddLight(sunLight);
}

void Assignment4::AddHLight()
{
    // add a hemispherical light which has a sky and ground color
    std::unique_ptr<LightProperties> lightProperties = make_unique<LightProperties>();
    lightProperties = make_unique<LightProperties>();
    lightProperties->groundColor = glm::vec3(0.f, 3.f, 1.f); // Green
    lightProperties->skyColor = glm::vec3(1.f, 1.f, 3.f);    // Blue
    lightProperties->radius = 1000.f;
    
    hemisphereLight = std::make_shared<Light>(std::move(lightProperties), Light::LightType::HEMISPHERE);
    scene->AddLight(hemisphereLight);
}

void Assignment4::SetupExample1()
{
    scene->ClearScene();
#ifndef DISABLE_OPENGL_SUBROUTINES
    std::unordered_map<GLenum, std::string> shaderSpec = {
        { GL_VERTEX_SHADER, "brdf/blinnphong/fragTexture/blinnphong.vert" },
        { GL_FRAGMENT_SHADER, "brdf/blinnphong/fragTexture/blinnphong.frag" }
    };
#else
    std::unordered_map<GLenum, std::string> shaderSpec = {
        { GL_VERTEX_SHADER, "brdf/blinnphong/fragTexture/noSubroutine/blinnphong.vert" },
        { GL_FRAGMENT_SHADER, "brdf/blinnphong/fragTexture/noSubroutine/blinnphong.frag"}
    };
#endif
    std::shared_ptr<BlinnPhongShader> shader = std::make_shared<BlinnPhongShader>(shaderSpec, GL_FRAGMENT_SHADER);
    shader->SetDiffuse(glm::vec4(0.8f, 0.8f, 0.8f, 1.f));
    shader->SetTexture(BlinnPhongShader::TextureSlots::DIFFUSE, TextureLoader::LoadTexture("brick/bricktexture.jpg"));
    shader->SetTexture(BlinnPhongShader::TextureSlots::SPECULAR, TextureLoader::LoadTexture("brick/bricktexture.jpg"));

    std::unique_ptr<LightProperties> lightProperties = make_unique<LightProperties>();
    lightProperties->diffuseColor = glm::vec4(1.f, 1.f, 1.f, 1.f);
    lightProperties->specularColor = glm::vec4(1.f, 1.f, 1.f, 1.f);

    std::shared_ptr<Light> pointLight = std::make_shared<Light>(std::move(lightProperties));
    pointLight->SetPosition(glm::vec3(10.f, 10.f, 10.f));
    scene->AddLight(pointLight);

    std::shared_ptr<RenderingObject> sphereTemplate = PrimitiveCreator::CreateIcoSphere(shader, 5.f, 4);
    std::shared_ptr<class SceneObject> sceneObject = std::make_shared<SceneObject>(sphereTemplate);
    sceneObject->Rotate(glm::vec3(SceneObject::GetWorldRight()), PI / 4.f);
    scene->AddSceneObject(sceneObject);
}


void Assignment4::SetupExample1Epic()
{
    scene->ClearScene();
    std::unordered_map<GLenum, std::string> shaderSpec = {
        { GL_VERTEX_SHADER, "brdf/epicshader/frag/noSubroutine/epicshader.vert" },
        { GL_FRAGMENT_SHADER, "brdf/epicshader/frag/noSubroutine/epicshader.frag"}
    };

    std::shared_ptr<EpicShader> shader = std::make_shared<EpicShader>(shaderSpec, GL_FRAGMENT_SHADER);
    shader->SetMetallic(0.5f);
    shader->SetRoughness(0.f);
    shader->SetSpecular(0.5f);
    shader->SetTexture(EpicShader::TextureSlots::DIFFUSE, TextureLoader::LoadTexture("brick/bricktexture.jpg"));

    std::unique_ptr<LightProperties> lightProperties = make_unique<LightProperties>();
    lightProperties->color = glm::vec4(1.f, 1.f, 1.f, 1.f);

    std::shared_ptr<Light> pointLight = std::make_shared<Light>(std::move(lightProperties));
    pointLight->SetPosition(glm::vec3(10.f, 10.f, 10.f));
    scene->AddLight(pointLight);

    std::shared_ptr<RenderingObject> sphereTemplate = PrimitiveCreator::CreateIcoSphere(shader, 5.f, 4);
    std::shared_ptr<class SceneObject> sceneObject = std::make_shared<SceneObject>(sphereTemplate);
    sceneObject->Rotate(glm::vec3(SceneObject::GetWorldRight()), PI / 4.f);
    scene->AddSceneObject(sceneObject);
}

void Assignment4::SetupExample2()
{
    scene->ClearScene();
    std::unordered_map<GLenum, std::string> shaderSpec = {
        { GL_VERTEX_SHADER, "brdf/blinnphong/fragTexture/noSubroutine/blinnphong.vert" },
        { GL_FRAGMENT_SHADER, "brdf/blinnphong/fragTexture/noSubroutine/blinnphong.frag" }
    };

    std::unique_ptr<LightProperties> lightProperties = make_unique<LightProperties>();
    lightProperties->diffuseColor = glm::vec4(1.f, 1.f, 1.f, 1.f);
    lightProperties->specularColor = glm::vec4(1.f, 1.f, 1.f, 1.f);

    std::shared_ptr<Light> pointLight = std::make_shared<Light>(std::move(lightProperties));
    pointLight->SetPosition(glm::vec3(10.f, 10.f, 10.f));
    scene->AddLight(pointLight);

    std::vector<std::shared_ptr<aiMaterial>> loadedMaterials;
    std::vector<std::shared_ptr<RenderingObject>> sphereTemplate = MeshLoader::LoadMesh(nullptr, "sphere.obj", &loadedMaterials);
    for (size_t i = 0; i < sphereTemplate.size(); ++i) {
        std::shared_ptr<BlinnPhongShader> shader = std::make_shared<BlinnPhongShader>(shaderSpec, GL_FRAGMENT_SHADER);
        shader->LoadMaterialFromAssimp(loadedMaterials[i]);
        sphereTemplate[i]->SetShader(std::move(shader));
    }

    std::shared_ptr<class SceneObject> sceneObject = std::make_shared<SceneObject>(sphereTemplate);
    sceneObject->Rotate(glm::vec3(SceneObject::GetWorldRight()), PI / 4.f);
    scene->AddSceneObject(sceneObject);
}

void Assignment4::Tick(double deltaTime)
{
}
