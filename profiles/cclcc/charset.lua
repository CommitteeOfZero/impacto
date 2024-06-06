root.Charset = {
    Flags = {},
    CharacterToSc3 = {
        [" "] = 0x803F, ["0"] = 0x8001, ["1"] = 0x8002, ["2"] = 0x8003, ["3"] = 0x8004, ["4"] = 0x8005, ["5"] = 0x8006,
        ["6"] = 0x8007, ["7"] = 0x8008, ["8"] = 0x8009, ["9"] = 0x800A, ["A"] = 0x800B, ["B"] = 0x800C, ["C"] = 0x800D,
        ["D"] = 0x800E, ["E"] = 0x800F, ["F"] = 0x8010, ["G"] = 0x8011, ["H"] = 0x8012, ["I"] = 0x8013, ["J"] = 0x8014,
        ["K"] = 0x8015, ["L"] = 0x8016, ["M"] = 0x8017, ["N"] = 0x8018, ["O"] = 0x8019, ["P"] = 0x801A, ["Q"] = 0x801B,
        ["R"] = 0x801C, ["S"] = 0x801D, ["T"] = 0x801E, ["U"] = 0x801F, ["V"] = 0x8020, ["W"] = 0x8021, ["X"] = 0x8022,
        ["Y"] = 0x8023, ["Z"] = 0x8024, ["a"] = 0x8025, ["b"] = 0x8026, ["c"] = 0x8027, ["d"] = 0x8028, ["e"] = 0x8029,
        ["f"] = 0x802A, ["g"] = 0x802B, ["h"] = 0x802C, ["i"] = 0x802D, ["j"] = 0x802E, ["k"] = 0x802F, ["l"] = 0x8030,
        ["m"] = 0x8031, ["n"] = 0x8032, ["o"] = 0x8033, ["p"] = 0x8034, ["q"] = 0x8035, ["r"] = 0x8036, ["s"] = 0x8037,
        ["t"] = 0x8038, ["u"] = 0x8039, ["v"] = 0x803A, ["w"] = 0x803B, ["x"] = 0x803C, ["y"] = 0x803D, ["z"] = 0x803E,
        ["　"] = 0x803F, ["/"] = 0x8040, [":"] = 0x8041, ["-"] = 0x8042, [";"] = 0x8043, ["!"] = 0x8044, ["?"] = 0x80C4,
        ["'"] = 0x8046, ["."] = 0x80C1, ["@"] = 0x8048, ["#"] = 0x8049, ["%"] = 0x8112, ["~"] = 0x805B, ["*"] = 0x805C,
        ["【"] = 0x80E0, ["】"] = 0x80E1
    }
};

for i = 0, (64 * 117) - 1 do root.Charset.Flags[i] = 0; end

local spaces = {[0]=0, 63};
for i = 0, #spaces do
    root.Charset.Flags[spaces[i]] = root.Charset.Flags[spaces[i]] | CharacterTypeFlags.Space;
end

-- 、 。 ． ， ？ ！ 〜 ” ー ） 〕 ］ ｝ 〉 》 」 』 】☆ ★ ♪ 々 ぁ ぃ ぅ ぇ
-- ぉ っ ゃ ゅ ょ ァ ィ ゥ ェ ォ ッ ャ ュ ョ –
local wordEndingPuncts = {
    [0]=0x00BE, 0x00BF, 0x00C1,
    0x00C0, 0x00C4, 0x00C5,
    0x00E4, 0x00CB, 0x00E5,
    0x00CD, 0x00CF, 0x00D1,
    0x00D3, 0x00D5, 0x00D7,
    0x00D9, 0x00DB, 0x00DD,
    0x01A5, 0x01A6, 0x00E6,
    0x0187, 0x00E8, 0x00E9,
    0x00EA, 0x00EB, 0x00EC,
    0x00ED, 0x00EE, 0x00EF,
    0x00F0, 0x00F2, 0x00F3,
    0x00F4, 0x00F5, 0x00F6,
    0x00F7, 0x00F8, 0x00F9,
    0x00FA, 0x0113
};
for i = 0, #wordEndingPuncts do
    root.Charset.Flags[wordEndingPuncts[i]] = root.Charset.Flags[wordEndingPuncts[i]] | CharacterTypeFlags.WordEndingPunct;
end

-- space(63) space(0) “ （ 〔 ［ ｛ 〈 《 「
local wordStartingPuncts = {
    [0]=63, 0, 0x00CA,
    0x00CC, 0x00CE, 0x00D0,
    0x00D2, 0x00D4, 0x00D6,
    0x00D8, 0x00DA, 0x00DC
};
for i = 0, #wordStartingPuncts do
    root.Charset.Flags[wordStartingPuncts[i]] = root.Charset.Flags[wordStartingPuncts[i]] | CharacterTypeFlags.WordStartingPunct;
end