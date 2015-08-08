/*
	BIN to TMX file extractor for Persona 4's (PS2) character poses,
	by Miguel "Micapucha" (2015/08/07).
	
	This tool retains the original names of the files.
	
	However, it does not work for other package formats used by the game;
	for those you will have much better luck with an hex editor or a
	bruteforce binary extractor.
	
	--------------------------------------------------------------------
	BRIEF DOCUMENTATION:
	
	You can convert a CVM to a ISO with dd, skip=12. This may not work
	with other games that encrypt its file index.
	
	Tmx files begin with the value 0x02; it's before the "TMX0" string.
	
	Bin files may contain .tmx (and also other data)
	In the poses' case, its format is:
	  
	- Header (size: 0x100)
	
		- String with the file name
		- Zero padding (also works as string terminator)
		- [0x100 - 4]: content size, 32 bit little endian
		
	- Content
	- Next header
*/

# include <fcntl.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/mman.h>
# include <sys/stat.h>
# include <sys/types.h>
# include <unistd.h>

typedef unsigned char u8;
typedef unsigned int  u32;

int main (int argc, char * argv[])
{
	int		f;
	off_t	orig_tam;
	struct stat sstat;
	
	FILE * dest;
	
	u8 * p;
	u32 dir, tmx_ini, tmx_tam;
	
	if (argc != 2)
	{
		printf("\nBin to TMX extractor for poses (Micapucha, 2015/08)\nUsage: ./BINtoTmx file.bin\n\n");
		return 1;
	}
	
	if (lstat(argv[1],&sstat))
	{
		printf("! Could not open the source file (%s)", argv[1]);
		return 2;
	}
	
	orig_tam = sstat.st_size;
	
	if (orig_tam == 0)
		return 3;
	
	f = open(argv[1], O_RDONLY);
	
	if (f == -1)
		return 1;
	
	p = mmap(NULL, orig_tam, PROT_READ, MAP_SHARED|MAP_FILE, f, 0);
	
	if (p == MAP_FAILED)
		return 2;
	
	for (dir = 0; dir + 0x100 < orig_tam;)
	{
		tmx_ini = dir + 0x100;
		tmx_tam	= *((u32*)(&p[ dir + (0x100 - 4) ]));
		
		printf("Extracting %s -> %s (start: 0x%x size: 0x%x)\n", argv[1], (char*)&p[dir], tmx_ini, tmx_tam);
		
		dest = fopen((char*)&p[dir],"wb");
		if (dest == NULL)
		{
			printf("! Could not write destination file (%s)\n", (char*)&p[dir]);
			return 4;
		}
		
		fwrite(&p[tmx_ini],tmx_tam,1,dest);
		
		fclose(dest);
		
		dir = tmx_ini + tmx_tam;
	}
	
	return 0;
}
