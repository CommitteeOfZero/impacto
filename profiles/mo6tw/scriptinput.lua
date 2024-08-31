root.Input = {
  -- Keyboard
  KB_PAD1A=40,             -- Return
  KB_PAD1B=42,             -- Backspace
  KB_PAD1X=60,             -- F3
  KB_PAD1Y=59,             -- F2
  KB_PAD1SELECT=61,        -- F4
  KB_PAD1START=30,         -- 1
  KB_PAD1UP=82,            -- Up
  KB_PAD1DOWN=81,          -- Down
  KB_PAD1LEFT=80,          -- Left
  KB_PAD1RIGHT=79,         -- Right
  KB_PAD1L1=20,            -- Q
  KB_PAD1L2=29,            -- Z
  KB_PAD1L3=32,            -- 3
  KB_PAD1R1=8,             -- E
  KB_PAD1R2=6,             -- C
  KB_PAD1R3=65,            -- F8
  -- Mouse
  MS_PAD1A=1,              -- Left
  MS_PAD1B=3,              -- Right
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
  GP_PAD1L2=4, --Triggers are on axis, not button			  
  GP_PAD1L3=7,
  GP_PAD1R1=10,
  GP_PAD1R2=5, --Triggers are on axis, not button	  
  GP_PAD1R3=8,
}

root.PADcustomSizeA = 41;
root.PADcustomSizeB = 0;
root.PADcustomType = 0;
root.PADcustomA = {
  0x10000,
  0x20000,
  0x40000,
  0x80000,
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
  0xF1000,
  root.PADinput.PAD1R1,
  root.PADinput.PAD1L1,
  root.PADinput.PAD1A,
  root.PADinput.PAD1B,
  0x100000 | root.PADinput.PAD1UP,
  0x200000 | root.PADinput.PAD1DOWN,
  0x400000 | root.PADinput.PAD1LEFT,
  0x800000 | root.PADinput.PAD1RIGHT,
  0x1000000,
  0x2000000,
  0x4000000,
  0x8000000,
  0x20000000,
  0x40000000,
  0x80000000,
  0,
  0
}