var sheet = "Data";
var name = "SysMesBox";

root.SysMesBoxDisplay = {
	Type: SysMesBoxType.MO6TW,
	BoxX: 602,
	BoxY: 204,
	BoxMiddleBaseX: 783,
	BoxMinimumWidth: 768,
	BoxMiddleBaseWidth: 140,
	BoxMiddleRemainBase: 154,
	BoxRightBaseX: 370,
	BoxRightRemainPad: 48,
	TextFontSize: 32,
	TextMiddleY: 236,
	TextX: 640,
	TextLineHeight: 34,
	TextMarginY: 14,
    AnimationSpeed: 55,
    FadeInDuration: 0.33,
    FadeOutDuration: 0.25
};

root.Sprites[name + "BoxPartLeft"] = {
    Sheet: sheet,
    Bounds: {
        X: 1,
        Y: 606,
        Width: 181,
        Height: 212
    }
};
root.SysMesBoxDisplay.BoxPartLeft = name + "BoxPartLeft";

root.Sprites[name + "BoxPartRight"] = {
    Sheet: sheet,
    Bounds: {
        X: 322,
        Y: 606,
        Width: 181,
        Height: 212
    }
};
root.SysMesBoxDisplay.BoxPartRight = name + "BoxPartRight";

root.Sprites[name + "BoxPartMiddle"] = {
    Sheet: sheet,
    Bounds: {
        X: 182,
        Y: 606,
        Width: 140,
        Height: 212
    }
};
root.SysMesBoxDisplay.BoxPartMiddle = name + "BoxPartMiddle";

root.Sprites[name + "BoxDecoration"] = {
    Sheet: sheet,
    Bounds: {
        X: 1383,
        Y: 117,
        Width: 592,
        Height: 8
    }
};
root.SysMesBoxDisplay.BoxDecoration = name + "BoxDecoration";