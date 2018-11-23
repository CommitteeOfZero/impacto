if (root.Animations === undefined) root.Animations = {};

AnimationDirections = {
    Left = 0,
    Right = 1,
    Down = 2,
    Up = 3
};

DeleteAnimation = function (name) {
    if (root.Animations[name] !== undefined) {
        for (var i = 0; i < root.Animations[name].Frames.length; i++) {
            delete root.Sprites[root.Animations[name].Frames[i]];
        }
        delete root.Animations[name];
    }
};

MakeAnimation = function (desc) {
    DeleteAnimation(desc.Name);

    var animation = {
        Duration: desc.Duration,
        Frames: []
    };
    var currentRow = 0;
    var currentCol = 0;
    var needMoveSecondary = false;

    for (var i = 0; i < desc.Frames; i++) {
        root.Sprites[desc.Name + i] = {
            Sheet: desc.Sheet,
            Bounds: {
                X: desc.FirstFrameX + currentCol * desc.ColWidth,
                Y: desc.FirstFrameY + currentRow * desc.RowHeight,
                Width: desc.FrameWidth,
                Height: desc.FrameHeight
            }
        };
        if (desc.BaseScale !== undefined) {
            root.Sprites[desc.Name + i].BaseScale = desc.BaseScale;
        }
        animation.Frames.push(desc.Name + i);

        switch (desc.PrimaryDirection) {
            case AnimationDirections.Left:
                currentCol--;
                if (currentCol == -desc.Columns) {
                    currentCol = 0;
                    needMoveSecondary = true;
                }
                break;
            case AnimationDirections.Right:
                currentCol++;
                if (currentCol == desc.Columns) {
                    currentCol = 0;
                    needMoveSecondary = true;
                }
                break;
            case AnimationDirections.Up:
                currentRow--;
                if (currentRow == -desc.Rows) {
                    currentRow = 0;
                    needMoveSecondary = true;
                }
                break;
            case AnimationDirections.Down:
                currentRow++;
                if (currentRow == desc.Rows) {
                    currentRow = 0;
                    needMoveSecondary = true;
                }
                break;
        }

        if (needMoveSecondary) {
            needMoveSecondary = false;
            switch (desc.SecondaryDirection) {
                case AnimationDirections.Left:
                    currentCol--;
                    break;
                case AnimationDirections.Right:
                    currentCol++;
                    break;
                case AnimationDirections.Up:
                    currentRow--;
                    break;
                case AnimationDirections.Down:
                    currentRow++;
                    break;
            }
        }
    }

    root.Animations[desc.Name] = animation;
};