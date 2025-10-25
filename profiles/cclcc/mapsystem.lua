root.MapSystem = {
    MapBackground = "MapBackground",
    MapPartsPhotoSprites = {},
    MapPartsArticleSprites = {},
    MapPartsPinSprites = {},
    MapPartsTagSprites = {},
    MapPartsPhotosNum = 18,
    MapPartsArticlesNum = 16,
    MapPartsPinsNum = 12,
    MapPartsTagsNum = 16,
    SelectedMapPoolTag = "SelectedMapPoolTag",
    MapLineRed = "MapLineRed",
    MapLine = "MapLine",
    FadeAnimationDuration = 16 / 60
};

root.Sprites["MapBackground"] = {
    Sheet = "MapBG",
    Bounds = { X = 0, Y = 0, Width = 3378, Height = 1900 }
};

root.Sprites["SelectedMapPoolTag"] = {
    Sheet = "MapParts",
    Bounds = { X = 1770, Y = 2832, Width = 172, Height = 228 }
};

root.Sprites["MapLineRed"] = {
    Sheet = "MapParts",
    Bounds = { X = 1770, Y = 2832, Width = 2048, Height = 228 }
};

root.Sprites["MapLine"] = {
    Sheet = "MapParts",
    Bounds = { X = 0, Y = 13, Width = 2048, Height = 10 }
};

for i = 0, root.MapSystem.MapPartsPhotosNum - 1 do
    root.Sprites["MapPartsPhoto" .. i] = {
        Sheet = "MapParts",
        Bounds = {
            X = (i % 12) * 166 + 1,
            Y = (i // 12) * 117 + 24 + 1,
            Width = 164,
            Height = 115
        },
    };
    root.MapSystem.MapPartsPhotoSprites[#root.MapSystem.MapPartsPhotoSprites + 1] = "MapPartsPhoto" .. i;
end


for i = 0, root.MapSystem.MapPartsArticlesNum - 1 do
    root.Sprites["MapPartsArticle" .. i] = {
        Sheet = "MapParts",
        Bounds = {
            X = (i % 5) * 354 + 1,
            Y = (i // 5) * 247 + 2014,
            Width = 354,
            Height = 247
        },
    };
    root.MapSystem.MapPartsArticleSprites[#root.MapSystem.MapPartsArticleSprites + 1] = "MapPartsArticle" .. i;
end


for i = 0, root.MapSystem.MapPartsPinsNum - 1 do
    root.Sprites["MapPartsPin" .. i] = {
        Sheet = "MapParts",
        Bounds = {
            X = 2001,
            Y = i * 60 + 24,
            Width = 46,
            Height = 58
        },
    };
    root.MapSystem.MapPartsPinSprites[#root.MapSystem.MapPartsPinSprites + 1] = "MapPartsPin" .. i;
end

for i = 0, root.MapSystem.MapPartsTagsNum - 1 do
    root.Sprites["MapPartsTag" .. i] = {
        Sheet = "MapParts",
        Bounds = {
            X = (i % 2) * 97 + 1770,
            Y = (i // 2) * 34 + 318,
            Width = 95,
            Height = 32
        },
    };
  root.MapSystem.MapPartsTagSprites[#root.MapSystem.MapPartsTagSprites + 1] = "MapPartsTag" .. i;
end
