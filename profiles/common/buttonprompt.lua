
function GetButtonSprite(x, y, sheet)
  return {
    Sheet = sheet,
    Bounds = { X = x * 64, Y = y * 64, Width = 64, Height = 64 },
    BaseScale = { X = 1, Y = 1 }
  }
end

-- Controller Sprites
root.Sprites["ButtonIconNorth"]          = GetButtonSprite(0, 6, "ControllerButtonIcons");
root.Sprites["ButtonIconEast"]           = GetButtonSprite(9, 7, "ControllerButtonIcons");
root.Sprites["ButtonIconSouth"]          = GetButtonSprite(2, 7, "ControllerButtonIcons");
root.Sprites["ButtonIconWest"]           = GetButtonSprite(6, 7, "ControllerButtonIcons");
root.Sprites["ButtonIconDpad"]           = GetButtonSprite(5, 6, "ControllerButtonIcons");
root.Sprites["ButtonIconDpadLeftRight"]  = GetButtonSprite(10, 6, "ControllerButtonIcons");
root.Sprites["ButtonIconDpadUpDown"]     = GetButtonSprite(8, 5, "ControllerButtonIcons");
root.Sprites["ButtonIconLeftStick"]      = GetButtonSprite(2, 2, "ControllerButtonIcons");
root.Sprites["ButtonIconLeftStick"]      = GetButtonSprite(2, 2, "ControllerButtonIcons");
root.Sprites["ButtonIconRightStick"]     = GetButtonSprite(10, 2, "ControllerButtonIcons");
root.Sprites["ButtonIconBack"]           = GetButtonSprite(9, 3, "ControllerButtonIcons");
root.Sprites["ButtonIconGuide"]          = GetButtonSprite(8, 10, "ControllerButtonIcons");
root.Sprites["ButtonIconStart"]          = GetButtonSprite(0, 2, "ControllerButtonIcons");
root.Sprites["ButtonIconLeftShoulder"]   = GetButtonSprite(6, 8, "ControllerButtonIcons");
root.Sprites["ButtonIconRightShoulder"]  = GetButtonSprite(8, 8, "ControllerButtonIcons");
root.Sprites["ButtonIconLeftTrigger"]    = GetButtonSprite(10, 10, "ControllerButtonIcons");
root.Sprites["ButtonIconRightTrigger"]   = GetButtonSprite(5, 9, "ControllerButtonIcons");

-- Keyboard Sprites
root.Sprites["ButtonIconArrow"]          = GetButtonSprite(3, 13, "KeyboardButtonIcons");
root.Sprites["ButtonIconArrowUpDown"]    = GetButtonSprite(0, 12, "KeyboardButtonIcons");
root.Sprites["ButtonIconArrowLeftRight"] = GetButtonSprite(8, 13, "KeyboardButtonIcons");
root.Sprites["ButtonIconA"]              = GetButtonSprite(4, 14, "KeyboardButtonIcons");
root.Sprites["ButtonIconD"]              = GetButtonSprite(1, 10, "KeyboardButtonIcons");
root.Sprites["ButtonIconF"]              = GetButtonSprite(0, 9, "KeyboardButtonIcons");
root.Sprites["ButtonIconQ"]              = GetButtonSprite(9, 5, "KeyboardButtonIcons");
root.Sprites["ButtonIconE"]              = GetButtonSprite(5, 10, "KeyboardButtonIcons");
root.Sprites["ButtonIconR"]              = GetButtonSprite(14, 5, "KeyboardButtonIcons");
root.Sprites["ButtonIconS"]              = GetButtonSprite(2, 4, "KeyboardButtonIcons");
root.Sprites["ButtonIconW"]              = GetButtonSprite(2, 2, "KeyboardButtonIcons");
root.Sprites["ButtonIconX"]              = GetButtonSprite(6, 2, "KeyboardButtonIcons");
root.Sprites["ButtonIconZ"]              = GetButtonSprite(10, 2, "KeyboardButtonIcons");
root.Sprites["ButtonIconC"]              = GetButtonSprite(2, 11, "KeyboardButtonIcons");
root.Sprites["ButtonIconSpace"]          = GetButtonSprite(2, 3, "KeyboardButtonIcons");
root.Sprites["ButtonIconCtrl"]           = GetButtonSprite(16, 11, "KeyboardButtonIcons");
root.Sprites["ButtonIconDel"]            = GetButtonSprite(3, 10, "KeyboardButtonIcons");
root.Sprites["ButtonIconEnter"]          = GetButtonSprite(0, 4, "KeyboardButtonIcons");
root.Sprites["ButtonIconEscape"]         = GetButtonSprite(13, 10, "KeyboardButtonIcons");
root.Sprites["ButtonIconBackspace"]      = GetButtonSprite(6, 12, "KeyboardButtonIcons");
root.Sprites["ButtonIconInsert"]         = GetButtonSprite(2, 7, "KeyboardButtonIcons");
root.Sprites["ButtonIconLeftBracket"]    = GetButtonSprite(0, 11, "KeyboardButtonIcons");
root.Sprites["ButtonIconRightBracket"]   = GetButtonSprite(11, 12, "KeyboardButtonIcons");
root.Sprites["ButtonIconHome"]           = GetButtonSprite(14, 8, "KeyboardButtonIcons");
root.Sprites["ButtonIconEnd"]            = GetButtonSprite(6, 10, "KeyboardButtonIcons");
root.Sprites["ButtonIconPageUp"]         = GetButtonSprite(14, 6, "KeyboardButtonIcons");
root.Sprites["ButtonIconPageDown"]       = GetButtonSprite(12, 6, "KeyboardButtonIcons");
root.Sprites["ButtonIconNumber1"]        = GetButtonSprite(3, 15, "KeyboardButtonIcons");
root.Sprites["ButtonIconKeypad0"]        = GetButtonSprite(1, 15, "KeyboardButtonIcons");
root.Sprites["ButtonIconKeypad1"]        = GetButtonSprite(3, 15, "KeyboardButtonIcons");
root.Sprites["ButtonIconKeypad2"]        = GetButtonSprite(5, 15, "KeyboardButtonIcons");
root.Sprites["ButtonIconKeypad3"]        = GetButtonSprite(7, 15, "KeyboardButtonIcons");
root.Sprites["ButtonIconKeypad4"]        = GetButtonSprite(9, 15, "KeyboardButtonIcons");
root.Sprites["ButtonIconKeypad6"]        = GetButtonSprite(13, 15, "KeyboardButtonIcons");
root.Sprites["ButtonIconKeypad7"]        = GetButtonSprite(15, 15, "KeyboardButtonIcons");
root.Sprites["ButtonIconKeypad8"]        = GetButtonSprite(0, 14, "KeyboardButtonIcons");
root.Sprites["ButtonIconKeypad9"]        = GetButtonSprite(2, 14, "KeyboardButtonIcons");
root.Sprites["ButtonIconKeypadDivide"]   = GetButtonSprite(14, 4, "KeyboardButtonIcons");
root.Sprites["ButtonIconKeypadMinus"]    = GetButtonSprite(12, 7, "KeyboardButtonIcons");
root.Sprites["ButtonIconKeypadPlus"]     = GetButtonSprite(5, 5, "KeyboardButtonIcons");
root.Sprites["ButtonIconPeriod"]         = GetButtonSprite(3, 5, "KeyboardButtonIcons");
root.Sprites["ButtonIconMouseWheel"]     = GetButtonSprite(9, 1, "KeyboardButtonIcons")
root.Sprites["ButtonIconF1"]             = GetButtonSprite(2, 9, "KeyboardButtonIcons");
root.Sprites["ButtonIconF2"]             = GetButtonSprite(10, 9, "KeyboardButtonIcons");
root.Sprites["ButtonIconF3"]             = GetButtonSprite(12, 9, "KeyboardButtonIcons");
root.Sprites["ButtonIconF4"]             = GetButtonSprite(14, 9, "KeyboardButtonIcons");
root.Sprites["ButtonIconF5"]             = GetButtonSprite(16, 9, "KeyboardButtonIcons");
root.Sprites["ButtonIconF6"]             = GetButtonSprite(1, 8, "KeyboardButtonIcons");
root.Sprites["ButtonIconF7"]             = GetButtonSprite(3, 8, "KeyboardButtonIcons");
root.Sprites["ButtonIconF8"]             = GetButtonSprite(5, 8, "KeyboardButtonIcons");
root.Sprites["ButtonIconF9"]             = GetButtonSprite(7, 8, "KeyboardButtonIcons");
root.Sprites["ButtonIconF10"]            = GetButtonSprite(4, 9, "KeyboardButtonIcons");
root.Sprites["ButtonIconF11"]            = GetButtonSprite(6, 9, "KeyboardButtonIcons");
root.Sprites["ButtonIconF12"]            = GetButtonSprite(8, 9, "KeyboardButtonIcons");

root.ButtonPromptIcons = {
  Controller = {
    [ControllerButton.NORTH] = "ButtonIconNorth",
    [ControllerButton.EAST] = "ButtonIconEast",
    [ControllerButton.SOUTH] = "ButtonIconSouth",
    [ControllerButton.WEST] = "ButtonIconWest",
    [ControllerButton.LEFT_SHOULDER] = "ButtonIconLeftShoulder",
    [ControllerButton.RIGHT_SHOULDER] = "ButtonIconRightShoulder",
    ["Directional"] = "ButtonIconDpad",
    ["ArrowUpDown"] = "ButtonIconDpadUpDown",
    ["ArrowLeftRight"] = "ButtonIconDpadLeftRight",
    ["Scroll"] = "ButtonIconRightStick",
  },
  Keyboard = {
    [KeyboardScanCode._UP] = "ButtonIconArrow",
    [KeyboardScanCode._DOWN] = "ButtonIconArrow",
    [KeyboardScanCode._LEFT] = "ButtonIconArrow",
    [KeyboardScanCode._RIGHT] = "ButtonIconArrow",
    [KeyboardScanCode._W] = "ButtonIconW",
    [KeyboardScanCode._A] = "ButtonIconA",
    [KeyboardScanCode._S] = "ButtonIconS",
    [KeyboardScanCode._D] = "ButtonIconD",
    [KeyboardScanCode._X] = "ButtonIconX",
    [KeyboardScanCode._Q] = "ButtonIconQ",
    [KeyboardScanCode._E] = "ButtonIconE",
    [KeyboardScanCode._Z] = "ButtonIconZ",
    [KeyboardScanCode._C] = "ButtonIconC",
    [KeyboardScanCode._R] = "ButtonIconR",
    [KeyboardScanCode._F] = "ButtonIconF",
    [KeyboardScanCode._RETURN] = "ButtonIconEnter",
    [KeyboardScanCode._KP_ENTER] = "ButtonIconEnter",
    [KeyboardScanCode._SPACE] = "ButtonIconSpace",
    [KeyboardScanCode._BACKSPACE] = "ButtonIconBackspace",
    [KeyboardScanCode._DELETE] = "ButtonIconDel",
    [KeyboardScanCode._INSERT] = "ButtonIconInsert",
    [KeyboardScanCode._HOME] = "ButtonIconHome",
    [KeyboardScanCode._END] = "ButtonIconEnd",
    [KeyboardScanCode._PAGEUP] = "ButtonIconPageUp",
    [KeyboardScanCode._PAGEDOWN] = "ButtonIconPageDown",
    [KeyboardScanCode._1] = "ButtonIconNumber1",
    [KeyboardScanCode._KP_0] = "ButtonIconKeypad0",
    [KeyboardScanCode._KP_1] = "ButtonIconKeypad1",
    [KeyboardScanCode._KP_2] = "ButtonIconKeypad2",
    [KeyboardScanCode._KP_3] = "ButtonIconKeypad3",
    [KeyboardScanCode._KP_4] = "ButtonIconKeypad4",
    [KeyboardScanCode._KP_6] = "ButtonIconKeypad6",
    [KeyboardScanCode._KP_7] = "ButtonIconKeypad7",
    [KeyboardScanCode._KP_8] = "ButtonIconKeypad8",
    [KeyboardScanCode._KP_9] = "ButtonIconKeypad9",
    [KeyboardScanCode._KP_DIVIDE] = "ButtonIconKeypadDivide",
    [KeyboardScanCode._KP_MINUS] = "ButtonIconKeypadMinus",
    [KeyboardScanCode._KP_PLUS] = "ButtonIconKeypadPlus",
    [KeyboardScanCode._COMMA] = "ButtonIconPeriod",
    [KeyboardScanCode._PERIOD] = "ButtonIconPeriod",
    [KeyboardScanCode._LEFTBRACKET] = "ButtonIconLeftBracket",
    [KeyboardScanCode._RIGHTBRACKET] = "ButtonIconRightBracket",
    [KeyboardScanCode._LCTRL] = "ButtonIconCtrl",
    [KeyboardScanCode._RCTRL] = "ButtonIconCtrl",
    [KeyboardScanCode._ESCAPE] = "ButtonIconEscape",
    [KeyboardScanCode._F1] = "ButtonIconF1",
    [KeyboardScanCode._F2] = "ButtonIconF2",
    [KeyboardScanCode._F3] = "ButtonIconF3",
    [KeyboardScanCode._F4] = "ButtonIconF4",
    [KeyboardScanCode._F5] = "ButtonIconF5",
    [KeyboardScanCode._F6] = "ButtonIconF6",
    [KeyboardScanCode._F7] = "ButtonIconF7",
    [KeyboardScanCode._F8] = "ButtonIconF8",
    [KeyboardScanCode._F9] = "ButtonIconF9",
    [KeyboardScanCode._F10] = "ButtonIconF10",
    [KeyboardScanCode._F11] = "ButtonIconF11",
    [KeyboardScanCode._F12] = "ButtonIconF12",
    ["Directional"] = "ButtonIconArrow",
    ["ArrowUpDown"] = "ButtonIconArrowUpDown",
    ["ArrowLeftRight"] = "ButtonIconArrowLeftRight",
    ["Scroll"] = "ButtonIconMouseWheel",
  },
}

function UpdateButtonSpriteScale(scale) 
  for inputKey, spriteKey in pairs(root.ButtonPromptIcons.Keyboard) do
    root.Sprites[spriteKey].BaseScale = scale;
  end
  for inputKey, spriteKey in pairs(root.ButtonPromptIcons.Controller) do
    root.Sprites[spriteKey].BaseScale = scale;
  end
end


root.SpriteSheets["ControllerButtonIcons"] = {
    Path = "resources/common/controller_glyphs.png",
    DesignWidth = 704,
    DesignHeight = 704,
}
root.SpriteSheets["KeyboardButtonIcons"] = {
    Path = "resources/common/keyboard_glyphs.png",
    DesignWidth = 1088,
    DesignHeight = 1024,
}