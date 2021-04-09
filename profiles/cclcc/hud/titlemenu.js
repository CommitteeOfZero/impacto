root.TitleMenu = {
    Type: TitleMenuType.CCLCC,
    PressToStartX: 823,
    PressToStartY: 749,
    PressToStartAnimDurationIn: 0.7,
    PressToStartAnimDurationOut: 0.7,
    PressToStartSprite: "TitleMenuPressToStart",
    BackgroundSprite: "TitleMenuBackground",
    MainBackgroundSprite: "MainMenuBackground",
    CopyrightTextSprite: "CopyrightText",
    CopyrightTextX: 566,
    CopyrightTextY: 955,
    MenuEntriesNum: 0
};

root.Sprites["TitleMenuBackground"] = {
    Sheet: "Title",
    Bounds: { X: 0, Y: 0, Width: 1920, Height: 1080 },
};

root.Sprites["MainMenuBackground"] = {
    Sheet: "Title",
    Bounds: { X: 0, Y: 1080, Width: 1920, Height: 1080 },
};

root.Sprites["TitleMenuPressToStart"] = {
    Sheet: "TitleChip",
    Bounds: { X: 583, Y: 75, Width: 198, Height: 169 },
};

root.Sprites["CopyrightText"] = {
    Sheet: "TitleChip",
    Bounds: { X: 582, Y: 9, Width: 785, Height: 21 },
};