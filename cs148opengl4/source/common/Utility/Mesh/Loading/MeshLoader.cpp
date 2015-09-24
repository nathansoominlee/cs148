#include "common/Utility/Mesh/Loading/MeshLoader.h"
#include "common/Rendering/RenderingObject.h"
#include "common/Rendering/Shaders/ShaderProgram.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

namespace MeshLoader
{

std::vector<std::shared_ptr<RenderingObject>> LoadMesh(std::shared_ptr<ShaderProgram> inputShader, const std::string& filename)
{

#ifndef ASSET_PATH
    static_assert(false, "ASSET_PATH is not defined. Check to make sure your projects are setup correctly");
#endif

    Assimp::Importer importer;

    const std::string completeFilename = std::string(STRINGIFY(ASSET_PATH)) + "/" + filename;
    const aiScene* scene = importer.ReadFile(completeFilename.c_str(),
            aiProcess_CalcTangentSpace       | 
            aiProcess_Triangulate            |
            aiProcess_JoinIdenticalVertices  |
            aiProcess_FixInfacingNormals |
            aiProcess_SortByPType);

    if (!scene) {
        return {};
    }

    std::vector<std::shared_ptr<RenderingObject>> loadedMeshes;
    for (decltype(scene->mNumMeshes) i = 0; i < scene->mNumMeshes; ++i) {
        const aiMesh* mesh = scene->mMeshes[i];
        if (!mesh->HasPositions()) {
            std::cerr << "WARNING: A mesh in " << filename << " does not have positions. Skipping." << std::endl;
            continue;
        }

        auto totalVertices = mesh->mNumVertices;
        std::unique_ptr<RenderingObject::PositionArray> positions = make_unique<RenderingObject::PositionArray>(totalVertices);
        std::unique_ptr<RenderingObject::NormalArray> normals = mesh->HasNormals() ? make_unique<RenderingObject::NormalArray>(totalVertices) : nullptr;
        std::unique_ptr<RenderingObject::UVArray> uv =  mesh->HasTextureCoords(0) ? make_unique<RenderingObject::UVArray>(totalVertices) : nullptr;
        std::unique_ptr<RenderingObject::ColorArray> colors = mesh->HasVertexColors(0) ? make_unique<RenderingObject::ColorArray>(totalVertices) : nullptr;
        std::unique_ptr<RenderingObject::IndexArray> indices = mesh->HasFaces() ? make_unique<RenderingObject::IndexArray>(mesh->mNumFaces * 3) : nullptr;

        for (decltype(totalVertices) v = 0; v < totalVertices; ++v) {
            positions->at(v) = glm::vec4(mesh->mVertices[v].x, mesh->mVertices[v].y, mesh->mVertices[v].z, 1.f);
            
            if (normals) {
                normals->at(v) = glm::vec3(mesh->mNormals[v].x, mesh->mNormals[v].y, mesh->mNormals[v].z);
            }

            if (uv) {
                uv->at(v) = glm::vec2(mesh->mTextureCoords[0][v].x, mesh->mTextureCoords[0][v].y);
            }

            if (colors) {
                colors->at(v) = glm::vec4(mesh->mColors[0][v].r, mesh->mColors[0][v].g, mesh->mColors[0][v].b, mesh->mColors[0][v].a);
            }
        }

        for (decltype(mesh->mNumFaces) f = 0; f < mesh->mNumFaces && indices; ++f) {
            const aiFace& face =  mesh->mFaces[f];
            if (face.mNumIndices != 3) {
                std::cerr << "WARNING: Input mesh may not be triangulated. Skipping face." << std::endl;
                continue;
            }

            for (int j = 0; j < 3; ++j) {
                indices->at(f * 3 + j) = face.mIndices[j];
            }
        }

        std::shared_ptr<RenderingObject> createdMesh = std::make_shared<RenderingObject>(inputShader, std::move(positions), std::move(indices),
            std::move(normals), std::move(uv), std::move(colors));
        loadedMeshes.push_back(std::move(createdMesh));
    }
    return loadedMeshes;
}

}
