root.Sprites["ADVBox"] = {
    Sheet = "MesBox",
    Bounds = { X = 0, Y = 1, Width = 1920, Height = 298 }
};

root.Sprites["ADVBoxMask"] = {
    Sheet = "MesBox",
    Bounds = { X = 0, Y = 301, Width = 1920, Height = 298 }
};

local waitIconPos = { X = 1583, Y = 907 };

root.Dialogue = {
    TipsMessageModeIdx = 7,
    TipsColorIndex = 10,
    REVMessageModeIdx = 9,
    REVColor = 10, -- Absolute guess
    REVNameColor = 0,
    REVOutlineMode = 0,
    REVNameOutlineMode = 2,
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

    NametagCurrentType = NametagType.CC,

    NametagMainSprites = {},
    NametagLabelSprites = {},

    NametagMainPos = { X = 0, Y = 780 },
    NametagLabelPos = { X = 0, Y = 998 },

    NametagShowDuration = 16 / 60;

    WaitIconCurrentType = WaitIconType.FixedSpriteAnimation,
    WaitIconSpriteAnimation = "WaitIconSpriteAnimation",
    KeyWaitIconPos = waitIconPos,
    WaitIconOffset = { X = 0, Y = 0 },
    WaitIconDrawableWithoutTextbox = false,

    AutoIconCurrentType = AutoIconType.SpriteAnimFixed,
    AutoIconSpriteAnim = "AutoIconSpriteAnimation",
    AutoIconOffset = { X = 1571, Y = 691 },

    SkipIconCurrentType = SkipIconType.SpriteAnimFixed,
    SkipIconSpriteAnim = "SkipIconSpriteAnimation",
    SkipIconOffset = { X = 1691, Y = 791 },

    DialogueFont = "Default",
    SetFontSizeRatio = 800.0,
    DefaultFontSize = 42,
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

    -- MAGES. engine hardcodes FixedPos in the binary regardless of WaitIconDispMode
    -- We force it to comply by overriding them in the profile
    TextModesInfo = {
        ["0"] = { -- ADV box
            WaitIconDispMode = WaitIconDispModeType.FixedPos,
            WaitIconPos = waitIconPos,
        },
        ["1"] = { -- NVL box
            WaitIconDispMode = WaitIconDispModeType.FixedPos,
            WaitIconPos = waitIconPos,
        },
    }
};

if root.Language == "English" then
    root.Dialogue.TextModesInfo =  {
        ["0"] = { -- ADV box
            WindowPos = { X = 220 * 1.5, Y = (518 + 11 + 5 + 1) * 1.5 }, -- Correct for missing ruby space
            WaitIconDispMode = WaitIconDispModeType.FixedPos,
            WaitIconPos = waitIconPos,
            TextGlyphSize = { X = 38, Y = 38 },
            LineSpacing = (1 + 5 + 1) * 1.5 - 3, -- (RubySpacing + RubyHeight + LineSpacing) * 1.5 - 3
            AlwaysAddRubySpacing = false,
        },
        ["1"] = { -- NVL box
            WaitIconDispMode = WaitIconDispModeType.FixedPos,
            WaitIconPos = waitIconPos,
            TextGlyphSize = { X = 38, Y = 38 },
            LineSpacing = (1 + 5 + 1) * 1.5 - 3,
            AlwaysAddRubySpacing = false,
        },
        ["7"] = { -- Tips box
            TextGlyphSize = { X = 38, Y = 38 },
            LineSpacing = (1 + 5 + 1) * 1.5 - 3,
            AlwaysAddRubySpacing = false,
        },
        ["9"] = { -- Backlog entry
            NameGlyphSize = { X = 38, Y = 38 },
            TextGlyphSize = { X = 38, Y = 38 },
            LineSpacing = (1 + 5 + 1) * 1.5 - 3,
            AlwaysAddRubySpacing = false,
        }
    }
end

MakeFixedSpriteAnimation({
    Name = "WaitIconSpriteAnimation",
    Sheet = "Data",
    FirstFrameX = 853,
    FirstFrameY = 889,
    FrameWidth = 216,
    ColWidth = 216,
    FrameHeight = 216,
    RowHeight = 216,
    Frames = 10,
    FixedFrameIdx = 6,
    Duration = 5 * 2 / 60,
    Rows = 1,
    Columns = 10,
    PrimaryDirection = AnimationDirections.Right
});

MakeFixedSpriteAnimation({
    Name = "AutoIconSpriteAnimation",
    Sheet = "Data",
    FirstFrameX = 852,
    FirstFrameY = 457,
    FrameWidth = 216,
    ColWidth = 216,
    FrameHeight = 216,
    RowHeight = 216,
    Frames = 10,
    FixedFrameIdx = 6,
    Duration = 5 * 2 / 60,
    Rows = 1,
    Columns = 10,
    PrimaryDirection = AnimationDirections.Right
});

MakeFixedSpriteAnimation({
    Name = "SkipIconSpriteAnimation",
    Sheet = "Data",
    FirstFrameX = 852,
    FirstFrameY = 673,
    FrameWidth = 216,
    ColWidth = 216,
    FrameHeight = 216,
    RowHeight = 216,
    Frames = 10,
    FixedFrameIdx = 6,
    Duration = 5 * 2 / 60,
    Rows = 1,
    Columns = 10,
    PrimaryDirection = AnimationDirections.Right
});

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
