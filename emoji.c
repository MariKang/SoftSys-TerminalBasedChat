// C program to search and replace
// all occurrences of a word with
// other word.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function to replace a string with another
// string
char* replaceWord(const char* s, const char* oldW,
				const char* newW)
{
	char* result;

	int i, cnt = 0;
	int newWlen = strlen(newW);
	int oldWlen = strlen(oldW);

	// Counting the number of times old word
	// occur in the string
	for (i = 0; s[i] != '\0'; i++) {
		if (strstr(&s[i], oldW) == &s[i]) {
			cnt++;

			// Jumping to index after the old word.
			i += oldWlen - 1;
		}
	}

	// Making new string of enough length
	result = (char*)malloc(i + cnt * (newWlen - oldWlen) + 1);
	i = 0;

	while (*s) {
		// compare the substring with the result
		if (strstr(s, oldW) == s) {
			strcpy(&result[i], newW);
			i += newWlen;
			s += oldWlen;
		}
		else
			result[i++] = *s++;
	}

	result[i] = '\0';
	return result;
}

char* get_emoji(char *flag){
	char *emoji;
    // printf("FLAG: %s\n", flag);
    if(strstr(flag, "BEAR") != NULL){
		emoji = "ʕ•́ᴥ•̀ʔっ";
        // printf("ʕ•́ᴥ•̀ʔっ\n");
    }
    else if(strstr(flag, "WHAT") != NULL){
		emoji = "(ㆆ_ㆆ)";
        // printf("(ㆆ_ㆆ)\n");
    }
	else if(strstr(flag, "HUH") != NULL){
		emoji = "(͡° ͜ʖ ͡°)";
     	// printf("(͡° ͜ʖ ͡°)\n");
    }
	else{
		emoji = "";
	}

	return emoji;
}

void get_tokens(char *s, char *str_tokens[])
{
    int token_i = 0;
    // return tokens and later we loop through all the tokens, and if it is a valid emoji, we replace it
    char *start = s;
    char *end = s;
    while(*s) {
        if(*s == '[') start = s;
        else if(*s == ']') end = s;
        if(start < end && *start) {
              *end = 0;
              str_tokens[token_i] = start+1;
              start = s = end;
              token_i++;
        }
        s++;
    }
    str_tokens[token_i] = "";       // end empty string
    // for (int i = 0; str_tokens[i][0] != '\0'; i++ ) {
    //     printf("%s\n", str_tokens[i]);
    // }
     
}

int main()
{   
	// char emoji[20];
	const char *res = NULL;
	char *str_tokens[100];
    // char str[] = "(test 1 (test 2 3)foo(bar test) again)";
    char msg[1000] = "jackie likes [BEAR] sdfhkjsdf [HUH]";
	char c[100] = "[BEAR]";
	char d[100] = "ʕ•́ᴥ•̀ʔっ";
	char* result = NULL;

	
	char msg_copy[1000];		// create copy bc strtok modifies orig str
	strcpy(msg_copy, msg);
    get_tokens(msg_copy, str_tokens);		//modifies str_tokens


    printf("INPUT: %s\n", msg);

    for (int i = 0; str_tokens[i][0] != '\0'; i++ ) {
		
		//update emoji_text
		strcpy(c, "[");
		strcat(c, str_tokens[i]);
		strcat(c, "]");
		// printf("c: %s\n", c);

		// update emoji
		strcpy(d, get_emoji(str_tokens[i]));
		// printf("d: %s\n", d);

		// replace emoji text with emoji
		strcpy(msg, replaceWord(msg, c, d));

    }
	printf("OUTPUT: %s\n", msg);


	free(result);

return 0;
}

