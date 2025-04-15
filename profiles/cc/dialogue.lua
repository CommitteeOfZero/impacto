root.Sprites["ADVBox"] = {
    Sheet = "MesBox",
    Bounds = { X = 0, Y = 1, Width = 1920, Height = 298 }
};

root.Sprites["ADVBoxMask"] = {
    Sheet = "MesBox",
    Bounds = { X = 0, Y = 301, Width = 1920, Height = 298 }
};

root.Sprites["WaitIconSprite"] = {
    Sheet = "Data",
    Bounds = { X = 1, Y = 456, Width = 112, Height = 64 },
};

root.Dialogue = {
    REVBounds = { X = 547, Y = 0, Width = 1043, Height = 400 },
    REVNameFontSize = 48,
    REVColor = 0,
    REVNameColor = 61, -- Unsure
    REVNameOffset = 22,
    REVNameLocation = REVNameLocationType.LeftTop,
    REVOutlineMode = 1,
    REVNameOutlineMode = 1,
    NVLBounds = { X = 188, Y = 128, Width = 1536, Height = 600 },
    ADVBounds = { X = 330, Y = 795, Width = 1240, Height = 270 },
    ADVBoxSprite = "ADVBox",
    ADVBoxMask = "ADVBoxMask",
    ADVBoxNamePlateMainSprites = {},
    ADVBoxNamePlateLabelSprites = {},
    ADVBoxNamePlateMainPos = { X = 0, Y = 764 },
    ADVBoxNamePlateLabelPos = { X = 0, Y = 988 },
    ADVBoxNamePlateCount = 80,
    ADVBoxShowName = false,
    ADVBoxEffectDuration = 10,
    ADVBoxPos = { X = 0, Y = 760 },
    FadeOutDuration = 0.33,
    FadeInDuration = 0.33,
    DialogueBoxCurrentType = DialogueBoxType.CC,
    NVLBoxMaxOpacity = 0.55,
    ADVNameAlignment = TextAlignment.Left,
    ADVNameFontSize = 33,
    ADVNamePos = { X = 173, Y = 773 },
    WaitIconCurrentType = WaitIconType.None,
    WaitIconSprite = "WaitIconSprite",
    WaitIconOffset = { X = 1578, Y = 940 },
    AutoIconCurrentType = AutoIconType.SpriteAnim,
    AutoIconSpriteAnim = "AutoIconSpriteAnim",
    AutoIconOffset = { X = 1570, Y = 696 },
    SkipIconCurrentType = SkipIconType.SpriteAnim,
    SkipIconSpriteAnim = "SkipIconSpriteAnim",
    SkipIconOffset = { X = 1712, Y = 702 },
    DialogueFont = "Default",
    SetFontSizeRatio = 800.0,
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
    ColorTagIsUint8 = false
};

MakeAnimation({
    Name = "SkipIconSpriteAnim",
    Sheet = "Data",
    FirstFrameX = 0,
    FirstFrameY = 812,
    FrameWidth = 160,
    ColWidth = 160,
    FrameHeight = 154,
    RowHeight = 154,
    Frames = 9,
    Duration = 0.7,
    Rows = 1,
    Columns = 9,
    PrimaryDirection = AnimationDirections.Right
});

MakeAnimation({
    Name = "AutoIconSpriteAnim",
    Sheet = "Data",
    FirstFrameX = 0,
    FirstFrameY = 966,
    FrameWidth = 160,
    ColWidth = 160,
    FrameHeight = 154,
    RowHeight = 154,
    Frames = 9,
    Duration = 0.7,
    Rows = 1,
    Columns = 9,
    PrimaryDirection = AnimationDirections.Right
});

local namePlateMainX = 0;
local namePlateMainY = 0;
local namePlateLabelX = 3600;
local namePlateLabelY = 0;
local namePlateMainWidth = 448;
local namePlateMainHeight = 218;
local namePlateLabelWidth = 240;
local namePlateLabelHeight = 40;

for i = 1, 80 do
    root.Sprites["ADVBoxNamePlateMain" .. i] = {
        Sheet = "NamePlate",
        Bounds = {
            X = namePlateMainX,
            Y = namePlateMainY,
            Width = namePlateMainWidth,
            Height = namePlateMainHeight
        }
    };
    root.Dialogue.ADVBoxNamePlateMainSprites[#root.Dialogue.ADVBoxNamePlateMainSprites + 1] = "ADVBoxNamePlateMain" .. i;

    if i % 8 == 0 then
        namePlateMainY = namePlateMainY + namePlateMainHeight;
        namePlateMainX = 0;
    else
        namePlateMainX = namePlateMainX + namePlateMainWidth;
    end

    root.Sprites["ADVBoxNamePlateLabel" .. i] = {
        Sheet = "NamePlate",
        Bounds = {
            X = namePlateLabelX,
            Y = namePlateLabelY,
            Width = namePlateLabelWidth,
            Height = namePlateLabelHeight
        }
    };
    root.Dialogue.ADVBoxNamePlateLabelSprites[#root.Dialogue.ADVBoxNamePlateLabelSprites + 1] = "ADVBoxNamePlateLabel" .. i;

    if i % 2 == 0 then
        namePlateLabelX = 3600;
        namePlateLabelY = namePlateLabelY + namePlateLabelHeight;
    else
        namePlateLabelX = namePlateLabelX + namePlateLabelWidth;
    end
end

include('cc/nametag.lua');