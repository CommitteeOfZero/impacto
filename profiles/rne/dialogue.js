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
    ADVNameAlignment: TextAlignment.Left,
    ADVNameFontSize: 22,
    ADVNamePos: { X: 104, Y: 498 },
    WaitIconSprite: "DialogueWaitIcon",
    DialogueFont: "Default",
    DefaultFontSize: 32,
    LineSpacing: 22,
    RubyFontSize: 14,
    ColorTable: [
        [0xFFFFFFFF, 0x000000FF], [0x5080FFFF, 0x000000FF],
        [0xFF7080FF, 0x000000FF], [0xFFA0F8FF, 0x000000FF],
        [0x46FF80FF, 0x000000FF], [0x90FFFFFF, 0x000000FF],
        [0xFFFF70FF, 0x000000FF], [0x80FFC0FF, 0x000000FF],
        [0xFFB080FF, 0x000000FF], [0xB080FFFF, 0x000000FF],
        [0x000000FF, 0x808080FF], [0x000000FF, 0x5080FFFF],
        [0x000000FF, 0xFF7080FF], [0x000000FF, 0xFFA0F8FF],
        [0x000000FF, 0x268840FF], [0x000000FF, 0x409999FF],
        [0x000000FF, 0x888830FF], [0x000000FF, 0x80FFC0FF],
        [0x000000FF, 0xFFB080FF], [0x000000FF, 0xB080FFFF],
        [0xD0D0D0FF, 0x000000FF], [0xD0D0FFFF, 0x000000FF],
        [0xFFD0D0FF, 0x000000FF], [0xFFD0FFFF, 0x000000FF],
        [0xD0FFD0FF, 0x000000FF], [0xD0FFFFFF, 0x000000FF],
        [0xFFFFD0FF, 0x000000FF], [0xE8FFD0FF, 0x000000FF],
        [0xFFE8D0FF, 0x000000FF], [0xD0E8FFFF, 0x000000FF],
        [0xFFFFFFFF, 0x808080FF], [0xFFFFFFFF, 0x5080FFFF],
        [0xFFFFFFFF, 0xFF7080FF], [0xFFFFFFFF, 0xFFA0F8FF],
        [0xFFFFFFFF, 0x46FF80FF], [0xFFFFFFFF, 0x90FFFFFF],
        [0xFFFFFFFF, 0xFFFF70FF], [0xFFFFFFFF, 0x80FFC0FF],
        [0xFFFFFFFF, 0xFFB080FF], [0xFFFFFFFF, 0xB080FFFF],
        [0xFFEEEEFF, 0x000000FF], [0xFFCCCCFF, 0x000000FF],
        [0xFFAAAAFF, 0x000000FF], [0xFF9999FF, 0x000000FF],
        [0xFF8888FF, 0x000000FF], [0xFFFF00FF, 0x000000FF],
        [0xFEF000FF, 0x000000FF], [0xFF7777FF, 0x000000FF],
        [0xFF6666FF, 0x000000FF], [0xFF5555FF, 0x000000FF],
        [0xFF4444FF, 0x000000FF], [0xFF3333FF, 0x000000FF],
        [0xFF2222FF, 0x000000FF], [0xFF0000FF, 0x000000FF],
        [0xDD0000FF, 0x000000FF], [0xBB0000FF, 0x000000FF],
        [0xB00000FF, 0x000000FF], [0xAA0000FF, 0x000000FF],
        [0x950000FF, 0x000000FF], [0x808080FF, 0x000000FF],
        [0xAAAAAAFF, 0x000000FF], [0xAAC1C9FF, 0x000000FF],
        [0x000000FF, 0x000000FF], [0x000000FF, 0x000000FF],
        [0x000000FF, 0x000000FF], [0x000000FF, 0x000000FF],
        [0x000000FF, 0x000000FF], [0xF80B0BFF, 0x000000FF],
        [0xF8910BFF, 0x000000FF], [0x33F12AFF, 0x000000FF]
    ],
    MaxPageSize: 2000,
    PageCount: 3
};

include('rne/nametag.js');