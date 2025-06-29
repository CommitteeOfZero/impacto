root.SaveIcon = {
    ForegroundAnimation = "SaveIcon",
    DefaultPosition = { X = 1120, Y = 495 },
    BackgroundSprite = "SaveIconBg",
    BackgroundOffset = { X = 0, Y = 0 },
    BackgroundMaxAlpha = 0.0,
    FadeInDuration = 0.5,
    FadeOutDuration = 0.25
};

MakeAnimation({
    Name = "SaveIcon",
    Sheet = "Data",
    FirstFrameX = 372,
    FirstFrameY = 51,
    FrameWidth = 127,
    ColWidth = 127,
    FrameHeight = 19,
    RowHeight = 21,
    Frames = 3,
    Duration = 1.0,
    Rows = 3,
    Columns = 1,
    PrimaryDirection = AnimationDirections.Down
});

root.Sprites["SaveIconBg"] = {
    Sheet = "Data",
    Bounds = { X = 0, Y = 0, Width = 0, Height = 0 }
};