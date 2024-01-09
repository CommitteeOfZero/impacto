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
        Position: {
            X: desc.ScreenX,
            Y: desc.ScreenY,
        },
        LeftSprite: "ADVNameTagLeft",
        LineSprite: "ADVNameTagLine",
        RightSprite: "ADVNameTagRight",
        BaseLineWidth: desc.BaseLineWidth
    };
};

MakeSecondaryNameTag = function (desc) {
    root.Sprites["SecondaryADVNameTagLeft"] = {
        Sheet: desc.Sheet,
        Bounds: { X: desc.X, Y: desc.Y, Width: desc.LeftWidth, Height: desc.Height }
    };
    root.Sprites["SecondaryADVNameTagLine"] = {
        Sheet: desc.Sheet,
        Bounds: { X: desc.X + desc.LeftWidth, Y: desc.Y, Width: desc.LineWidth, Height: desc.Height }
    };
    root.Sprites["SecondaryADVNameTagRight"] = {
        Sheet: desc.Sheet,
        Bounds: { X: desc.X + desc.LeftWidth + desc.LineWidth, Y: desc.Y, Width: desc.RightWidth, Height: desc.Height }
    };
    if (desc.BaseScale !== undefined) {
        root.Sprites["SecondaryADVNameTagLeft"].BaseScale = desc.BaseScale;
        root.Sprites["SecondaryADVNameTagLine"].BaseScale = desc.BaseScale;
        root.Sprites["SecondaryADVNameTagRight"].BaseScale = desc.BaseScale;
    }

    root.Dialogue.SecondaryADVNameTag = {
        SecondaryPosition: {
            X: desc.ScreenX,
            Y: desc.ScreenY,
        },
        SecondaryLeftSprite: "SecondaryADVNameTagLeft",
        SecondaryLineSprite: "SecondaryADVNameTagLine",
        SecondaryRightSprite: "SecondaryADVNameTagRight",
        SecondaryBaseLineWidth: desc.BaseLineWidth
    };
};