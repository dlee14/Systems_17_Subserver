#include "pipe_networking.h"
#include <signal.h>

void process(char *s);
void subserver(int from_client);

static void sighandler(int signo) {
  if (signo == SIGINT) {
    remove("luigi");
    exit(0);
  }
}

int main() {
  int from_client;
  int server = getpid();
  while (1) {
    // Setup subserver
    from_client = server_setup();
    if (getpid() != server) {
      // Subserver
      subserver(from_client);
    }
  }
  return 0;
}

void subserver(int from_client) {
  int to_client = server_connect(from_client);
  char buffer[BUFFER_SIZE];
  while (read(from_client, buffer, sizeof(buffer))) {
    char ssid[8];
    sprintf(ssid, "%d", getpid());
    printf("[subserver %s]: received [%s]\n", ssid, buffer);
    process(buffer);
    write(to_client, buffer, sizeof(buffer));
  }
  exit(0);
}

void process(char * s) {
  int i = 0;
  while(s[i]) {
    s[i] = 'Q';
    i++;
  }
}
