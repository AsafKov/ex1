#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* fooFixed(char* str, int* x);
void printMemoryAllocationErrorMessage();

int main(){
    int *x = malloc(sizeof(int));
    char *str = malloc(2*sizeof(char)+1);
    str[0] = ' ';
    str[1] = ' ';
    str[2] = '\0';
//    str[3] = 'l';
//    str[4] = 'o';
//    str[5] = '\0';
    fooFixed(str, x);
}

char* foo(char* str, int* x) {
    // should check if x is a valid pointer (not NULL)
    char* str2; // Should be initialized as NULL
    int i;
    // should check if str is not NULL (not handled by strlen())
    x = strlen(str); // should be *x
    str2 = malloc(*x); //should be *x * (sizeof(char) + 1
    // Missing memory allocation check
    // Should set last char to be '\0'
    for (i = 0; i < *x; i++) //missing brackets?
        str2[i] = str[*x - i]; //should be *x-i-1, out of bounds error
    if (*x % 2 == 0) { //should be == 1
        printf("%s", str);
    }
    if (*x % 2 != 0) { // Useless condition and wrong
        printf("%s", str2);
    }
    return str2;
}


void printMemoryAllocationErrorMessage(){
    printf("Process failed due to memory allocation error.\n");
}

char* fooFixed(char* str, int* x) {
    // should check if x is a valid pointer (not NULL)
    if(str == NULL || x == NULL)
        return NULL;
    char* str2 = NULL; // Should be initialized as NULL
    int i;
    // should check if str is not NULL (not handled by strlen())
    *x = (int) strlen(str); // should be *x
    printf("%d\n",*x);
    str2 = malloc((unsigned)*x * sizeof(char) +1); //should be *x * (sizeof(char)
    // Missing memory allocation check
    if(str2 == NULL){
        printMemoryAllocationErrorMessage();
        return NULL;
    }
    str2[*x] = '\0'; // Should set last char to be '\0'
    for (i = 0; i < *x; i++) {//missing brackets?
        str2[i] = str[*x - i -1]; //should be *x-i-1, out of bounds error
    }
    if (*x % 2 == 1) { //should be == 1
        printf("%s", str);
    } else {
        printf("%s", str2);
    }
    return str2;
}
