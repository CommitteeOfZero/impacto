root.TitleMenu = {
    Type: TitleMenuType.Dash,
    PressToStartX: 744,
    PressToStartY: 586,
    PressToStartAnimDurationIn: 0.7,
    PressToStartAnimDurationOut: 0.7,
    PressToStartSprite: "TitleMenuPressToStart",
    BackgroundSprite: "TitleMenuBackground",
    MenuEntriesNum: 0
};

root.Sprites["TitleMenuPressToStart"] = {
    Sheet: "TitleChip",
    Bounds: { X: 0, Y: 308, Width: 430, Height: 80 },
};

root.Sprites["TitleMenuBackground"] = {
    Sheet: "Title",
    Bounds: { X: 0, Y: 0, Width: 1920, Height: 1080 },
};