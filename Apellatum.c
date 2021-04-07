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
#define BUFFER_SZ 2048

static _Atomic unsigned int cli_count = 0;
static int user_id = 10;

/* Client structure */
typedef struct {
    struct sockaddr_in addr; /* Client remote address */
    int connect_f;              /* Connection file descriptor */
    int user_id;                 /* Client unique identifier */
    char name[32];           /* Client name */
} client_t;
client_t *clients[MAX_CLIENTS];

pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;
char *_strdup(const char *s) {
    size_t size = strlen(s) + 1;
    char *p = malloc(size);
    if (p) {
        memcpy(p, s, size);
    }
    return p;
}

void add_user(client_t *cl){
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (!clients[i]) {
            clients[i] = cl;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

/* Send message to all clients */
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

void broadcast_self(const char *s, int connect_f){
    if (write(connect_f, s, strlen(s)) < 0) {
        perror("Write to descriptor failed");
        exit(-1);
    }
}

int listener_d;
void handle_shutdown(int sig){
	if (listener_d)
		close(listener_d);

	fprintf(stderr, "Bye!\n");
	exit(0);
}


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

void error(char *msg){
	fprintf(stderr, "%s: %s\n", msg, strerror(errno));
	exit(1);
}

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

int read_in(int socket, char *buf, int len){
	char *s = buf;
	int slen = len;
	int c = recv(socket, s, slen, 0);
	while ((c > 0) && (s[c-1] != '\n')) {
		s += c; slen -= c;
		c = recv(socket, s, slen, 0);
	}
	if (c < 0)
		return c;
	else if (c == 0)
		buf[0] = '\0';
	else
		s[c-1]='\0';
	return len - slen;
}

int catch_signal(int sig, void (*handler)(int)){
	struct sigaction action;
	action.sa_handler = handler;
	sigemptyset(&action.sa_mask);
	action.sa_flags = 0;
	return sigaction (sig, &action, NULL);
}


// Returns hostname for the local computer
void checkHostName(int hostname)
{
    if (hostname == -1)
    {
        perror("gethostname");
        exit(1);
    }
}

// Returns host information corresponding to host name
void checkHostEntry(struct hostent * hostentry)
{
    if (hostentry == NULL)
    {
        perror("gethostbyname");
        exit(1);
    }
}

// Converts space-delimited IPv4 addresses
// to dotted-decimal format
void checkIPbuffer(char *IPbuffer)
{
    if (NULL == IPbuffer)
    {
        perror("inet_ntoa");
        exit(1);
    }
}

void *client_handle(void *arg){
  char output[BUFFER_SZ];
   char input[BUFFER_SZ / 2];
   int rlen;
   const char *res = NULL;
   char *str_tokens[100];
   cli_count++;
   client_t *cli = (client_t *)arg;
   char c[100] = "[BEAR]";
   char d[100] = "ʕ•́ᴥ•̀ʔっ";
   char* result = NULL;


   char msg_copy[1000];		// create copy bc strtok modifies orig str
  		//modifies str_tokens



  char nick[60];
  broadcast_self("Set your username\r\n", cli->connect_f);

  while ((rlen = read(cli->connect_f, input, sizeof(input) - 1)) > 0) {
    input[rlen] = '\0';
        output[0] = '\0';
        strip_newline(input);

        /* Ignore empty buffer */
        if (!strlen(input)) {
            continue;
        }


    char *old_name = _strdup(cli->name);
    strncpy(cli->name, input, sizeof(cli->name));
    cli->name[sizeof(cli->name)-1] = '\0';
    sprintf(output, "%s is now known as %s\r\n", old_name, cli->name);
    free(old_name);

  break;
}

   broadcast_all(output);
   while ((rlen = read(cli->connect_f, input, sizeof(input) - 1)) > 0) {
        input[rlen] = '\0';
        output[0] = '\0';
        strip_newline(input);
        strcpy(msg_copy, input);
        get_tokens(msg_copy, str_tokens);
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
        strcpy(input, replaceWord(input, c, d));

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

	// struct sockaddr_storage client_addr;
	unsigned int address_size = sizeof(cli_addr);

	puts("Welcome to the Apellatum Messenger.\n");
  char hostbuffer[256];
    char *IPbuffer;
    struct hostent *host_entry;
    int hostname;

    // To retrieve hostname
    hostname = gethostname(hostbuffer, sizeof(hostbuffer));
    checkHostName(hostname);

    // To retrieve host information
    host_entry = gethostbyname(hostbuffer);
    checkHostEntry(host_entry);

    // To convert an Internet network
    // address into ASCII string
    IPbuffer = inet_ntoa(*((struct in_addr*)
                           host_entry->h_addr_list[0]));

    printf("Host IP: %s \n", IPbuffer);

	puts("Waiting for connection...");
	char msg[1000];

	while(1){
    connect_f = accept(listener_d, (struct sockaddr*)&cli_addr, &address_size);
	if (connect_f == -1)
		error("Cannot Open Secondary Socket");

    client_t *cli = (client_t *)malloc(sizeof(client_t));
          cli->addr = cli_addr;
          cli->connect_f = connect_f;
          cli->user_id = user_id++;
          sprintf(cli->name, "%d", cli->user_id);

          /* Add client to the queue and fork thread */
          add_user(cli);
          pthread_create(&tid, NULL, &client_handle, (void*)cli);

        }
        return 0;
}
