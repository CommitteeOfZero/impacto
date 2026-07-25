root.Fonts = {
    ["Default"] = {
        Type = FontType.LanguageBarrier,
        ForegroundSheet = "FontLBForeground",
        OutlineSheet = "FontLBOutline",
        ForegroundGridSize = { X = 64, Y = 125 },
        AdvanceWidths = "games/cc/font-lb/widths.bin",
        AdvanceWidthsEmWidth = 48,
        OutlineOffset = { X = -4, Y = -4 },
        ForegroundOpacityCurve = OpacityCurve.Smoothstep,
        OutlineOpacityCurve = OpacityCurve.Smoothstep,
    }
};

root.SpriteSheets["Font"] = nil;

root.SpriteSheets["FontLBForeground"] = {
    Path = "games/cc/font-lb/foreground.png",
    DesignWidth = 3072,
    DesignHeight = 6000
};

root.SpriteSheets["FontLBOutline"] = {
    Path = "games/cc/font-lb/outline.png",
    DesignWidth = 3648,
    DesignHeight = 7128
};