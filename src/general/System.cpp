#include <set>
#include "../structs.cpp"

class System {
public:
    std::set<Entity> entities;
    virtual void Update(float dt) = 0;
};