root.AchievementNotificationPath = "games/common/achievementnotification.png";

root.SpriteSheets["AchievementNotificationFontLBForeground"] = {
    Path = "games/chlcc/font-lb/foreground.png",
    DesignWidth = 4096,
    DesignHeight = 2496
};

root.SpriteSheets["AchievementNotificationFontLBOutline"] = {
    Path = "games/chlcc/font-lb/outline.png",
    DesignWidth = 4096,
    DesignHeight = 2496
};

root.Fonts["AchievementNotificationFont"] = {
    Type = FontType.LB,
    ForegroundSheet = "AchievementNotificationFontLBForeground",
    OutlineSheet = "AchievementNotificationFontLBOutline",
    Columns = 64,
    Rows = 39,
    AdvanceWidths = "games/chlcc/font-lb/widths.bin",
    AdvanceWidthsEmWidth = 60,
    ForegroundOffset = { X = -7, Y = 0 },
    OutlineOffset = { X = -6, Y = 1 },
    LineSpacing = 0,
};
