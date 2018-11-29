root.MainMenu = {
    Type: MainMenuType.RNE,
    BackgroundInDuration: 0.75,
    BackgroundOutDuration: 1.0,
    BackgroundSprite: "MainMenuBackground"
};

root.Sprites["MainMenuBackground"] = {
    Sheet: "Menu",
    Bounds: { X: 0, Y: 0, Width: 960, Height: 544 },
    BaseScale: { X: 1280 / 960, Y: 720 / 544 }
};