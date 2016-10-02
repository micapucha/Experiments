TMXConverter
=======

Tools for converting TMX images into PNG, so they can be viewed on most computers.
In practice, this means you can convert the assets from *Persona 4* and (potentially) other videogames from the same developer.

#Improvements with regard to other tools

Unlike other tools, this one **keeps the original palette** and references the alpha by it, not pixel by pixel. This means that:
- The resulting images are much smaller (half size or less) than 32bpp conversions
- They are much closer to the original (if you want to preserve the original palette)
- The transparency is readily available

#Building
If you use Debian/Ubuntu/Mint Linux, type the following commands:

```
sudo apt-get install build-essential libfreeimage-dev libfreeimage3
make
```

If you prefer FreeBSD (10.1), type these instead:

```
pkg install freeimage-3.16.0

export C_INCLUDE_PATH=/usr/include:/usr/local/include
export LIBRARY_PATH=/usr/lib:/usr/local/lib

make
```

I am not experienced in native Windows C development, so it's left as an exercise for the reader. If you manage to build on it, patches are welcome :)

#Usage
This directory contain two tools: **BINtoTmx** and **TMXtoPng**.

- BINToTMX extracts TMX files from BIN packages retaining the original filenames. It is meant to be used with P4's character poses; other packages may not work. For the rest of files, you will have much better luck with an hex editor or a bruteforce binary scanner.

- TMXtoPng converts the aforementioned TMX images to PNG.

They are easy to use, just invoke them from the command line:
```
./BINtoTMX package.bin
./TMXtoPng 1.tmx 2.tmx ...
```

#License and disclaimer
This software is provided 'as-is', without any express or implied warranty. 
In no event will the author be held liable for any damages arising from the use of this software.
This repository does not include code, art or any other intellectual property from the game's developer.
Please, use this program with your legally purchased game disc.

Credits to *PersonaRipper* and *bbrcher* for their research about the image format.
