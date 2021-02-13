var sheet = "Data";
var name = "SysMesBox";

root.SysMesBoxDisplay = {
    Type: SysMesBoxType.RNE,
    LinePositionXFirst: 1622,
    LinePositionX: 1652,
    LinePositionMultiplier: 30,
    LineWidthFirst: 114,
    LineWidthBase: 54,
    LineWidthMultiplier: 60,
    Line1SpriteY: 107,
    Line2SpriteY: 117,
    LineSpriteHeight: 8,
    LineDisplayXBase: 639,
    Line1DisplayY: 360,
    Line2DisplayY: 352,
    BoxDisplayStartCount: 9,
    BoxHeightBase: 112,
    BoxHeightMultiplier: 14,
    BoxWidth: 592,
    BoxTextFontSize: 28,
    BoxTopYBase: 360,
    BoxDisplayX: 344,
    MessageLabelSpriteXBase: 1574,
    MessageLabelSpriteY: 131,
    MessageLabelSpriteHeight: 31,
	MessageLabelSpriteMultiplier: 12,
	ButtonYesDisplayXBase: 897,
	ButtonRightDisplayXBase: 936,
	ButtonWidth: 105,
	ButtonYOffset: 29,
    ButtonYWidthBase: 39,
    ButtonRightWidthBase: -75,
	TextDecorationStart: 56,
	TextDecorationTopYOffset: 36,
	TextDecorationBottomYOffset: 32,
	TextFontSize: 26,
	TextMiddleY: 418,
	TextX: 370,
	TextLineHeight: 28,
	TextMarginY: 14,
	SpriteMargin: 3,
    AnimationSpeed: 55,
    FadeInDuration: 0.33,
    FadeOutDuration: 0.25
};

root.Sprites[name + "BoxDecorationTop"] = {
    Sheet: sheet,
    Bounds: {
        X: 1383,
        Y: 107,
        Width: 592,
        Height: 8
    }
};
root.SysMesBoxDisplay.BoxDecorationTop = name + "BoxDecorationTop";

root.Sprites[name + "BoxDecorationBottom"] = {
    Sheet: sheet,
    Bounds: {
        X: 1383,
        Y: 117,
        Width: 592,
        Height: 8
    }
};
root.SysMesBoxDisplay.BoxDecorationBottom = name + "BoxDecorationBottom";

root.Sprites[name + "TextDecoration"] = {
    Sheet: sheet,
    Bounds: {
        X: 1383,
        Y: 127,
        Width: 592,
        Height: 2
    }
};
root.SysMesBoxDisplay.TextDecoration = name + "TextDecoration";

root.Sprites[name + "MessageLabel"] = {
    Sheet: sheet,
    Bounds: {
        X: 1383,
        Y: 127,
        Width: 592,
        Height: 2
    }
};
root.SysMesBoxDisplay.MessageLabel = name + "MessageLabel";

root.Sprites[name + "Line1"] = {
    Sheet: sheet,
    Bounds: {
        X: 1622,
        Y: 107,
        Width: 114,
        Height: 8
    }
};
root.SysMesBoxDisplay.Line1 = name + "Line1";

root.Sprites[name + "Line2"] = {
    Sheet: sheet,
    Bounds: {
        X: 1622,
        Y: 117,
        Width: 114,
        Height: 8
    }
};
root.SysMesBoxDisplay.Line2 = name + "Line2";

root.Sprites[name + "ButtonYes"] = {
    Sheet: sheet,
    Bounds: {
        X: 1656,
        Y: 131,
        Width: 105,
        Height: 26
    }
};
root.SysMesBoxDisplay.ButtonYes = name + "ButtonYes";

root.Sprites[name + "ButtonNo"] = {
    Sheet: sheet,
    Bounds: {
        X: 1763,
        Y: 131,
        Width: 105,
        Height: 26
    }
};
root.SysMesBoxDisplay.ButtonNo = name + "ButtonNo";

root.Sprites[name + "ButtonOK"] = {
    Sheet: sheet,
    Bounds: {
        X: 1870,
        Y: 131,
        Width: 105,
        Height: 26
    }
};
root.SysMesBoxDisplay.ButtonOK = name + "ButtonOK";

root.Sprites[name + "ButtonYesHighlighted"] = {
    Sheet: sheet,
    Bounds: {
        X: 1656,
        Y: 159,
        Width: 105,
        Height: 26
    }
};
root.SysMesBoxDisplay.ButtonYesHighlighted = name + "ButtonYesHighlighted";

root.Sprites[name + "ButtonNoHighlighted"] = {
    Sheet: sheet,
    Bounds: {
        X: 1763,
        Y: 159,
        Width: 105,
        Height: 26
    }
};
root.SysMesBoxDisplay.ButtonNoHighlighted = name + "ButtonNoHighlighted";

root.Sprites[name + "ButtonOKHighlighted"] = {
    Sheet: sheet,
    Bounds: {
        X: 1870,
        Y: 159,
        Width: 105,
        Height: 26
    }
};
root.SysMesBoxDisplay.ButtonOKHighlighted = name + "ButtonOKHighlighted";
