root.Sprites["ADVBox"] = {
    Sheet: "Data",
    Bounds: { X: 768, Y: 807, Width: 1280, Height: 206 },
    // Even though the sprite is sized for 1280x720, they draw it unscaled at 960x544, cutting off the remainder
    BaseScale: { X: 1280 / 960, Y: 720 / 544 }
};

root.Sprites["DialogueWaitIcon"] = {
    Sheet: "Data",
    Bounds: { X: 1, Y: 97, Width: 32, Height: 32 }
};

root.Dialogue = {
    NVLBounds: { X: 125, Y: 85, Width: 1024, Height: 400 },
    ADVBounds: { X: 161, Y: 546, Width: 960, Height: 180 },
    ADVBoxSprite: "ADVBox",
    ADVBoxPos: { X: 0, Y: 477.8 },
    FadeOutDuration: 0.33,
    FadeInDuration: 0.33,
    DialogueBoxCurrentType: DialogueBoxType.Plain,
    NVLBoxMaxOpacity: 0.55,
    ADVNameAlignment: TextAlignment.Left,
    ADVNameFontSize: 22,
    ADVNamePos: { X: 104, Y: 498 },
    WaitIconCurrentType: WaitIconType.Rotate,
    WaitIconSprite: "DialogueWaitIcon",
    WaitIconOffset: { X: 4, Y: 4 },
    WaitIconAnimationDuration: 3.2,
    DialogueFont: "Default",
    DefaultFontSize: 32,
    RubyFontSize: 14,
    RubyYOffset: -18,
    ColorTable: [
        [0xFFFFFF, 0x000000], [0x5080FF, 0x000000],
        [0xFF7080, 0x000000], [0xFFA0F8, 0x000000],
        [0x46FF80, 0x000000], [0x90FFFF, 0x000000],
        [0xFFFF70, 0x000000], [0x80FFC0, 0x000000],
        [0xFFB080, 0x000000], [0xB080FF, 0x000000],
        [0x000000, 0x808080], [0x000000, 0x5080FF],
        [0x000000, 0xFF7080], [0x000000, 0xFFA0F8],
        [0x000000, 0x268840], [0x000000, 0x409999],
        [0x000000, 0x888830], [0x000000, 0x80FFC0],
        [0x000000, 0xFFB080], [0x000000, 0xB080FF],
        [0xD0D0D0, 0x000000], [0xD0D0FF, 0x000000],
        [0xFFD0D0, 0x000000], [0xFFD0FF, 0x000000],
        [0xD0FFD0, 0x000000], [0xD0FFFF, 0x000000],
        [0xFFFFD0, 0x000000], [0xE8FFD0, 0x000000],
        [0xFFE8D0, 0x000000], [0xD0E8FF, 0x000000],
        [0xFFFFFF, 0x808080], [0xFFFFFF, 0x5080FF],
        [0xFFFFFF, 0xFF7080], [0xFFFFFF, 0xFFA0F8],
        [0xFFFFFF, 0x46FF80], [0xFFFFFF, 0x90FFFF],
        [0xFFFFFF, 0xFFFF70], [0xFFFFFF, 0x80FFC0],
        [0xFFFFFF, 0xFFB080], [0xFFFFFF, 0xB080FF],
        [0xFFEEEE, 0x000000], [0xFFCCCC, 0x000000],
        [0xFFAAAA, 0x000000], [0xFF9999, 0x000000],
        [0xFF8888, 0x000000], [0xFFFF00, 0x000000],
        [0xFEF000, 0x000000], [0xFF7777, 0x000000],
        [0xFF6666, 0x000000], [0xFF5555, 0x000000],
        [0xFF4444, 0x000000], [0xFF3333, 0x000000],
        [0xFF2222, 0x000000], [0xFF0000, 0x000000],
        [0xDD0000, 0x000000], [0xBB0000, 0x000000],
        [0xB00000, 0x000000], [0xAA0000, 0x000000],
        [0x950000, 0x000000], [0x808080, 0x000000],
        [0xAAAAAA, 0x000000], [0xAAC1C9, 0x000000],
        [0x000000, 0x000000], [0x000000, 0x000000],
        [0x000000, 0x000000], [0x000000, 0x000000],
        [0x000000, 0x000000], [0xF80B0B, 0x000000],
        [0xF8910B, 0x000000], [0x33F12A, 0x000000]
    ],
    MaxPageSize: 2000,
    PageCount: 3,
    ColorTagIsUint8: false
};

include('rne/nametag.js');