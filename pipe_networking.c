#include "pipe_networking.h"

/*=========================
server_setup
args:
creates the WKP (upstream) and opens it, waiting for a
connection.
removes the WKP once a connection has been made
returns the file descriptor for the upstream pipe.
=========================*/
int server_setup() {
  int from_client;
  char buffer[HANDSHAKE_BUFFER_SIZE];
  char sid[8];
  sprintf(sid, "%d", getpid());

  // Create WKP
  mkfifo("luigi", 0600);

  // Connect to client
  printf("[server %s] setup: making wkp, waiting for connection...\n\n", sid);
  from_client = open( "luigi", O_RDONLY, 0); // Open
  printf("\n[server %s] setup: I got a job for you, subserver!\n", sid);
  return from_client;
}


/*=========================
server_connect
args: int from_client
handles the subserver portion of the 3 way handshake
returns the file descriptor for the downstream pipe.
=========================*/
int server_connect(int from_client) {
  char buffer[HANDSHAKE_BUFFER_SIZE];

  // Create identification for the subserver
  char ssid[8];
  sprintf(ssid, "%d", getpid());

  // Read from client; Block until there is something to read
  read(from_client, buffer, sizeof(buffer));
  printf("[subserver %s] connect: received [%s]\n", ssid, buffer);

  // Connect to client
  int to_client = open(buffer, O_WRONLY, 0);

  // Send message to client
  write(to_client, buffer, sizeof(buffer));

  // Read from client; Block until there is something to read
  read(from_client, buffer, sizeof(buffer));
  printf("[subserver %s] connect: received [%s]\n", ssid, buffer);

  return to_client;
}

/*=========================
server_handshake
args: int * to_client
Performs the server side pipe 3 way handshake.
Sets *to_client to the file descriptor to the downstream pipe.
returns the file descriptor for the upstream pipe.
=========================*/
// int server_handshake(int *to_client) {
//   int from_client;
//
//   char buffer[HANDSHAKE_BUFFER_SIZE];
//
//   mkfifo("luigi", 0600);
//
//   //block on open, recieve mesage
//   printf("[server] handshake: making wkp\n");
//   from_client = open( "luigi", O_RDONLY, 0);
//   read(from_client, buffer, sizeof(buffer));
//   printf("[server] handshake: received [%s]\n", buffer);
//
//   remove("luigi");
//   printf("[server] handshake: removed wkp\n");
//
//   //connect to client, send message
//   *to_client = open(buffer, O_WRONLY, 0);
//   write(*to_client, buffer, sizeof(buffer));
//
//   //read for client
//   read(from_client, buffer, sizeof(buffer));
//   printf("[server] handshake received: %s\n", buffer);
//
//   return from_client;
// }

/*=========================
client_handshake
args: int * to_server
Performs the client side pipe 3 way handshake.
Sets *to_server to the file descriptor for the upstream pipe.
returns the file descriptor for the downstream pipe.
=========================*/
int client_handshake(int *to_server) {

  int from_server;
  char buffer[HANDSHAKE_BUFFER_SIZE];
  char sid[8];
  sprintf(sid, "%d", getpid());

  //send pp name to server
  printf("[client %s] setup: connecting to wkp\n", sid);
  *to_server = open( "luigi", O_WRONLY, 0);
  if ( *to_server == -1 )
  exit(1);

  //make private pipe
  sprintf(buffer, "%d", getpid() );
  mkfifo(buffer, 0600);

  // Sending private FIFO name
  write(*to_server, buffer, sizeof(buffer));

  // Open and wait for connection
  from_server = open(buffer, O_RDONLY, 0);
  read(from_server, buffer, sizeof(buffer));

  /*validate buffer code goes here */
  printf("[client %s] setup: received [%s]\n", sid, buffer);

  //remove pp
  remove(buffer);
  printf("[client %s] setup: removed pp\n", sid);

  //send ACK to server
  write(*to_server, ACK, sizeof(buffer));

  return from_server;
}
