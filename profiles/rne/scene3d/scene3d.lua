root.Scene3D = {
    Version = LKMVersion.RNE,
    MaxRenderables = 8,
    DefaultCamera = {
        Position = {
            X = 0,
            Y = 12.5,
            Z = 23
        },
        Target = {
            X = 0,
            Y = 12.5,
            Z = 0
        },
        Fov = 3.141592653 / 8 -- pi / 8
    },
    AnimationDesignFrameRate = 30,
    -- Animation 22 of Model 273 (c11_010) has flat-out broken data (and isn't used by the game anyway)
    AnimationParseBlacklist = {
        [273] = {22}
    }
};

include('rne/scene3d/characters.lua');