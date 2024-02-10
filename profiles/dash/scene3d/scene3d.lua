root.Scene3D = {
    Version = LKMVersion.DaSH,
    MaxRenderables = 8,
    DefaultCamera = {
        Position = {
            X = 0,
            Y = 1.25,
            Z = 2.3
        },
        Target = {
            X = 0,
            Y = 1.25,
            Z = 0
        },
        Fov = 3.141592653 / 8 -- pi / 8
    },
    AnimationDesignFrameRate = 30,
    AnimationParseBlacklist = {}
};

include('dash/scene3d/characters.lua');