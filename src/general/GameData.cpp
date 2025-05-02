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
#include "ScriptComponent.cpp"

namespace fs = std::filesystem;


class GameData {
    public:
        GameData() {};
        
        void SaveEntity(Entity entity, ComponentArray<TransformComponent>* transformArray, ComponentArray<ModelComponent>* modelArray, ComponentArray<ScriptComponent>* scriptArray) {
            std::string filename = "game/data/" + std::to_string(entity);

            TransformComponent* t = transformArray->GetComponent(entity);
            ModelComponent* m = modelArray->GetComponent(entity);
            ScriptComponent* s = scriptArray->GetComponent(entity);

            std::ofstream outFile(filename);

            outFile << "Transform" << std::endl;
            outFile << mat4ToString(t->transform) << std::endl;
            outFile << vec3ToString(t->position) << std::endl;
            outFile << vec3ToString(t->rotation) << std::endl;
            outFile << vec3ToString(t->scale) << std::endl;
            outFile << "Model" << std::endl;
            outFile << m->filename << std::endl;

            if (s) {
                outFile << "Script" << std::endl;
                outFile << s->filename << std::endl;
            }
            else {
                outFile << "Script" << std::endl;
                outFile << "" << std::endl;
            }

            outFile.close();
        }

        std::vector<Entity> LoadEntities() {
            std::vector<Entity> entities;
            std::string folderPath = "data";

            // Check if the provided path is a directory
            if (!fs::is_directory(folderPath)) {
                std::cerr << "Provided path is not a directory: " << folderPath << std::endl;
                return entities;
            }

            // Iterate over the directory and collect filenames
            for (const auto& entry : fs::directory_iterator(folderPath)) {
                if (entry.is_regular_file()) {
                    entities.push_back(std::stoi(entry.path().filename().string()));
                }
            }

            return entities;
        }

        TransformComponent LoadEntityTransform(Entity entity) {
            std::string filepath = "data/" + std::to_string(entity);

            std::ifstream file(filepath);
            if (!file.is_open()) {
                std::cerr << "Failed to open the file!" << std::endl;
            }

            // Vector to store the lines
            std::vector<std::string> lines;
            std::string line;

            // Read lines from the file
            int line_number = 0;
            while (std::getline(file, line)) {
                line_number++;
                if (line_number >= 2 && line_number <= 5) {
                    lines.push_back(line);
                }
                // Stop reading after the 5th line
                if (line_number > 5) {
                    break;
                }
            }

            // Close the file
            file.close();

            TransformComponent transform = {
                stringToMat4(lines[0]),
                stringToVec3(lines[1]),
                stringToVec3(lines[2]),
                stringToVec3(lines[3])
            };

            return transform;
        }

        std::string LoadEntityModel(Entity entity) {
            std::string filepath = "data/" + std::to_string(entity);

            std::ifstream file(filepath);
            if (!file.is_open()) {
                std::cerr << "Failed to open the file!" << std::endl;
            }

            // Vector to store the lines
            std::vector<std::string> lines;
            std::string line;

            // Read lines from the file
            int line_number = 0;
            while (std::getline(file, line)) {
                line_number++;
                if (line_number == 7) {
                    lines.push_back(line);
                }
                // Stop reading after the 7th line
                if (line_number > 7) {
                    break;
                }
            }

            // Close the file
            file.close();

            std::string filename = lines[0];
            return filename;
        }

        std::string LoadEntityScript(Entity entity) {
            std::string filepath = "data/" + std::to_string(entity);

            std::ifstream file(filepath);
            if (!file.is_open()) {
                std::cerr << "Failed to open the file!" << std::endl;
            }

            // Vector to store the lines
            std::vector<std::string> lines;
            std::string line;

            // Read lines from the file
            int line_number = 0;
            while (std::getline(file, line)) {
                line_number++;
                if (line_number == 9) {
                    lines.push_back(line);
                }
                // Stop reading after the 5th line
                if (line_number > 9) {
                    break;
                }
            }

            // Close the file
            file.close();

            std::string filename = lines[0];
            return filename;
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

        void writeStringToFile(const std::string& content, const std::string& filePath) {
            // Open the file in output mode, which will overwrite the file if it exists
            std::ofstream outputFile(filePath);
        
            // Check if the file was successfully opened
            if (!outputFile.is_open()) {
                std::cerr << "Error: Could not open file " << filePath << " for writing." << std::endl;
                return;
            }
        
            // Write the content to the file
            outputFile << content;
        
            // Close the file
            outputFile.close();
        
            // Check if the write operation was successful
            if (outputFile.fail()) {
                std::cerr << "Error: Failed to write content to file " << filePath << "." << std::endl;
            } else {
                std::cout << "Content successfully written to " << filePath << "." << std::endl;
            }
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