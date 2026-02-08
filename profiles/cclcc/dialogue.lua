root.Sprites["ADVBox"] = {
    Sheet = "MesBox",
    Bounds = { X = 0, Y = 1, Width = 1920, Height = 298 }
};

root.Sprites["ADVBoxMask"] = {
    Sheet = "MesBox",
    Bounds = { X = 0, Y = 301, Width = 1920, Height = 298 }
};

root.Dialogue = {
    TipsBounds = { X = 1042, Y = 394, Width = 720, Height = 542 },
    TipsColorIndex = 10,
    REVBounds = { X = 547, Y = 0, Width = 1043, Height = 400 },
    REVNameFontSize = 42,
    REVColor = 10, -- Absolute guess
    REVNameColor = 0,
    REVNameOffset = 18,
    REVNameLocation = REVNameLocationType.LeftTop,
    REVOutlineMode = 0,
    REVNameOutlineMode = 2,
    NVLBounds = { X = 188, Y = 128, Width = 1536, Height = 600 },
    ADVBounds = { X = 330, Y = 800, Width = 1250, Height = 270 },
    ADVBoxSprite = "ADVBox",
    ADVBoxMask = "ADVBoxMask",
    ADVBoxEffectDuration = 10,
    ADVBoxPos = { X = 0, Y = 760 },
    FadeOutDuration = 0.33,
    FadeInDuration = 0.33,
    TextFadeInDuration = 0.33,
    TextFadeOutDuration = 0.33,
    DialogueBoxCurrentType = DialogueBoxType.CC,
    NVLBoxMaxOpacity = 0.55,

    ADVNameAlignment = TextAlignment.Left,
    ADVNameFontSize = 33,
    ADVNamePos = { X = 173, Y = 773 },

    NametagCurrentType = NametagType.CC,

    NametagMainSprites = {},
    NametagLabelSprites = {},

    NametagMainPos = { X = 0, Y = 780 },
    NametagLabelPos = { X = 0, Y = 998 },

    NametagShowDuration = 16 / 60;

    WaitIconCurrentType = WaitIconType.SpriteAnimFixed,
    WaitIconSpriteAnim = "WaitIconSpriteAnim",
    WaitIconAnimationDuration = 0.7,
    WaitIconOffset = { X = 1595, Y = 895 },
    WaitIconFixedSpriteId = 6,
    AutoIconCurrentType = AutoIconType.SpriteAnimFixed,
    AutoIconSpriteAnim = "AutoIconSpriteAnim",
    AutoIconOffset = { X = 1568, Y = 690 },
    AutoIconFixedSpriteId = 6,
    SkipIconCurrentType = SkipIconType.SpriteAnimFixed,
    SkipIconSpriteAnim = "SkipIconSpriteAnim",
    SkipIconOffset = { X = 1688, Y = 794 },
    SkipIconFixedSpriteId = 6,
    DialogueFont = "Default",
    SetFontSizeRatio = 800.0,
    DefaultFontSize = 42,
    RubyFontSize = 21,
    RubyYOffset = -21,
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
};

MakeAnimation({
    Name = "WaitIconSpriteAnim",
    Sheet = "Data",
    FirstFrameX = 864, --1114
    FirstFrameY = 877,
    FrameWidth = 190,
    ColWidth = 216,
    FrameHeight = 185,
    RowHeight = 185,
    Frames = 10,
    Duration = 1,
    Rows = 1,
    Columns = 10,
    PrimaryDirection = AnimationDirections.Right
});

MakeAnimation({
    Name = "AutoIconSpriteAnim",
    Sheet = "Data",
    FirstFrameX = 854,
    FirstFrameY = 456,
    FrameWidth = 215,
    ColWidth = 215,
    FrameHeight = 220,
    RowHeight = 220,
    Frames = 10,
    Duration = 0.7,
    Rows = 1,
    Columns = 10,
    PrimaryDirection = AnimationDirections.Right
});

MakeAnimation({
    Name = "SkipIconSpriteAnim",
    Sheet = "Data",
    FirstFrameX = 854,
    FirstFrameY = 676,
    FrameWidth = 215,
    ColWidth = 215,
    FrameHeight = 220,
    RowHeight = 220,
    Frames = 10,
    Duration = 0.7,
    Rows = 1,
    Columns = 10,
    PrimaryDirection = AnimationDirections.Right
})

local nametagMainX = 0;
local nametagMainY = 0;
local nametagLabelX = 3600;
local nametagLabelY = 0;
local nametagMainWidth = 448;
local nametagMainHeight = 232;
local nametagLabelWidth = 310;
local nametagLabelHeight = 40;

for i = 1, 47 do
    root.Sprites["NametagMainSprite" .. i] = {
        Sheet = "NamePlate",
        Bounds = {
            X = nametagMainX,
            Y = nametagMainY,
            Width = nametagMainWidth,
            Height = nametagMainHeight
        }
    };
    root.Dialogue.NametagMainSprites[#root.Dialogue.NametagMainSprites + 1] = "NametagMainSprite" .. i;

    if i % 8 == 0 then
        nametagMainY = nametagMainY + nametagMainHeight;
        nametagMainX = 0;
    else
        nametagMainX = nametagMainX + nametagMainWidth;
    end

    root.Sprites["NametagLabelSprite" .. i] = {
        Sheet = "NamePlate",
        Bounds = {
            X = nametagLabelX,
            Y = nametagLabelY,
            Width = nametagLabelWidth,
            Height = nametagLabelHeight
        }
    };
    root.Dialogue.NametagLabelSprites[#root.Dialogue.NametagLabelSprites + 1] = "NametagLabelSprite" .. i;

    nametagLabelY = nametagLabelY + nametagLabelHeight;
end
