root.SaveIcon = {
    ForegroundAnimation = "SaveIcon",
    DefaultPosition = { X = 1729.5, Y = 34.5 },
    BackgroundSprite = "SaveIconBg",
    BackgroundOffset = { X = -10.5, Y = -6 },
    BackgroundMaxAlpha = 0.5,
    FadeInDuration = 0.5,
    FadeOutDuration = 0.25
};

MakeAnimation({
    Name = "SaveIcon",
    Sheet = "Data",
    FirstFrameX = 2965.5,
    FirstFrameY = 1.5,
    FrameWidth = 105,
    ColWidth = 105,
    FrameHeight = 105,
    RowHeight = 108,
    Frames = 8,
    Duration = 0.4,
    Rows = 8,
    Columns = 1,
    PrimaryDirection = AnimationDirections.Down
});

root.Sprites["SaveIconBg"] = {
    Sheet = "Data",
    Bounds = { X = 2158.5, Y = 1.5, Width = 126, Height = 126 }
};