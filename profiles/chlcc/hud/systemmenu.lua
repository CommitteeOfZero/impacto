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

    MenuLoopDuration = 1.815,
    MenuHoverLerpSpeed = 20,
    SystemMenuBackground = "SystemMenuBackground",
    SystemMenuBackgroundPosition = {X = 93,Y = 0},
    SystemMenuItemsLine = "SystemMenuItemsLine",
    SystemMenuItemsLinePosition = {X = 82,Y = 0},
    MainMenuTitleText = "MainMenuTitleText",
    MenuTitleTextPosition = {X = 260, Y = 0},
    MainMenuLabelRightPosition = {X = 793, Y = 135},
    MenuTitleTextAngle = -1.83,
    SystemMenuSelectionDot = "SystemMenuSelectionDot",
    SystemMenuSelectionDotPosition = {X = 87, Y = 162},
    SystemMenuSelectionDotMultiplier = 52,
    SystemMenuSelection = "SystemMenuSelection",
    SystemMenuSelectionPosition = {X = 93, Y = 73},
    SystemMenuRunningSelectedLabel = "SystemMenuRunningSelectedLabel",
    SystemMenuRunningSelectedLabelPosition = {X = 0,Y=112},
    SystemMenuRunningSelectedLabelAngle = -0.268,
    SystemMenuButtonPrompt= "SystemMenuButtonPrompt",
    SystemMenuButtonPromptPosition = {X = 1022, Y = 651},
    InitialRedBarPosition = { X = 0, Y = 538 },
    RightRedBarPosition = { X = 1059, Y = 538 },
    RedBarDivision = 1826,
    RedBarBaseX = 1059,
    RedBarSprite = "RedBar",
    RedBarLabelPosition = { X = 1067, Y = 573 },
    RedBarLabel = "RedBarLabel",
    MenuSelectedLabelSpeed = -400,
    MenuEntriesNum = 9,
    MenuEntriesHNum = 0,
    SelectMenuSpritesCount = 10,
    ------
    --Not used since it isn't laid out in a way that would make it easier to use this
    MenuEntriesX = 173,
    MenuEntriesXOffset = 0,
    MenuEntriesFirstY = 445,
    MenuEntriesYPadding = 0,
    ------
    FocusTint = 0xff9cb6,
    MenuEntriesPositions = {
        {X = 110, Y = 107}, {X = 110, Y = 180},
        {X = 110, Y = 234}, {X = 110, Y = 254},
        {X = 110, Y = 309}, {X = 110, Y = 379},
        {X = 110, Y = 445}, {X = 110, Y = 479},
        {X = 110, Y = 506}
    },
        SelectMenuTextPositions = {
        {X = 95, Y = 50}, {X = 110, Y = 50},
        {X = 123, Y = 50}, {X = 135, Y = 50},
        {X = 148, Y = 50}, {X = 162, Y = 50},
        {X = 178, Y = 50}, {X = 196, Y = 50},
        {X = 209, Y = 50}, {X = 225, Y = 50}
    },
    MenuEntriesSprites = {},
    SelectMenuSprites = {},
};

root.Sprites["SystemMenuBackground"] = {
    Sheet = "Main",
    Bounds = {X = 1770, Y = 1, Width = 278, Height = 720}
};

root.Sprites["SystemMenuItemsLine"] = {
    Sheet = "Main",
    Bounds = {X = 0, Y = 361, Width = 171, Height = 600}
};

root.Sprites["MainMenuTitleText"] = {
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
    Bounds = {X = 1789, Y = 723, Width = 258, Height = 28}
};

root.Sprites["SystemMenuSelection"] = {
    Sheet = "Main",
    Bounds = {X = 1771, Y = 756, Width = 277, Height = 116}
};

root.Sprites["SystemMenuSelectionDot"] = {
    Sheet = "Main",
    Bounds = {X = 511, Y = 550, Width = 14, Height = 14}
};

root.Sprites["RedBar"] = {
    Sheet = "Main",
    Bounds = { X = 767, Y = 913, Width = 1280, Height = 110 }
}

root.Sprites["RedBarLabel"] = {
    Sheet = "Main",
    Bounds = { X = 506, Y = 469, Width = 133, Height = 74 }
}
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

local fadeTextSprites = {
    {name = "S",  x = 447, w = 18},
    {name = "E",  x = 467, w = 16},
    {name = "L",  x = 485, w = 15},
    {name = "E2", x = 502, w = 16},
    {name = "C",  x = 520, w = 17},
    {name = "T",  x = 539, w = 22},
    {name = "M",  x = 560, w = 22},
    {name = "E3", x = 583, w = 17},
    {name = "N",  x = 601, w = 18},
    {name = "U",  x = 622, w = 17},
}

for _, sprite in ipairs(fadeTextSprites) do
    local id =  "SelectMenu_" .. sprite.name
    root.Sprites[id] = {
        Sheet = "Main",
        Bounds = {X = sprite.x, Y = 587, Width = sprite.w, Height = 55}
    }
    root.SystemMenu.SelectMenuSprites[#root.SystemMenu.SelectMenuSprites + 1] = id;
end
