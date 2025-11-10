#pragma once

#include <inttypes.h>
#include <limits.h>
#include <string.h>
#include <sys/param.h>

#include <lwip/err.h>
#include <lwip/netdb.h>
#include <lwip/sockets.h>
#include <lwip/sys.h>

enum SocketStatus
{
    SOCKET_STATUS_GETTING_ADDRESS_INFO = 0U,
    SOCKET_STATUS_UNKNOWN_SOCKET_TYPE,
    SOCKET_STATUS_FAILED_GET_ADDRESS_INFO,
    SOCKET_STATUS_CREATING_SOCKET,
    SOCKET_STATUS_FAILED_CREATE_SOCKET,
    SOCKET_STATUS_OK,
    SOCKET_STATUS_FAILED_BIND,
};

struct Socket
{
    enum SocketStatus status;
    struct addrinfo hints;
    struct addrinfo *p_address_info;
    int descriptor;
};

enum SocketType
{
    SOCKET_TYPE_TCP = 0,
    SOCKET_TYPE_UDP,
};

enum SocketStatus socket_init(enum SocketType type, char *ip_string, char *port_string,
                              struct Socket *p_socket);
void socket_free_address_info(struct Socket *socket);
void socket_bind(struct Socket *p_socket);