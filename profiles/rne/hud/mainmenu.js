root.MainMenu = {
    Type: MainMenuType.RNE,
    Background: {
        DurationIn: 0.75,
        DurationOut: 1.0,
        Sprite: "MainMenuBackground",
        Seed: 0,
        Rows: 7,
        Columns: 12,
        CenterY: 0.7,
        VanishingPointX: 0.66,
        Depth: 2,
        MaxAngle: Math.PI / 2
    }
};

root.Sprites["MainMenuBackground"] = {
    Sheet: "Menu",
    Bounds: { X: 0, Y: 0, Width: 960, Height: 544 },
    BaseScale: { X: 1280 / 960, Y: 720 / 544 }
};