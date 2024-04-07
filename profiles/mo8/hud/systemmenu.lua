root.SystemMenu = {
    Type = SystemMenuType.MO8,
    DrawType = DrawComponentType.SystemMenu,
    SystemMenuBackgroundSprite = "SystemMenuBackground",
    SystemMenuX = 1496,
    SystemMenuY = 54,
    MenuEntriesSprites = {},
    MenuEntriesHighlightedSprites = {},
    MenuEntriesLockedSprites = {},
    MenuEntriesNum = 8,
    MenuEntriesHNum = 8,
    MenuEntriesLNum = 8,
    MenuEntriesX = 1512,
    MenuEntriesXOffset = 100,
    MenuEntriesFirstY = 84,
    MenuEntriesYPadding = 72,
    MenuEntriesTargetWidth = 400,
    ExitMenuButtonId = 7,
    FadeInDuration = 0.2,
    FadeOutDuration = 0.2
};

root.Sprites["SystemMenuEntryQuickSaveHighlighted"] = {
    Sheet = "MenuChip",
    Bounds = { X = 840, Y = 0, Width = root.SystemMenu.MenuEntriesTargetWidth, Height = 72 }
};

root.Sprites["SystemMenuEntryQuickSaveLocked"] = {
    Sheet = "MenuChip",
    Bounds = { X = 840, Y = 80, Width = root.SystemMenu.MenuEntriesTargetWidth, Height = 72 }
};

root.Sprites["SystemMenuEntryQuickSave"] = {
    Sheet = "MenuChip",
    Bounds = { X = 840, Y = 160, Width = root.SystemMenu.MenuEntriesTargetWidth, Height = 72 }
};

root.Sprites["SystemMenuEntryQuickLoadHighlighted"] = {
    Sheet = "MenuChip",
    Bounds = { X = 816, Y = 720, Width = root.SystemMenu.MenuEntriesTargetWidth, Height = 72 }
};

root.Sprites["SystemMenuEntryQuickLoadLocked"] = {
    Sheet = "MenuChip",
    Bounds = { X = 816, Y = 800, Width = root.SystemMenu.MenuEntriesTargetWidth, Height = 72 }
};

root.Sprites["SystemMenuEntryQuickLoad"] = {
    Sheet = "MenuChip",
    Bounds = { X = 816, Y = 880, Width = root.SystemMenu.MenuEntriesTargetWidth, Height = 72 }
};

root.Sprites["SystemMenuEntrySaveHighlighted"] = {
    Sheet = "MenuChip",
    Bounds = { X = 432, Y = 480, Width = root.SystemMenu.MenuEntriesTargetWidth, Height = 72 }
};

root.Sprites["SystemMenuEntrySaveLocked"] = {
    Sheet = "MenuChip",
    Bounds = { X = 432, Y = 560, Width = root.SystemMenu.MenuEntriesTargetWidth, Height = 72 }
};

root.Sprites["SystemMenuEntrySave"] = {
    Sheet = "MenuChip",
    Bounds = { X = 816, Y = 640, Width = root.SystemMenu.MenuEntriesTargetWidth, Height = 72 }
};

root.Sprites["SystemMenuEntryLoadHighlighted"] = {
    Sheet = "MenuChip",
    Bounds = { X = 840, Y = 240, Width = root.SystemMenu.MenuEntriesTargetWidth, Height = 72 }
};

root.Sprites["SystemMenuEntryLoadLocked"] = {
    Sheet = "MenuChip",
    Bounds = { X = 432, Y = 320, Width = root.SystemMenu.MenuEntriesTargetWidth, Height = 72 }
};

root.Sprites["SystemMenuEntryLoad"] = {
    Sheet = "MenuChip",
    Bounds = { X = 432, Y = 400, Width = root.SystemMenu.MenuEntriesTargetWidth, Height = 72 }
};

root.Sprites["SystemMenuEntryOptionsHighlighted"] = {
    Sheet = "MenuChip",
    Bounds = { X = 432, Y = 0, Width = root.SystemMenu.MenuEntriesTargetWidth, Height = 72 }
};

root.Sprites["SystemMenuEntryOptionsLocked"] = {
    Sheet = "MenuChip",
    Bounds = { X = 432, Y = 80, Width = root.SystemMenu.MenuEntriesTargetWidth, Height = 72 }
};

root.Sprites["SystemMenuEntryOptions"] = {
    Sheet = "MenuChip",
    Bounds = { X = 432, Y = 160, Width = root.SystemMenu.MenuEntriesTargetWidth, Height = 72 }
};

root.Sprites["SystemMenuEntryManualHighlighted"] = {
    Sheet = "MenuChip",
    Bounds = { X = 432, Y = 240, Width = root.SystemMenu.MenuEntriesTargetWidth, Height = 72 }
};

root.Sprites["SystemMenuEntryManualLocked"] = {
    Sheet = "MenuChip",
    Bounds = { X = 408, Y = 824, Width = root.SystemMenu.MenuEntriesTargetWidth, Height = 72 }
};

root.Sprites["SystemMenuEntryManual"] = {
    Sheet = "MenuChip",
    Bounds = { X = 408, Y = 904, Width = root.SystemMenu.MenuEntriesTargetWidth, Height = 72 }
};

root.Sprites["SystemMenuEntryTitleHighlighted"] = {
    Sheet = "MenuChip",
    Bounds = { X = 0, Y = 904, Width = root.SystemMenu.MenuEntriesTargetWidth, Height = 72 }
};

root.Sprites["SystemMenuEntryTitleLocked"] = {
    Sheet = "MenuChip",
    Bounds = { X = 408, Y = 744, Width = root.SystemMenu.MenuEntriesTargetWidth, Height = 72 }
};

root.Sprites["SystemMenuEntryTitle"] = {
    Sheet = "MenuChip",
    Bounds = { X = 408, Y = 664, Width = root.SystemMenu.MenuEntriesTargetWidth, Height = 72 }
};

root.Sprites["SystemMenuEntryCloseHighlighted"] = {
    Sheet = "MenuChip",
    Bounds = { X = 0, Y = 664, Width = root.SystemMenu.MenuEntriesTargetWidth, Height = 72 }
};

root.Sprites["SystemMenuEntryCloseLocked"] = {
    Sheet = "MenuChip",
    Bounds = { X = 0, Y = 824, Width = root.SystemMenu.MenuEntriesTargetWidth, Height = 72 }
};

root.Sprites["SystemMenuEntryClose"] = {
    Sheet = "MenuChip",
    Bounds = { X = 0, Y = 744, Width = root.SystemMenu.MenuEntriesTargetWidth, Height = 72 }
};

root.SystemMenu.MenuEntriesSprites[#root.SystemMenu.MenuEntriesSprites + 1] = "SystemMenuEntryQuickSave";
root.SystemMenu.MenuEntriesSprites[#root.SystemMenu.MenuEntriesSprites + 1] = "SystemMenuEntryQuickLoad";
root.SystemMenu.MenuEntriesSprites[#root.SystemMenu.MenuEntriesSprites + 1] = "SystemMenuEntrySave";
root.SystemMenu.MenuEntriesSprites[#root.SystemMenu.MenuEntriesSprites + 1] = "SystemMenuEntryLoad";
root.SystemMenu.MenuEntriesSprites[#root.SystemMenu.MenuEntriesSprites + 1] = "SystemMenuEntryOptions";
root.SystemMenu.MenuEntriesSprites[#root.SystemMenu.MenuEntriesSprites + 1] = "SystemMenuEntryManual";
root.SystemMenu.MenuEntriesSprites[#root.SystemMenu.MenuEntriesSprites + 1] = "SystemMenuEntryTitle";
root.SystemMenu.MenuEntriesSprites[#root.SystemMenu.MenuEntriesSprites + 1] = "SystemMenuEntryClose";

root.SystemMenu.MenuEntriesHighlightedSprites[#root.SystemMenu.MenuEntriesHighlightedSprites + 1] = "SystemMenuEntryQuickSaveHighlighted";
root.SystemMenu.MenuEntriesHighlightedSprites[#root.SystemMenu.MenuEntriesHighlightedSprites + 1] = "SystemMenuEntryQuickLoadHighlighted";
root.SystemMenu.MenuEntriesHighlightedSprites[#root.SystemMenu.MenuEntriesHighlightedSprites + 1] = "SystemMenuEntrySaveHighlighted";
root.SystemMenu.MenuEntriesHighlightedSprites[#root.SystemMenu.MenuEntriesHighlightedSprites + 1] = "SystemMenuEntryLoadHighlighted";
root.SystemMenu.MenuEntriesHighlightedSprites[#root.SystemMenu.MenuEntriesHighlightedSprites + 1] = "SystemMenuEntryOptionsHighlighted";
root.SystemMenu.MenuEntriesHighlightedSprites[#root.SystemMenu.MenuEntriesHighlightedSprites + 1] = "SystemMenuEntryManualHighlighted";
root.SystemMenu.MenuEntriesHighlightedSprites[#root.SystemMenu.MenuEntriesHighlightedSprites + 1] = "SystemMenuEntryTitleHighlighted";
root.SystemMenu.MenuEntriesHighlightedSprites[#root.SystemMenu.MenuEntriesHighlightedSprites + 1] = "SystemMenuEntryCloseHighlighted";

root.SystemMenu.MenuEntriesLockedSprites[#root.SystemMenu.MenuEntriesLockedSprites + 1] = "SystemMenuEntryQuickSaveLocked";
root.SystemMenu.MenuEntriesLockedSprites[#root.SystemMenu.MenuEntriesLockedSprites + 1] = "SystemMenuEntryQuickLoadLocked";
root.SystemMenu.MenuEntriesLockedSprites[#root.SystemMenu.MenuEntriesLockedSprites + 1] = "SystemMenuEntrySaveLocked";
root.SystemMenu.MenuEntriesLockedSprites[#root.SystemMenu.MenuEntriesLockedSprites + 1] = "SystemMenuEntryLoadLocked";
root.SystemMenu.MenuEntriesLockedSprites[#root.SystemMenu.MenuEntriesLockedSprites + 1] = "SystemMenuEntryOptionsLocked";
root.SystemMenu.MenuEntriesLockedSprites[#root.SystemMenu.MenuEntriesLockedSprites + 1] = "SystemMenuEntryManualLocked";
root.SystemMenu.MenuEntriesLockedSprites[#root.SystemMenu.MenuEntriesLockedSprites + 1] = "SystemMenuEntryTitleLocked";
root.SystemMenu.MenuEntriesLockedSprites[#root.SystemMenu.MenuEntriesLockedSprites + 1] = "SystemMenuEntryCloseLocked";

root.Sprites["SystemMenuBackground"] = {
    Sheet = "MenuChip",
    Bounds = { X = 0, Y = 0, Width = 424, Height = 656 }
};
