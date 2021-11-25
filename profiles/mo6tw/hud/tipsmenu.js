root.TipsMenu = {
    Type: TipsMenuType.MO6TW,
    BackgroundSprite: "TipsMenuBackground",
    Thumbnails: [],
    TextOnlyThumbnail: "TipsTextOnlyThumbnail",
    FadeInDuration: 0.2,
    FadeOutDuration: 0.2
};

root.Sprites["TipsMenuBackground"] = {
    Sheet: "Tips",
    Bounds: { X: 0, Y: 0, Width: 1280, Height: 720 }
};

root.Sprites["TipsTextOnlyThumbnail"] = {
    Sheet: "Tips",
    Bounds: { X: 1300, Y: 0, Width: 256, Height: 192 }
};

var firstX = 0;
var firstY = 0;

for (var i = 1; i < 38; i++) {
    root.Sprites["TipThumbnail" + i] = {
        Sheet: "TipThumbnails",
        Bounds: {
            X: firstX,
            Y: firstY,
            Width: 256,
            Height: 192
        }
    };
    root.TipsMenu.Thumbnails.push("TipThumbnail" + i);

    if (i % 8 == 0) {
        firstX = 0;
        firstY += 192;
    } else {
        firstX += 256;
    }
}
