root.Fonts = {
    "Default": {
        Type: FontType.LB,
        ForegroundSheet: "FontLBForeground",
        OutlineSheet: "FontLBOutline",
        Columns: 64,
        Rows: 50,
        Widths: "games/rne/font-lb/widths.bin",
        DesignColWidth: 48,
        OutlineOffset: { X: -4, Y: -4 },
        LineSpacing: 14,
    }
};

delete root.SpriteSheets["Font"];

root.SpriteSheets["FontLBForeground"] = {
    Path: "games/rne/font-lb/foreground.png",
    DesignWidth: 3072,
    DesignHeight: 2400
};

root.SpriteSheets["FontLBOutline"] = {
    Path: "games/rne/font-lb/outline.png",
    DesignWidth: 3648,
    DesignHeight: 2852
};