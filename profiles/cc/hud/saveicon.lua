root.SaveIcon = {
    ForegroundAnimation = "SaveIcon",
    DefaultPosition = { X = 92, Y = 713 },
    BackgroundSprite = "SaveIconBg",
    BackgroundOffset = { X = 0, Y = 0 },
    BackgroundMaxAlpha = 0.0,
    FadeInDuration = 0.5,
    FadeOutDuration = 0.25
};

MakeAnimation({
    Name = "SaveIcon",
    Sheet = "Data",
    FirstFrameX = 115,
    FirstFrameY = 456,
    FrameWidth = 258,
    ColWidth = 258,
    FrameHeight = 178,
    RowHeight = 178,
    Frames = 2,
    Duration = 0.4,
    Rows = 2,
    Columns = 1,
    PrimaryDirection = AnimationDirections.Down
});

root.Sprites["SaveIconBg"] = {
    Sheet = "Data",
    Bounds = { X = 630, Y = 479, Width = 179, Height = 62 }
};