#ifndef GAMEDATA_CPP
#define GAMEDATA_CPP

#include "../structs.cpp"
#include <vector>
#include "../graphics/GraphicsComponents.cpp"
#include "ComponentManager.cpp"
#include <glm/glm.hpp>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>


class GameData {
    public:
        GameData();
        
        void SaveEntity(Entity entity, ComponentArray<TransformComponent>* transformArray) {
            std::string filename = "game/data/" + std::to_string(entity);

            TransformComponent* t = transformArray->GetComponent(entity);

            std::ofstream outFile(filename);

            outFile << mat4ToString(t->transform) << std::endl;
            outFile << vec3ToString(t->position) << std::endl;
            outFile << vec3ToString(t->rotation) << std::endl;
            outFile << vec3ToString(t->scale) << std::endl;

            outFile.close();
        }


    private:
        std::string mat4ToString(const glm::mat4& mat) {
            std::ostringstream oss;
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    oss << mat[i][j];
                    if (i != 3 || j != 3) {
                        oss << " "; // Use space as a delimiter
                    }
                }
            }
            return oss.str();
        }

        glm::mat4 stringToMat4(const std::string& str) {
            glm::mat4 mat;
            std::istringstream iss(str);
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    iss >> mat[i][j];
                }
            }
            return mat;
        }

        std::string vec3ToString(const glm::vec3& vec) {
            std::ostringstream oss;
            oss << vec.x << " " << vec.y << " " << vec.z; // Use space as a delimiter
            return oss.str();
        }

        glm::vec3 stringToVec3(const std::string& str) {
            glm::vec3 vec;
            std::istringstream iss(str);
            iss >> vec.x >> vec.y >> vec.z;
            return vec;
        }
};

#endif