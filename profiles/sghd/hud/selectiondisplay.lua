-- TODO: Copied from SGPS3 - check and fix

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
    SelectionHighlightSprite = "SelectionHighlight",
    SelectionMaxCount = 5,
    SelectionBackgroundX = 228,
    SelectionBackgroundY = {277, 277, 196, 129, 95},
    SelectionYSpacing = 81,
    PlainSelectionYSpacing = 10,
    FadeAnimationDurationInOut = 0.2
};

root.Sprites["SelectionBackground"] = {
    Sheet = "Data01",
    Bounds = { X = 1, Y = 535, Width = 829, Height = 69 }
};

root.Sprites["PlainSelectionFrameTopLeft"] = {
    Sheet = "Data01",
    Bounds = { X = 0, Y = 0, Width = 16, Height = 16 }
};

root.Sprites["PlainSelectionFrameTopSide"] = {
    Sheet = "Data01",
    Bounds = { X = 16, Y = 0, Width = 16, Height = 16 }
};

root.Sprites["PlainSelectionFrameTopRight"] = {
    Sheet = "Data01",
    Bounds = { X = 32, Y = 0, Width = 16, Height = 16 }
};

root.Sprites["PlainSelectionFrameLeftSide"] = {
    Sheet = "Data01",
    Bounds = { X = 0, Y = 16, Width = 16, Height = 16 }
};

root.Sprites["PlainSelectionFrameBottomLeft"] = {
    Sheet = "Data01",
    Bounds = { X = 0, Y = 32, Width = 16, Height = 16 }
};

root.Sprites["PlainSelectionFrameRightSide"] = {
    Sheet = "Data01",
    Bounds = { X = 32, Y = 16, Width = 16, Height = 16 }
};

root.Sprites["PlainSelectionFrameBottomRight"] = {
    Sheet = "Data01",
    Bounds = { X = 32, Y = 32, Width = 16, Height = 16 }
};

root.Sprites["PlainSelectionFrameBottomSide"] = {
    Sheet = "Data01",
    Bounds = { X = 16, Y = 32, Width = 16, Height = 16 }
};

root.Sprites["PlainSelectionFrameMiddle"] = {
    Sheet = "Data01",
    Bounds = { X = 16, Y = 16, Width = 16, Height = 16 }
};

root.Sprites["SelectionHighlight"] = {
    Sheet = "Data01",
    Bounds = { X = 65, Y = 33, Width = 94, Height = 30 }
};
