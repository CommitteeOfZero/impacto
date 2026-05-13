root.SaveIcon = {
    ForegroundAnimation = "SaveIcon",
    DefaultPosition = { X = 104, Y = 736 },
    BackgroundSprite = "SaveIconBg",
    BackgroundOffset = { X = 0, Y = 0 },
    BackgroundMaxAlpha = 0.0,
    FadeInDuration = 16/60,
    FadeOutDuration = 12/60,
    FullyVisibleSpriteIndex = 1
};

MakeAnimation({
    Name = "SaveIcon",
    Sheet = "Data",
    FirstFrameX = 127,
    FirstFrameY = 541,
    FrameWidth = 179,
    ColWidth = 179,
    FrameHeight = 62,
    RowHeight = 62,
    Frames = 2,
    Duration = 2*18/60, -- 18 frames per sprite
    Rows = 2,
    Columns = 1,
    PrimaryDirection = AnimationDirections.Up
});

root.Sprites["SaveIconBg"] = {
    Sheet = "Data",
    Bounds = { X = 127, Y = 479, Width = 179, Height = 62 }
};