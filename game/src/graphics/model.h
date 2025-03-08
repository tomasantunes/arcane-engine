#ifndef MODEL_H
#define MODEL_H

#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include "shader.h"
#include "../structs.cpp"

class Model {
public:
    Model(const std::string& path);
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    GLuint VAO, VBO, EBO;
    Entity entity;

private:
    void LoadModel(const std::string& path);
    void ProcessNode(aiNode* node, const aiScene* scene);
    void ProcessMesh(aiMesh* mesh, const aiScene* scene);

    
};

#endif