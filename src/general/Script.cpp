#ifndef SCRIPT_CPP
#define SCRIPT_CPP

class Script {
    public:
        virtual void Load() {}
        virtual void Update(float deltaTime) {}
};

#endif