root.SaveIcon = {
    ForegroundAnimation: "SaveIcon",
    DefaultPosition: { X: 1072, Y: 36 },
    BackgroundSprite: "SaveIconBg",
    BackgroundOffset: { X: -7, Y: -4 },
    BackgroundMaxAlpha: 0.5,
    FadeInDuration: 0.25,
    FadeOutDuration: 0.25
};

MakeAnimation({
    Name: "SaveIcon",
    Sheet: "Data",
    FirstFrameX: 257,
    FirstFrameY: 121,
    FrameWidth: 96,
    ColWidth: 96,
    FrameHeight: 22,
    RowHeight: 24,
    Frames: 4,
    Duration: 0.4,
    Rows: 4,
    Columns: 1,
    PrimaryDirection: AnimationDirections.Down
});

root.Sprites["SaveIconBg"] = {
    Sheet: "Data",
    Bounds: { X: 1439, Y: 1, Width: 84, Height: 84 }
};