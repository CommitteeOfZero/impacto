root.TipsMenu = {
    Type: TipsMenuType.CCLCC,
    DrawType: DrawComponentType.SystemMenu,
    BackgroundSprite: "TipsMenuBackground",
    FadeInDuration: 0.2,
    FadeOutDuration: 0.2,
    TipsBookLayerSprite: "TipsBookLayer",
    TipsGuideSprite: "TipsGuide",
    TipsGuideX: 0,
    TipsGuideY: 990,
    TipsMaskSprite: "TipsMask"
};

root.Sprites["TipsMenuBackground"] = {
    Sheet: "Title",
    Bounds: { X: 0, Y: 1080, Width: 1920, Height: 1080 }
};

root.Sprites["TipsBookLayer"] = {
    Sheet: "Tips",
    Bounds: { X: 0, Y: 0, Width: 1920, Height: 1080 }
};

root.Sprites["TipsGuide"] = {
    Sheet: "Tips",
    Bounds: { X: 0, Y: 1152, Width: 1925, Height: 55 }
};

root.Sprites["TipsMask"] = {
    Sheet: "MenuChip",
    Bounds: { X: 154, Y: 140, Width: 1900, Height: 1061 },
};