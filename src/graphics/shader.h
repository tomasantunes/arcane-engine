#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

class Shader {
public:
    GLuint Program;

    Shader(const std::string& vertexPath, const std::string& fragmentPath);

    void setVec3(const std::string& name, const glm::vec3& value) const;

private:
    // Utility function for checking shader compilation/linking errors.
    void checkCompileErrors(GLuint shader, const std::string& type);
};

#endif