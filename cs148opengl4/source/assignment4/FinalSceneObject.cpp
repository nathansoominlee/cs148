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

// Driver function for ParseSFO
int FinalSceneObject::MakeContainer(const Rows &rows,                      // input parameter
                      std::vector<FinalSceneObject> &final_scene_objects)  // output parameter
{
    // r = 1 skips the first row of headers
    for (int r = 1; r < rows.size(); r++)
    {
        auto row = rows[r];

        if (row.size() > 0 && row[0] == "y")
        {
            //std::cout << "Told to parse: " << std::endl;
            //PrintRow(row);

            FinalSceneObject fso = ParseFSO(row);
            final_scene_objects.push_back(fso);
        }
        else if (row.size() > 0 && row[0] == "n")
        {
            // We are told to skip this object
        }
        else
        {
            std::cerr << "Error: Unsure whether to parse row based on indicator: " << std::endl;
            PrintRow(row);
            exit(1);
        }

    } // for j

    for (auto object : final_scene_objects)
    {
        std::cout << object << std::endl;
    }

} // FinalSceneObject::MakeContainer()

void FinalSceneObject::PrintRow(std::vector<std::string> row)
{
    for (auto field : row)
    {
        std::cout << field << " ";
    }

    std::cout << std::endl;
}

FinalSceneObject FinalSceneObject::ParseFSO(std::vector<std::string> row)
{
    std::string name = "";
    std::string description = "";
    std::string texture_path = "";
    ShaderType shader = ShaderType::None;
    float epic_metallic = -1.f;
    float epic_roughness = -1.f;
    float epic_specular = -1.f;
    glm::vec4 bp_diffuse = glm::vec4(-1.f, -1.f, -1.f, -1.f);
    glm::vec4 bp_specular = glm::vec4(-1.f, -1.f, -1.f, -1.f);
    float scale = -1.f;
    float tx = -1.f;
    float ty = -1.f;
    float tz = -1.f;
    float rx = -1.f;
    float ry = -1.f;
    float rz = -1.f;


    std::string field;
    for (int i = 0; i < row.size(); i++)
    {
        field = row[i];

        switch(i)
        {

            case(Column::TryToParse):

                // already handled this column
                break;

            case(Column::Object):

                name = field;
                std::cout << "Reading in " << name << ": ";
                break;

            case(Column::Description):

                description = field;
                std::cout << description << std::endl;
                break;

            case(Column::Texture):

                texture_path = field;
                break;

            case(Column::Shader):

                FinalSceneObject::ParseShader(field,
                                              shader,
                                              &epic_metallic,
                                              &epic_roughness,
                                              &epic_specular,
                                              bp_diffuse,
                                              bp_specular);
                break;

            case(Column::Scale):
                FinalSceneObject::ParseScale(field, &scale);
                break;
            case(Column::Transformations):
                FinalSceneObject::ParseTransformations(field, &tx, &ty, &tz);
                break;
            case(Column::Rotations):
                FinalSceneObject::ParseRotations(field, &rx, &ry, &rz);
                break;
        } // switch

    } // for i

    // Build and return the object
    FinalSceneObject fso(name, description, texture_path, shader, \
                    epic_metallic, epic_roughness, epic_specular, bp_diffuse, bp_specular, \
                    scale, tx, ty, tz, rx, ry, rz);

    return fso;

} //FinalSceneObject::ParseFSO()

// returns 0 on success
int FinalSceneObject::ParseShader(const std::string& field,          // input parameter
                                 ShaderType& shader,  // output parameter
                                 float* epic_metallic,              // output parameter
                                 float* epic_roughness,             // output parameter
                                 float* epic_specular,              // output parameter
                                 glm::vec4& bp_diffuse,                 // output parameter
                                 glm::vec4& bp_specular)                // output parameter
{
    // Should look like either 
    // Epic:0.5,0,0.6 or BP:0.5,0.5,0.5,1.0,0.5,0.5,0.5,1.0

    const char *haystack = field.c_str();
    const char *colon = ":"; 
    char *colon_location;

    //std::cout << "ParseShader, field: " << field << std::endl;

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
        shader = ShaderType::Epic;
        bp_diffuse = glm::vec4(-1.f, -1.f, -1.f, -1.f);
        bp_specular = glm::vec4(-1.f, -1.f, -1.f, -1.f);

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
        shader = ShaderType::BP;
        (*epic_metallic) = -1;
        (*epic_roughness) = -1;
        (*epic_specular) = -1;

        haystack = colon_location + 1;
        std::string bp_params(haystack);

        retval = FinalSceneObject::ParseBPParams(bp_params,
                                                 bp_diffuse,
                                                 bp_specular);
    }
    else
    {
        std::cerr << "Error: Unrecognized shader specification " << shader_spec << std::endl;
        exit(1);
    }

    return retval;
}

// returns 0 on success
int FinalSceneObject::ParseEpicParams(const std::string& epic_params, // input parameter
                                      float* epic_metallic,           // output parameter
                                      float* epic_roughness,          // output parameter
                                      float* epic_specular)           // output parameter
{

    // Collect the 3 parameters

    std::string::size_type sz = 0;
    for (int i = 0; i < 3; i++)
    {

        float param = std::stof(epic_params.substr(sz), &sz);
        sz += 1;

        if (i == 0) 
        {
            (*epic_metallic) = param;
        }
        else if (i == 1)
        {
            (*epic_roughness) = param;
        }
        else //if (i == 2)
        {
            (*epic_specular) = param;
        }

    } 

    return 0;
}

// returns 0 on success
int FinalSceneObject::ParseBPParams(const std::string& bp_params, // input parameter
                                    glm::vec4 &bp_diffuse,        // output parameter
                                    glm::vec4 &bp_specular)    // output parameter
{
    // Collect the 4 parameters for each vec4, 4 * 2 = 8 
    int n = 8;
    
    std::string::size_type sz = 0;
    float v1 = -1;
    float v2 = -1;
    float v3 = -1;
    float v4 = -1;
    for (int i = 0; i < n; i++)
    {
        float param = std::stof(bp_params.substr(sz), &sz);
        sz += 1;

        if ( i == 0 || (i == n - 4) )
        {
            v1 = param;
        }
        else if ( i == 1 || (i == n - 3) )
        {
            v2 = param;
        }
        else if ( i == 2 || (i == n - 2) )
        {
            v3 = param;
        }
        else if ( i == 3 || (i == n - 1) )
        {
            v4 = param;
            if (i == 3)
            {
                bp_diffuse = glm::vec4(v1, v2, v3, v4);
                v1 = -1;
                v2 = -1;
                v3 = -1;
                v4 = -1;
            }
            else if (i == n - 1)
            {
                bp_specular = glm::vec4(v1, v2, v3, v4);
            }
        }
    }

    return 0;
}

// Returns 0 on success
int FinalSceneObject::ParseTransformations(const std::string& t_params,
                                           float* tx, float* ty, float* tz)
{
    std::string::size_type sz = 0;
    for (int i = 0; i < 3; i++)
    {
        float param = std::stof(t_params.substr(sz), &sz);
        sz += 1;

        if (i == 0)
        {
            (*tx) = param;
        }
        else if (i == 1)
        {
            (*ty) = param;
        }
        else //if (i == 1)
        {
            (*tz) = param;
        }
    }

    return 0;
}

// Returns 0 on success
int FinalSceneObject::ParseRotations(const std::string& r_params,
                                     float* rx, float* ry, float* rz)
{
    std::string::size_type sz = 0;
    for (int i = 0; i < 3; i++)
    {
        float param = std::stof(r_params.substr(sz), &sz);
        sz += 1;

        if (i == 0)
        {
            (*rx) = param;
        }
        else if (i == 1)
        {
            (*ry) = param;
        }
        else //if (i == 1)
        {
            (*rz) = param;
        }
    }
    return 0;
}

// Returns 0 on success
int FinalSceneObject::ParseScale(const std::string& scale_param,
                                 float* scale)
{
    float param = std::stof(scale_param);
    (*scale) = param;
    return 0;
}

void FinalSceneObject::PrintContainer(const std::vector<FinalSceneObject>& final_scene_objects)
{
    std::cout << "container.size(): " << final_scene_objects.size() << std::endl;
    for (int i = 0; i < final_scene_objects.size(); i++)
    {
        std::cout << "i: " << i << " fso: " << final_scene_objects[i] << std::endl;
    }
}

std::ostream& operator<< (std::ostream& os, const FinalSceneObject& fso)
{
    std::string shader = "None";
    switch(fso.shader)
    {
        case (FinalSceneObject::ShaderType::Epic):

            shader = "Epic";
            break;

        case (FinalSceneObject::ShaderType::BP):

            shader = "BP";
            break;

        case (FinalSceneObject::ShaderType::None):

            shader = "None";
            break;
    }

    os << "name: "           << fso.name
       << " description: "    << fso.description
       << " texture_path: "   << fso.texture_path
       << " shader: "         << shader
       << " epic_metallic: "  << fso.epic_metallic
       << " epic_roughness: " << fso.epic_roughness
       << " epic_specular: "  << fso.epic_specular
       << " bp_diffuse: "     << fso.bp_diffuse[0] << " " << fso.bp_diffuse[1] << " " << fso.bp_diffuse[2] << " " << fso.bp_diffuse[3]
       << " bp_specular: "    << fso.bp_specular[0] << " " << fso.bp_specular[1] << " " << fso.bp_specular[2] << " " << fso.bp_specular[3]
       << " scale: "          << fso.scale
       << " tx: "             << fso.tx
       << " ty: "             << fso.ty
       << " tz: "             << fso.tz
       << " rx: "             << fso.rx
       << " ry: "             << fso.ry
       << " rz: "             << fso.rz;

    return os;
}
