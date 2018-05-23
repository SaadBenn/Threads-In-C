#include <stdio.h>
#include <stdlib.h>

#define BUFFERSIZE 100

void fileReader(char *fileName) {
    char buffer[BUFFERSIZE];
    
    FILE *fp = fopen(fileName, "r");
    if (fp == NULL) {
        printf("Error opening the file.\n");
        exit(1); // make a graceful exit
    } // end if
    
    while (fgets(buffer, 100, fp)) {
        printf("%s\n", buffer);
    } // end while
    
    fclose(fp);
} // end method

int main()
{
    fileReader("password.txt");
    return(0);
}
