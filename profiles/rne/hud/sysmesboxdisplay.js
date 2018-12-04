var sheet = "Data";
var name = "SysMesBox";

root.SysMesBoxDisplay = {
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