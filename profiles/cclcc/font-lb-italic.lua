root.Fonts = {
    ["Default"] = {
        Type = FontType.LB,
        ForegroundSheet = "FontLBForeground",
        OutlineSheet = "FontLBOutline",
        Columns = 64,
        Rows = 125,
        AdvanceWidths = "games/cclcc/font-lb-italic/widths.bin",
        AdvanceWidthsEmWidth = 48,
        ForegroundOffset = { X = -13, Y = -13 },
        OutlineOffset = { X = -13, Y = -13 },
        BitmapEmWidth = 48,
        BitmapEmHeight = 48,
        LineSpacing = 14,
    }
};

root.SpriteSheets["Font"] = nil;

root.SpriteSheets["FontLBForeground"] = {
    Path = "games/cclcc/font-lb-italic/foreground.png",
    DesignWidth = 4096,
    DesignHeight = 8000
};

root.SpriteSheets["FontLBOutline"] = {
    Path = "games/cclcc/font-lb-italic/outline.png",
    DesignWidth = 4096,
    DesignHeight = 8000
};
