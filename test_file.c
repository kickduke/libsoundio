#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    FILE *fp = NULL;
    fp = fopen("APU_SQUA1_LOG.txt", "r");
    char file_buff[255];
    const char delim = ':';
    char *str0 = NULL;
    char *str1 = NULL;
    char *str2 = NULL;
    char *str3 = NULL;
    char *str4 = NULL;
    fgets(file_buff, 255, fp);
    fprintf(stderr, "%s", file_buff);
    str0 = strtok(file_buff, &delim);
    str1 = strtok(NULL, &delim);
    str2 = strtok(NULL, &delim);
    str3 = strtok(NULL, &delim);
    str4 = strtok(NULL, &delim);
    fprintf(stderr, "%s %s %c\n", str2, str3, str4[0]);
    fprintf(stderr, "%d %d %d", atoi(str2), atoi(str3), atoi(str4));
    fclose(fp);
}