-- STEINS;GATE 0 phone overlay (phone_b.dds): Rintaro's phone slides in at the
-- right while flag 2600 is set; positions are 1920x1080 design pixels.
root.GameSpecific = {
    Type = GameSpecificType.SG0
};

root.SpriteSheets["PhoneB"] = {
    Path = { Mount = "system", Id = 18 },
    DesignWidth = 3072,
    DesignHeight = 1536
};

root.SpriteSheets["PhoneRine"] = {
    Path = { Mount = "system", Id = 30 },
    DesignWidth = 704,
    DesignHeight = 1216
};

root.Sprites["PhoneBody"] = { Sheet = "PhoneB", Bounds = { X = 12, Y = 16, Width = 660, Height = 1260 } };
root.Sprites["PhoneWallpaper"] = { Sheet = "PhoneB", Bounds = { X = 1472, Y = 0, Width = 456, Height = 700 } };
root.Sprites["PhoneSelection"] = { Sheet = "PhoneB", Bounds = { X = 1932, Y = 480, Width = 426, Height = 60 } };

root.Phone = {
    DrawType = DrawComponentType.Main,
    BodySprite = "PhoneBody",
    Scale = 0.75,
    ShownPos = { X = 1400, Y = 24 },
    HiddenX = 1940,
    -- The screen sits at (104, 104) in the 660x1260 body and is 456x804, scaled by 0.75.
    ScreenOffset = { X = 78, Y = 78 },
    ScreenSize = { X = 342, Y = 603 },
    WallpaperSprite = "PhoneWallpaper",
    ScreenShadeAlpha = 0.45,
    SelectionSprite = "PhoneSelection",
    Padding = { X = 20, Y = 24 },
    ItemsTop = 132,
    RowHeight = 56,
    RowPadding = 10,
    TitleFontSize = 32,
    ItemFontSize = 30,
    TextColor = 0xFFFFFF,
    OutlineColor = 0x000000,

    -- RINE: phone_rine.dds holds 12 avatars (92 px, 96 apart) and 21 stickers
    -- (192x168 on a 3-column grid at 96 + 198c, 174r).
    RineAvatarSprite = "PhoneRineAvatar",
    RineAvatarStepY = 96,
    RineStickerSprite = "PhoneRineSticker",
    RineStickerStep = { X = 198, Y = 174 },
    RineStickerColumns = 3,
    RineTop = 70,
    RineMargin = 8,
    RineGap = 8,
    RineAvatarSize = 36,
    RineStickerSize = { X = 96, Y = 84 },
    RineFontSize = 20,
    RineLineHeight = 22,
    RineBubbleMaxWidth = 210,
    RineBubblePadding = { X = 8, Y = 6 },
    RineOwnColor = 0x9EDC70,
    RineOtherColor = 0xFFFFFF,
    RineTextColor = 0x1E1E1E,
    RineReplyRowHeight = 34,
    RineReplyStickerSize = { X = 96, Y = 84 },
    RineInputHeight = 40
};

root.Sprites["PhoneRineAvatar"] = { Sheet = "PhoneRine", Bounds = { X = 0, Y = 0, Width = 92, Height = 92 } };
root.Sprites["PhoneRineSticker"] = { Sheet = "PhoneRine", Bounds = { X = 96, Y = 0, Width = 192, Height = 168 } };
