var sheet = "Data";
var name = "SysMesBox";

root.SysMesBoxDisplay = {
	Type: SysMesBoxType.Darling,
    DrawType: DrawComponentType.SystemMessage,
	BoxX: 602,
	BoxY: 224,
	BoxMiddleBaseX: 604,
	BoxMinimumWidth: 768,
	BoxMiddleBaseWidth: 434,
	BoxRightBaseWidth: 24,
	BoxRightRemainPad: 48,
	TextFontSize: 32,
	TextMiddleY: 236,
	TextX: 640,
	TextLineHeight: 34,
	TextMarginY: 14,
    ChoicePadding: 40,
    ChoiceY: 365,
    ChoiceXBase: 680,
    MinMaxMesWidth: 294,
    AnimationSpeed: 55,
    FadeInDuration: 0.33,
    FadeOutDuration: 0.25
};

root.Sprites["SelectionHighlight"] = {
    Sheet: "Data",
    Bounds: { X: 65, Y: 33, Width: 94, Height: 30 }
};
root.SysMesBoxDisplay.SelectionHighlight = "SelectionHighlight";

root.Sprites[name + "BoxPartLeft"] = {
    Sheet: sheet,
    Bounds: {
        X: 1,
        Y: 842,
        Width: 48,
        Height: 182
    }
};
root.SysMesBoxDisplay.BoxPartLeft = name + "BoxPartLeft";

root.Sprites[name + "BoxPartRight"] = {
    Sheet: sheet,
    Bounds: {
        X: 483,
        Y: 842,
        Width: 48,
        Height: 182
    }
};
root.SysMesBoxDisplay.BoxPartRight = name + "BoxPartRight";

root.Sprites[name + "BoxPartMiddle"] = {
    Sheet: sheet,
    Bounds: {
        X: 49,
        Y: 842,
        Width: 434,
        Height: 182
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