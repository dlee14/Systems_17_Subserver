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
  int from_client = server_setup();
  int to_client;
  int f = fork();
  if (!f) {
    // Subserver
    to_client = server_connect(from_client);
  } else {
    // Server
    // Remove WKP
    remove("luigi");
  }
}

void subserver(int from_client) {

}

void process(char * s) {

}
