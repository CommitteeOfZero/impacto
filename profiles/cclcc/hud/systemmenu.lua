root.SystemMenu = {
    Type = SystemMenuType.CCLCC,
    DrawType = DrawComponentType.SystemMenu,
    FadeInDuration = 64 / 60,
    FadeOutDuration = 64 / 60,
    TitleFadeInDuration = 40 / 60,
    TitleFadeOutDuration = 28 / 60,

    MenuEntriesNum =9,
    MenuEntriesHNum = 9,
    ------
    --Not used since it isn't laid out in a way that would make it easier to use this
    MenuEntriesX = 173,
    MenuEntriesXOffset = 0,
    MenuEntriesFirstY = 445,
    MenuEntriesYPadding = 0,
    ------
    FocusTint = 0xff9cb6,
    MenuEntriesPositions = { 
        {X=41, Y=-4},
        {X=41, Y=87},
        {X=41, Y=119},
        {X=41, Y=284},
        {X=41, Y=365},
        {X=41, Y=458},
        {X=41, Y=574},
        {X=41, Y=649},
        {X=41, Y=725},
    },
    MenuEntriesSprites = {},
    MenuEntriesHighlightedSprites = {},
    MenuButtonGuide = "SystemMenuButtonGuide",
};

root.Sprites["SystemMenuBacklog"] = {
    Sheet = "MenuChip",
    Bounds = {X = 2752, Y = 0, Width = 420, Height = 202}
};
root.SystemMenu.MenuEntriesSprites[#root.SystemMenu.MenuEntriesSprites + 1] = "SystemMenuBacklog";

root.Sprites["SystemMenuQuickSave"] = {
    Sheet = "MenuChip",
    Bounds = {X = 2252, Y = 274, Width = 498, Height = 225}
};
root.SystemMenu.MenuEntriesSprites[#root.SystemMenu.MenuEntriesSprites + 1] = "SystemMenuQuickSave";

root.Sprites["SystemMenuQuickLoad"] = {
    Sheet = "MenuChip",
    Bounds = {X = 2252, Y = 1092, Width = 498, Height = 348}
};
root.SystemMenu.MenuEntriesSprites[#root.SystemMenu.MenuEntriesSprites + 1] = "SystemMenuQuickLoad";

root.Sprites["SystemMenuSave"] = {
    Sheet = "MenuChip",
    Bounds = {X = 2752, Y = 204, Width = 420, Height = 241}
};
root.SystemMenu.MenuEntriesSprites[#root.SystemMenu.MenuEntriesSprites + 1] = "SystemMenuSave";

root.Sprites["SystemMenuLoad"] = {
    Sheet = "MenuChip",
    Bounds = {X = 2252, Y = 0, Width = 498, Height = 305}
};
root.SystemMenu.MenuEntriesSprites[#root.SystemMenu.MenuEntriesSprites + 1] = "SystemMenuLoad";

root.Sprites["SystemMenuTips"] = {
    Sheet = "MenuChip",
    Bounds = {X = 2252, Y = 501, Width = 498, Height = 305}
};
root.SystemMenu.MenuEntriesSprites[#root.SystemMenu.MenuEntriesSprites + 1] = "SystemMenuTips";

root.Sprites["SystemMenuConfig"] = {
    Sheet = "MenuChip",
    Bounds = {X = 2752, Y = 447, Width = 420, Height = 253}
};
root.SystemMenu.MenuEntriesSprites[#root.SystemMenu.MenuEntriesSprites + 1] = "SystemMenuConfig";

root.Sprites["SystemMenuHelp"] = {
    Sheet = "MenuChip",
    Bounds = {X = 2752, Y = 702, Width = 420, Height = 259}
};
root.SystemMenu.MenuEntriesSprites[#root.SystemMenu.MenuEntriesSprites + 1] = "SystemMenuHelp";

root.Sprites["SystemMenuReturnTitle"] = {
    Sheet = "MenuChip",
    Bounds = {X = 2252, Y = 808, Width = 498, Height = 282}
};
root.SystemMenu.MenuEntriesSprites[#root.SystemMenu.MenuEntriesSprites + 1] = "SystemMenuReturnTitle";

for i=1, #root.SystemMenu.MenuEntriesSprites do
    local name = root.SystemMenu.MenuEntriesSprites[i] .. "Highlighted"
    root.Sprites[name] = {
        Sheet = "MenuChip",
        Bounds = {
            X = root.Sprites[root.SystemMenu.MenuEntriesSprites[i]].Bounds.X + 922,
            Y = root.Sprites[root.SystemMenu.MenuEntriesSprites[i]].Bounds.Y,
            Width = root.Sprites[root.SystemMenu.MenuEntriesSprites[i]].Bounds.Width,
            Height = root.Sprites[root.SystemMenu.MenuEntriesSprites[i]].Bounds.Height
        }
    }
    root.SystemMenu.MenuEntriesHighlightedSprites[#root.SystemMenu.MenuEntriesHighlightedSprites + 1] = name
end

root.Sprites["SystemMenuButtonGuide"] = {
    Sheet = "MenuChip",
    Bounds = {X = 0, Y = 1384, Width = 1920, Height = 59}
};