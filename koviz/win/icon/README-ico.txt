To create icon, use ImageMagik's convert.

I tried a 512x512 png, but convert gave an error, so 256x256 is
the largest.

% convert  koviz-icon-256x256.png koviz-icon-128x128.png koviz-icon-64x64.png \
           koviz-icon-32x32.png koviz-icon-16x16.png koviz.ico
