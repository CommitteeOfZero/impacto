root.PADinput = {
    PAD1UP = 0x10000,
    PAD1DOWN = 0x20000,
    PAD1LEFT = 0x40000,
    PAD1RIGHT = 0x80000,
    PAD1START = 0x10,
    PAD1SELECT = 0x20,
    PAD1L3 = 0x40,
    PAD1R3 = 0x80,
    PAD1L1 = 0x100,
    PAD1R1 = 0x200,
    PAD1L2 = 0x400,
    PAD1R2 = 0x800,
    PAD1A = 0x1000,
    PAD1B = 0x2000,
    PAD1X = 0x4000,
    PAD1Y = 0x8000,
    PAD1UP_LS = 0x100000,
    PAD1DOWN_LS = 0x200000,
    PAD1LEFT_LS = 0x400000,
    PAD1RIGHT_LS = 0x800000,
    PAD1UP_RS = 0x1000000,
    PAD1DOWN_RS = 0x2000000,
    PAD1LEFT_RS = 0x4000000,
    PAD1RIGHT_RS = 0x8000000,
    PAD1UP_DIRECT = 0x1,
    PAD1DOWN_DIRECT = 0x2,
    PAD1LEFT_DIRECT = 0x4,
    PAD1RIGHT_DIRECT = 0x8,
};

root.Input = {
    PADtoKBcustom  = {},
    PADtoMS  = {},
    PADtoGP  = {},
    PADtoGPA = {},

    PADcustomA = {},
    PADcustomB = {},
    KBcustom = {},

}

KbInputMode = {
    Tap = 1,
    Held = 2,
    Any = 3,
}

-- Keyboard Custom Mapping
root.Input.PADtoKBcustom = {
    [root.PADinput.PAD1UP]            = {{ Id =  0, Mode = KbInputMode.Any  }, { Id = 16, Mode = KbInputMode.Any }, { Id = 20, Mode = KbInputMode.Any }},
    [root.PADinput.PAD1DOWN]          = {{ Id =  1, Mode = KbInputMode.Any  }, { Id = 17, Mode = KbInputMode.Any }, { Id = 21, Mode = KbInputMode.Any }},
    [root.PADinput.PAD1LEFT]          = {{ Id =  2, Mode = KbInputMode.Any  }, { Id = 18, Mode = KbInputMode.Any }, { Id = 22, Mode = KbInputMode.Any }},
    [root.PADinput.PAD1RIGHT]         = {{ Id =  3, Mode = KbInputMode.Any  }, { Id = 19, Mode = KbInputMode.Any }, { Id = 23, Mode = KbInputMode.Any }},
    [root.PADinput.PAD1UP_DIRECT]     = {{ Id =  0, Mode = KbInputMode.Held }},
    [root.PADinput.PAD1DOWN_DIRECT]   = {{ Id =  1, Mode = KbInputMode.Held }},
    [root.PADinput.PAD1LEFT_DIRECT]   = {{ Id =  2, Mode = KbInputMode.Held }},
    [root.PADinput.PAD1RIGHT_DIRECT]  = {{ Id =  3, Mode = KbInputMode.Held }},
    [root.PADinput.PAD1A]             = {{ Id =  4, Mode = KbInputMode.Any  }},
    [root.PADinput.PAD1B]             = {{ Id =  5, Mode = KbInputMode.Any  }},
    [root.PADinput.PAD1X]             = {{ Id =  6, Mode = KbInputMode.Any  }},
    [root.PADinput.PAD1Y]             = {{ Id =  7, Mode = KbInputMode.Any  }},
    [root.PADinput.PAD1L1]            = {{ Id =  8, Mode = KbInputMode.Any  }},
    [root.PADinput.PAD1L2]            = {{ Id =  9, Mode = KbInputMode.Any  }},
    [root.PADinput.PAD1L3]            = {{ Id = 10, Mode = KbInputMode.Any  }},
    [root.PADinput.PAD1R1]            = {{ Id = 11, Mode = KbInputMode.Any  }},
    [root.PADinput.PAD1R2]            = {{ Id = 12, Mode = KbInputMode.Any  }},
    [root.PADinput.PAD1R3]            = {{ Id = 13, Mode = KbInputMode.Any  }},
    [root.PADinput.PAD1START]         = {{ Id = 14, Mode = KbInputMode.Any  }},
    [root.PADinput.PAD1SELECT]        = {{ Id = 15, Mode = KbInputMode.Any  }},
    [root.PADinput.PAD1UP_LS]         = {{ Id = 16, Mode = KbInputMode.Any  }},
    [root.PADinput.PAD1DOWN_LS]       = {{ Id = 17, Mode = KbInputMode.Any  }},
    [root.PADinput.PAD1LEFT_LS]       = {{ Id = 18, Mode = KbInputMode.Any  }},
    [root.PADinput.PAD1RIGHT_LS]      = {{ Id = 19, Mode = KbInputMode.Any  }},
    [root.PADinput.PAD1UP_RS]         = {{ Id = 20, Mode = KbInputMode.Any  }},
    [root.PADinput.PAD1DOWN_RS]       = {{ Id = 21, Mode = KbInputMode.Any  }},
    [root.PADinput.PAD1LEFT_RS]       = {{ Id = 22, Mode = KbInputMode.Any  }},
    [root.PADinput.PAD1RIGHT_RS]      = {{ Id = 23, Mode = KbInputMode.Any  }},
}

-- Mouse
root.Input.PADtoMS = {
    [root.PADinput.PAD1A]     = 1, -- Left
    [root.PADinput.PAD1B]     = 3, -- Right
    [root.PADinput.PAD1START] = 3, -- Right
}

-- Controller
root.Input.PADtoGP = {
    [root.PADinput.PAD1A]            =  0,
    [root.PADinput.PAD1B]            =  1,
    [root.PADinput.PAD1X]            =  2,
    [root.PADinput.PAD1Y]            =  3,
    [root.PADinput.PAD1SELECT]       =  4,
    [root.PADinput.PAD1START]        =  6,
    [root.PADinput.PAD1L3]           =  7,
    [root.PADinput.PAD1R3]           =  8,
    [root.PADinput.PAD1L1]           =  9,
    [root.PADinput.PAD1R1]           = 10,
    [root.PADinput.PAD1UP_DIRECT]    = 11,
    [root.PADinput.PAD1DOWN_DIRECT]  = 12,
    [root.PADinput.PAD1LEFT_DIRECT]  = 13,
    [root.PADinput.PAD1RIGHT_DIRECT] = 14,
};

root.Input.PADtoGPA = {
    -- { Axis Id, Direction }
    [root.PADinput.PAD1UP_LS]        = {1, -1},  -- Left stick Y axis
    [root.PADinput.PAD1DOWN_LS]      = {1,  1},  -- Left stick Y axis
    [root.PADinput.PAD1LEFT_LS]      = {0, -1},  -- Left stick X axis
    [root.PADinput.PAD1RIGHT_LS]     = {0,  1},  -- Left stick X axis
    [root.PADinput.PAD1UP_RS]        = {3, -1},  -- Right stick Y axis
    [root.PADinput.PAD1DOWN_RS]      = {3,  1},  -- Right stick Y axis
    [root.PADinput.PAD1LEFT_RS]      = {2, -1},  -- Right stick X axis
    [root.PADinput.PAD1RIGHT_RS]     = {2,  1},  -- Right stick X axis
    [root.PADinput.PAD1L2]           = {4,  1},  -- Left trigger
    [root.PADinput.PAD1R2]           = {5,  1},  -- Right trigger
};


local AllDir = root.PADinput.PAD1UP | root.PADinput.PAD1DOWN | root.PADinput.PAD1LEFT | root.PADinput.PAD1RIGHT;

-- Default to cclcc profile

root.Input.PADcustomA = {
    root.PADinput.PAD1UP,
    root.PADinput.PAD1DOWN,
    root.PADinput.PAD1LEFT,
    root.PADinput.PAD1RIGHT,
    root.PADinput.PAD1A | root.PADinput.PAD1START,
    root.PADinput.PAD1A,
    root.PADinput.PAD1B,
    root.PADinput.PAD1L1,
    root.PADinput.PAD1R1,
    root.PADinput.PAD1X,
    root.PADinput.PAD1START,
    0,
    root.PADinput.PAD1Y,
    root.PADinput.PAD1R3,
    root.PADinput.PAD1A | root.PADinput.PAD1B,
    root.PADinput.PAD1A,
    root.PADinput.PAD1B,
    root.PADinput.PAD1X,
    root.PADinput.PAD1Y,
    0,
    0,
    root.PADinput.PAD1SELECT,
    0,
    root.PADinput.PAD1A,
    0,
    0,
    0,
    0,
    root.PADinput.PAD1UP_LS | root.PADinput.PAD1UP_DIRECT,
    root.PADinput.PAD1DOWN_LS | root.PADinput.PAD1DOWN_DIRECT,
    root.PADinput.PAD1LEFT_LS | root.PADinput.PAD1LEFT_DIRECT,
    root.PADinput.PAD1RIGHT_LS | root.PADinput.PAD1RIGHT_DIRECT,
    root.PADinput.PAD1UP_RS,
    root.PADinput.PAD1DOWN_RS,
    root.PADinput.PAD1LEFT_RS,
    root.PADinput.PAD1RIGHT_RS,
    root.PADinput.PAD1L2,
    root.PADinput.PAD1R2,
    root.PADinput.PAD1LEFT | root.PADinput.PAD1L2,
    root.PADinput.PAD1RIGHT | root.PADinput.PAD1R2,
    AllDir | root.PADinput.PAD1A,
    root.PADinput.PAD1L1,
    root.PADinput.PAD1R1,
}

root.Input.PADcustomB = {
  root.PADinput.PAD1UP,
  root.PADinput.PAD1DOWN,
  root.PADinput.PAD1LEFT,
  root.PADinput.PAD1RIGHT,
  root.PADinput.PAD1A | root.PADinput.PAD1START,
  root.PADinput.PAD1A,
  root.PADinput.PAD1B,
  root.PADinput.PAD1L2,
  root.PADinput.PAD1R1,
  root.PADinput.PAD1R2,
  root.PADinput.PAD1START,
  0,
  root.PADinput.PAD1Y,
  root.PADinput.PAD1R3,
  root.PADinput.PAD1A | root.PADinput.PAD1B | root.PADinput.PAD1START,
  root.PADinput.PAD1A,
  root.PADinput.PAD1B,
  root.PADinput.PAD1X,
  root.PADinput.PAD1Y,
  root.PADinput.PAD1R1,
  root.PADinput.PAD1R2,
  root.PADinput.PAD1L1,
  0,
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
};

root.Input.KBcustom = {
     [0] = {82, 96, 26},      -- UP, NUMPAD_8, W
     [1] = {81, 90, 22},      -- DOWN, NUMPAD_2, S
     [2] = {80, 92, 4},       -- LEFT, NUMPAD_4, A
     [3] = {79, 94, 7},       -- RIGHT, NUMPAD_6, D
     [4] = {40, 88, 44},      -- RETURN, NUMPAD_ENTER, SPACE
     [5] = {42, 27, 98},      -- BACKSPACE, X, NUMPAD_0
     [6] = {76},              -- DELETE
     [7] = {73},              -- INSERT
     [8] = {20, 95, 54, 74},  -- Q, NUMPAD_7, COMMA, HOME  
     [9] = {29, 89, 77},      -- Z, NUMPAD_1, END
    [11] = {8, 97, 55, 157},  -- E, NUMPAD_9, PERIOD, PRIOR  
    [12] = {6, 91, 78},       -- C, NUMPAD_3, PAGEDOWN
    [13] = {65},              -- F8
    [14] = {30, 84},          -- 1, NUMPAD_DIVIDE
    [20] = {21, 86, 47},      -- R, NUMPAD_MINUS, LEFTBRACKET
    [21] = {9, 87, 48},       -- F, NUMPAD_PLUS, RIGHTBRACKET
    [50] = {224, 228},        -- LCTRL, RCTRL
    [51] = {58},              -- F1
    [52] = {59},              -- F2
    [53] = {60},              -- F3
    [54] = {61},              -- F4
    [55] = {62},              -- F5
    [56] = {63},              -- F6
    [57] = {66},              -- F9
    [58] = {64},              -- F7
    [59] = {68},              -- F11
    [60] = {41},              -- ESCAPE
    [61] = {67},              -- F10
}