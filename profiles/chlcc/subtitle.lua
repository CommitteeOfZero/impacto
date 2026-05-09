root.Subtitle = {
  SubtitleMappings = {
    movie = {
      [10] = {
        {
          Type = SubtitleType.Ass,
          Config = SubtitleConfigType.Karaoke,
          Path = "games/chlcc/subtitles/OP3_karaonly.ass",
        },
        {
          Type = SubtitleType.Ass,
          Config = SubtitleConfigType.Translation,
          Path = "games/chlcc/subtitles/OP3_tlonly.ass"
        },
      },
    },
    bgm = {
      [70] = {{
        Type = SubtitleType.Ass,
        Config = SubtitleConfigType.Translation,
        Path = "games/chlcc/subtitles/contract.ass"
      }},
    }
  },
  SubtitleFontsDir = {"games/chlcc/fonts"}
};
