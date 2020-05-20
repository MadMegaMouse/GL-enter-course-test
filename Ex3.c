#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <fcntl.h>
#include <memory.h>
#include <stdlib.h>

/*
  I don’t know why it does not display all the necessary information,
  when it launched through the command line, but it works fine in sublime.
  If you know what is wrong with this programm, send an explanation message on townofthegoo@gmail.com.
  I'll be happy to understood what mistake I make.
*/   

void Permissions(struct stat itemStats)
{
    //filestat mode
    mode_t mode = itemStats.st_mode;

    printf( (mode & S_IRUSR) ? "r" : "-");
    printf( (mode & S_IWUSR) ? "w" : "-");
    printf( (mode & S_IXUSR) ? "x" : "-");

    printf( (mode & S_IRGRP) ? "r" : "-");
    printf( (mode & S_IWGRP) ? "w" : "-");
    printf( (mode & S_IXGRP) ? "x" : "-");

    printf( (mode & S_IROTH) ? "r" : "-");
    printf( (mode & S_IWOTH) ? "w" : "-");
    printf( (mode & S_IXOTH) ? "x" : "-");
}

void MonthDayYear(struct stat itemStats)
{
    //This function is responsible for printing the file last access date
    char buffer [80];
    struct tm * timeinfo;
    time_t timeStamp = itemStats.st_atime;
    timeinfo = localtime (&timeStamp);
    strftime (buffer,80,"\t%h\t%e\t%Y\t",timeinfo);
    printf("\t%s",buffer);
}

void FileType(struct stat itemStats)
{
//This function prints the type of the file
    printf( (S_ISDIR(itemStats.st_mode)) ? "d" : "");
    printf( (S_ISREG(itemStats.st_mode)) ? "-" : "");
    printf( (S_ISLNK(itemStats.st_mode)) ? "l" : "");
}

void FileNames(char *name,struct stat *itemStats){
    // This function is responsible for printing the file name, and if the file is a link, print the orginal file also.
    char buff[100];
    if(S_ISREG(itemStats->st_mode)){
        if(itemStats->st_mode & S_IXOTH){
            printf("\t%s", name);
            return;
        }
        printf("\t%s", name);
    }
    if(S_ISDIR(itemStats->st_mode)){
        printf("\t%s", name);
    }


    if(S_ISLNK(itemStats->st_mode)){
        printf("\t%s", name);
    }
    readlink(name,buff,100);

    //check if the file is a symbolic Link and if it is a SL print the name
    if(S_ISLNK(itemStats->st_mode)){
            printf (" -> %s", buff);
    }

}

char* AbsolutePath(char *path)
{
    //This file will handle the case on local path and convert it to absolute path
    char *newPath  = malloc( sizeof(char) * ( 255 ) );
    char buff[255];
    if(path[0]=='/'){
        return path;
    }
    newPath = getcwd(buff,255);
    strcat(newPath,"/");
    strcat(newPath,path);
    return newPath;
}

int main(int argc, char*argv[]) 
{
    int i = 0;
    int argsSize = argc;

    DIR *currentDir;

    struct dirent *myCurrentArg;
    struct stat myCurrentStat;

    //handle the case of zero arguments to the program
    getcwd(argv[0],255);

    char *path = argv[0];
    char backupStr[1024];

    //we put the current WorkingDirectory at argv[0]
    if(argc > 1){
        i=1;
    }

    //loop over all args
    for(;i<argsSize;i++) {
        path = AbsolutePath(argv[i]);
        strcpy(backupStr,path);
        //put check error condition file not found and such.
        char buf[1000] = {0};
        if(argc > 1){
          printf("\nThe Output of %s is:\n\n",backupStr);
        }
        currentDir = opendir(path);
        //print folders contents and of her files content
        if(currentDir!=NULL){
            while ((myCurrentArg = readdir(currentDir)) != NULL) {
                sprintf(buf, "%s/%s", argv[i], myCurrentArg->d_name);
                lstat(buf, &myCurrentStat);
                FileType(myCurrentStat);
                Permissions(myCurrentStat);
                printf("\t%ld", myCurrentStat.st_nlink);
                printf("\t%s", getpwuid(myCurrentStat.st_uid)->pw_name);
                printf("\t%s", getgrgid(myCurrentStat.st_gid)->gr_name);
                printf("\t%ld ", myCurrentStat.st_size);
                MonthDayYear(myCurrentStat);
                FileNames(myCurrentArg->d_name, &myCurrentStat);
                printf("\n");
            }
            closedir(currentDir);
        }
        else{ //its a file and not a folder! print file statics and name
            int file=0;
            if((file= open(backupStr,O_RDONLY)) <= -1){
                perror("Cannot open file!\n");
                exit(0);
            }
            struct stat fileStat;
            if(fstat(file,&fileStat) < 0){
                perror("Cannot open file status!\n");
                exit(0);
            }
            fstat(file, &myCurrentStat);
            FileType(myCurrentStat);
            Permissions(myCurrentStat);
            printf("\t%ld", myCurrentStat.st_nlink);
            printf("\t%s", getpwuid(myCurrentStat.st_uid)->pw_name);
            printf("\t%s", getgrgid(myCurrentStat.st_gid)->gr_name);
            printf("\t%ld ", myCurrentStat.st_size);
            MonthDayYear(myCurrentStat);
            FileNames(argv[1], &myCurrentStat);
            printf("\n");
        }
    }
    return 0;
}
