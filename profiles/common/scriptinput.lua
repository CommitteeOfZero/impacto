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
local BTN = ControllerButton;
root.Input.PADtoGP = {
    [root.PADinput.PAD1A]            = BTN.A,
    [root.PADinput.PAD1B]            = BTN.B,
    [root.PADinput.PAD1X]            = BTN.X,
    [root.PADinput.PAD1Y]            = BTN.Y,
    [root.PADinput.PAD1SELECT]       = BTN.BACK,
    [root.PADinput.PAD1START]        = BTN.START,
    [root.PADinput.PAD1L3]           = BTN.LEFTSTICK,
    [root.PADinput.PAD1R3]           = BTN.RIGHTSTICK,
    [root.PADinput.PAD1L1]           = BTN.LEFTSHOULDER,
    [root.PADinput.PAD1R1]           = BTN.RIGHTSHOULDER,
    [root.PADinput.PAD1UP_DIRECT]    = BTN.DPAD_UP,
    [root.PADinput.PAD1DOWN_DIRECT]  = BTN.DPAD_DOWN,
    [root.PADinput.PAD1LEFT_DIRECT]  = BTN.DPAD_LEFT,
    [root.PADinput.PAD1RIGHT_DIRECT] = BTN.DPAD_RIGHT,
};
local AXIS = ControllerAxis;
local DIR = { POS = 1, NEG = -1 };
root.Input.PADtoGPA = {
    -- { Axis Id, Direction }
    [root.PADinput.PAD1UP_LS]    = {AXIS.LEFTY,       DIR.NEG},  -- Left stick Y axis
    [root.PADinput.PAD1DOWN_LS]  = {AXIS.LEFTY,       DIR.POS},  -- Left stick Y axis
    [root.PADinput.PAD1LEFT_LS]  = {AXIS.LEFTX,       DIR.NEG},  -- Left stick X axis
    [root.PADinput.PAD1RIGHT_LS] = {AXIS.LEFTX,       DIR.POS},  -- Left stick X axis
    [root.PADinput.PAD1UP_RS]    = {AXIS.RIGHTY,      DIR.NEG},  -- Right stick Y axis
    [root.PADinput.PAD1DOWN_RS]  = {AXIS.RIGHTY,      DIR.POS},  -- Right stick Y axis
    [root.PADinput.PAD1LEFT_RS]  = {AXIS.RIGHTX,      DIR.NEG},  -- Right stick X axis
    [root.PADinput.PAD1RIGHT_RS] = {AXIS.RIGHTX,      DIR.POS},  -- Right stick X axis
    [root.PADinput.PAD1L2]       = {AXIS.TRIGGERLEFT,  DIR.POS},  -- Left trigger
    [root.PADinput.PAD1R2]       = {AXIS.TRIGGERRIGHT, DIR.POS},  -- Right trigger
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

local SC = KeyboardScanCode;
root.Input.KBcustom = {
     [0] = {SC._UP, SC._KP_8, SC._W},
     [1] = {SC._DOWN, SC._KP_2, SC._S},
     [2] = {SC._LEFT, SC._KP_4, SC._A},
     [3] = {SC._RIGHT, SC._KP_6, SC._D},
     [4] = {SC._RETURN, SC._KP_ENTER, SC._SPACE},
     [5] = {SC._BACKSPACE, SC._X, SC._KP_0},
     [6] = {SC._DELETE},
     [7] = {SC._INSERT},
     [8] = {SC._Q, SC._KP_7, SC._COMMA, SC._HOME},
     [9] = {SC._Z, SC._KP_1, SC._END},
    [11] = {SC._E, SC._KP_9, SC._PERIOD, SC._PAGEUP},
    [12] = {SC._C, SC._KP_3, SC._PAGEDOWN},
    [13] = {SC._F8},
    [14] = {SC._1, SC._KP_DIVIDE},
    [20] = {SC._R, SC._KP_MINUS, SC._LEFTBRACKET},
    [21] = {SC._F, SC._KP_PLUS, SC._RIGHTBRACKET},
    [50] = {SC._LCTRL, SC._RCTRL},
    [51] = {SC._F1},
    [52] = {SC._F2},
    [53] = {SC._F3},
    [54] = {SC._F4},
    [55] = {SC._F5},
    [56] = {SC._F6},
    [57] = {SC._F9},
    [58] = {SC._F7},
    [59] = {SC._F11},
    [60] = {SC._ESCAPE},
    [61] = {SC._F10},
};