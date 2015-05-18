#include <stdlib.h>
#include <stdio.h>
#include <string.h>


void trim(const char* src, char* buff, const unsigned int sizeBuff){
    if(sizeBuff < 1)
    return;

    const char* current = src;
    unsigned int i = 0;
    while(current != '\0' && i < sizeBuff-1)
    {
        if(*current != ' ')
            buff[i++] = *current;
        ++current;
    }
    buff[i] = '\0';
}

int main(int argc, char **argv){
    char operator[64];
    int num1 = 0;
    int num2 = 0;
    char input[1024];
    char buffer[1024];
    bzero(buffer, 1024);
    bzero(input, 1024);

    while(fgets(input, 1024, stdin) != NULL){
        bzero(buffer, 1024);
        trim(input, buffer, 1024);
        //num1 = buffer[0] - '0';
        //operator = buffer[1];
        //num2 = buffer[2] - '0';
        sscanf(buffer, "%d%c%d", &num1, operator, &num2);

        switch(operator[0]) {
            case '+':
                printf("%d + %d = %d",num1, num2, num1+num2);
                break;
            case '-':
                printf("%d - %d = %d",num1, num2, num1-num2);
                break;
            case '*':
                printf("%d * %d = %d",num1, num2, num1*num2);
                break;
            case '/':
                printf("%d / %d = %d",num1, num2, num1/num2);
                break;
            default:
                printf("Invalid Input!", operator);
                break;
        }
        printf("\n");
        bzero(input, 1024);
        if (feof(stdin)){
            exit(0);
        }
    }
    return 0;
}
