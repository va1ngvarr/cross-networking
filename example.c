#include <"socket.h"> 

inline int create_client(SOCKET Socket) {
  // Attempt to connect to an address until one succeeds
  for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
    // Create a SOCKET for connecting to server
    Socket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
    if (Socket == INVALID_SOCKET) {
      printf("Socket failed with error: %ld\n", WSAGetLastError());

      return 1;
    }

    // Connect to server.
    if (connect(Socket, ptr->ai_addr, (int)ptr->ai_addrlen) == SOCKET_ERROR) {
      turn_socket(Socket);
      Socket = INVALID_SOCKET;

      continue;
    }

    break;
  }

  freeaddrinfo(result);

  if (Socket == INVALID_SOCKET) {
    printf("Unable to connect to server!\n");
    return 1;
  }

  return 0;
}

inline int create_server(SOCKET Socket) {
  // Create a SOCKET for the server to listen for client connections.
  Socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
  if (Socket == INVALID_SOCKET) {
    printf("Socket failed with error: %ld\n", WSAGetLastError());
    freeaddrinfo(result);
    cleanup();

    return 1;
  }

  // Setup the TCP listening socket
  if (bind(Socket, result->ai_addr, (int)result->ai_addrlen) == SOCKET_ERROR) {
    printf("Bind failed with error: %d\n", WSAGetLastError());
    freeaddrinfo(result);
    turn_socket(Socket);
    cleanup();
  }

  return 0;
}
