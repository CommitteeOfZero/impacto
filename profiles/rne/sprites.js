root.SpriteSheets = {
    "Data": {
        Path: { Mount: "system", Id: 9 },
        DesignWidth: 2048,
        DesignHeight: 1024
    },
    "Font": {
        Path: { Mount: "system", Id: 12 },
        DesignWidth: 2048,
        DesignHeight: 1600
    }
};

root.Sprites = {
    "ADVBox": {
        Sheet: "Data",
        Bounds: { X: 768, Y: 807, Width: 1280, Height: 206 },
        // Even though the sprite is sized for 1280x720, they draw it unscaled at 960x544, cutting off the remainder
        BaseScale: { X: 1280 / 960, Y: 720 / 544 }
    },
    "DialogueWaitIcon": {
        Sheet: "Data",
        Bounds: { X: 1, Y: 97, Width: 32, Height: 32 }
    }
};