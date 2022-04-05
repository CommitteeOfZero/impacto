var sheet = "SysMesBox";
var name = "SysMesBox";

root.SysMesBoxDisplay = {
    Type: SysMesBoxType.CC,
    DrawType: DrawComponentType.SystemMessage,
    SumoSealSprites: [],
    SumoSealCenterPosX: [
        1175, 1269, 542, 1386, 970, 813, 725, 997
    ],
    SumoSealCenterPosY: [
        612, 454, 520, 606, 625, 425, 584, 461
    ],
    ButtonYesCenterPosX: 1098,
    ButtonYesCenterPosY: 846,
    ButtonNoCenterPosX: 1211,
    ButtonNoCenterPosY: 736,
    ButtonOKCenterPosX: 1170,
    ButtonOKCenterPosY: 824,
	TextFontSize: 32,
	TextMiddleY: 1096,
	TextX: 1920,
	TextLineHeight: 32,
	TextMarginY: 48,
	SpriteMargin: 3,
    AnimationSpeed: 25,
    AnimationProgressWidgetsStartOffset: 9,
    WidgetsAlphaMultiplier: 0.0625,
    ButtonYesAnimationProgressEnd: 12,
    ButtonNoDisplayStart: 4,
    ButtonNoAnimationProgressOffset: 13,
    ButtonYesNoScaleMultiplier: 0.0416,
    ButtonYesNoAlphaDivider: 12,
    ButtonOKScaleMultiplier: 0.03125,
    ButtonScaleMax: 1.5,
    FadeInDuration: 0.4,
    FadeOutDuration: 0.4
};

root.Sprites[name + "ButtonYes"] = {
    Sheet: sheet,
    Bounds: {
        X: 640,
        Y: 1229,
        Width: 232,
        Height: 298
    }
};
root.SysMesBoxDisplay.ButtonYes = name + "ButtonYes";

root.Sprites[name + "ButtonNo"] = {
    Sheet: sheet,
    Bounds: {
        X: 1109,
        Y: 1124,
        Width: 244,
        Height: 200
    }
};
root.SysMesBoxDisplay.ButtonNo = name + "ButtonNo";

root.Sprites[name + "ButtonOK"] = {
    Sheet: sheet,
    Bounds: {
        X: 0,
        Y: 1229,
        Width: 318,
        Height: 298
    }
};
root.SysMesBoxDisplay.ButtonOK = name + "ButtonOK";

root.Sprites[name + "ButtonYesHighlighted"] = {
    Sheet: sheet,
    Bounds: {
        X: 874,
        Y: 1229,
        Width: 232,
        Height: 298
    }
};
root.SysMesBoxDisplay.ButtonYesHighlighted = name + "ButtonYesHighlighted";

root.Sprites[name + "ButtonNoHighlighted"] = {
    Sheet: sheet,
    Bounds: {
        X: 1109,
        Y: 1326,
        Width: 244,
        Height: 200
    }
};
root.SysMesBoxDisplay.ButtonNoHighlighted = name + "ButtonNoHighlighted";

root.Sprites[name + "ButtonOKHighlighted"] = {
    Sheet: sheet,
    Bounds: {
        X: 320,
        Y: 1229,
        Width: 318,
        Height: 298
    }
};
root.SysMesBoxDisplay.ButtonOKHighlighted = name + "ButtonOKHighlighted";

var sealX = 0;
var sealY = 0;

for (var i = 0; i < 8; i++) {
    root.Sprites[name + "SumoSeal" + i] = {
        Sheet: sheet,
        Bounds: {
            X: sealX,
            Y: sealY,
            Width: 478,
            Height: 538
        }
    };

    sealX += 480;
    if (sealX == 1920) {
        sealX = 0;
        sealY += 540;
    }

    root.SysMesBoxDisplay.SumoSealSprites.push(name + "SumoSeal" + i);
}