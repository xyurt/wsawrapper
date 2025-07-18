#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

static int winsock_initialize() {
	WSADATA _wsadata;
	return WSAStartup(MAKEWORD(2, 2), &_wsadata) == 0;
}

static int winsock_cleanup() {
	return WSACleanup();
}

static SOCKET socket_create() {
	return socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
}

static void socket_close(SOCKET socket) {
	closesocket(socket);
}

static int socket_set_control(SOCKET socket, long cmd, u_long argp) {
	return ioctlsocket(socket, cmd, &argp) == 0;
}

static int socket_set_option(SOCKET socket, int level, int name, const char *value, int value_length) {
	return setsockopt(socket, level, name, value, value_length) == 0;
}

static int socket_set_unblocking(SOCKET socket) {
	return socket_set_control(socket, FIONBIO, 1);
}

static int socket_set_blocking(SOCKET socket) {
	return socket_set_control(socket, FIONBIO, 0);
}

static int socket_connect(SOCKET socket, struct sockaddr_in address) {
	return connect(socket, (const struct sockaddr *)&address, sizeof(struct sockaddr_in)) == 0;
}

static int socket_connect_ex(SOCKET socket, const char *ip, const unsigned short port) {
	if (ip == NULL) return 0;
	struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	if (InetPtonA(AF_INET, ip, &address.sin_addr) != 1) return 0;
	return socket_connect(socket, address);
}

static int socket_send(SOCKET socket, const char *buffer, int buffer_size) {
	return send(socket, buffer, buffer_size, 0) == buffer_size;
}

static int socket_recv(SOCKET socket, char *buffer, int buffer_size) {
	return recv(socket, buffer, buffer_size, 0);
}

static int socket_peek(SOCKET socket, char *buffer, int buffer_size) {
	return recv(socket, buffer, buffer_size, MSG_PEEK);
}

static void fd_clear(fd_set *_fd_set, SOCKET socket) {
	FD_CLR(socket, _fd_set);
}

static void fd_zero(fd_set *_fd_set) {
	FD_ZERO(_fd_set);
}

static void fd_set_socket(fd_set *_fd_set, SOCKET socket) {
	FD_SET(socket, _fd_set);
}

static int fd_socket_is_set(fd_set *_fd_set, SOCKET socket) {
	return FD_ISSET(socket, _fd_set);
}

static int select_timeout(fd_set *readfds, fd_set *writefds, long tv_sec, long tv_microsec) {
	struct timeval timeout;
	timeout.tv_sec = tv_sec;
	timeout.tv_usec = tv_microsec;

	return select(0, readfds, writefds, NULL, (const struct timeval *)&timeout);
}

static int select_unblocking(fd_set *readfds, fd_set *writefds) {
	return select_timeout(readfds, writefds, 0, 0);
}

static int select_blocking(fd_set *readfds, fd_set *writefds) {
	return select(0, readfds, writefds, NULL, NULL);
}

static int host_resolve(const char *hostname, struct in_addr *out) {
	struct addrinfo hints = { 0 };
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	struct addrinfo *result;
	if (getaddrinfo(hostname, NULL, &hints, &result) != 0 || result == NULL || result->ai_addr == NULL) return 0;

	*out = ((struct sockaddr_in *)result->ai_addr)->sin_addr;
	freeaddrinfo(result);
	return 1;
}
