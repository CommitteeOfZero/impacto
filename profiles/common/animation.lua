if root.Animations == nil then root.Animations = {} end
AnimationDirections = {
    Left = 0,
    Right = 1,
    Down = 2,
    Up = 3
}

function DeleteAnimation(name)
    if root.Animations[name] ~= nil then
        for i = 1, #root.Animations[name].Frames do
            root.Sprites[root.Animations[name].Frames[i]] = nil
        end
        root.Animations[name] = nil
    end
end

function MakeAnimation(desc)
    DeleteAnimation(desc.Name)
    local animation = {
        Duration = desc.Duration,
        Frames = {}
    }
    local currentRow = 0
    local currentCol = 0
    local needMoveSecondary = false
    for i = 1, desc.Frames do
        root.Sprites[desc.Name .. i] = {
            Sheet = desc.Sheet,
            Bounds = {
                X = desc.FirstFrameX + currentCol * desc.ColWidth,
                Y = desc.FirstFrameY + currentRow * desc.RowHeight,
                Width = desc.FrameWidth,
                Height = desc.FrameHeight
            }
        }
        if desc.BaseScale ~= nil then
            root.Sprites[desc.Name .. i].BaseScale = desc.BaseScale
        end
        animation.Frames[#animation.Frames + 1] = desc.Name .. i
        if desc.PrimaryDirection == AnimationDirections.Left then
            currentCol = currentCol - 1
            if currentCol == -desc.Columns then
                currentCol = 0
                needMoveSecondary = true
            end
        elseif desc.PrimaryDirection == AnimationDirections.Right then
            currentCol = currentCol + 1
            if currentCol == desc.Columns then
                currentCol = 0
                needMoveSecondary = true
            end
        elseif desc.PrimaryDirection == AnimationDirections.Up then
            currentRow = currentRow - 1
            if currentRow == -desc.Rows then
                currentRow = 0
                needMoveSecondary = true
            end
        elseif desc.PrimaryDirection == AnimationDirections.Down then
            currentRow = currentRow + 1
            if currentRow == desc.Rows then
                currentRow = 0
                needMoveSecondary = true
            end
        end
        if needMoveSecondary then
            needMoveSecondary = false
            if desc.SecondaryDirection == AnimationDirections.Left then
                currentCol = currentCol - 1
            elseif desc.SecondaryDirection == AnimationDirections.Right then
                currentCol = currentCol + 1
            elseif desc.SecondaryDirection == AnimationDirections.Up then
                currentRow = currentRow - 1
            elseif desc.SecondaryDirection == AnimationDirections.Down then
                currentRow = currentRow + 1
            end
        end
    end
    root.Animations[desc.Name] = animation
end
