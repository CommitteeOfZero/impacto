root.TipsMenu = {
    DrawType: DrawComponentType.SystemMenu,
    Type: TipsMenuType.CHLCC,
    TransitionDuration: 64 / 60,
    BackgroundColor: 0x007efe,
    CircleSprite: "CircleTips",
    CircleStartPosition: { X: 20, Y: 20 },
    CircleOffset: 200,
    ErinSprite: "Erin",
    ErinPosition: { X: 301, Y: 1 },
    BackgroundFilter: "BackgroundFilter",
    InitialRedBarPosition: { X: 0, Y: 538 },
    RightRedBarPosition: { X: 1059, Y: 538 },
    RedBarDivision: 1826,
    RedBarBaseX: 1059,
    RedBarSprite: "RedBar",
    RedBarLabelPosition: { X: 1067, Y: 573 },
    RedBarLabel: "RedBarLabel",
    MenuTitleTextRightPos: { X: 790, Y: 212 },
    MenuTitleTextLeftPos: { X: 260, Y: 3 },
    MenuTitleTextAngle: 4.45,
    TitleFadeInDuration: 40 / 60,
    TitleFadeOutDuration: 28 / 60,
    MenuTitleText: "MenuTitleTextTips",
};

root.Sprites["CircleTips"] = {
    Sheet: "Tips",
    Bounds: { X: 1, Y: 917, Width: 106, Height: 106 }
}

root.Sprites["Erin"] = {
    Sheet: "Main",
    Bounds: { X: 641, Y: 1, Width: 978, Height: 798 }
}

root.Sprites["BackgroundFilter"] = {
    Sheet: "Main",
    Bounds: { X: 0, Y: 0, Width: 640, Height: 360 }
}

root.Sprites["RedBar"] = {
    Sheet: "Main",
    Bounds: { X: 767, Y: 913, Width: 1280, Height: 110 }
}

root.Sprites["RedBarLabel"] = {
    Sheet: "Main",
    Bounds: { X: 506, Y: 469, Width: 133, Height: 74 }
}

root.Sprites["MenuTitleTextTips"] = {
    Sheet: "Tips",
    Bounds: { X: 1154, Y: 1, Width: 113, Height: 742 }
}