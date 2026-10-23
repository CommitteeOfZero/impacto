-- STEINS;GATE 0 TIPS (sysm_tips.dds backdrop, tipschips.dds panels).
-- Panel geometry measured from the tipschips overlay: list x 112-709 (tabs
-- y 111-157, rows y 160-1008), tip view x 778-1794 (header y 58-264, note
-- y 299-1008). The note text is laid out by the scripts' text mode 7.
root.SpriteSheets["SysTips"] = { Path = { Mount = "system", Id = 23 }, DesignWidth = 3072, DesignHeight = 1928 };
root.SpriteSheets["TipsChips"] = { Path = { Mount = "system", Id = 24 }, DesignWidth = 3072, DesignHeight = 2168 };

local function sprite(name, sheet, x, y, w, h)
    root.Sprites[name] = { Sheet = sheet, Bounds = { X = x, Y = y, Width = w, Height = h } };
    return name;
end

root.TipsMenu = {
    Type = TipsMenuType.SG0,
    DrawType = DrawComponentType.SystemMenu,
    -- _Startup_win counts SW_SYSSUBMENUCT to 32 while TIPS opens.
    CountMax = 32,
    FadeInDuration = 32 / 60,
    FadeOutDuration = 32 / 60,

    BackgroundSprite = sprite("TipsBackground", "SysTips", 0, 0, 1920, 1080),
    PanelsSprite = sprite("TipsPanels", "TipsChips", 0, 0, 1920, 1080),
    HeaderSprite = sprite("TipsHeaderRing", "SysTips", 0, 1084, 600, 560),
    HeaderPos = { X = -110, Y = 760 },
    HeaderIconSprite = sprite("TipsHeaderIcon", "SysTips", 2144, 216, 152, 144),
    HeaderIconPos = { X = 114, Y = 968 },
    GuideSprite = sprite("TipsGuide", "SysTips", 856, 1700, 1032, 60),
    GuidePos = { X = 864, Y = 1014 },

    TabSprites = {
        sprite("TipsTabAll", "TipsChips", 1968, 64, 68, 24),
        sprite("TipsTabAcquired", "TipsChips", 2100, 64, 116, 32),
        sprite("TipsTabUnread", "TipsChips", 2268, 64, 92, 24),
        sprite("TipsTabNew", "TipsChips", 2428, 64, 80, 24),
    },
    TabDisabledSprites = {
        sprite("TipsTabAllOff", "TipsChips", 1968, 16, 68, 24),
        sprite("TipsTabAcquiredOff", "TipsChips", 2100, 16, 116, 32),
        sprite("TipsTabUnreadOff", "TipsChips", 2268, 16, 92, 24),
        sprite("TipsTabNewOff", "TipsChips", 2428, 16, 80, 24),
    },
    TabSelectedSprites = {
        sprite("TipsTabAllOn", "TipsChips", 1924, 100, 152, 44),
        sprite("TipsTabAcquiredOn", "TipsChips", 2080, 100, 152, 44),
        sprite("TipsTabUnreadOn", "TipsChips", 2236, 100, 152, 44),
        sprite("TipsTabNewOn", "TipsChips", 2392, 100, 152, 44),
    },
    TabBounds = {
        { X = 112, Y = 111, Width = 156, Height = 46 },
        { X = 268, Y = 111, Width = 156, Height = 46 },
        { X = 424, Y = 111, Width = 156, Height = 46 },
        { X = 580, Y = 111, Width = 129, Height = 46 },
    },

    ListBounds = { X = 113, Y = 166, Width = 596, Height = 836 },
    ListTop = 174,
    RowHeight = 48,
    RowPadding = 6,
    VisibleRows = 17,
    ListNumberX = 128,
    ListNameX = 214,
    ListNameMaxWidth = 420,
    ListFontSize = 32,
    SelectionSprite = sprite("TipsSelection", "TipsChips", 1924, 148, 596, 44),
    NewBadgeSprite = sprite("TipsNewBadge", "TipsChips", 2552, 80, 60, 32),
    NewBadgeX = 642,

    NumberPos = { X = 870, Y = 76 },
    CategoryPos = { X = 1440, Y = 76 },
    NamePos = { X = 806, Y = 166 },
    NameMaxWidth = 970,
    HeaderFontSize = 36,
    NameFontSize = 48,
    NoteBounds = { X = 790, Y = 350, Width = 1000, Height = 650 },
    NoteThumbSprite = sprite("TipsNoteThumb", "TipsChips", 2852, 16, 12, 60),
    NoteThumbX = 1799,
    NoteScrollSpeed = 600,

    TextColor = 0xFFFFFF,
    LockedColor = 0x8A8A8A
};
