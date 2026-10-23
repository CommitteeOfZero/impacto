root.TitleMenu = {
    Type = TitleMenuType.SG0,
    DrawType = DrawComponentType.SystemMenu,
    PressToStartPos = { X = 728, Y = 880 },
    PressToStartAnimDurationIn = 0.7,
    PressToStartAnimDurationOut = 0.7,
    PressToStartAnimFastDurationIn = 0.1,
    PressToStartAnimFastDurationOut = 0.1,
    PressToStartSprite = "TitleMenuPressToStart",
    LogoSprite = "TitleMenuLogo",
    LogoPos = { X = 432, Y = 150 },
    ItemPos = { X = 120, Y = 640 },
    ItemSpacing = 52,
    SubItemOffsetX = 300,
    SubItemSpacing = 48,
    MenuEntriesNum = 5,
    MenuEntriesSprites = {},
    MenuEntriesHighlightedSprites = {},
    SubItemSprites = {},
    SubItemHighlightedSprites = {},
};

root.Sprites["TitleMenuPressToStart"] = {
    Sheet = "TitleChip",
    Bounds = { X = 8, Y = 1428, Width = 464, Height = 40 },
};
root.Sprites["TitleMenuLogo"] = {
    Sheet = "TitleLogo",
    Bounds = { X = 48, Y = 1788, Width = 1056, Height = 284 },
};

for i = 0, 4 do
    root.Sprites["TitleMenuEntry" .. i] = {
        Sheet = "TitleChip",
        Bounds = { X = 648, Y = 1088 + i * 42, Width = 268, Height = 40 },
    };
    root.Sprites["TitleMenuEntryHighlight" .. i] = {
        Sheet = "TitleChip",
        Bounds = { X = 920, Y = 1088 + i * 42, Width = 268, Height = 40 },
    };
    root.TitleMenu.MenuEntriesSprites[i + 1] = "TitleMenuEntry" .. i;
    root.TitleMenu.MenuEntriesHighlightedSprites[i + 1] = "TitleMenuEntryHighlight" .. i;
end

for i = 0, 6 do
    root.Sprites["TitleMenuSubEntry" .. i] = {
        Sheet = "TitleChip",
        Bounds = { X = 4, Y = 1088 + i * 48, Width = 312, Height = 40 },
    };
    root.Sprites["TitleMenuSubEntryHighlight" .. i] = {
        Sheet = "TitleChip",
        Bounds = { X = 328, Y = 1088 + i * 48, Width = 312, Height = 40 },
    };
    root.TitleMenu.SubItemSprites[i + 1] = "TitleMenuSubEntry" .. i;
    root.TitleMenu.SubItemHighlightedSprites[i + 1] = "TitleMenuSubEntryHighlight" .. i;
end
