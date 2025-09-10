root.Fonts = {
    ["Default"] = {
        Type = FontType.LB,
        ForegroundSheet = "FontLBForeground",
        OutlineSheet = "FontLBOutline",
        Columns = 64,
        Rows = 39,
        AdvanceWidths = "games/chlcc/font-lb/widths.bin",
        AdvanceWidthsEmWidth = 60,
        OutlineOffset = { X = 1, Y = 1 },
        LineSpacing = 0,
    }
};

root.SpriteSheets["Font"] = nil;

root.SpriteSheets["FontLBForeground"] = {
    Path = "games/chlcc/font-lb/foreground.png",
    DesignWidth = 4096,
    DesignHeight = 2496
};

root.SpriteSheets["FontLBOutline"] = {
    Path = "games/chlcc/font-lb/outline.png",
    DesignWidth = 4096,
    DesignHeight = 2496
};