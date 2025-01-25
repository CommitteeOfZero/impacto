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
  KB_PAD1L1_ALT=224,       -- LCTRL
  KB_PAD1UP_RS=47,         -- [
  KB_PAD1DOWN_RS=48,       -- ]

  -- Mouse
  MS_LEFT=1,              -- Left
  MS_RIGHT=3,              -- Right
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

root.PADcustomSizeA = 37;
root.PADcustomSizeB = 0;
root.PADcustomType = 0;
root.PADcustomA = {
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
  root.PADinput.PAD1Y,
  0,
  root.PADinput.PAD1A | root.PADinput.PAD1B | root.PADinput.PAD1START,
  root.PADinput.PAD1A,
  root.PADinput.PAD1B,
  root.PADinput.PAD1X,
  root.PADinput.PAD1Y,
  root.PADinput.PAD1R1,
  root.PADinput.PAD1START,
  root.PADinput.PAD1L1,
  root.PADinput.PAD1LEFT_RS | root.PADinput.PAD1L2,
  root.PADinput.PAD1RIGHT_RS | root.PADinput.PAD1R2,
  0xf0100f,
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
}