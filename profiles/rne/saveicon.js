root.SaveIcon = {
    ForegroundAnimation: "SaveIcon",
    DefaultPosition: { X: 1153, Y: 23 },
    BackgroundSprite: "SaveIconBg",
    BackgroundOffset: { X: -7, Y: -4 },
    BackgroundMaxAlpha: 0.5
};

MakeAnimation({
    Name: "SaveIcon",
    Sheet: "Data",
    FirstFrameX: 1977,
    FirstFrameY: 1,
    FrameWidth: 70,
    ColWidth: 70,
    FrameHeight: 70,
    RowHeight: 72,
    Frames: 8,
    Duration: 0.5,
    Rows: 8,
    Columns: 1,
    PrimaryDirection: AnimationDirections.Down
});

root.Sprites["SaveIconBg"] = {
    Sheet: "Data",
    Bounds: { X: 1439, Y: 1, Width: 84, Height: 84 }
};