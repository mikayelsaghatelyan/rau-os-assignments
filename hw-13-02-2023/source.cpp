#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <iostream>

int copy_paste(char *src, char *dst)
{
    return -1;
}

int main(int argc, char *argv[])
{
    if (argc < 2 || argc > 3)
    {
        perror("Invalid number of arguments.\n");
        return -1;
    }
    int src_fd = open(argv[1], O_RDONLY, 0666);
    if (src_fd < 0)
    {
        perror("ERROR! Source file does not exist.\n");
        return -1;
    }
    char answer;
    int dst_fd;
    if (argc == 3)
    {
        printf("WARNING! Destination file already exists. Do you want to override it? y|n\n");
        scanf("%c", &answer);
        if (answer == 'y')
        {
            dst_fd = open(argv[2], O_CREAT | O_WRONLY, 0666);
        }
        else if (answer == 'n')
        {
            dst_fd = open(argv[2], O_CREAT | O_WRONLY | O_TRUNC, 0666);
        }
    }
    else // argc == 2
    {
        printf("INFO. No destination file. Opening one.\n");
        dst_fd = open("dst_file.txt", O_CREAT | O_WRONLY | O_TRUNC, 0666);
    }
    if (dst_fd < 0)
    {
        perror("ERROR! The file cannot be oppened.\n");
        return -1;
    }

    int n;
    enum
    {
        BUF_SIZE = 1024
    };
    char buf[BUF_SIZE];
    while ((n = read(src_fd, buf, BUF_SIZE)) > 0)
    {
        lseek(dst_fd, 0, SEEK_END);
        write(dst_fd, buf, n);
    }
    close(src_fd);
    close(dst_fd);
    return 0;
}