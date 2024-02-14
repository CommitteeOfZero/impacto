root.Fonts = {
    "Default" = {
        Type = FontType.LB,
        ForegroundSheet = "FontLBForeground",
        OutlineSheet = "FontLBOutline",
        Columns = 64,
        Rows = 37,
        Widths = "games/chlcc/font-lb/widths.bin",
        DesignColWidth = 48,
        OutlineOffset = { X = -4, Y = -4 },
        LineSpacing = 14,
    }
};

root.SpriteSheets["Font"] = nil;

root.SpriteSheets["FontLBForeground"] = {
    Path = "games/chlcc/font-lb/foreground.png",
    DesignWidth = 3072,
    DesignHeight = 1776
};

root.SpriteSheets["FontLBOutline"] = {
    Path = "games/chlcc/font-lb/outline.png",
    DesignWidth = 3648,
    DesignHeight = 2112
};