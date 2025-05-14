function load(entity)
    print("Script loaded!")
    local tc = getTransform(entity)
    setTransform(entity, {
        position = {2, 2, 2},
        rotation = {0, 45, 0},
        scale = {1, 1, 1}
    })
end

function update(entity, deltaTime)

end