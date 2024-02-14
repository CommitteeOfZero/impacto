root.SystemMenu = {
    Type = SystemMenuType.None,
    DrawType = DrawComponentType.SystemMenu,
    ButtonBackgroundSprite = "SystemMenuButtonBackground",
    ButtonPromptsSprite = "SystemMenuButtonPrompts",
    ButtonBackgroundStartX = 1257,
    ButtonBackgroundX = 0,
    ButtonBackgroundY = 681,
    ButtonBackgroundTargetWidth = 943,
    ButtonBackgroundSprStartX = 1499,
    MenuEntriesSprites = {},
    MenuEntriesHighlightedSprites = {},
    MenuEntriesX = 0,
    MenuEntriesXOffset = 100,
    MenuEntriesFirstY = 220,
    MenuEntriesYPadding = 50,
};

for i = 0, 7 do
    root.Sprites["SystemMenuEntry" .. i] = {
        Sheet = "Data",
        Bounds = {
            X = 964,
            Y = 296 + i * 38,
            Width = root.SystemMenu.MenuEntriesTargetWidth,
            Height = 26
        },
        BaseScale = { X = 1280 / 960, Y = 720 / 544 }
    };
    root.SystemMenu.MenuEntriesSprites[#root.SystemMenu.MenuEntriesSprites + 1] = "SystemMenuEntry" .. i;
end

for i = 0, 7 do
    root.Sprites["SystemMenuEntryHighlighted" .. i] = {
        Sheet = "Data",
        Bounds = {
            X = 964,
            Y = 2 + i * 38,
            Width = root.SystemMenu.MenuEntriesTargetWidth,
            Height = 26
        },
        BaseScale = { X = 1280 / 960, Y = 720 / 544 }
    };
    root.SystemMenu.MenuEntriesHighlightedSprites[#root.SystemMenu.MenuEntriesHighlightedSprites + 1] = "SystemMenuEntryHighlighted" .. i;
end

root.Sprites["SystemMenuBackground"] = {
    Sheet = "Menu",
    Bounds = { X = 0, Y = 0, Width = 960, Height = 544 },
    BaseScale = { X = 1280 / 960, Y = 720 / 544 }
};

root.Sprites["SystemMenuButtonBackground"] = {
    Sheet = "Menu",
    Bounds = { X = root.SystemMenu.ButtonBackgroundSprStartX, Y = 975, Width = 0, Height = 30 },
    BaseScale = { X = 1280 / 960, Y = 720 / 544 }
};

root.Sprites["SystemMenuButtonPrompts"] = {
    Sheet = "Menu",
    Bounds = { X = 1231, Y = 528, Width = 400, Height = 21 },
    BaseScale = { X = 1280 / 960, Y = 720 / 544 }
};