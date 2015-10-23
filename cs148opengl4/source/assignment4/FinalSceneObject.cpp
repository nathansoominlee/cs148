#include "assignment4/FinalSceneObject.h"

// The format for the .TSV file looks like this
//
// Try to parse? object description texture shader:p1,p2{,p3} transformations:tx,ty,tz 
// y blind_window blinds and the window. pane not added. None Epic:0.5,0,0.5 23,30,21.4 
// y pearl_necklace Hangs off dummy's right arm None Epic:0.5,0,0.5 -6.107,26.359,-2.537 
// y banister The ground plane for our scene None Epic:0.5,0,0.6 0,0,0 
// y wall  The background of the scene None Epic:0.5,0,0.7 0,0,-23 
// y dummy_body Full body of the dummy dummy/Texture/dummy_wood.jpg Epic:0.5,0,0.8 0,2.8,0 
// y dummy_stand The pole extending from the base to body dummy/Texture/dummy_wood.jpg Epic:0.5,0,0.9 0,2.8,0 
// n light_christmas    0,2.7,0 
// n necklace_pearl    -5.113,27.9,-1.648 
// n camera  FOV: 35.9  zNear: 0.1  zFar: 1000  -0.607,40.868,115.363 
// n persp (camera)  FOV: 39  zNear: 0.1  zFar: 1000  0,31.5,0 
//
// Which we have have in rows and need to turn into a container of FinalSceneObjects

int FinalSceneObject::MakeContainer(const Rows &rows,                      // input parameter
                      std::vector<FinalSceneObject> &final_scene_objects)  // output parameter
{
    std::string name;
    std::string description;
    std::string texture_path;
    FinalSceneObject::Shader shader;
    float epic_metallic;
    float epic_roughness;
    float epic_specular;
    glm::vec4 bp_diffuse;
    glm::vec4 bp_specular;
    float scale;
    float tx;
    float ty;
    float tz;
    float rx;
    float ry;
    float rz;

    std::string field;
    for (auto row : rows)
    {

        for (int i = 0; i < row.size(); i++)
        {

            field = row[i];

            switch(i)
            {

                case(TryToParse):

                    if (field == 'n')
                    {
                        continue;
                    }

                    break;

                case(Object):

                    name = field;
                    break;

                case(Description):

                    description = field;
                    break;

                case(Texture):

                    texture_path = field;
                    break;

                case(Shader):

                    FinalSceneObject::ParseShader(&field,
                                                  &shader,
                                                  &epic_metallic,
                                                  &epic_roughness,
                                                  &epic_specular,
                                                  &bp_diffuse,
                                                  &bp_specular);
                    break;

                case(Transformations)
                    // TODO: Parse transformations
                    break;
                case(Rotations)
                    // TODO: Parse rotations 
                    break;
            }
        }
        std::cout << std::endl;
    }
}

// returns 0 on success
int FinalSeneObject::ParseShader(const std::string& field,          // input parameter
                                 FinalSceneObject::Shader& shader,  // output parameter
                                 float& epic_metallic,              // output parameter
                                 float& epic_roughness,             // output parameter
                                 float& epic_specular,              // output parameter
                                 float& bp_diffuse,                 // output parameter
                                 float& bp_specular)                // output parameter
{
    // Should look like either 
    // Epic:0.5,0,0.6 or BP:(0.1,0.2,0.3),(0.7, 0.2, 0.9)

    const char *haystack = field.c_str();
    const char *colon = ":"; 
    char *colon_location;

    colon_location = strnstr(haystack, colon, field.length());

    // Check that we found a colon

    if (!colon_location)
    {
        std::cerr << "Error: Malformatted shader specification " << field << std::endl;
        exit(1);
    }

    // Put in a null character and check which shader it is
    (*colon_location) = '\0';

    std::string shader_spec(haystack);
    int retval = -1;
    if (shader_spec == "Epic")
    {
        // Set what we know
        shader = FinalSceneObject::Shader::Epic;
        bp_diffuse = -1;
        bp_specular = -1;

        // Process Epic parameters
        haystack = colon_location + 1;
        std::string epic_params(haystack);

        retval = FinalSceneObject::ParseEpicParams(epic_params,
                                                   epic_metallic,
                                                   epic_roughness,
                                                   epic_specular);
    }
    else if (shader_spec == "BP")
    {
        // Set what we know
        shader = FinalSceneObject::Shader::BP;
        epic_metallic = -1;
        epic_roughness = -1;
        epic_specular = -1;

        haystack = colon_location + 1;
        std::string bp_params(haystack);

        retval = FinalSceneObject::ParseBPParams(bp_params,
                                                 bp_diffuse,
                                                 bp_specular);
    }
    else
    {
        std::cerr << "Error: Unrecognized shader specificatoin " << shader_spec << std::endl;
        exit(1);
    }

    return retval;
}

// returns 0 on success
int FinalSceneObject::ParseEpicParams(const std::string& epic_params, // input parameter
                                      float& epic_metallic,           // output parameter
                                      float& epic_roughness,          // output parameter
                                      float& epic_specular)           // output parameter
{

    // Collect the 3 parameters

    std::string::size_type sz = 0;
    for (int j = 0; j < 3; j++)
    {

        float param = std::stof(epic_params.substr(sz), &sz);

        if (j == 0) 
        {
            epic_metallic = param;
        }
        else if (j == 1)
        {
            epic_roughness = param;
        }
        else //if (j == 2)
        {
            epic_specular = param;
        }

    } 

    return 0;
}

int FinalSceneObject::ParseBPParams(const std::string& bp_params, // input parameter
                                    glm::vec4 &bp_diffuse,        // output parameter
                                    glm::vec4 &epic_roughness);   // output parameter
{
    // TODO: Collect the 2 parameter vectors
}
