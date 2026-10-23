-- STEINS;GATE 0 system menu (menuchip.dds): the screen capture inside a monitor
-- frame and a ring of icon buttons. Entry i is the script's SW_SYSMENUCNO value:
-- 0 Backlog, 1 Quick Save, 2 Save, 3 Load, 4 Quick Load, 5 TIPS, 6 Config,
-- 7 Help, 8 Title, 9 Full Screen, 10 Exit Game, 11 Close Menu.
local sheet = "MenuChip";

local function sprite(name, x, y, w, h)
    root.Sprites[name] = { Sheet = sheet, Bounds = { X = x, Y = y, Width = w, Height = h } };
    return name;
end

root.SystemMenu = {
    Type = SystemMenuType.SG0,
    DrawType = DrawComponentType.SystemMenu,
    -- _Startup_win counts SW_SYSMENUCT 0..96 in steps of 3.
    CountMax = 96,

    BackgroundSprite = sprite("SystemMenuMonitor", 0, 0, 1920, 1080),
    MonitorScreen = { X = 528, Y = 104, Width = 1164, Height = 884 },
    GlassSprite = sprite("SystemMenuGlass", 528, 1184, 1164, 884),
    GlassOpacity = 0.3,
    RingSprite = sprite("SystemMenuRing", 1924, 0, 532, 1072),
    RingPos = { X = 1388, Y = 4 },

    MenuEntriesNum = 12,
    MenuEntriesHNum = 12,
    MenuEntriesSprites = {
        sprite("SystemMenuBacklog", 2640, 44, 136, 136),
        sprite("SystemMenuQuickSave", 2524, 140, 100, 100),
        sprite("SystemMenuSave", 2640, 200, 136, 136),
        sprite("SystemMenuLoad", 2640, 352, 136, 136),
        sprite("SystemMenuQuickLoad", 2524, 296, 100, 100),
        sprite("SystemMenuTips", 2640, 508, 136, 136),
        sprite("SystemMenuConfig", 2640, 660, 136, 136),
        sprite("SystemMenuHelp", 2640, 812, 136, 136),
        sprite("SystemMenuTitle", 2524, 756, 100, 100),
        sprite("SystemMenuFullScreen", 2520, 596, 104, 104),
        sprite("SystemMenuExit", 2520, 908, 104, 104),
        sprite("SystemMenuClose", 2520, 452, 104, 104),
    },
    MenuEntriesHighlightedSprites = {
        sprite("SystemMenuBacklogGlow", 2904, 44, 136, 136),
        sprite("SystemMenuQuickSaveGlow", 2796, 136, 104, 104),
        sprite("SystemMenuSaveGlow", 2904, 200, 136, 136),
        sprite("SystemMenuLoadGlow", 2904, 352, 136, 136),
        sprite("SystemMenuQuickLoadGlow", 2796, 292, 104, 108),
        sprite("SystemMenuTipsGlow", 2904, 504, 136, 140),
        sprite("SystemMenuConfigGlow", 2904, 660, 136, 136),
        sprite("SystemMenuHelpGlow", 2904, 812, 136, 136),
        sprite("SystemMenuTitleGlow", 2796, 756, 104, 104),
        sprite("SystemMenuFullScreenGlow", 2796, 600, 104, 104),
        sprite("SystemMenuExitGlow", 2796, 908, 104, 104),
        sprite("SystemMenuCloseGlow", 3036, 908, 104, 104),
    },
    -- Button centres on screen: the ring's large circles sit on x 1616, the
    -- small ones on x 1498 (Close at the ring's lower right).
    EntryCenters = {
        { X = 1616, Y = 106 }, { X = 1498, Y = 194 }, { X = 1616, Y = 269 },
        { X = 1616, Y = 422 }, { X = 1498, Y = 341 }, { X = 1616, Y = 575 },
        { X = 1616, Y = 728 }, { X = 1616, Y = 881 }, { X = 1498, Y = 810 },
        { X = 1498, Y = 657 }, { X = 1498, Y = 959 }, { X = 1734, Y = 959 },
    },
    -- Cursor order, top to bottom.
    NavOrder = { 0, 1, 2, 4, 3, 5, 9, 6, 8, 7, 10, 11 },
    FocusScale = 1.12,
    UnfocusedBrightness = 0.6,
    FadeInDuration = 32 / 60,
    FadeOutDuration = 32 / 60
};
