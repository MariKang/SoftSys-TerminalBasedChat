#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <regex.h> 

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


//Prints the status of reg ex complilation
void print_regex_complile_status(int return_val){
    if (return_val == 0){
        printf("Regular expression compiled successfully.\n");
    }
    else{
        printf("Compilation error.\n");
    }
}

// use regex to find the 

void find_emoji_regex(){
    char *msg = "jackie likes :BEAR: sdfhkjsdf";
    regex_t find_emoji;
    regmatch_t pmatch[1];
    size_t     nmatch = 1;

    int return_value = regcomp(&find_emoji, ":(.*?):", 0);
    print_regex_complile_status(return_value);

    if (0 == regexec(&find_emoji, "lol:BEAR:lol", 1, pmatch, 0)) {
        
      printf("With the whole expression, "
             "a matched substring \"%.*s\" is found at position %d to %d.\n",
             (int)(pmatch[0].rm_eo - pmatch[0].rm_so), &msg[pmatch[0].rm_so],
             (int)(pmatch[0].rm_so), (int)(pmatch[0].rm_eo - 1));
    } else {
        printf("no match\n");
    }


}


void find_emoji_easy(){
    char str[80] = "jackie likes :BEAR: sdfhkjsdf";
    const char delim[3] = "::";
    char *token;
    
    /* get the first token */
    token = strtok(str, delim);
    
    /* walk through other tokens */
    while( token != NULL ) {
        printf( " %s\n", token );
        
        token = strtok(NULL, delim);
    }
}



int main(){
    // char str[20];
    // printf("type an emoji:\n");
    // if( fgets (str, sizeof(str), stdin)!=NULL ) {
    // /* writing content to stdout */
    // }
    // // printf("STRING: %s", str);
    // get_emoji(str);

    // find_emoji_regex();
    find_emoji_easy();
    // char *msg = "jackie likes :BEAR: sdfhkjsdf";
    // char second_string[64];
    // int n1 = sscanf(msg, ":(.*?):", second_string);
    // printf("content: %s\n", second_string);
    return 0;
}