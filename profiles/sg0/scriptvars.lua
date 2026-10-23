local sv = root.ScriptVars;

sv.SW_TITLECUR1 = 2139;
sv.SW_TITLECUR2 = 2140; --Actual guess
sv.SW_BG1POSX_OFS = 2400;
sv.SW_BG1POSY_OFS = 2401;
sv.SW_BG1SX_OFS = 2402;
sv.SW_BG1SY_OFS = 2403;
sv.SW_BG1SIZE_OFS = 2404;
sv.SW_BG1LX_OFS = 2405;
sv.SW_BG1LY_OFS = 2406;
sv.SW_BG1ROTZ_OFS = 2407;
sv.SW_BG1ALPHA_OFS = 2408;
sv.SW_MASK1COLOR = 4400;
sv.SW_MASK1ALPHA = 4401;
sv.SW_MASK1PRI = 4402;
sv.SW_MASK1POSX = 4403;
sv.SW_MASK1POSY = 4404;
sv.SW_MASK1SIZEX = 4405;
sv.SW_MASK1SIZEY = 4406;
sv.SW_MESMODE0 = 4363;
-- Cut-in slots (macrosys label658: 5800, 5820, 5840, 5860).
sv.SW_CUTIN1 = 5800;

sv.SF_SYSTEMMENUCAPTURE=1378;
-- SG0 reuses CC's SW_MESWIN0TYPE slot (4422) for an effect state (values 99/33/34/14),
-- which made the dialogue pages read "don't draw text". SG0's own message-window type
-- variable is not identified; 4430/4440/4450 (pages 0-2, stride 10) are never written by
-- any SG0 script, so the window type reads 0 (normal window and text).
sv.SW_MESWIN0TYPE = 4430;
