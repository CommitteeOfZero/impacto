root.Charset = {
    Flags = {}
};

for i = 0, (64 * 50) - 1 do root.Charset.Flags[i] = 0; end

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