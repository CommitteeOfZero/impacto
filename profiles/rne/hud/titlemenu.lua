root.TitleMenu = {
    Type = TitleMenuType.RNE,
    DrawType = DrawComponentType.SystemMenu,
    Background = {
        DurationIn = 1.0,
        DurationOut = 1.0,
        Sprite = "TitleMenuBackground",
        Seed = 0,
        Rows = 6,
        Columns = 12,
        CenterY = 0.7,
        VanishingPointX = 0.66,
        Depth = 2,
        MaxAngle = 3.141592653 / 1.5 -- pi / 1.5
    },
    PreTitleAnimDurationIn = 0.4,
    PreTitleAnimDurationOut = 0.4,
    PressToStartAnimDurationIn = 0.7,
    PressToStartAnimDurationOut = 0.7,
    PressToStartSprite = "TitleMenuPressToStart",
    LineSprite = "TitleMenuLine",
    CopyrightSprite = "TitleMenuCopyright",
    EliteSprite = "TitleMenuElite",
    LogoSprite = "TitleMenuLogo",
    LineWidth = 1280,
    CopyrightWidth = 370,
    LogoWidth = 524,
    EliteHeight = 60,
    PressToStartX = 329 * (1280 / 960),
    PressToStartY = 394 * (720 / 544),
    LineX = 0,
    LineY = 272 * (720 / 544),
    CopyrightX = 299 * (1280 / 960),
    CopyrightY = 489 * (720 / 544),
    EliteX = 488 * (1280 / 960),
    EliteY = 275 * (720 / 544),
    LogoX = 487 * (1280 / 960),
    LogoY = 216 * (720 / 544),
    MenuEntriesNum = 0
};

root.Sprites["TitleMenuBackground"] = {
    Sheet = "TitleBackground",
    Bounds = { X = 0, Y = 0, Width = 960, Height = 544 },
    BaseScale = { X = 1280 / 960, Y = 720 / 544 }
};

root.Sprites["TitleMenuPressToStart"] = {
    Sheet = "Title",
    Bounds = { X = 0, Y = 160, Width = 320, Height = 26 },
    BaseScale = { X = 1280 / 960, Y = 720 / 544 }
};

root.Sprites["TitleMenuLine"] = {
    Sheet = "Title",
    Bounds = { X = 0, Y = 1, Width = 1280, Height = 2 },
};

root.Sprites["TitleMenuCopyright"] = {
    Sheet = "Title",
    Bounds = { X = 0, Y = 188, Width = 370, Height = 20 },
    BaseScale = { X = 1280 / 960, Y = 720 / 544 }
};

root.Sprites["TitleMenuElite"] = {
    Sheet = "Title",
    Bounds = { X = 0, Y = 66, Width = 280, Height = 60 },
    BaseScale = { X = 1280 / 960, Y = 720 / 544 }
};

root.Sprites["TitleMenuLogo"] = {
    Sheet = "Title",
    Bounds = { X = 0, Y = 5, Width = 524, Height = 60 },
    BaseScale = { X = 1280 / 960, Y = 720 / 544 }
};