root.SaveIcon = {
    ForegroundAnimation = "SaveIcon",
    DefaultPosition = { X = 1672, Y = 32 },
    BackgroundSprite = "SaveIconBg",
    BackgroundOffset = { X = -10.5, Y = -6 },
    BackgroundMaxAlpha = 0.5,
    FadeInDuration = 0.5,
    FadeOutDuration = 0.25
};

MakeAnimation({
    Name = "SaveIcon",
    Sheet = "Data",
    FirstFrameX = 312,
    FirstFrameY = 634,
    FrameWidth = 216,
    ColWidth = 216,
    FrameHeight = 56,
    RowHeight = 64,
    Frames = 4,
    Duration = 0.7,
    Rows = 4,
    Columns = 1,
    PrimaryDirection = AnimationDirections.Down
});

root.Sprites["SaveIconBg"] = {
    Sheet = "Data",
    Bounds = { X = 2158.5, Y = 1.5, Width = 126, Height = 126 }
};