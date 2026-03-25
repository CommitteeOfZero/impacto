root.Input.KBcustom = {
     [0] = {40},     -- Return
     [1] = {42},     -- Backspace
     [2] = {60},     -- F3
     [3] = {59},     -- F2
     [4] = {61},     -- F4
     [5] = {30},     -- 1
     [6] = {82},     -- Up
     [7] = {81},     -- Down
     [8] = {80},     -- Left
     [9] = {79},     -- Right
    [10] = {20,224}, -- Q,LCTRL
    [11] = {29},     -- Z
    [12] = {32},     -- 3
    [13] = {8},      -- E
    [14] = {6},      -- C
    [15] = {65},     -- F8
    [16] = {47},     -- [
    [17] = {48},     -- ]
}

root.Input.PADtoKBcustom = {
    [root.PADinput.PAD1A]       = {{ Id =  0, Mode = KbInputMode.Any  }},
    [root.PADinput.PAD1B]       = {{ Id =  1, Mode = KbInputMode.Any  }},
    [root.PADinput.PAD1X]       = {{ Id =  2, Mode = KbInputMode.Any  }},
    [root.PADinput.PAD1Y]       = {{ Id =  3, Mode = KbInputMode.Any  }},
    [root.PADinput.PAD1SELECT]  = {{ Id =  4, Mode = KbInputMode.Any  }},
    [root.PADinput.PAD1START]   = {{ Id =  5, Mode = KbInputMode.Any  }},
    [root.PADinput.PAD1UP]      = {{ Id =  6, Mode = KbInputMode.Any  }},
    [root.PADinput.PAD1DOWN]    = {{ Id =  7, Mode = KbInputMode.Any  }},
    [root.PADinput.PAD1LEFT]    = {{ Id =  8, Mode = KbInputMode.Any  }},
    [root.PADinput.PAD1RIGHT]   = {{ Id =  9, Mode = KbInputMode.Any  }},
    [root.PADinput.PAD1L1]      = {{ Id = 10, Mode = KbInputMode.Any  }},
    [root.PADinput.PAD1L2]      = {{ Id = 11, Mode = KbInputMode.Any  }},
    [root.PADinput.PAD1L3]      = {{ Id = 12, Mode = KbInputMode.Any  }},
    [root.PADinput.PAD1R1]      = {{ Id = 13, Mode = KbInputMode.Any  }},
    [root.PADinput.PAD1R2]      = {{ Id = 14, Mode = KbInputMode.Any  }},
    [root.PADinput.PAD1R3]      = {{ Id = 15, Mode = KbInputMode.Any  }},
    [root.PADinput.PAD1UP_RS]   = {{ Id = 16, Mode = KbInputMode.Any  }},
    [root.PADinput.PAD1DOWN_RS] = {{ Id = 17, Mode = KbInputMode.Any  }},
}

local AllDir = root.PADinput.PAD1UP | root.PADinput.PAD1DOWN | root.PADinput.PAD1LEFT | root.PADinput.PAD1RIGHT;

root.Input.PADcustomA = {
    root.PADinput.PAD1UP,
    root.PADinput.PAD1DOWN,
    root.PADinput.PAD1LEFT,
    root.PADinput.PAD1RIGHT,
    root.PADinput.PAD1A | root.PADinput.PAD1START,
    root.PADinput.PAD1A,
    root.PADinput.PAD1B,
    root.PADinput.PAD1R1,
    root.PADinput.PAD1START,
    root.PADinput.PAD1SELECT,
    root.PADinput.PAD1X,
    0,
    root.PADinput.PAD1L2,
    root.PADinput.PAD1R3,
    root.PADinput.PAD1A | root.PADinput.PAD1B | root.PADinput.PAD1START,
    root.PADinput.PAD1A,
    root.PADinput.PAD1B,
    root.PADinput.PAD1X,
    root.PADinput.PAD1Y,
    root.PADinput.PAD1R1,
    root.PADinput.PAD1R2,
    root.PADinput.PAD1L1,
    root.PADinput.PAD1Y,
    AllDir | root.PADinput.PAD1A,
    root.PADinput.PAD1R1,
    root.PADinput.PAD1L1,
    root.PADinput.PAD1A,
    root.PADinput.PAD1B,
    root.PADinput.PAD1UP_LS | root.PADinput.PAD1UP_DIRECT,
    root.PADinput.PAD1DOWN_LS | root.PADinput.PAD1DOWN_DIRECT,
    root.PADinput.PAD1LEFT_LS | root.PADinput.PAD1LEFT_DIRECT,
    root.PADinput.PAD1RIGHT_LS | root.PADinput.PAD1RIGHT_DIRECT,
    root.PADinput.PAD1UP_RS,
    root.PADinput.PAD1DOWN_RS,
    root.PADinput.PAD1LEFT_RS,
    root.PADinput.PAD1RIGHT_RS,
    0x20000000,
    0x40000000,
    0x80000000,
    0,
    0
}