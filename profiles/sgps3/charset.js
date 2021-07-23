root.Charset = {
    Flags: new Array(64 * 37)
};

for (var i = 0; i < 64 * 37; i++) root.Charset.Flags[i] = 0;

var spaces = [0, 63];
for (var i = 0; i < spaces.length; i++) {
    root.Charset.Flags[spaces[i]] |= CharacterTypeFlags.Space;
}

// 、 。 ． ， ？ ！ 〜 ” ー ） 〕 ］ ｝ 〉 》 」 』 】☆ ★ ♪ 々 ぁ ぃ ぅ ぇ
// ぉ っ ゃ ゅ ょ ァ ィ ゥ ェ ォ ッ ャ ュ ョ –
var wordEndingPuncts = [
    0x00BE, 0x00BF, 0x00C1,
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
];
for (var i = 0; i < wordEndingPuncts.length; i++) {
    root.Charset.Flags[wordEndingPuncts[i]] |= CharacterTypeFlags.WordEndingPunct;
}

// space(63) space(0) “ （ 〔 ［ ｛ 〈 《 「
var wordStartingPuncts = [
    63, 0, 0x00CA,
    0x00CC, 0x00CE, 0x00D0,
    0x00D2, 0x00D4, 0x00D6,
    0x00D8, 0x00DA, 0x00DC
];
for (var i = 0; i < wordStartingPuncts.length; i++) {
    root.Charset.Flags[wordStartingPuncts[i]] |= CharacterTypeFlags.WordStartingPunct;
}