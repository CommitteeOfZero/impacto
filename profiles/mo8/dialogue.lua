root.Sprites["ADVBox"] = {
    Sheet = "MesBox",
    Bounds = { X = 0, Y = 0, Width = 1920, Height = 330 }
};

root.Dialogue = {
    REVBounds = { X = 0, Y = 0, Width = 1440, Height = 600 },
    REVNameFontSize = 24,
    REVColor = 0,
    REVNameColor = 24,
    REVNameOffset = 34,
    REVNameLocation = REVNameLocationType.TopLeft,
    REVOutlineMode = 2,
    REVNameOutlineMode = 2,
    NVLBounds = { X = 188, Y = 128, Width = 1536, Height = 600 },
    ADVBounds = { X = 518, Y = 838, Width = 1240, Height = 230 },
    ADVBoxSprite = "ADVBox",
    ADVBoxPos = { X = 0, Y = 760 },
    FadeOutDuration = 0.33,
    FadeInDuration = 0.33,
    DialogueBoxCurrentType = DialogueBoxType.Plain,
    NVLBoxMaxOpacity = 0.55,
    ADVNameAlignment = TextAlignment.Center,
    ADVNameFontSize = 42,
    ADVNamePos = { X = 168, Y = 974 },
    WaitIconSpriteAnim = "WaitIconSpriteAnimDef",
    WaitIconCurrentType = WaitIconType.SpriteAnim,
    WaitIconOffset = { X = 0, Y = 0 },
    DialogueFont = "Default",
    SetFontSizeRatio = 1000.0,
    DefaultFontSize = 48,
    RubyFontSize = 21,
    RubyYOffset = -27,
    ColorTable = {
        {0xFFFFFF, 0x000000}, {0x5080FF, 0x000000},
        {0xFF7080, 0x000000}, {0xFFA0F8, 0x000000},
        {0x46FF80, 0x000000}, {0x90FFFF, 0x000000},
        {0xFFFF70, 0x000000}, {0x80FFC0, 0x000000},
        {0xFFB080, 0x000000}, {0xB080FF, 0x000000},
        {0x000000, 0x808080}, {0x000000, 0x5080FF},
        {0x000000, 0xFF7080}, {0x000000, 0xFFA0F8},
        {0x000000, 0x268840}, {0x000000, 0x409999},
        {0x000000, 0x888830}, {0x000000, 0x80FFC0},
        {0x000000, 0xFFB080}, {0x000000, 0xB080FF},
        {0xD0D0D0, 0x000000}, {0xD0D0FF, 0x000000},
        {0xFFD0D0, 0x000000}, {0xFFD0FF, 0x000000},
        {0xD0FFD0, 0x000000}, {0xD0FFFF, 0x000000},
        {0xFFFFD0, 0x000000}, {0xE8FFD0, 0x000000},
        {0xFFE8D0, 0x000000}, {0xD0E8FF, 0x000000},
        {0xFFFFFF, 0x808080}, {0xFFFFFF, 0x5080FF},
        {0xFFFFFF, 0xFF7080}, {0xFFFFFF, 0xFFA0F8},
        {0xFFFFFF, 0x46FF80}, {0xFFFFFF, 0x90FFFF},
        {0xFFFFFF, 0xFFFF70}, {0xFFFFFF, 0x80FFC0},
        {0xFFFFFF, 0xFFB080}, {0xFFFFFF, 0xB080FF},
        {0xFFEEEE, 0x000000}, {0xFFCCCC, 0x000000},
        {0xFFAAAA, 0x000000}, {0xFF9999, 0x000000},
        {0xFF8888, 0x000000}, {0xFFFF00, 0x000000},
        {0xFEF000, 0x000000}, {0xFF7777, 0x000000},
        {0xFF6666, 0x000000}, {0xFF5555, 0x000000},
        {0xFF4444, 0x000000}, {0xFF3333, 0x000000},
        {0xFF2222, 0x000000}, {0xFF0000, 0x000000},
        {0xDD0000, 0x000000}, {0xBB0000, 0x000000},
        {0xB00000, 0x000000}, {0xAA0000, 0x000000},
        {0x950000, 0x000000}, {0x808080, 0x000000},
        {0xAAAAAA, 0x000000}, {0xAAC1C9, 0x000000},
        {0x000000, 0x000000}, {0x000000, 0x000000},
        {0x000000, 0x000000}, {0x000000, 0x000000},
        {0x000000, 0x000000}, {0xF80B0B, 0x000000},
        {0xF8910B, 0x000000}, {0x33F12A, 0x000000}
    },
    MaxPageSize = 2000,
    PageCount = 3,
    ColorTagIsUint8 = false,
    HasSpeakerPortraits = true,
    
    HasAutoButton = true,
    AutoButtonSprite = "AutoButton",
    AutoButtonActiveSprite = "AutoButtonActive",
    AutoButtonPosition = { X = 1786, Y = 792 },
    
    HasSkipButton = true,
    SkipButtonSprite = "SkipButton",
    SkipButtonActiveSprite = "SkipButtonActive",
    SkipButtonPosition = { X = 1786, Y = 860 },
    
    HasBacklogButton = true,
    BacklogButtonSprite = "BacklogButton",
    BacklogButtonActiveSprite = "BacklogButtonActive",
    BacklogButtonPosition = { X = 1786, Y = 928 },
    
    HasMenuButton = true,
    MenuButtonSprite = "MenuButton",
    MenuButtonActiveSprite = "MenuButtonActive",
    MenuButtonPosition = { X = 1786, Y = 996 }
};

MakeAnimation({
    Name = "WaitIconSpriteAnimDef",
    Sheet = "MesBox",
    FirstFrameX = 0,
    FirstFrameY = 440,
    FrameWidth = 64,
    ColWidth = 64,
    FrameHeight = 64,
    RowHeight = 64,
    Frames = 3,
    Duration = 9.0,
    Rows = 1,
    Columns = 3,
    PrimaryDirection = AnimationDirections.Right,
    SecondaryDirection = AnimationDirections.Down
});

root.Sprites["AutoButton"] = {
    Sheet = "MesBox",
    Bounds = { X = 770, Y = 330, Width = 68, Height = 66 },
};

root.Sprites["AutoButtonActive"] = {
    Sheet = "MesBox",
    Bounds = { X = 846, Y = 404, Width = 68, Height = 66 },
};

root.Sprites["SkipButton"] = {
    Sheet = "MesBox",
    Bounds = { X = 618, Y = 404, Width = 68, Height = 66 },
};

root.Sprites["SkipButtonActive"] = {
    Sheet = "MesBox",
    Bounds = { X = 618, Y = 330, Width = 68, Height = 66 },
};

root.Sprites["BacklogButton"] = {
    Sheet = "MesBox",
    Bounds = { X = 770, Y = 404, Width = 68, Height = 66 },
};

root.Sprites["BacklogButtonActive"] = {
    Sheet = "MesBox",
    Bounds = { X = 846, Y = 330, Width = 68, Height = 66 },
};

root.Sprites["MenuButton"] = {
    Sheet = "MesBox",
    Bounds = { X = 694, Y = 404, Width = 68, Height = 66 },
};

root.Sprites["MenuButtonActive"] = {
    Sheet = "MesBox",
    Bounds = { X = 694, Y = 330, Width = 68, Height = 66 },
};

include('mo8/nametag.lua');