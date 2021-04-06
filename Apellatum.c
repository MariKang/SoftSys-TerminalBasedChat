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

int listener_d;
void handle_shutdown(int sig){
	if (listener_d)
		close(listener_d);

	fprintf(stderr, "Bye!\n");
	exit(0);
}

void error(char *msg){
	fprintf(stderr, "%s: %s\n", msg, strerror(errno));
	exit(1);
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

int say(int socket, char *s){
	int result = send(socket, s, strlen(s), 0);
	if (result == -1)
		fprintf(stderr, "%s: %s\n", "Error talking to the client", strerror(errno));
	return result;
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

struct read_message_args{
	char buf[255];
	int connect_d;
};


void *read_msg(void *arguments){
	struct read_message_args *args = arguments;

	while(1){
		read_in(args -> connect_d, args -> buf, sizeof(args -> buf));
		puts(args -> buf);
	}
}



int main(int argc, char *argv[]){

	int src_hst;
	if(catch_signal(SIGINT, handle_shutdown) == -1)
		error("Can't set interrupt handler");

	listener_d = open_listener_socket();
	bind_to_port(listener_d, 30000);

	if(listen(listener_d, 10) == -1)
		error("Can't Listen");

	struct sockaddr_storage client_addr;
	unsigned int address_size = sizeof(client_addr);

	puts("Welcome to the Apellatum Messenger.\n");


	if(gethostname(src_host, sizeof(src_host)) < 0)  {
		printf("Error in getting host name...\n");
		return 0;
	} else{
		for(int idx=0; src_hst-> h_addr_list[idx];idx++)
		{
			puts("Your IP Address is");
			puts(inet_ntoa((*(struct in_addr *) src_hst-> h_addr_list[idx])));
		}
	}

	puts("Waiting for connection...");
	char msg[1000];

	while(1){
	int connect_d = accept(listener_d, (struct sockaddr *)&client_addr, &address_size);
	if (connect_d == -1)
		error("Cannot Open Secondary Socket");

	if (say(connect_d, "Connected\n") != -1){
		// create thread

		pthread_t thread1;
		struct read_message_args args;
		args.connect_d = connect_d;
		pthread_create(&thread1, NULL, &read_msg, (void *)&args);

		while(1){
			fgets(msg,sizeof(msg),stdin);
			say(connect_d, msg);
		}

		pthread_join(thread1, NULL);
	}
	close(connect_d);
	}
	return 0;
}
