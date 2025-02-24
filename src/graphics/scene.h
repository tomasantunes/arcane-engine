#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include "model.h"

class Scene {
    public:
        Scene();

        std::vector<Model> models = {};

        void AddModel(Model model);
    
    private:

    };

#endif