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
#include <filesystem>

namespace fs = std::filesystem;


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


        void replaceStringsInFile(const std::string& inputFilePath, const std::string& outputFilePath, const std::vector<std::string>& searchStrs, const std::vector<std::string>& replaceStrs) {
            // Check if the search and replace vectors have the same size
            if (searchStrs.size() != replaceStrs.size()) {
                std::cerr << "Error: searchStrs and replaceStrs must have the same size." << std::endl;
                return;
            }
        
            // Open the input file
            std::ifstream inputFile(inputFilePath);
            if (!inputFile.is_open()) {
                std::cerr << "Error: Could not open input file: " << inputFilePath << std::endl;
                return;
            }
        
            // Read the entire file into a string
            std::stringstream buffer;
            buffer << inputFile.rdbuf();
            std::string fileContent = buffer.str();
            inputFile.close();
        
            // Perform all replacements
            for (size_t i = 0; i < searchStrs.size(); ++i) {
                const std::string& searchStr = searchStrs[i];
                const std::string& replaceStr = replaceStrs[i];
        
                size_t pos = 0;
                while ((pos = fileContent.find(searchStr, pos)) != std::string::npos) {
                    fileContent.replace(pos, searchStr.length(), replaceStr);
                    pos += replaceStr.length(); // Move past the replaced string
                }
            }
        
            // Ensure the output directory exists
            fs::path outputDir = fs::path(outputFilePath).parent_path();
            if (!fs::exists(outputDir)) {
                fs::create_directories(outputDir);
            }
        
            // Write the modified content to the output file
            std::ofstream outputFile(outputFilePath);
            if (!outputFile.is_open()) {
                std::cerr << "Error: Could not open output file: " << outputFilePath << std::endl;
                return;
            }
            outputFile << fileContent;
            outputFile.close();
        
            std::cout << "File successfully modified and saved to: " << outputFilePath << std::endl;
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