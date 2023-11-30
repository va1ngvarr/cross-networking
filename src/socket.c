#include "socket.h"

static int status;

static inline int init_socket() {
#if defined(_WIN32)
  // Initialization Winsock2
  WSADATA wsaData;
  status = WSAStartup(MAKEWORD(2, 2), &wsaData);
  if (status != 0) {
    printf("WSAStartup failed with error: %d\n", status);
    return 1;
  }
  ZeroMemory(&hints, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;

  // Resolve the server address and port
  status = getaddrinfo(host, DEFAULT_PORT, &hints, &result);
  if (status != 0) {
    printf("getaddrinfo failed with error: %d\n", status);
    cleanup();
    return 1;
  }
#endif

  return 0;
}

inline void cleanup() {
#if defined(_WIN32)
  WSACleanup();
#else
  return;
#endif
}

static inline void quit_socket() {
  // shutdown the connection since no more data will be sent
  printf("Connection closing...\n");
  if (shutdown(Socket, SD_SEND) == SOCKET_ERROR) {
    printf("Shutdown failed with error: %d\n", WSAGetLastError());
  }

  turn_socket(Socket);
  cleanup();
}

/*
 * Set the timeout for socket input (in milliseconds).
 * This has to be wrapped as Windows has a weird arg type.
 * Return value is the same as setsockopt().
 * You should probably be using poll() or select(), but
 * this can be useful in certain cases where those are not
 * practical.
 */
static inline int sock_setrecvtimeout(socket_t sock, int32_t ms) {
#ifdef _WIN32
  return setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO,
                    reinterpret_cast<char *>(&ms), sizeof(ms));
#else
  struct timeval tv;
  tv.tv_usec = 1000L * ((long)ms - (long)ms / 1000L * 1000L);
  tv.tv_sec = (long)ms / 1000L;
  return setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO,
                    reinterpret_cast<void *>(&tv), sizeof(tv));
#endif /* _WIN32 */
}

/*
 * Set the timeout for socket output (in milliseconds).
 * This has to be wrapped as Windows has a weird arg type.
 * Return value is the same as setsockopt().
 */
static inline int sock_setsendtimeout(socket_t sock, int32_t ms) {
#ifdef _WIN32
  return setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO,
                    reinterpret_cast<char *>(&ms), sizeof(ms));
#else
  struct timeval tv;
  tv.tv_usec = 1000L * ((long)ms - (long)ms / 1000L * 1000L);
  tv.tv_sec = (long)ms / 1000L;
  return setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO,
                    reinterpret_cast<void *>(&tv), sizeof(tv));
#endif /* _WIN32 */
}

inline void sock_perror(const char *msg) {
#ifdef _WIN32
  // TODO use Win32's awful message formatting functions
  printf("%s: %ld\n", msg, WSAGetLastError());
#else
  perror(msg);
#endif /* _WIN32 */
}

inline int make_nonblocking(SOCKET Socket) {
#if defined(_WIN32)

  DWORD nonBlocking = 1;
  if (ioctlsocket(Socket, FIONBIO, &nonBlocking) != 0) {
    printf("failed to set non-blocking socket\n");
    return 1;
  }

#else

  if (fcntl(Socket, F_SETFL, O_NONBLOCK, 1) == -1) {
    printf("failed to set non-blocking socket\n");
    return 1;
  }

#endif

  return 0;
}

inline void send_message(const char *sendbuf) {
  if (send(Socket, sendbuf, DEFAULT_BUFLEN, 0) == SOCKET_ERROR)
    printf("Send failed with error: %d\n", WSAGetLastError());
}

inline void recv_message(const char *recvbuf) {
  if (recv(Socket, recvbuf, DEFAULT_BUFLEN, 0) < 0)
    printf("Rend failed with error: %d\n", WSAGetLastError());
}

inline void turn_socket(SOCKET Socket) {
#if defined(_WIN32)
  closesocket(Socket);
#else
  close(Socket);
#endif
}

void connecting() {
  if (accept(Socket, NULL, NULL) == INVALID_SOCKET)
    printf("Accept failed with error: %d\n", WSAGetLastError());
}
