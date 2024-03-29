root.TitleMenu = {
    Type = TitleMenuType.MO8,
    DrawType = DrawComponentType.SystemMenu,
    PressToStartX = 745,
    PressToStartY = 586,
    LogoX = 560,
    LogoY = 24,
    MenuEntriesNum = 13,
    MenuEntriesSprites = {},
    MenuEntriesHighlightedSprites = {},
    MenuEntriesX = 745,
    MenuEntriesFirstY = 586,
    MenuEntriesGalleryFirstY = 546,
    MenuEntriesYPadding = 80,
    NewGameSpriteIndex = 10,
    ContinueSpriteIndex = 9,
    OptionSpriteIndex = 8,
    GallerySpriteIndex = 6,
    AlbumSpriteIndex = 3,
    MusicSpriteIndex = 2,
    ClearListSpriteIndex = 4,
    WarningSpriteIndex = 0,
    AdditionalSpriteIndex = 8,
    DLCSpriteIndex = 1,
    LoadSpriteIndex = 5,
    QuickLoadSpriteIndex = 12,
    HasAdditional = false,
    PressToStartAnimated = false,
    PressToStartAnimDurationIn = 0,
    PressToStartAnimDurationOut = 0,
    PrimaryFadeAnimDuration = 0.5,
    ItemFadeAnimDuration = 0.3,
    PressToStartSprite = "TitleMenuPressToStart",
    BackgroundSprite = "TitleMenuBackground",
    LogoSprite = "TitleMenuLogo"
};

for i = 1, 8 do
    root.Sprites["MenuEntry" .. i] = {
        Sheet = "TitleChip",
        Bounds = {
            X = 0,
            Y = 308 + (i * 88),
            Width = 430,
            Height = 80
        }
    };
    root.TitleMenu.MenuEntriesSprites[#root.TitleMenu.MenuEntriesSprites + 1] = "MenuEntry" .. i;
end

for i = 0, 4 do
    root.Sprites["MenuEntry" .. (i + 10)] = {
        Sheet = "TitleChip",
        Bounds = {
            X = 438,
            Y = 308 + (i * 88),
            Width = 430,
            Height = 80
        }
    };
    root.TitleMenu.MenuEntriesSprites[#root.TitleMenu.MenuEntriesSprites + 1] = "MenuEntry" .. (i + 10);
end

root.TitleMenu.MenuEntriesHighlightedSprites = root.TitleMenu.MenuEntriesSprites;

root.Sprites["TitleMenuPressToStart"] = {
    Sheet = "TitleChip",
    Bounds = { X = 0, Y = 308, Width = 430, Height = 80 },
};

root.Sprites["TitleMenuBackground"] = {
    Sheet = "Title",
    Bounds = { X = 0, Y = 0, Width = 1920, Height = 1080 },
};

root.Sprites["TitleMenuLogo"] = {
    Sheet = "TitleChip",
    Bounds = { X = 0, Y = 0, Width = 810, Height = 300 },
};