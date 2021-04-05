#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

/* TODOS
    - detect emoji using regex
*/


void get_emoji(char *flag){
    printf("FLAG: %s\n", flag);
    if(strstr(flag, ":BEAR:") != NULL){
        printf("ʕ•́ᴥ•̀ʔっ\n");
    }
    if(strstr(flag, ":WHAT:") != NULL){
        printf("(ㆆ_ㆆ)\n");
    }
    // case ":HUH:":
    //   printf('(͡° ͜ʖ ͡°)');
}

// use regex to find the 

\[(.*?)\]


int main(){
    char str[20];
    printf("type an emoji:\n");
    if( fgets (str, sizeof(str), stdin)!=NULL ) {
    /* writing content to stdout */
    }
    // printf("STRING: %s", str);
    get_emoji(str);


    return 0;
}