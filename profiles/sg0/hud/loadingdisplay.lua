root.LoadingDisplay = {
    ResourceLoadBgAnim = "ResourceLoadingBg",
    SaveLoadBgAnim = "SaveLoadingBg",
    LoadingIconAnim = "LoadingDisc",
    LoadingTextAnim = "LoadingText",
    ResourceBgPos = { X = 1074, Y = 611 },
    SaveBgPos = { X = 1019, Y = 598 },
    IconPos = { X = 986, Y = 608 },
    TextPos = { X = 1025, Y = 628 },
    FadeInDuration = 0.66,
    FadeOutDuration = 0.33
};

MakeAnimation({
    Name = "ResourceLoadingBg",
    Sheet = "Data",
    FirstFrameX = 555,
    FirstFrameY = 544,
    FrameWidth = 206,
    ColWidth = 206,
    FrameHeight = 58,
    RowHeight = 60,
    Frames = 8,
    Duration = 0.8,
    Rows = 8,
    Columns = 1,
    PrimaryDirection = AnimationDirections.Down
});

MakeAnimation({
    Name = "SaveLoadingBg",
    Sheet = "Data",
    FirstFrameX = 388,
    FirstFrameY = 0,
    FrameWidth = 277,
    ColWidth = 279,
    FrameHeight = 81,
    RowHeight = 83,
    Frames = 4,
    Duration = 0.8,
    Rows = 2,
    Columns = 2,
    PrimaryDirection = AnimationDirections.Down,
    SecondaryDirection = AnimationDirections.Right
});

MakeAnimation({
    Name = "LoadingDisc",
    Sheet = "Data",
    FirstFrameX = 172,
    FirstFrameY = 1,
    FrameWidth = 60,
    ColWidth = 62,
    FrameHeight = 60,
    RowHeight = 62,
    Frames = 3,
    Duration = 0.8,
    Rows = 1,
    Columns = 3,
    PrimaryDirection = AnimationDirections.Right
});
root.Animations["LoadingDisc"].Frames[#root.Animations["LoadingDisc"].Frames + 1] = "LoadingDisc1";

MakeAnimation({
    Name = "LoadingText",
    Sheet = "Data",
    FirstFrameX = 173,
    FirstFrameY = 91,
    FrameWidth = 214,
    ColWidth = 214,
    FrameHeight = 21,
    RowHeight = 23,
    Frames = 5,
    Duration = 0.8,
    Rows = 5,
    Columns = 1,
    PrimaryDirection = AnimationDirections.Down
});