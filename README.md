# Thermal Printer
Barebones arduino-driven thermal printer using ROHM KD series print head.

Partly based on https://github.com/3zuli/thermalPrinter

Can print text directly from the serial terminal with the string ending in carriage return '\r'.

Can use any width and height font as long as the width is a multiple of 8 (no bit-wise addressing, each byte is directly pulled from the font array and transferred into the line buffer).
Compatible fonts can be found at: http://www.rinkydinkelectronics.com/r_fonts.php

To use a new font, just change the width and height in the source file.

Works with just about every printhead that has a shift-register/latch/strobe mechanism implemented, with minor changes in some scenarios.

### In progress:
- AT commands
- Change fonts using AT commands
- Bitmap printing by serial parsing
- Font styling options by AT commands (strikethrough, underline, page alignment)

### Nice to have:
- GUI emulator to show how the text will look on paper
- built-in barcode/qr code generator

