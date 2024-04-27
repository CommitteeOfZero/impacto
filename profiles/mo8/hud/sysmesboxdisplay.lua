local sheet = "Data";
local name = "SysMesBox";

root.SysMesBoxDisplay = {
    Type = SysMesBoxType.CHLCC,
    DrawType = DrawComponentType.SystemMessage,
    BoxX = 383,
    BoxY = 366,
    TextFontSize = 42,
    TextMiddleY = 412,
    TextX = 942,
    TextLineHeight = 34,
    TextMarginY = 14,
    ChoicePadding = 60,
    ChoiceY = 647,
    ChoiceXBase = 1211,
    MinMaxMesWidth = 294,
    MinHighlightWidth = 48,
    HighlightBaseWidth = 144,
    HighlightYOffset = 12,
    HighlightXOffset = 0,
    HighlightXBase = 658,
    HighlightXStep = 132,
    HighlightRightPartSpriteWidth = 24,
    AnimationSpeed = 55,
    FadeInDuration = 0.33,
    FadeOutDuration = 0.25
};

root.Sprites[name .. "Box"] = {
    Sheet = sheet,
    Bounds = {
        X = 0,
        Y = 0,
        Width = 1153,
        Height = 350
    }
};
root.SysMesBoxDisplay.Box = name .. "Box";

root.Sprites[name .. "BoxDecoration"] = {
    Sheet = sheet,
    Bounds = {
        X = 1383,
        Y = 117,
        Width = 592,
        Height = 8
    }
};
root.SysMesBoxDisplay.BoxDecoration = name .. "BoxDecoration";

root.Sprites[name .. "SelectionLeftPart"] = {
    Sheet = sheet,
    Bounds = {
        X = 820,
        Y = 666,
        Width = 91,
        Height = 32
    }
};
root.SysMesBoxDisplay.SelectionLeftPart = name .. "SelectionLeftPart";

root.Sprites[name .. "SelectionRightPart"] = {
    Sheet = sheet,
    Bounds = {
        X = 820,
        Y = 666,
        Width = 91,
        Height = 32
    }
};
root.SysMesBoxDisplay.SelectionRightPart = name .. "SelectionRightPart";

root.Sprites[name .. "SelectionMiddlePart"] = {
    Sheet = sheet,
    Bounds = {
        X = 820,
        Y = 666,
        Width = 91,
        Height = 32
    }
};
root.SysMesBoxDisplay.SelectionMiddlePart = name .. "SelectionMiddlePart";