root.SelectionDisplay = {
    DrawType = DrawComponentType.Text,
    SelectionBackgroundSprite = "SelectionBackground",
    PlainSelectionFrameTopLeftSprite = "PlainSelectionFrameTopLeft",
    PlainSelectionFrameTopSideSprite = "PlainSelectionFrameTopSide",
    PlainSelectionFrameTopRightSprite = "PlainSelectionFrameTopRight",
    PlainSelectionFrameLeftSideSprite = "PlainSelectionFrameLeftSide",
    PlainSelectionFrameBottomLeftSprite = "PlainSelectionFrameBottomLeft",
    PlainSelectionFrameRightSideSprite = "PlainSelectionFrameRightSide",
    PlainSelectionFrameBottomRightSprite = "PlainSelectionFrameBottomRight",
    PlainSelectionFrameBottomSideSprite = "PlainSelectionFrameBottomSide",
    PlainSelectionFrameMiddleSprite = "PlainSelectionFrameMiddle",
    SelectionFocusedSprite = "SelectionHighlight",
    HighlightTextOnly = false,
    SelectionMaxCount = 5,
    SelectionBackgroundX = 470,
    SelectionBackgroundY = {332, 332, 210, 110, 59},
    SelectionYSpacing = 93,
    PlainSelectionYSpacing = 10,
    FadeAnimationDurationInOut = 0.2
};

root.Sprites["SelectionBackground"] = {
    Sheet = "Data",
    Bounds = { X = 0, Y = 358, Width = 980, Height = 80 }
};

root.Sprites["SelectionHighlight"] = {
    Sheet = "Data",
    Bounds = { X = 0, Y = 543, Width = 980, Height = 80 }
};

root.Sprites["PlainSelectionFrameTopLeft"] = {
    Sheet = "Data",
    Bounds = { X = 0, Y = 0, Width = 24, Height = 24 }
};

root.Sprites["PlainSelectionFrameTopSide"] = {
    Sheet = "Data",
    Bounds = { X = 24, Y = 0, Width = 24, Height = 24 }
};

root.Sprites["PlainSelectionFrameTopRight"] = {
    Sheet = "Data",
    Bounds = { X = 48, Y = 0, Width = 24, Height = 24 }
};

root.Sprites["PlainSelectionFrameLeftSide"] = {
    Sheet = "Data",
    Bounds = { X = 0, Y = 24, Width = 24, Height = 24 }
};

root.Sprites["PlainSelectionFrameBottomLeft"] = {
    Sheet = "Data",
    Bounds = { X = 0, Y = 48, Width = 24, Height = 24 }
};

root.Sprites["PlainSelectionFrameRightSide"] = {
    Sheet = "Data",
    Bounds = { X = 48, Y = 24, Width = 24, Height = 24 }
};

root.Sprites["PlainSelectionFrameBottomRight"] = {
    Sheet = "Data",
    Bounds = { X = 48, Y = 48, Width = 24, Height = 24 }
};

root.Sprites["PlainSelectionFrameBottomSide"] = {
    Sheet = "Data",
    Bounds = { X = 24, Y = 48, Width = 24, Height = 24 }
};

root.Sprites["PlainSelectionFrameMiddle"] = {
    Sheet = "Data",
    Bounds = { X = 24, Y = 24, Width = 24, Height = 24 }
};
