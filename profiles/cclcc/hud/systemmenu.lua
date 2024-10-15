root.SystemMenu = {
    Type = SystemMenuType.CCLCC,
    DrawType = DrawComponentType.SystemMenu,

    FadeInDuration = 40 / 60,
    FadeOutDuration = 40 / 60,
    MoveInDuration = 40 / 60,
    MoveOutDuration = 28 / 60,

    ItemsFadeInDuration = 20/60,
    ItemsFadeOutDuration = 20/60,

    Seed = 0,

    MenuEntriesNum = 9,
    MenuEntriesHNum = 9,

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

    MenuEntriesButtonBounds = { 
        {X=120, Y= 74, Width=228, Height=60},
        {X=120, Y=172, Width=260, Height=60},
        {X=120, Y=268, Width=260, Height=60},
        {X=120, Y=362, Width=128, Height=60},
        {X=120, Y=454, Width=128, Height=60},
        {X=120, Y=552, Width=228, Height=60},
        {X=120, Y=650, Width=186, Height=60},
        {X=120, Y=742, Width=128, Height=60},
        {X=120, Y=838, Width=316, Height=60},
    },
    MenuEntriesSprites = {},
    MenuEntriesHighlightedSprites = {},

    MenuButtonGuide = "SystemMenuButtonGuide",
    SystemMenuBG = "SystemMenuBG",
    SystemMenuFrame = "SystemMenuFrame"
};

root.Sprites["SystemMenuBG"] = {
    Sheet = "MenuBG",
    Bounds = {X = 0, Y = 0, Width = 3000, Height = 1500}
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

root.Sprites["SystemMenuFrame"] = {
    Sheet = "MenuChip",
    Bounds = {X = 0, Y = 0, Width = 2252, Height = 1383}
};