root.SaveIcon = {
    ForegroundAnimation = "SaveIcon",
    DefaultPosition = { X = 104, Y = 736 },
    BackgroundSprite = "SaveIconBg",
    BackgroundOffset = { X = 0, Y = 0 },
    BackgroundMaxAlpha = 0.0,
    FadeInDuration = 0.5,
    FadeOutDuration = 0.25
};

MakeAnimation({
    Name = "SaveIcon",
    Sheet = "Data",
    FirstFrameX = 127,
    FirstFrameY = 479,
    FrameWidth = 179,
    ColWidth = 179,
    FrameHeight = 62,
    RowHeight = 62,
    Frames = 2,
    Duration = 0.4,
    Rows = 2,
    Columns = 1,
    PrimaryDirection = AnimationDirections.Down
});

root.Sprites["SaveIconBg"] = {
    Sheet = "Data",
    Bounds = { X = 127, Y = 479, Width = 179, Height = 62 }
};