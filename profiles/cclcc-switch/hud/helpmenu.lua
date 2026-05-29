root.HelpMenu.ManualPages = {};

local numberOfPages = root.Language == "English" and 1 or 16

for i = 0, numberOfPages do
    root.Sprites["ManualPage" .. i] = nil
end