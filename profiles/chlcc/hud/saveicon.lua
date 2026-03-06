root.SaveIcon = {
    DefaultPosition = { X = 1120, Y = 495 },
    SaveIconCurrentType = SaveIconType.CHLCC,
    SaveIconSprites = {},
    FadeInDuration = 15/60,
    FadeOutDuration = 15/60,
    ActiveAnimationDuration = 48/60
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

for i = 1, 3 do
    root.Sprites["SaveIcon" .. i - 1] = {
        Sheet = "Data",
        Bounds = {
            X = 372,
            Y = 51 + (i - 1) * 21,
            Width = 127,
            Height = 19
        }
    }
    root.SaveIcon.SaveIconSprites[i] = "SaveIcon" .. i - 1;
end