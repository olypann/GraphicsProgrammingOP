


#pragma once

#include <vector>
#include <stdio.h>
#include <string>

#include <GL/glew.h>
#include <glm/glm.hpp>

// texture construct
struct Texture
{
    unsigned int id;
    std::string type;
};

class Model
{
public:
    // attribute list
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;

    std::vector<glm::vec3> tangents;
    std::vector<glm::vec3> bitangents;

    std::vector<Texture>   textures;

    unsigned int textureID;

    float ka, kd, ks, Ns;

    Model(const char* path);

    void draw(unsigned int& shaderID);
    void addTexture(const char* path, const std::string type);

    void deleteBuffers();

private:

    // buffers
    unsigned int VAO;
    unsigned int vertexBuffer;
    unsigned int uvBuffer;
    unsigned int normalBuffer;

    unsigned int tangentBuffer;
    unsigned int bitangentBuffer;

    bool loadObj(const char* path,
        std::vector<glm::vec3>& inVertices,
        std::vector<glm::vec2>& inUVs,
        std::vector<glm::vec3>& inNormals);

    void setupBuffers();
    unsigned int loadTexture(const char* path);

    void calculateTangents();
};
