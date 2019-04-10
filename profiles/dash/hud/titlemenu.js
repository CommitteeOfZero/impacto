root.TitleMenu = {
    Type: TitleMenuType.Dash,
    PressToStartX: 777,
    PressToStartY: 611,
    PressToStartAnimDurationIn: 0.7,
    PressToStartAnimDurationOut: 0.7,
    PressToStartSprite: "TitleMenuPressToStart",
    BackgroundSprite: "TitleMenuBackground",
};

root.Sprites["TitleMenuPressToStart"] = {
    Sheet: "Title",
    Bounds: { X: 1947, Y: 983, Width: 365, Height: 38 },
};

root.Sprites["TitleMenuBackground"] = {
    Sheet: "Title",
    Bounds: { X: 0, Y: 0, Width: 1920, Height: 1080 },
};