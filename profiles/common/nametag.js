MakeNameTag = function (desc) {
    root.Sprites["ADVNameTagLeft"] = {
        Sheet: desc.Sheet,
        Bounds: { X: desc.X, Y: desc.Y, Width: desc.LeftWidth, Height: desc.Height }
    };
    root.Sprites["ADVNameTagLine"] = {
        Sheet: desc.Sheet,
        Bounds: { X: desc.X + desc.LeftWidth, Y: desc.Y, Width: desc.LineWidth, Height: desc.Height }
    };
    root.Sprites["ADVNameTagRight"] = {
        Sheet: desc.Sheet,
        Bounds: { X: desc.X + desc.LeftWidth + desc.LineWidth, Y: desc.Y, Width: desc.RightWidth, Height: desc.Height }
    };
    if (desc.BaseScale !== undefined) {
        root.Sprites["ADVNameTagLeft"].BaseScale = desc.BaseScale;
        root.Sprites["ADVNameTagLine"].BaseScale = desc.BaseScale;
        root.Sprites["ADVNameTagRight"].BaseScale = desc.BaseScale;
    }

    root.Dialogue.ADVNameTag = {
        X: desc.ScreenX,
        Y: desc.ScreenY,
        LeftSprite: "ADVNameLeft",
        LineSprite: "ADVNameLine",
        RightSprite: "ADVNameRight"
    };
};