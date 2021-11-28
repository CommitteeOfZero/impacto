root.TipsMenu = {
    Type: TipsMenuType.MO6TW,
    BackgroundSprite: "TipsMenuBackground",
    Thumbnails: [],
    TextOnlyThumbnail: "TipsTextOnlyThumbnail",
    ThumbnailPosition: { X: 413, Y: 45 },
    CategoryString: "【 】",
    DefaultColorIndex: 0,
    UnreadColorIndex: 70,
    NameInitialBounds: { X: 703, Y: 117, Width: 0, Height: 0 },
    NameFontSize: 32,
    PronounciationInitialBounds: { X: 703, Y: 87, Width: 0, Height: 0 },
    PronounciationFontSize: 20,
    CategoryInitialBounds: { X: 1135, Y: 206, Width: 0, Height: 0 },
    CategoryEndX: 1200,
    CategoryFontSize: 32,
    SortStringTable: 2,
    SortStringIndex: 5,
    TipListInitialY: 87,
    TipListCategoriesPerPage: 5,
    TipListMaxPages: 10,
    TipListEntryBounds: { X: 69, Y: 87, Width: 300, Height: 24 },
    TipListEntryFontSize: 20,
    TipListYPadding: 20,
    TipListEntryHighlightOffset: { X: 0, Y: -9 },
    TipListEntryNameXOffset: 50,
    TipListEntryNewText: "New",
    TipListEntryNewOffset: 261,
    TipListEntryLockedTable: 2,
    TipListEntryLockedIndex: 7,
    NumberLabelStrTable: 2,
    NumberLabelStrIndex: 8,
    NumberLabelPosition: { X: 1070, Y: 45 },
    NumberLabelFontSize: 32,
    NumberBounds: { X: 1130, Y: 45, Width: 0, Height: 0 },
    NumberFontSize: 32,
    PageSeparatorTable: 2,
    PageSeparatorIndex: 9,
    PageSeparatorPosition: { X: 1154, Y: 645 },
    PageSeparatorFontSize: 32,
    CurrentPageBounds: { X: 1130, Y: 645, Width: 0, Height: 0 },
    TotalPagesBounds: { X: 1184, Y: 645, Width: 0, Height: 0 },
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
