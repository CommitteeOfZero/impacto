-- STEINS;GATE 0 save / load / quick-load screen (savemenu.dds, sysm_savemenu.dds).
-- Rectangles were measured from the sheets' alpha; positions are 1920x1080 design pixels.
root.SaveMenu = {
    Type = SaveMenuType.SG0,
    DrawType = DrawComponentType.SystemMenu,
    EmptyThumbnailSprite = "EmptyThumbnail",
    FadeInDuration = 16 / 60,
    FadeOutDuration = 16 / 60,
    -- The scripts count SW_SYSSUBMENUCT up to 16 while the screen opens.
    SubMenuCountMax = 16,

    BackgroundSprite = "SaveMenuBackground",
    PanelSprite = "SaveMenuPanel",
    PanelPos = { X = 230, Y = 34 },
    HeaderSprites = { "SaveMenuHeaderQuickLoad", "SaveMenuHeaderSave", "SaveMenuHeaderLoad" },
    HeaderPos = { X = -120, Y = -120 },
    ButtonGuideSprite = "SaveMenuButtonGuide",
    ButtonGuidePos = { X = 1100, Y = 1012 },

    NoDataSprite = "SaveMenuNoData",
    EntryFrameSprite = "SaveMenuEntryFrame",
    PlayTimeLabelSprite = "SaveMenuPlayTimeLabel",
    SaveDateLabelSprite = "SaveMenuSaveDateLabel",
    LockedSprite = "SaveMenuLocked",
    PageDigitSprites = {
        "SaveMenuPageSlash", "SaveMenuPage1", "SaveMenuPage2", "SaveMenuPage3",
        "SaveMenuPage4", "SaveMenuPage5", "SaveMenuPage6"
    },

    -- Entry origin = PanelPos + EntryStart + (column, row) * EntrySpacing.
    EntryStart = { X = 44, Y = 120 },
    EntrySpacing = { X = 800, Y = 216 },
    FrameOffset = { X = 0, Y = 0 },
    ThumbnailOffset = { X = 8, Y = 6 },
    ThumbnailSize = { X = 292, Y = 164 },
    PlayTimeLabelOffset = { X = 320, Y = 30 },
    SaveDateLabelOffset = { X = 320, Y = 90 },
    PlayTimeTextOffset = { X = 476, Y = 26 },
    SaveDateTextOffset = { X = 476, Y = 86 },
    LockedOffset = { X = 572, Y = 132 },
    PageNumberPos = { X = 1700, Y = 60 },
    TextFontSize = 32
};

local function sprite(name, sheet, x, y, w, h)
    root.Sprites[name] = { Sheet = sheet, Bounds = { X = x, Y = y, Width = w, Height = h } };
end

sprite("EmptyThumbnail", "SaveMenu", 0, 0, 0, 0);
sprite("SaveMenuBackground", "SysSaveMenu", 0, 0, 1920, 1080);
sprite("SaveMenuPanel", "SaveMenu", 4, 0, 1668, 1012);
sprite("SaveMenuHeaderSave", "SysSaveMenu", 8, 1088, 584, 564);
sprite("SaveMenuHeaderLoad", "SysSaveMenu", 608, 1088, 584, 564);
sprite("SaveMenuHeaderQuickLoad", "SysSaveMenu", 1208, 1088, 584, 564);
sprite("SaveMenuButtonGuide", "SysSaveMenu", 1100, 1700, 788, 60);

sprite("SaveMenuNoData", "SaveMenu", 1684, 84, 292, 164);
sprite("SaveMenuEntryFrame", "SaveMenu", 1692, 336, 780, 176);
sprite("SaveMenuPlayTimeLabel", "SaveMenu", 1684, 4, 140, 32);
sprite("SaveMenuSaveDateLabel", "SaveMenu", 1684, 40, 140, 32);
sprite("SaveMenuLocked", "SaveMenu", 1984, 228, 196, 36);

sprite("SaveMenuPageSlash", "SaveMenu", 2280, 84, 40, 36);
sprite("SaveMenuPage1", "SaveMenu", 2336, 88, 16, 32);
sprite("SaveMenuPage2", "SaveMenu", 2368, 84, 24, 36);
sprite("SaveMenuPage3", "SaveMenu", 2404, 84, 24, 36);
sprite("SaveMenuPage4", "SaveMenu", 2440, 88, 24, 32);
sprite("SaveMenuPage5", "SaveMenu", 2480, 84, 20, 36);
sprite("SaveMenuPage6", "SaveMenu", 2512, 84, 24, 36);
