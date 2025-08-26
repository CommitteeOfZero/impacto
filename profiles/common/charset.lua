root.Charset = {
    Flags = {},
    CharacterToSc3 = {}
};

function AddCharsetFlag(characterString, flag)
    if characterString == nil then return; end

    for sourceBytePos, sourceCodePoint in utf8.codes(characterString) do
        local charsetIndex = 0;
        for targetBytePos, targetCodePoint in utf8.codes(root.CharsetInternal.CharsetStr) do
            if sourceCodePoint == targetCodePoint then
                root.Charset.Flags[charsetIndex] = root.Charset.Flags[charsetIndex] | flag;
                break;
            end
            
            charsetIndex = charsetIndex + 1;
        end
    end
end

if root.CharsetInternal ~= nil and root.CharsetInternal.CharsetStr ~= nil then
    local i = 0
    for p, c in utf8.codes(root.CharsetInternal.CharsetStr) do
        local high_byte = 0x80 + math.floor(i / 256);
        local low_byte = i % 256;
        local code = high_byte << 8 | low_byte;
        root.Charset.CharacterToSc3[utf8.char(c)] = code
        i = i + 1
    end

    for i = 0, #root.CharsetInternal.CharsetStr do root.Charset.Flags[i] = 0; end
    AddCharsetFlag(root.CharsetInternal.Spaces, CharacterTypeFlags.Space);
    AddCharsetFlag(root.CharsetInternal.WordEndingPuncts, CharacterTypeFlags.WordEndingPunct);
    AddCharsetFlag(root.CharsetInternal.WordStartingPuncts, CharacterTypeFlags.WordStartingPunct);
end
