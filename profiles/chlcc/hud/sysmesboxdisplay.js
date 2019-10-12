var sheet = "Data";
var name = "SysMesBox";

root.SysMesBoxDisplay = {
	Type: SysMesBoxType.CHLCC,
	BoxX: 0,
	BoxY: 230,
	TextFontSize: 32,
	TextMiddleY: 236,
	TextX: 640,
	TextLineHeight: 34,
	TextMarginY: 14,
    AnimationSpeed: 55,
    FadeInDuration: 0.33,
    FadeOutDuration: 0.25
};

root.Sprites[name + "Box"] = {
    Sheet: sheet,
    Bounds: {
        X: 767,
        Y: 637,
        Width: 1280,
        Height: 172
    }
};
root.SysMesBoxDisplay.Box = name + "Box";

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