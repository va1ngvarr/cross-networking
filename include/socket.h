#pragma once

#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")
#pragma comment(lib, "AdvApi32.lib")

typedef SOCKET socket_t;

#else

#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

// POSIX sockets do not have a SOCKET structure and use integers
#define INVALID_SOCKET -1
typedef int socket_t;

#endif

#include <stdbool.h>
#include <stdio.h>

#define DEFAULT_BUFLEN 1024
#define DEFAULT_PORT "28916"

static struct addrinfo *result = NULL, *ptr = NULL, hints;

static SOCKET Socket = INVALID_SOCKET;

inline int init_socket();
inline void quit_socket();

inline void send_message(const char *sendbuf);
inline void recv_message(const char *recvbuf);

inline void turn_socket(SOCKET Socket);

inline void cleanup();

inline int make_nonblocking(SOCKET Socket);
