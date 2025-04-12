root.HelpMenu = {
    DrawType = DrawComponentType.SystemMenu,
    Type = HelpMenuType.CCLCC,
    FadeInDuration = 32/60,
    FadeOutDuration = 32/60,
    NextPageInDuration = 0.4,
    NextPageOutDuration = 0.4,
    ManualPages = {},
    HelpMaskSprite = "HelpMask",
}

local numberOfPages = root.Language == "English" and 1 or 16

for i = 0, numberOfPages do
    root.Sprites["ManualPage" .. i] = {
        Sheet = "ManualSheet" .. i,
        Bounds = { X = 0, Y = 0, Width = 1920, Height = 1080 }
    };
    root.HelpMenu.ManualPages[#root.HelpMenu.ManualPages + 1] = "ManualPage" .. i;
end

root.Sprites["HelpMask"] = {
    Sheet = "MenuChip",
    Bounds = { X = 154, Y = 140, Width = 1900, Height = 1061 },
};