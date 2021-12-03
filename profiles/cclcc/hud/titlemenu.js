root.TitleMenu = {
    Type: TitleMenuType.Dash,
    DrawType: DrawComponentType.SystemMenu,
    PressToStartX: 777,
    PressToStartY: 611,
    PressToStartAnimDurationIn: 0.7,
    PressToStartAnimDurationOut: 0.7,
    PressToStartSprite: "TitleMenuPressToStart",
    BackgroundSprite: "TitleMenuBackground",
    MenuEntriesNum: 0
};

root.Sprites["TitleMenuPressToStart"] = {
    Sheet: "TitleChip",
    Bounds: { X: 568, Y: 65, Width: 227, Height: 188 },
};

root.Sprites["TitleMenuBackground"] = {
    Sheet: "Title",
    Bounds: { X: 0, Y: 0, Width: 1920, Height: 1080 },
};