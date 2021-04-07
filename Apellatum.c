#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>


#define MAX_CLIENTS 100
#define MSG_BUFFER_SZ 2048

static _Atomic unsigned int cli_count = 0;
static int user_id = 10;

int listener_d;


/* Client structure */
typedef struct {
    struct sockaddr_in addr;    /* Client remote address */
    int connect_f;              /* Connection file descriptor */
    int user_id;                /* Client unique identifier */
    char name[32];              /* Client name */
} client;
client *clients[MAX_CLIENTS];       /* Queue of current clients */

/* Initialize mutex to protect the shared resource: client */
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;


/*  Creates a new client by adding it to the current client queue

    Args:
        client *cl: client struct containing the info of the new client
*/
void add_user(client *cl){
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (!clients[i]) {
            clients[i] = cl;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}


/*  Send message to all clients

    Args:
        const char *s: message to send
*/
void broadcast_all(char *s){
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i <MAX_CLIENTS; ++i){
        if (clients[i]) {
            if (write(clients[i]->connect_f, s, strlen(s)) < 0) {
                perror("Write to descriptor failed");
                break;
            }
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}


/*  Sends message to self.

    Args:
        const char *s: message to send
        int connect_f: file descriptor for socket to write to
*/
void broadcast_self(const char *s, int connect_f){
    if (write(connect_f, s, strlen(s)) < 0) {
        perror("Write to descriptor failed");
        exit(-1);
    }
}


/*  Send message to everyone except for sender
    
    Args:
        char *s: Message to send
        int user_id: User ID of sender
*/
void send_message(char *s, int user_id){
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i]) {
            if (clients[i]->user_id != user_id) {
                if (write(clients[i]->connect_f, s, strlen(s)) < 0) {
                    perror("Write to descriptor failed");
                    break;
                }
            }
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}


/*  Shuts down the program.

    Args:
        int sig: signal of socket
*/
void handle_shutdown(int sig){
	if (listener_d)
		close(listener_d);

	fprintf(stderr, "Bye!\n");
	exit(0);
}


/* Function to replace a string with another string

    Args:
        char s: string to replace
        char oldW: words that will be replaced
        char newW: word to replace

    Returns:
        result: edited string
*/
char* replaceWord(const char* s, const char* oldW,
				const char* newW)
{
	char* result;

	int i, cnt = 0;
	int newWlen = strlen(newW);
	int oldWlen = strlen(oldW);

	// Counting the number of times old word occur in the string
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


/*  Returns text emojis that correspond to different emoji flags 
    
    Args:
        char *flag: emoji name in all caps
    Returns:
        char *emoji: text-styled emoji
*/
char* get_emoji(char *flag){
	char *emoji;
    if(strstr(flag, "BEAR") != NULL){
		emoji = "ʕ•́ᴥ•̀ʔっ";
    }
    else if(strstr(flag, "WHAT") != NULL){
		emoji = "(ㆆ_ㆆ)";
    }
	else if(strstr(flag, "HUH") != NULL){
		emoji = "(͡° ͜ʖ ͡°)";
    }
	else{
		emoji = "";
	}

	return emoji;
}


/*  Tokenize message if there are emojis (ex: [BEAR]) present. 

    Args:
        char *s: pointer to message to tokenize
        char *str_tokens[]: pointer to array of tokens as output
*/
void get_tokens(char *s, char *str_tokens[])
{
    int token_i = 0; 
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
    str_tokens[token_i] = "";       // signify end with empty string
}


/* Function to print errors */
void error(char *msg){
	fprintf(stderr, "%s: %s\n", msg, strerror(errno));
	exit(1);
}


void strip_newline(char *s){
    while (*s != '\0') {
        if (*s == '\r' || *s == '\n') {
            *s = '\0';
        }
        s++;
    }
}

void bind_to_port(int socket, int port){
	struct sockaddr_in name;
	name.sin_family = PF_INET;
	name.sin_port = (in_port_t)htons(30000);
	name.sin_addr.s_addr = htonl(INADDR_ANY);
	int reuse = 1;
	if (setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(int)) == -1)
		error("Can't set the reuse option on the socket");
	int c = bind (socket, (struct sockaddr *) &name, sizeof(name));
	if (c == -1)
		error("Can't bind to socket");
}


int open_listener_socket(){
	int s = socket(PF_INET, SOCK_STREAM, 0);
	if (s == -1)
		error("Can't open socket");
	return s;
}


int catch_signal(int sig, void (*handler)(int)){
	struct sigaction action;
	action.sa_handler = handler;
	sigemptyset(&action.sa_mask);
	action.sa_flags = 0;
	return sigaction (sig, &action, NULL);
}


/*  Returns hostname for the local computer

    Args:
        int hostname: the name of the host
*/
void check_host_name(int hostname){
    if (hostname == -1){
        perror("gethostname");
        exit(1);
    }
}


/*  Returns host information corresponding to host name

    Args:
        struct hostentry: host name acceptance
*/
void check_host_entry(struct hostent * hostentry){
    if (hostentry == NULL){
        perror("gethostbyname");
        exit(1);
    }
}


/*  Handles all client activies

Handles the client's messages and username settings. Uses the buffers read function to accept a username from
prompt and resets the user_id value.

Reads the messages and converts given commands into emojis determined by the function replace_word.
Outputs the rewritten message and outputs it to the users

Args:
    void arg: client struct containing client information
*/
void *client_handle(void *arg){
    char output[MSG_BUFFER_SZ];
    char input[MSG_BUFFER_SZ / 2];
    int rlen;
    cli_count++;
    client *cli = (client *)arg;
    
    /* Variables for emoji handling */
    char *str_tokens[100];
    char emoji_name[100] = "";
    char emoji[100] = "";
    char* result = NULL;
    char msg_copy[1000];		// create copy bc strtok modifies orig str modifies str_tokens
    

    /* Prompt user for new username and updates name */
    broadcast_self("Set your username\r\n", cli->connect_f);
    while ((rlen = read(cli->connect_f, input, sizeof(input) - 1)) > 0) {
        input[rlen] = '\0';
        output[0] = '\0';
        strip_newline(input);

        /* Ignore empty buffer */
        if (!strlen(input)) {
            continue;
        }

        /* Update client name */
        char *old_name = strdup(cli->name);
        strncpy(cli->name, input, sizeof(cli->name));
        cli->name[sizeof(cli->name)-1] = '\0';
        sprintf(output, "%s is now known as %s\r\n", old_name, cli->name);
        free(old_name);
        broadcast_all(output);
        break;
    }
    
    /* Handles messaging (includes emojis parsing) */
    while ((rlen = read(cli->connect_f, input, sizeof(input) - 1)) > 0) {
        input[rlen] = '\0';
        output[0] = '\0';
        strip_newline(input);
        strcpy(msg_copy, input);

        get_tokens(msg_copy, str_tokens);
        for (int i = 0; str_tokens[i][0] != '\0'; i++ ) {
            //update emoji name
            strcpy(emoji_name, "[");
            strcat(emoji_name, str_tokens[i]);
            strcat(emoji_name, "]");

            // update emoji
            strcpy(emoji, get_emoji(str_tokens[i]));

            // replace emoji text with emoji
            strcpy(input, replaceWord(input, emoji_name, emoji));
        }
        free(result);

        /* Ignore empty buffer */
        if (!strlen(input)) {
            continue;
        }
        else {
            /* Send message */
            snprintf(output, sizeof(output), "\n[%s] %s\r\n", cli->name, input);
            send_message(output, cli->user_id);
        }
    }
}

/* Catches the signal in the sockets using the listener. 

Identifies the IP address of the host computer and shows the introduction
with the IP address. Initializes the client.

Args:
    int argc: the number of strings pointed to by argv
    char *argv[]: input of the command

Return:
    0
*/
int main(int argc, char *argv[]){
    int connect_f = 0;
    struct sockaddr_in cli_addr;
    pthread_t tid;

	if(catch_signal(SIGINT, handle_shutdown) == -1)
		error("Can't set interrupt handler");

	listener_d = open_listener_socket();
	bind_to_port(listener_d, 30000);

	if(listen(listener_d, 10) == -1)
		error("Can't Listen");

	unsigned int address_size = sizeof(cli_addr);

	puts("Welcome to the Apellatum Messenger.\n");
    char hostbuffer[256];
    char *IPbuffer;
    struct hostent *host_entry;
    int hostname;

    // Retrieve hostname
    hostname = gethostname(hostbuffer, sizeof(hostbuffer));
    check_host_name(hostname);

    // Retrieve host information
    host_entry = gethostbyname(hostbuffer);
    check_host_entry(host_entry);

    // Convert an Internet network address into ASCII string
    IPbuffer = inet_ntoa(*((struct in_addr*)
                           host_entry->h_addr_list[0]));
    printf("Host IP: %s \n", IPbuffer);

	puts("Waiting for connection...");
	while(1){
        connect_f = accept(listener_d, (struct sockaddr*)&cli_addr, &address_size);
        if (connect_f == -1)
            error("Cannot Open Secondary Socket");

        client *cli = (client *)malloc(sizeof(client));
        cli->addr = cli_addr;
        cli->connect_f = connect_f;
        cli->user_id = user_id++;
        sprintf(cli->name, "%d", cli->user_id);

        /* Add client to the queue*/
        add_user(cli);
        pthread_create(&tid, NULL, &client_handle, (void*)cli);

    }
    return 0;
}
