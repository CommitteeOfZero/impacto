local sheet = "Data";
local name = "SysMesBox";

root.SysMesBoxDisplay = {
    Type = SysMesBoxType.Dash,
    DrawType = DrawComponentType.SystemMessage,
    LinePositionXFirst = 2433,
    LinePositionX = 2478,
    LinePositionMultiplier = 30,
    LineWidthFirst = 171,
    LineWidthBase = 121.5,
    LineWidthMultiplier = 60,
    Line1SpriteY = 160.5,
    Line2SpriteY = 175.5,
    LineSpriteHeight = 12,
    LineDisplayXBase = 961.5,
    Line1DisplayY = 540,
    Line2DisplayY = 528,
    BoxDisplayStartCount = 9,
    BoxHeightBase = 168,
    BoxHeightMultiplier = 21,
    BoxWidth = 900,
    BoxTextFontSize = 42,
    BoxTopYBase = 540,
    BoxDisplayX = 510,
    MessageLabelSpriteXBase = 2368,
    MessageLabelSpriteY = 196.5,
    MessageLabelSpriteHeight = 64,
	MessageLabelSpriteMultiplier = 18.4,
    ButtonYesDisplayXBase = 1350.5,
	ButtonRightDisplayXBase = 1409,
	ButtonSpriteY = 196.5,
	ButtonWidth = 157.5, 
	ButtonSelectedSpriteY = 238.5,
	ButtonYOffset = 50,
    ButtonYWidthBase = 58.5,
    ButtonRightWidthBase = -112.5,
	TextDecorationStart = 56,
	TextDecorationTopYOffset = 36,
	TextDecorationBottomYOffset = 32,
	TextFontSize = 39,
	TextMiddleY = 627,
	TextX = 555,
	TextLineHeight = 42,
	TextMarginY = 21,
	SpriteMargin = 3,
    AnimationSpeed = 55,
    FadeInDuration = 0.33,
    FadeOutDuration = 0.25
};

root.Sprites[name .. "BoxDecorationTop"] = {
    Sheet = sheet,
    Bounds = {
        X = 2049,
        Y = 400,
        Width = 912,
        Height = 61
    }
};
root.SysMesBoxDisplay.BoxDecorationTop = name .. "BoxDecorationTop";

root.Sprites[name .. "BoxDecorationBottom"] = {
    Sheet = sheet,
    Bounds = {
        X = 2049,
        Y = 536,
        Width = 912,
        Height = 12
    }
};
root.SysMesBoxDisplay.BoxDecorationBottom = name .. "BoxDecorationBottom";

root.Sprites[name .. "TextDecoration"] = {
    Sheet = sheet,
    Bounds = {
        X = 2074.5,
        Y = 127*1.5,
        Width = 888,
        Height = 2*1.5
    }
};
root.SysMesBoxDisplay.TextDecoration = name .. "TextDecoration";

root.Sprites[name .. "MessageLabel"] = {
    Sheet = sheet,
    Bounds = {
        X = 2074.5,
        Y = 190.5,
        Width = 888,
        Height = 3
    }
};
root.SysMesBoxDisplay.MessageLabel = name .. "MessageLabel";

root.Sprites[name .. "Line1"] = {
    Sheet = sheet,
    Bounds = {
        X = 1622*1.5,
        Y = 107*1.5,
        Width = 114*1.5,
        Height = 8*1.5
    }
};
root.SysMesBoxDisplay.Line1 = name .. "Line1";

root.Sprites[name .. "Line2"] = {
    Sheet = sheet,
    Bounds = {
        X = 1622*1.5,
        Y = 117*1.5,
        Width = 114*1.5,
        Height = 8*1.5
    }
};
root.SysMesBoxDisplay.Line2 = name .. "Line2";

root.Sprites[name .. "ButtonYes"] = {
    Sheet = sheet,
    Bounds = {
        X = 1656 * 1.5,
        Y = 131 * 1.5,
        Width = 105 * 1.5,
        Height = 26 * 1.5
    }
};
root.SysMesBoxDisplay.ButtonYes = name .. "ButtonYes";

root.Sprites[name .. "ButtonNo"] = {
    Sheet = sheet,
    Bounds = {
        X = 1763 * 1.5,
        Y = 131 * 1.5,
        Width = 105 * 1.5,
        Height = 26 * 1.5
    }
};
root.SysMesBoxDisplay.ButtonNo = name .. "ButtonNo";

root.Sprites[name .. "ButtonOK"] = {
    Sheet = sheet,
    Bounds = {
        X = 1870 * 1.5,
        Y = 131 * 1.5,
        Width = 105 * 1.5,
        Height = 26 * 1.5
    }
};
root.SysMesBoxDisplay.ButtonOK = name .. "ButtonOK";

root.Sprites[name .. "ButtonYesHighlighted"] = {
    Sheet = sheet,
    Bounds = {
        X = 1656,
        Y = 159,
        Width = 105,
        Height = 26
    }
};
root.SysMesBoxDisplay.ButtonYesHighlighted = name .. "ButtonYesHighlighted";

root.Sprites[name .. "ButtonNoHighlighted"] = {
    Sheet = sheet,
    Bounds = {
        X = 1763,
        Y = 159,
        Width = 105,
        Height = 26
    }
};
root.SysMesBoxDisplay.ButtonNoHighlighted = name .. "ButtonNoHighlighted";

root.Sprites[name .. "ButtonOKHighlighted"] = {
    Sheet = sheet,
    Bounds = {
        X = 1870,
        Y = 159,
        Width = 105,
        Height = 26
    }
};
root.SysMesBoxDisplay.ButtonOKHighlighted = name .. "ButtonOKHighlighted";