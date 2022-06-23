root.ExtraMenus = {
    ClearListMenu: {
        DrawType: DrawComponentType.SystemMenu,
        Type: ClearListMenuType.CHLCC,
        TransitionDuration: 64 / 60,
        BackgroundColor: 0x405f80,
        CircleSprite: "Circle",
        CircleStartPosition: {X: 20, Y: 20},
        CircleOffset: 200,
        ErinSprite: "Erin",
        ErinPosition: {X: 301, Y: 0},
        BackgroundFilter: "ClearListFilter",
        InitialRedBarPosition: {X: 0, Y: 538},
        RightRedBarPosition: {X: 1059, Y: 538},
        RedBarDivision: 1826,
        RedBarBaseX: 1059,
        RedBarSprite: "RedBar",
        RedBarLabelPosition: {X: 1067, Y: 573},
        RedBarLabel: "RedBarLabel",
        MenuTitleTextRightPos: {X: 788, Y: 127},
        MenuTitleTextLeftPos: {X: 1, Y: 1},
        MenuTitleTextAngle: 4.45,
        TitleFadeInDuration: 40 / 60,
        TitleFadeOutDuration: 28 / 60,
        MenuTitleText: "MenuTitleText",
        ClearListLabel: "ClearListLabel",
        LabelPosition: {X: 800, Y: 44},
        Digits: [],
        TimePositions: [
            {X: 988, Y: 69}, {X: 1008, Y: 69},
            {X: 1079, Y: 69}, {X: 1099, Y: 69},
            {X: 1148, Y: 69}, {X: 1168, Y: 69},
        ],
        AlbumPositions: [
            {X: 1125, Y: 152},
            {X: 1145, Y: 152},
            {X: 1165, Y: 152}
        ],
        EndingCountPosition: {X: 1159, Y: 96},
        TIPSCountPositions: [
            {X: 1125, Y: 124}, {X: 1145, Y: 124},
        ],
        EndingList: "EndingList",
        ListPosition: {X: 0, Y: 0},
        EndingBox: "EndingBox",
        BoxPositions: [
            {X: 341, Y: 218}, {X: 572, Y: 146},
            {X: 341, Y: 345}, {X: 341, Y: 472},
            {X: 572, Y: 400}, {X: 572, Y: 273},
            {X: 572, Y: 527}, {X: 110, Y: 146}
        ],
        EndingThumbnails: [],
        ThumbnailPositions: [],
        LockedThumbnail: "LockedThumbnail",
        ButtonPromptPosition: {X: 1112, Y: 650},
        ButtonPromptSprite: "ButtonPrompt"
    }
}

//Clear list

root.Sprites["Circle"] = {
    Sheet: "ClearListLabels",
    Bounds: {X: 1, Y: 917, Width: 106, Height: 106}
}

root.Sprites["Erin"] = {
    Sheet: "ClearList",
    Bounds: {X: 641, Y: 1, Width: 978, Height: 798}
}

root.Sprites["RedBar"] = {
    Sheet: "ClearList",
    Bounds: {X: 767, Y: 913, Width: 1280, Height: 110}
}

root.Sprites["RedBarLabel"] = {
    Sheet: "ClearList",
    Bounds: {X: 506, Y: 469, Width: 133, Height: 74}
}

root.Sprites["MenuTitleText"] = {
    Sheet: "ClearListLabels",
    Bounds: {X: 607, Y: 1, Width: 120, Height: 912}
}

root.Sprites["ClearListLabel"] = {
    Sheet: "ClearListLabels",
    Bounds: {X: 1, Y: 591, Width: 490, Height: 136}
}

for (var i = 0; i <= 9; i++) {
    root.Sprites["Digit" + i] = {
        Sheet: "ClearListLabels",
        Bounds: {
            X: 211 + 22 * i,
            Y: 729,
            Width: 20,
            Height: 30
        }
    }
    root.ExtraMenus.ClearListMenu.Digits.push("Digit" + i);
}

root.Sprites["ClearListFilter"] = {
    Sheet: "ClearList",
    Bounds: {X: 0, Y: 0, Width: 640, Height: 360}
}

root.Sprites["EndingList"] = {
    Sheet: "ClearListLabels",
    Bounds: {X: 1, Y: 1, Width: 604, Height: 588}
}

root.Sprites["EndingBox"] = {
    Sheet: "ClearListLabels",
    Bounds: {X: 1, Y: 761, Width: 198, Height: 96}
}

var thumbnailXOffest = 30;
var thumbnailYOffset = 3;
var bp = root.ExtraMenus.ClearListMenu.BoxPositions;

for (var i = 0; i < bp.length; i++) {
    root.ExtraMenus.ClearListMenu.ThumbnailPositions.push({
        X: bp[i].X + thumbnailXOffest,
        Y: bp[i].Y + thumbnailYOffset
    });
}

root.Sprites["LockedThumbnail"] = {
    Sheet: "ClearListLabels",
    Bounds: {X: 729, Y: 1, Width: 160, Height: 90}
}

var firstX = 729;
var firstY = 93;
var yDelta = 92;

for (var i = 1; i <= 8; i++) {
    root.Sprites["EndingThumbnail" + i] = {
        Sheet: "ClearListLabels",
        Bounds: {
            X: firstX,
            Y: firstY,
            Width: 160,
            Height: 90
        }
    };
    root.ExtraMenus.ClearListMenu.EndingThumbnails.push("EndingThumbnail" + i);
    firstY += yDelta;
}

//Reorganize to be in the flag order (801 -> 808)
//Yes I know it's bad
et = root.ExtraMenus.ClearListMenu.EndingThumbnails;
temp = et[0];
et[0] = et[1];
et[1] = temp;
et[1] = et[4];
et[4] = temp;
et[4] = et[6];
et[6] = temp;
et[6] = et[7];
et[7] = temp;

root.Sprites["ButtonPrompt"] = {
    Sheet: "ClearListLabels",
    Bounds: {X: 1, Y: 729, Width: 208, Height: 30}
}