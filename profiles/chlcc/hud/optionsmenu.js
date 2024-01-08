root.OptionsMenu = {
    DrawType: DrawComponentType.SystemMenu,
    Type: OptionsMenuType.CHLCC,
    TransitionDuration: 64 / 60,
    BackgroundColor: 0xa988e5,
    CircleSprite: "CircleConfig",
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
    MenuTitleTextRightPos: { X: 550, Y: 525 },
    MenuTitleTextAngle: 6.02,
    TitleFadeInDuration: 40 / 60,
    TitleFadeOutDuration: 28 / 60,
    MenuTitleText: "MenuTitleTextConfig",
    ButtonPromptSprite: "ButtonPromptConfig",
    ButtonPromptPosition: { X: 685, Y: 651 },
    SelectWord: [],
    SelectWordPos: [
        { X: 94, Y: 51 },
        { X: 109, Y: 51 },
        { X: 122, Y: 51 },
        { X: 134, Y: 51 },
        { X: 147, Y: 51 },
        { X: 161, Y: 51 },
        { X: 180, Y: 51 },
        { X: 199, Y: 51 },
        { X: 213, Y: 51 },
        { X: 228, Y: 51 }
    ],
};

root.Sprites["CircleConfig"] = {
    Sheet: "Options",
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

root.Sprites["MenuTitleTextConfig"] = {
    Sheet: "Options",
    Bounds: { X: 482, Y: 604, Width: 592, Height: 117 }
}

root.Sprites["ButtonPromptConfig"] = {
    Sheet: "Options",
    Bounds: { X: 481, Y: 824, Width: 676, Height: 28 }
}