include('common/animation.js');

MakeAnimation({
    Name: "ResourceLoadingBg",
    Sheet: "Data",
    FirstFrameX: 555,
    FirstFrameY: 544,
    FrameWidth: 206,
    ColWidth: 206,
    FrameHeight: 58,
    RowHeight: 60,
    Frames: 8,
    Duration: 1,
    Rows: 8,
    Columns: 1,
    PrimaryDirection: AnimationDirections.Down
});

MakeAnimation({
    Name: "SaveLoadingBg",
    Sheet: "Data",
    FirstFrameX: 388,
    FirstFrameY: 0,
    FrameWidth: 277,
    ColWidth: 279,
    FrameHeight: 81,
    RowHeight: 83,
    Frames: 4,
    Duration: 1,
    Rows: 2,
    Columns: 2,
    PrimaryDirection: AnimationDirections.Down,
    SecondaryDirection: AnimationDirections.Right
});

MakeAnimation({
    Name: "LoadingDisc",
    Sheet: "Data",
    FirstFrameX: 172,
    FirstFrameY: 1,
    FrameWidth: 60,
    ColWidth: 62,
    FrameHeight: 60,
    RowHeight: 62,
    Frames: 3,
    Duration: 1,
    Rows: 1,
    Columns: 3,
    PrimaryDirection: AnimationDirections.Right
});
root.Animations["LoadingDisc"].Frames.push("LoadingDisc1");

MakeAnimation({
    Name: "LoadingText",
    Sheet: "Data",
    FirstFrameX: 173,
    FirstFrameY: 91,
    FrameWidth: 214,
    ColWidth: 214,
    FrameHeight: 21,
    RowHeight: 23,
    Frames: 5,
    Duration: 1,
    Rows: 5,
    Columns: 1,
    PrimaryDirection: AnimationDirections.Down
});