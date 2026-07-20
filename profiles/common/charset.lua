root.Charset = {
    Flags = {},
    CharacterToSc3 = {}
};

function AddCharsetFlag(characterString, charsetStr, flag)
    if characterString == nil then return; end

    for sourceBytePos, sourceCodePoint in utf8.codes(characterString) do
        local charsetIndex = 0;
        for targetBytePos, targetCodePoint in utf8.codes(charsetStr) do
            if sourceCodePoint == targetCodePoint then
                root.Charset.Flags[charsetIndex] = root.Charset.Flags[charsetIndex] | flag;
            end

            charsetIndex = charsetIndex + 1;
        end
    end
end

function UpdateCharset(charsetInternal)
    root.Charset.Flags = {};
    root.Charset.CharacterToSc3 = {};
    local i = 0
    for p, c in utf8.codes(charsetInternal.CharsetStr) do
        local high_byte = 0x80 + math.floor(i / 256);
        local low_byte = i % 256;
        local code = high_byte << 8 | low_byte;
        if root.Charset.CharacterToSc3[utf8.char(c)] == nil then
            root.Charset.CharacterToSc3[utf8.char(c)] = code;
        end
        i = i + 1
    end

    for i = 0, #charsetInternal.CharsetStr do root.Charset.Flags[i] = 0; end

    if charsetInternal.Spaces ~= nil then
        AddCharsetFlag(charsetInternal.Spaces, charsetInternal.CharsetStr, CharacterTypeFlags.Space);
    end
    if charsetInternal.WordEndingPuncts ~= nil then
        AddCharsetFlag(charsetInternal.WordEndingPuncts, charsetInternal.CharsetStr, CharacterTypeFlags.WordEndingPunct);
    end
    if charsetInternal.WordStartingPunct ~= nil then
        AddCharsetFlag(charsetInternal.WordStartingPuncts, charsetInternal.CharsetStr, CharacterTypeFlags.WordStartingPunct);
    end

    if charsetInternal.AlphabetChars == nil then
        charsetInternal.AlphabetChars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz０１２３４５６７８９ＡＢＣＤＥＦＧＨＩＪＫＬＭＮＯＰＱＲＳＴＵＶＷＸＹＺａｂｃｄｅｆｇｈｉｊｋｌｍｎｏｐｑｒｓｔｕｖｗｘｙｚ/／.．";
    end
    AddCharsetFlag(charsetInternal.AlphabetChars, charsetInternal.CharsetStr, CharacterTypeFlags.Alphabet);
end
