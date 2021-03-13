#include <stdio.h>

int main(int argc, char *argv[]){
  if(catch_signal(SIGINT, handle_shutdown) == -1)
    error("Can't set interrupt handler");

  listener_d = open_listener_socket();
  bind_to_port(listener_d, 30000);

  if(listen(listener_d, 10) == -1)
    error("Can't Listen");

  struct sockaddr_storage client_addr;
  unsigned int address_size = sizeof(client_addr);
  puts("Waiting for connection...");
  char buf[255];

  while(1){
    int connect_d = accept(listener_d, (struct sockaddr *)&client_addr, &address_size);
    if (connect_d == -1)
      error("Cannot Open Secondary Socket");
    if (say(connect_d, "DATA") != -1){
      read_in(connect_d, buf, sizeof(buf));
      if (strncasecmp("Who's There?", buf, 12))
        say(connect_d, "You should say 'Who's There?'");
      else{
          if(sat(connect_d, "Oscar \r\n")!=-1){
            read_in(connect_d, buf, sizeof(buf));
            if (strncasecmp("Oscar Who?", buf, 10))
              say(connect_d, "You should say 'Oscar Who?'");
            else
              say(connect_d, "Heh");

          }

      }

    }
    close(connect_d)
  }
  return 0;
}
