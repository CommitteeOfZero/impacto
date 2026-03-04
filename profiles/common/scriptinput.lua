root.Input = {
    -- Keyboard
    KB_PAD1A={40,44},          -- Return,Space
    KB_PAD1B={42},             -- Backspace
    KB_PAD1X={60,76},          -- F3,Del   :function keys are probably supposed to be scripted in
    KB_PAD1Y={59,73},          -- F2,Insert
    KB_PAD1SELECT={61},        -- F4
    KB_PAD1START={30},         -- 1
    KB_PAD1UP={82,26},         -- Up,W
    KB_PAD1DOWN={81,22},       -- Down,S
    KB_PAD1LEFT={80,4},        -- Left,A
    KB_PAD1RIGHT={79,7},       -- Right,D
    KB_PAD1L1={20,224},        -- Q,LCTRL
    KB_PAD1L2={29},            -- Z
    KB_PAD1L3={32},            -- 3
    KB_PAD1R1={8},             -- E
    KB_PAD1R2={6},             -- C
    KB_PAD1R3={65},            -- F8
    KB_PAD1UP_RS={47},         -- [
    KB_PAD1DOWN_RS={48},       -- ]
    -- Mouse
    MS_LEFT=1,                 -- Left
    MS_RIGHT=3,                -- Right
    -- Controller
    GP_PAD1A=0,
    GP_PAD1B=1,
    GP_PAD1X=2,
    GP_PAD1Y=3,
    GP_PAD1SELECT=4,
    GP_PAD1START=6,
    GP_PAD1UP=11,
    GP_PAD1DOWN=12,
    GP_PAD1LEFT=13,
    GP_PAD1RIGHT=14,
    GP_PAD1L1=9,
    GP_PAD1L3=7,
    GP_PAD1R1=10,
    GP_PAD1R3=8,
    
    GPA_PAD1LX=0, --Left stick X axis
    GPA_PAD1LY=1, --Left stick Y axis
    GPA_PAD1RX=2, --Right stick X axis
    GPA_PAD1RY=3, --Right stick Y axis
    GPA_PAD1LT=4, --Left trigger		  
    GPA_PAD1RT=5, --Right trigger
}

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

local AllDir = root.PADinput.PAD1UP | root.PADinput.PAD1DOWN | root.PADinput.PAD1LEFT | root.PADinput.PAD1RIGHT;

-- Default to cclcc profile
root.PADcustomType = 0;
root.PADcustomSizeA = 64;
root.PADcustomSizeB = 0;
root.PADcustomA = {
    root.PADinput.PAD1UP, --0
    root.PADinput.PAD1DOWN, --1
    root.PADinput.PAD1LEFT, --2
    root.PADinput.PAD1RIGHT, --3
    root.PADinput.PAD1A | root.PADinput.PAD1START, --4
    root.PADinput.PAD1A, --5
    root.PADinput.PAD1B, --6
    root.PADinput.PAD1L1, --7
    root.PADinput.PAD1R1, --8
    root.PADinput.PAD1X, --9
    root.PADinput.PAD1START, --10
    0, --11
    root.PADinput.PAD1Y, --12
    root.PADinput.PAD1R3, --13
    root.PADinput.PAD1A | root.PADinput.PAD1B, --14
    root.PADinput.PAD1A, --15
    root.PADinput.PAD1B, --16
    root.PADinput.PAD1X, --17
    root.PADinput.PAD1Y, --18
    0, --19
    0, --20
    root.PADinput.PAD1SELECT, --21
    0, --22
    root.PADinput.PAD1A, --23
    0, --24
    0, --25
    0, --26
    0, --27
    root.PADinput.PAD1UP_LS | root.PADinput.PAD1UP_DIRECT, --28
    root.PADinput.PAD1DOWN_LS | root.PADinput.PAD1DOWN_DIRECT, --29
    root.PADinput.PAD1LEFT_LS | root.PADinput.PAD1LEFT_DIRECT, --30
    root.PADinput.PAD1RIGHT_LS | root.PADinput.PAD1RIGHT_DIRECT, --31
    root.PADinput.PAD1UP_RS, --32
    root.PADinput.PAD1DOWN_RS, --33
    root.PADinput.PAD1LEFT_RS, --34
    root.PADinput.PAD1RIGHT_RS, --35
    root.PADinput.PAD1L2, --36
    root.PADinput.PAD1R2, --34
    root.PADinput.PAD1LEFT | root.PADinput.PAD1L2, --35
    root.PADinput.PAD1RIGHT | root.PADinput.PAD1R2, --36
    AllDir | root.PADinput.PAD1A, --37
    root.PADinput.PAD1L1, --38
    root.PADinput.PAD1R1, --39
    0, --40
    0, --41
    0, --42
    0, --43
    0, --44
    0, --45
    0, --46
    0, --47
    0, --48
    0, --49
    0, --50
    0, --51
    0, --52
    0, --53
    0, --54
    0, --55
    0, --56
    0, --57
    0, --58
    0, --59
    0, --60
}

root.PADcustomB = {};
