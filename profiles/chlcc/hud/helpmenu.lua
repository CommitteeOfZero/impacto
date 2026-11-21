root.HelpMenu = {
    Type = HelpMenuType.None,
    UseOverlayHelp = true,
    OverlayHelpKbCustomId = 51, -- F1
    Actions = {
        { Section = "Dialogue & Navigation", Action = "Confirm", KeyboardMouse = "Enter, Left Click", Controller = "South Button" },
        { Section = "Dialogue & Navigation", Action = "Back / Hide Text", KeyboardMouse = "Backspace, Right Click", Controller = "East Button" },
        { Section = "Dialogue & Navigation", Action = "Select / Navigate", KeyboardMouse = "Arrow Keys", Controller = "D-Pad, Left Stick" },
        { Section = "Dialogue & Navigation", Action = "Skip Mode / Next Page", KeyboardMouse = "E", Controller = "Right Shoulder" },
        { Section = "Dialogue & Navigation", Action = "Force Skip / Previous Page", KeyboardMouse = "Ctrl", Controller = "Left Shoulder" },
        { Section = "Dialogue & Navigation", Action = "Delusion Trigger (Left)", KeyboardMouse = "Z", Controller = "Left Trigger" },
        { Section = "Dialogue & Navigation", Action = "Delusion Trigger (Right)", KeyboardMouse = "C", Controller = "Right Trigger" },
        { Section = "Dialogue & Navigation", Action = "Text Scroll (Tips)", KeyboardMouse = "[ and ]", Controller = "-" },
        { Section = "Dialogue & Navigation", Action = "Scroll Text & List / Backlog", KeyboardMouse = "Mouse Wheel", Controller = "-" },

        { Section = "Menus & System", Action = "System Menu", KeyboardMouse = "1, Right Click", Controller = "Start" },
        { Section = "Menus & System", Action = "Help", KeyboardMouse = "F1", Controller = "-" },
        { Section = "Menus & System", Action = "Backlog", KeyboardMouse = "F2, Insert", Controller = "North Button" },
        { Section = "Menus & System", Action = "Auto Mode", KeyboardMouse = "F3, Delete", Controller = "West Button" },
        { Section = "Menus & System", Action = "Tips", KeyboardMouse = "F4", Controller = "-" },
        { Section = "Menus & System", Action = "Quick Load", KeyboardMouse = "F5", Controller = "-" },
        { Section = "Menus & System", Action = "Quick Save", KeyboardMouse = "F6, F8", Controller = "Right Stick Click" },
        { Section = "Menus & System", Action = "Load", KeyboardMouse = "F7", Controller = "-" },
        { Section = "Menus & System", Action = "Config", KeyboardMouse = "F9", Controller = "-" },
        { Section = "Menus & System", Action = "Title", KeyboardMouse = "F10", Controller = "-" },
        { Section = "Menus & System", Action = "Toggle Fullscreen", KeyboardMouse = "F11", Controller = "-" },
        { Section = "Menus & System", Action = "Screenshot", KeyboardMouse = "F12", Controller = "-" },
    },
}
