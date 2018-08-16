#ifndef MODEL_H
#define MODEL_H

#if 0
#include "util/Logging.h"

#include "Shader.h"
#include <string>
#include <vector>

#include "lib.h"

#include "util/Helpers.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

GLuint loadTexture (const std::string& filename);

namespace Model {

namespace experimental {

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

struct Texture {
    unsigned int id;
    Uniform_t location;
};

struct Mesh_t {
    lib::vector<Vertex> vertices;
    lib::vector<GLuint> indices;
    lib::vector<Texture> textures;
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
};

struct Model_t {
    std::vector<Mesh_t> meshes;
    Uniform_t transform;
};

void setup(Mesh_t& mesh)
{
    // create buffers/arrays
    glGenVertexArrays(1, &mesh.vao);
    glGenBuffers(1, &mesh.vbo);
    glGenBuffers(1, &mesh.ebo);

    glBindVertexArray(mesh.vao);
    // load data into vertex buffers
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    // A great thing about structs is that their memory layout is sequential for all its items.
    // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
    // again translates to 3/2 floats which translates to a byte array.
    glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Vertex), &mesh.vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), &mesh.indices[0], GL_STATIC_DRAW);

    // set the vertex attribute pointers
    // vertex Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, normal)));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, texCoords)));

    glBindVertexArray(0);
}

void draw(Model_t model, glm::mat4& transform)
{
    Shader::setUniform(model.transform, transform);
    for (auto& mesh : model.meshes) {
        for (std::size_t i = 0; i < mesh.textures.size(); ++i) {
            glActiveTexture(GL_TEXTURE0 + i);
            auto& texture = mesh.textures[i];
            Shader::setUniform(texture.location, i);
            glBindTexture(GL_TEXTURE_2D, texture.id);
        }
        // draw mesh
        glBindVertexArray(mesh.vao);
        glDrawElements(GL_TRIANGLES, GLsizei(mesh.indices.size()), GL_UNSIGNED_INT, nullptr);
    }
    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);
}

}


struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
    glm::vec3 color;
};

struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
};

struct TextureBinding {
    unsigned int textureID;
    Uniform_t location;
};

class Mesh {
public:
    /*  Mesh Data  */
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;
    unsigned int VAO;

    /*  Functions  */
    // constructor
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;

        // now that we have all the required data, set the vertex buffers and its attribute pointers.
        setupMesh();
    }

    // render the mesh
    void Draw(Shader::Shader shader)
    {
        // bind appropriate textures
        unsigned int diffuseNr  = 1;
        unsigned int specularNr = 1;
        unsigned int normalNr   = 1;
        unsigned int heightNr   = 1;
        for(unsigned int i = 0; i < textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
            // retrieve texture number (the N in diffuse_textureN)
            std::string number;
            std::string name = textures[i].type;
            if(name == "texture_diffuse")
                number = std::to_string(diffuseNr++);
            else if(name == "texture_specular")
                number = std::to_string(specularNr++); // transfer unsigned int to stream
            else if(name == "texture_normal")
                number = std::to_string(normalNr++); // transfer unsigned int to stream
             else if(name == "texture_height")
                number = std::to_string(heightNr++); // transfer unsigned int to stream

                                                     // now set the sampler to the correct texture unit
            glUniform1i(glGetUniformLocation(shader.programID, (name + number).c_str()), i);
            // and finally bind the texture
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }

        // draw mesh
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, GLsizei(indices.size()), GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);

        // always good practice to set everything back to defaults once configured.
        glActiveTexture(GL_TEXTURE0);
    }

private:
    /*  Render data  */
    unsigned int VBO, EBO;

    /*  Functions    */
    // initializes all the buffer objects/arrays
    void setupMesh()
    {
        // create buffers/arrays
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        // load data into vertex buffers
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // A great thing about structs is that their memory layout is sequential for all its items.
        // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
        // again translates to 3/2 floats which translates to a byte array.
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // set the vertex attribute pointers
        // vertex Positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
        // vertex normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, Normal)));
        // vertex texture coords
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, TexCoords)));
        // vertex tangent
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, Tangent)));
        // vertex bitangent
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, Bitangent)));

        glBindVertexArray(0);
    }
};


class Model
{
public:
    /*  Model Data */
    std::vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    std::vector<Mesh> meshes;
    std::string directory;
    bool gammaCorrection;

    /*  Functions   */
    // constructor, expects a filepath to a 3D model.
    Model(std::string const &path, bool gamma = false) : gammaCorrection(gamma)
    {
        loadModel(path);
    }

    // draws the model, and thus all its meshes
    void Draw(Shader::Shader shader)
    {
        shader.use();
        for(unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].Draw(shader);
    }

private:
    /*  Functions   */
    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(std::string const &path)
    {
        // read file via ASSIMP
        Assimp::Importer importer;
        try {
            std::string buffer = Helpers::readToString(path);
            const aiScene* scene = importer.ReadFileFromMemory(reinterpret_cast<const unsigned char*>(buffer.c_str()), buffer.size(), aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_OptimizeGraph | aiProcess_OptimizeMeshes);
            // check for errors
            if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
            {
                error("Could not load model '{}': {}", path, importer.GetErrorString());
                return;
            }
            info("Number of textures: {}", scene->mNumTextures);
            // retrieve the directory path of the filepath
            directory = path.substr(0, path.find_last_of('/'));

            // process ASSIMP's root node recursively
            processNode(scene->mRootNode, scene);
        } catch (const std::exception& e) {
            error("Could not load model {}: {}", path, e.what());
        }
    }

    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void processNode(aiNode *node, const aiScene *scene)
    {
        // process each mesh located at the current node
        for(unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            // the node object only contains indices to index the actual objects in the scene.
            // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene));
        }
        // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
        for(unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene);
        }
    }

    Mesh processMesh(aiMesh *mesh, const aiScene *scene)
    {
        // data to fill
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;

        // Walk through each of the mesh's vertices
        for(unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
            // positions
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;
            // normals
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;
            // texture coordinates
            if(mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
            {
                glm::vec2 vec;
                // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
                // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;
            }
            else
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);
            // tangent
            vector.x = mesh->mTangents[i].x;
            vector.y = mesh->mTangents[i].y;
            vector.z = mesh->mTangents[i].z;
            vertex.Tangent = vector;
            // bitangent
            vector.x = mesh->mBitangents[i].x;
            vector.y = mesh->mBitangents[i].y;
            vector.z = mesh->mBitangents[i].z;
            vertex.Bitangent = vector;
            vertices.push_back(vertex);
        }

        // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
        for(unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            // retrieve all indices of the face and store them in the indices vector
            for(unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }
        warn("Has materials? {}", scene->HasMaterials());
        // process materials
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
        // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER.
        // Same applies to other texture as the following list summarizes:
        // diffuse: texture_diffuseN
        // specular: texture_specularN
        // normal: texture_normalN
        aiColor3D color;
        material->Get(AI_MATKEY_COLOR_AMBIENT, color);
        info("Ambient color: {}, {}, {}", color.r, color.g, color.b);
        material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
        info("Diffuse color: {}, {}, {}", color.r, color.g, color.b);
        material->Get(AI_MATKEY_COLOR_SPECULAR, color);
        info("Specular color: {}, {}, {}", color.r, color.g, color.b);

//        for (unsigned type = 0; type <= aiTextureType_UNKNOWN; ++type) {
//            info("Loading type {}", type);
//            std::vector<Texture> testMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "test");
//            textures.insert(textures.end(), testMaps.begin(), testMaps.end());
//        }

        // 1. diffuse maps
        std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        // 2. specular maps
        std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        // 3. normal maps
        std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        // 4. height maps
        std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

        // return a mesh object created from the extracted mesh data
        return Mesh(vertices, indices, textures);
    }

    // checks all material textures of a given type and loads the textures if they're not loaded yet.
    // the required info is returned as a Texture struct.
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
    {
        info("Material {}, typeName: {}", mat->GetTextureCount(type), typeName);
        std::vector<Texture> textures;
        for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);
            // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
            bool skip = false;
            info("mat->GetTexture({}, {}, ...) = {} {}", type, i, str.C_Str(), str.length);
            if (str.length != 0) {
                for(unsigned int j = 0; j < textures_loaded.size(); j++)
                {
                    if(std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
                    {
                        textures.push_back(textures_loaded[j]);
                        skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                        break;
                    }
                }
                info("!skip? {} {}", !skip, str.C_Str());
                if(!skip)
                {   // if texture hasn't been loaded already, load it
                    Texture texture;
                    warn("Loading texture {}, {}", this->directory, str.C_Str());
                    texture.id = loadTexture(this->directory + '/' + str.C_Str());
                    warn("Texture loaded");
                    texture.type = typeName;
                    texture.path = str.C_Str();
                    textures.push_back(texture);
                    textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
                }
            }
        }
        warn("End of texture loading");
        return textures;
    }
};

}

#endif

#endif // MODEL_H
