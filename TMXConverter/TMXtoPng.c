/*
	Optimized TMX to PNG image converter, compatible with Persona 4 (PS2),
	by Miguel "Micapucha" (2015/08/08).
	
	Unlike other tools, __this one keeps the original palette__ and references
	the alpha by it, not pixel by pixel. This means that the resulting images
	are much smaller (half size or less) than 32bpp conversions, and are
	much closer to the original.
	
	Alpha is scaled to full range; no other gamma or color corrections are applied.
	
	Based upon PersonaRipper's and bbrcher's research (Xentax forum).
	Uses the FreeImage library.
	
	--------------------------------------------------------------------
	BRIEF FORMAT DOCUMENTATION:
	
	TMX is a proprietary uncompressed palettized bitmap image format.
	According to public knowledge, it supports two bitdepths:
	
	- 4bpp (16 colors)
	- 8bpp (256)
	
	Both bitdepths represent a fixed palette size.
	
	The file is made up by the following sections:
	
	- [0x0] Header (TMX_HEADER)
		- Flags == 0x14 -> 16 colors; if not, 256
	
	- [0x40] Palette. In 8bpp mode, it is not arranged lineally,
	         but in blocks of size 8x2.
	
	- [0x80 or 0x440] Image data, flipped vertically
*/

# include <FreeImage.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/param.h>

typedef unsigned char	u8;
typedef unsigned short	u16;
typedef unsigned int 	u32;

typedef struct
{
	u32		startUnknown;	// 0x02000000
	u32		size;			// size of file including this header
	u8		magicTag[4];	// "TMX0"
	u32		unknown1;		// padding 0
	u16		unknown2;		// non zero, may be different version value
	u16		width;
	u16		height;
	u32		flags;			// so far only discerned 2 formats
	u16		tmxEnd;			// 0x00FF 
	u8		reserved[32];	// nulls can have the file name in it
	
}  __attribute__((packed)) TMX_HEADER;

typedef struct
{
	u8 r;	//	[0, 255]
	u8 g;	//	""
	u8 b;	//	""
	u8 a;	//	[0, 128]
	
} TMX_RGBA;

int main (int argc, char * argv[])
{
	FILE * tmx;
	TMX_HEADER tmx_header;
	TMX_RGBA tmx_palette [256];
	u8 * tmx_bitmap;
	int tmx_ncolors;
	
	FIBITMAP * png;
	char * png_name;
	RGBQUAD * png_palette;
	u8 png_palette_a[256];
	int png_name_length;
	
	FreeImage_Initialise(0);
	
	if (argc < 2)
	{
		printf("\nOptimized TMX to PNG converter (Micapucha, 2015/08)\nUsage: ./TMXtoPng 1.tmx [2.tmx] [...]\n\n");
		return 1;
	}
	
	for (int i = 1; i < argc; i++)
	{
		tmx_ncolors = 256;
		
		tmx = fopen(argv[i], "rb");
		
		if (tmx == NULL)
		{
			printf("\n! Could not open source file (%s)\n\n", argv[i]);
			return 2;
		}
		
		// TMX: Load header and palette into memory
		
		fread(&tmx_header, sizeof(TMX_HEADER), 1, tmx);
		
		if ((tmx_header.flags & 0x14) == 0x14)
			tmx_ncolors = 16;
		
		fseek(tmx, 0x40, SEEK_SET);
		
		fread(tmx_palette, sizeof(TMX_RGBA), tmx_ncolors, tmx);
		
		// PNG: Initialize output
		
		png = FreeImage_Allocate(tmx_header.width, tmx_header.height, (tmx_ncolors == 16 ? 4 : 8), 0,0,0);
		
		// PNG: Build palette
		
		png_palette = FreeImage_GetPalette(png);
		
		if (tmx_ncolors == 16)
			for (int j = 0; j < tmx_ncolors; j++)
			{
				png_palette[j].rgbRed	= tmx_palette[j].r;
				png_palette[j].rgbGreen	= tmx_palette[j].g;
				png_palette[j].rgbBlue	= tmx_palette[j].b;
				
				png_palette_a[j] = MIN((tmx_palette[j].a / (float)128 * 255), 255);
			}
		else
			for (int y = 0, j = 0; y < 16; y += 2)
				for (int x = 0; x < 16; x += 8)
					for (int y_i = y; y_i -y < 2; y_i++)
						for (int x_i = x; x_i -x < 8; x_i++, j++)
						{
							png_palette[j].rgbRed	= tmx_palette[y_i * 16 + x_i].r;
							png_palette[j].rgbGreen	= tmx_palette[y_i * 16 + x_i].g;
							png_palette[j].rgbBlue	= tmx_palette[y_i * 16 + x_i].b;
							
							png_palette_a[j] = MIN((tmx_palette[y_i * 16 + x_i].a / (float)128 * 255), 255);
						}
		
		FreeImage_SetTransparencyTable(png, png_palette_a, tmx_ncolors);
		
		// PNG: Save
		
		png_name_length = strlen(argv[i]);
		
		if (png_name_length > 4 && strcasecmp(&argv[i][png_name_length -4], ".tmx") == 0)
		{
			png_name = strdup(argv[i]);
			
			png_name[png_name_length - 3] = 'p';
			png_name[png_name_length - 2] = 'n';
			png_name[png_name_length - 1] = 'g';
			
		}else
		{
			png_name_length += 4;
			
			png_name = malloc(png_name_length);
			snprintf(png_name, png_name_length, "%s.%s", argv[i], ".png");
		}
		
		tmx_bitmap = malloc(tmx_header.width * tmx_header.height / (tmx_ncolors == 16 ? 2 : 1));
		
		if (tmx_bitmap == NULL)
		{
			printf("! Not enough memory\n");
			return 4;
		}
		
		fread(tmx_bitmap, tmx_header.width * tmx_header.height / (tmx_ncolors == 16 ? 2 : 1), 1, tmx);
		
		if (tmx_ncolors == 16)
		{
			for (int y = 0; y < tmx_header.height; y++)
				for (int x = 0; x < tmx_header.width; x += 2)
				{
					u8 p1, p2;
					
					p2 = tmx_bitmap[(y * tmx_header.width + x) / 2];
					p1 = p2 & 0xf;
					p2 >>= 4;
					
					FreeImage_SetPixelIndex(png, x,   tmx_header.height -1 -y, &p1);
					FreeImage_SetPixelIndex(png, x+1, tmx_header.height -1 -y, &p2);
				}
			
		}else
		{
			for (int y = 0; y < tmx_header.height; y++)
				for (int x = 0; x < tmx_header.width; x++)
					FreeImage_SetPixelIndex(png, x, tmx_header.height -1 -y, &tmx_bitmap[y * tmx_header.width + x]);
		}
		
		free(tmx_bitmap);
		
		if (FreeImage_Save(FIF_PNG, png, png_name, PNG_Z_BEST_COMPRESSION))
		{
			printf("Converted %s -> %s (%d x %d @ %d)\n", argv[i], png_name, tmx_header.width, tmx_header.height, tmx_ncolors);
			
		}else
		{
			printf("! Could not write destination file (%s)\n\n", argv[i]);
			return 3;
		}
		
		FreeImage_Unload(png);
		free(png_name);
		fclose(tmx);
	}
	
	FreeImage_DeInitialise();
	return 0;
}
