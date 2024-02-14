root.SystemMenu = {
    Type = SystemMenuType.CHLCC,
    DrawType = DrawComponentType.SystemMenu,
    BackgroundColor = 0xff9cb6,
    BackgroundFilter = "BackgroundFilter",
    FadeInDuration = 64 / 60,
    FadeOutDuration = 64 / 60,
    CircleStartPosition = {X = 20, Y = 20},
    CircleSprite = "SystemMenuCircle",
    CircleOffset = 200,
    ErinPosition = {X = 301, Y = 1},
    ErinSprite = "Erin",
    TitleFadeInDuration = 40 / 60,
    TitleFadeOutDuration = 28 / 60,

    MenuEntriesNum = 8,
    MenuEntriesHNum = 0,
    ------
    --Not used since it isn't laid out in a way that would make it easier to use this
    MenuEntriesX = 173,
    MenuEntriesXOffset = 0,
    MenuEntriesFirstY = 445,
    MenuEntriesYPadding = 0,
    ------
    FocusTint = 0xff9cb6,
    MenuEntriesPositions = {
        {X = 110, Y = 107}, {X = 110, Y = 183},
        {X = 110, Y = 240}, {X = 110, Y = 263},
        {X = 110, Y = 321}, {X = 110, Y = 408},
        {X = 110, Y = 449}, {X = 110, Y = 475}
    },
    MenuEntriesSprites = {},
};

root.Sprites["SystemMenuCircle"] = {
    Sheet = "Main",
    Bounds = {X = 533, Y = 361, Width = 106, Height = 106}
}

root.Sprites["SystemMenuBacklog"] = {
    Sheet = "Main",
    Bounds = {X = 173, Y = 445, Width = 243, Height = 67}
};
root.SystemMenu.MenuEntriesSprites[#root.SystemMenu.MenuEntriesSprites + 1] = "SystemMenuBacklog";

root.Sprites["SystemMenuSave"] = {
    Sheet = "Main",
    Bounds = {X = 173, Y = 514, Width = 243, Height = 44}
};
root.SystemMenu.MenuEntriesSprites[#root.SystemMenu.MenuEntriesSprites + 1] = "SystemMenuSave";

root.Sprites["SystemMenuLoad"] = {
    Sheet = "Main",
    Bounds = {X = 173, Y = 560, Width = 243, Height = 45}
};
root.SystemMenu.MenuEntriesSprites[#root.SystemMenu.MenuEntriesSprites + 1] = "SystemMenuLoad";

root.Sprites["SystemMenuQuickSave"] = {
    Sheet = "Main",
    Bounds = {X = 173, Y = 607, Width = 243, Height = 75}
};
root.SystemMenu.MenuEntriesSprites[#root.SystemMenu.MenuEntriesSprites + 1] = "SystemMenuQuickSave";

root.Sprites["SystemMenuQuickLoad"] = {
    Sheet = "Main",
    Bounds = {X = 173, Y = 684, Width = 243, Height = 73}
};
root.SystemMenu.MenuEntriesSprites[#root.SystemMenu.MenuEntriesSprites + 1] = "SystemMenuQuickLoad";

root.Sprites["SystemMenuTips"] = {
    Sheet = "Main",
    Bounds = {X = 173, Y = 759, Width = 243, Height = 39}
};
root.SystemMenu.MenuEntriesSprites[#root.SystemMenu.MenuEntriesSprites + 1] = "SystemMenuTips";

root.Sprites["SystemMenuConfig"] = {
    Sheet = "Main",
    Bounds = {X = 173, Y = 800, Width = 243, Height = 53}
};
root.SystemMenu.MenuEntriesSprites[#root.SystemMenu.MenuEntriesSprites + 1] = "SystemMenuConfig";

root.Sprites["SystemMenuReturnTitle"] = {
    Sheet = "Main",
    Bounds = {X = 173, Y = 855, Width = 243, Height = 86}
};
root.SystemMenu.MenuEntriesSprites[#root.SystemMenu.MenuEntriesSprites + 1] = "SystemMenuReturnTitle";
