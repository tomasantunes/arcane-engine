function load(entity)
    print("Script loaded!")
    local tc = getTransform(entity)
    if tc then
        tc.position = {10, 0, 5}
        tc.rotation = {0, 45, 0}
        tc.scale = {2, 2, 2}
        
        setTransform(entity, tc)
        
        transformSystem:updateTransforms()
    end
end

function update(entity, deltaTime)
    print("Script updated!")
end