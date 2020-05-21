#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

/* 
********************************************************************************************************
	IN COMMAND LINE ARGUMENTS:
	1. File from which files are extracted
	2. Folder to which files are extracted
	NECESSARILY
********************************************************************************************************	
*/

char *dirr(unsigned char *path, long int m, char *from)
{
	int pos = -1;
	char *dir; 
	for(int i = 0; i < m; i++)
	{
		if(path[i] == '/')
		{
			pos = i;
		}
	}
	if(pos != -1)
	{
		dir = (char*)calloc(pos, sizeof(char));
		strcat(from, "/");
		strncpy(dir, path, pos);
		strcat(from, dir);
	}
	else 
	{
		dir = (char*)calloc(1, sizeof(char));
		dir[0] = '\0';
	} 
	return from;   	
}

int main(int argc, char *argv[])
{
	// Open file as read only
	FILE *file = fopen( argv[1], "r");
    
    // Test for files not existing     
    if (file == NULL) 
    {   
        printf("Error! Could not open file\n"); 
        exit(-1); // From stdlib 
    }  
    long len;
    char *mmr;
    fseek(file, 0, SEEK_END); // Put marker to end of file
    len = ftell(file);
    printf("Size = %ld \n", len);
    rewind(file);

    mmr = (char*)malloc(len * sizeof(char)); // Dynamic memory
    fread(mmr, len, 1, file); // Read file

    // Search size of files
    int NumFile = (mmr[3]<<8)+mmr[2];
    int *size = calloc(NumFile, sizeof(int));
	for(int i = 0, j = 0; i < NumFile*7; i += 7)
    { 
    	size[j] = (mmr[6+i]<<16)+(mmr[5+i]<<8)+mmr[4+i];
    	j++;
    }
	
    // Search shift of files
    int *shift = calloc(NumFile, sizeof(int));
	for(int i = 0, j = 0; i < NumFile*7; i += 7)
    {
    	shift[j] = (mmr[10+i]<<32)+(mmr[9+i]<<16)+(mmr[8+i]<<8)+mmr[7+i];
    	j++;
    }

    // File name
    char **name = (char**)calloc(NumFile, sizeof(char));
	int cntr1 = 0;
	for(int i = 0; i < NumFile-1; i++)
	{
		cntr1 = shift[i+1]-(shift[i]+size[i]);
    	name[i] = calloc(cntr1, sizeof(char));
    	strncpy(name[i], mmr+(shift[i]+size[i]), cntr1);
	}
	cntr1 = len-(shift[NumFile-1]+size[NumFile-1]);
    name[NumFile-1]=calloc(cntr1, sizeof(char));
    strncpy(name[NumFile-1], mmr+(shift[NumFile-1]+size[NumFile-1]), cntr1);

    // Path to extract ("for" path from command line)
	char **path = (char**)calloc(NumFile, sizeof(char));
	int cntr2 = 0;
	for(int i = 0; i < NumFile; i++)
	{
    	path[i] = calloc(strlen(name[i])+strlen(argv[2])+1,sizeof(char));
    	strcat(path[i],argv[2]);
    	strcat(path[i],"/");
    	strcat(path[i],name[i]);
    }

    // Creating files and directories from .img
    for(int i = 0; i < NumFile; i++)
    {
		char *dir = dirr(name[i], strlen(name[i]), argv[2]);
    	mkdir(dir, 0700);
    	FILE *unpfile = fopen(path[i], "w+"); // Test for file not creat
   		if(unpfile  == NULL)
   		{
  			printf("Error! Could not creat file");
   		}
  		for(int j = shift[i]; j < (shift[i]+size[i]); j++)
   		{    				
   			putc(mmr[j], unpfile);	// File filling									
   		}
   	}

	fclose(file); // Close read file

	return 0;
}