root.HelpMenu.ManualPages = {};

local numberOfPages = root.Language == "English" and 1 or 16

for i = 0, numberOfPages do
    root.Sprites["ManualPage" .. i] = nil
end

root.Sprites["ManualPage0"] ={
        Sheet = "Help",
        Bounds = { X = 0, Y = 0, Width = 1920, Height = 1080 }
};
root.HelpMenu.ManualPages[#root.HelpMenu.ManualPages + 1] = "ManualPage0";