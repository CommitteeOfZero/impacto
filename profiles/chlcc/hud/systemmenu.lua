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

    SystemMenuBackground = "SystemMenuBackground",
    SystemMenuBackgroundPosition = {X = 93,Y = 0},
    SystemMenuItemsLine = "SystemMenuItemsLine",
    SystemMenuItemsLinePosition = {X = 80,Y = 0},
    MainMenuLabel = "MainMenuLabel",
    MainMenuLabelPosition = {X=260, Y = 0},
    MainMenuLabelRightPosition = {X=800, Y = 165},
    MainMenuLabelRightAngle = -1.8,
    SystemMenuSelectionDot = "SystemMenuSelectionDot",
    SystemMenuSelectionDotPosition = {X = 86, Y = 162},
    SystemMenuSelectionDotMultiplier = 52,
    SystemMenuSelection = "SystemMenuSelection",
    SystemMenuSelectionPosition = {X = 93, Y = 73},
    SystemSelectMenuHeader ="SystemSelectMenuHeader",
    SystemSelectMenuHeaderPosition = {X = 96, Y = 50},
    SystemMenuRunningSelectedLabel = "SystemMenuRunningSelectedLabel",
    SystemMenuRunningSelectedLabelPosition = {X = 0,Y=112},
    SystemMenuRunningSelectedLabelAngle = -0.27,
    SystemMenuButtonPrompt= "SystemMenuButtonPrompt",
    SystemMenuButtonPromptPosition = {X = 1022, Y = 651},
    SystemMenuLine="SystemMenuLine",
    SystemMenuLinePosition = {X = 0, Y = 539},
    SystemMenuCHLCCLabel = "SystemMenuCHLCCLabel",
    SystemMenuCHLCCLabelPosition = {X = 1068, Y = 575},
    MenuEntriesNum = 9,
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
        {X = 110, Y = 107}, {X = 110, Y = 182},
        {X = 110, Y = 233}, {X = 110, Y = 253},
        {X = 110, Y = 304}, {X = 110, Y = 390},
        {X = 110, Y = 428}, {X = 110, Y = 475},
        {X = 110, Y = 499}
    },
    MenuEntriesSprites = {},
};

root.Sprites["SystemMenuBackground"] = {
    Sheet = "Main",
    Bounds = {X = 1771, Y = 1, Width = 277, Height = 720}
};

root.Sprites["SystemMenuItemsLine"] = {
    Sheet = "Main",
    Bounds = {X = 0, Y = 361, Width = 174, Height = 600}
};

root.Sprites["MainMenuLabel"] = {
    Sheet = "Main",
    Bounds = {X = 1634, Y = 5, Width = 115, Height = 900}
};

root.Sprites["SystemMenuRunningSelectedLabel"] = {
    Sheet = "Main",
    Bounds = {X = 1396, Y = 801, Width = 226, Height = 102}
};

root.Sprites["SystemSelectMenuHeader"] = {
    Sheet = "Main",
    Bounds = {X = 449, Y = 587, Width = 191, Height = 55}
};

root.Sprites["SystemMenuButtonPrompt"] = {
    Sheet = "Main",
    Bounds = {X = 1790, Y = 723, Width = 258, Height = 28}
};

root.Sprites["SystemMenuSelection"] = {
    Sheet = "Main",
    Bounds = {X = 1771, Y = 756, Width = 277, Height = 116}
};

root.Sprites["SystemMenuSelectionDot"] = {
    Sheet = "Main",
    Bounds = {X = 512, Y = 550, Width = 14, Height = 14}
};

root.Sprites["SystemMenuLine"] = {
    Sheet = "Main",
    Bounds = {X = 768, Y = 913, Width = 1280, Height = 109}
};

root.Sprites["SystemMenuCHLCCLabel"] = {
    Sheet = "Main",
    Bounds = {X = 508, Y = 471, Width = 131, Height = 71}
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

root.Sprites["SystemMenuConfig"] = {
    Sheet = "Main",
    Bounds = {X = 173, Y = 800, Width = 243, Height = 53}
};
root.SystemMenu.MenuEntriesSprites[#root.SystemMenu.MenuEntriesSprites + 1] = "SystemMenuConfig";


root.Sprites["SystemMenuTips"] = {
    Sheet = "Main",
    Bounds = {X = 173, Y = 759, Width = 243, Height = 39}
};
root.SystemMenu.MenuEntriesSprites[#root.SystemMenu.MenuEntriesSprites + 1] = "SystemMenuTips";

root.Sprites["SystemMenuTrophy"] = {
    Sheet = "Main",
    Bounds = {X = 173, Y = 945, Width = 243, Height = 58}
};
root.SystemMenu.MenuEntriesSprites[#root.SystemMenu.MenuEntriesSprites + 1] = "SystemMenuTrophy";

root.Sprites["SystemMenuReturnTitle"] = {
    Sheet = "Main",
    Bounds = {X = 173, Y = 855, Width = 243, Height = 86}
};
root.SystemMenu.MenuEntriesSprites[#root.SystemMenu.MenuEntriesSprites + 1] = "SystemMenuReturnTitle";


