#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

void get_emoji(char *flag){
    printf("%s", flag);
    if(strcmp(flag, ":BEAR:")==0){
        printf("ʕ•́ᴥ•̀ʔっ\n");
    }
    if(strcmp(flag, ":WHAT:")==0){
        printf("(ㆆ_ㆆ)\n");
    }
    // case ":HUH:":
    //   printf('(͡° ͜ʖ ͡°)');
}

// use regex to find the 

int main(){
    char str[6];
    printf("type an emoji:\n");
    if( fgets (str, sizeof(str), stdin)!=NULL ) {
    /* writing content to stdout */
    }
    printf("STRING: %s", str);
    get_emoji(str);


    return 0;
}