Font is generated with mgsfontgen-dx (we should really publish that some time...) and the following patch:

```
diff --git a/src/Program.cs b/src/Program.cs
index a69248b..016ad4f 100644
--- a/src/Program.cs
+++ b/src/Program.cs
@@ -55,8 +55,8 @@ namespace MgsFontGenDX
             var charset = File.ReadAllText(arguments.CharsetFileName);
             var compoundCharTable = ReadCompoundCharacterTable(arguments.CompoundCharTableFileName);

-            const int batchSize_outline = 4544;//5440;
-            const int batchSize_font = 5440;
+            const int batchSize_outline = 3200;//5440;
+            const int batchSize_font = 3200;
             using (var textRenderer = new TextRenderer())
             using (var widthTableFile = File.Create("widths.bin"))
             using (var widthWriter = new BinaryWriter(widthTableFile))
diff --git a/src/TextRenderer.cs b/src/TextRenderer.cs
index 8b5f51c..05782fb 100644
--- a/src/TextRenderer.cs
+++ b/src/TextRenderer.cs
@@ -17,7 +17,7 @@ namespace MgsFontGenDX
         private const int OutlineCellWidth = 57;
         private const int OutlineCellHeight = 57;
         private const float Dpi = 96.0f;
-        private const float GameWidthMultiplier = 1.5f;
+        private const float GameWidthMultiplier = 1.0f;

         private int _cellWidth;
         private int _cellHeight;
@@ -44,6 +44,7 @@ namespace MgsFontGenDX
         {
             _cellWidth = drawOutline ? OutlineCellWidth : NormalCellWidth;
             _cellHeight = drawOutline ? OutlineCellHeight : NormalCellHeight;
+            Console.WriteLine(characters.Length);
             int rowCount = (int)Math.Ceiling((double)characters.Length / ColumnCount);
             int bitmapWidth = _cellWidth * ColumnCount;
             int bitmapHeight = 4 * (int)Math.Ceiling((double)_cellHeight * rowCount / 4);
@@ -158,7 +159,7 @@ namespace MgsFontGenDX
         private byte Measure(string character, TextLayout layout, bool stretched)
         {
             double multiplier = stretched ? GameWidthMultiplier * character.Length : GameWidthMultiplier;
-            return  (byte)(Math.Ceiling(layout.Metrics.WidthIncludingTrailingWhitespace / multiplier) + 1);
+            return  (byte)(Math.Ceiling(layout.Metrics.WidthIncludingTrailingWhitespace / multiplier));
         }

         private void DrawCompoundCharacter(string compoundCharacter, TextLayout layout)
```

The charset uses U+2004 THREE-PER-EM SPACE because we don't have proper metrics and spaces happen to end up too small the way we're generating the widths...