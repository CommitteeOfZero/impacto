root.TitleMenu = {
    Type: TitleMenuType.CC,
    DrawType: DrawComponentType.SystemMenu,
    PressToStartX: 816,
    PressToStartY: 738,
    PressToStartAnimDurationIn: 0.7,
    PressToStartAnimDurationOut: 0.7,
    PressToStartAnimFastDurationIn: 0.1,
    PressToStartAnimFastDurationOut: 0.1,
    BackgroundX: 0,
    BackgroundY: 0,
    BackgroundBoundsX: 580,
    BackgroundBoundsYNormal: 1,
    BackgroundBoundsYTrue: 1535,
    BackgroundBoundsWidth: 1920,
    BackgroundBoundsHeight: 318,
    FenceX: 0,
    FenceY: 288,
    FenceBoundsX: 1536,
    FenceBoundsYNormal: 1,
    FenceBoundsYTrue: 865,
    FenceBoundsWidth: 1920,
    FenceBoundsHeight: 862,
    CopyrightX: 640,
    CopyrightY: 947,
    CopyrightXMoveOffset: 1536,
    SmokeX: 0,
    SmokeY: 580,
    SmokeBoundsX: 20,
    SmokeBoundsY: 1800,
    SmokeBoundsWidth: 1920,
    SmokeBoundsHeight: 500,
    SmokeAnimationBoundsXOffset: 20,
    SmokeAnimationBoundsXMax: 1919,
    SmokeOpacityNormal: 0.25,
    SmokeAnimationDurationIn: 32,
    SmokeAnimationDurationOut: 32,
    MoveLeftAnimationDurationIn: 0.7,
    MoveLeftAnimationDurationOut: 0.7,
    PressToStartSprite: "TitleMenuPressToStart",
    BackgroundSprite: "TitleMenuBackground",
    FenceSprite: "TitleMenuFence",
    CopyrightSprite: "TitleMenuCopyright",
    OverlaySprite: "TitleMenuOverlay",
    SmokeSprite: "TitleMenuSmoke",
    MenuEntriesNum: 0
};

root.Sprites["TitleMenuPressToStart"] = {
    Sheet: "TitleChip",
    Bounds: { X: 1508, Y: 689, Width: 304, Height: 94 },
};

root.Sprites["TitleMenuFence"] = {
    Sheet: "Title",
    Bounds: { X: 1536, Y: 1, Width: 1920, Height: 862 },
};

root.Sprites["TitleMenuCopyright"] = {
    Sheet: "TitleChip",
    Bounds: { X: 1, Y: 667, Width: 860, Height: 20 },
};

root.Sprites["TitleMenuBackground"] = {
    Sheet: "TitleChip",
    Bounds: { X: 580, Y: 1, Width: 1920, Height: 318 },
};

root.Sprites["TitleMenuOverlay"] = {
    Sheet: "MenuChip",
    Bounds: { X: 155, Y: 142, Width: 1898, Height: 1058 },
};

root.Sprites["TitleMenuSmoke"] = {
    Sheet: "MenuChip",
    Bounds: { X: 20, Y: 1800, Width: 1920, Height: 500 },
};