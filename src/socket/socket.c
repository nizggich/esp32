#include "socket.h"

#define GETADDRINFO_SUCCESSFUL 0

void socket_free_address_info(struct Socket *p_socket)
{
    if (p_socket->p_address_info)
    {
        freeaddrinfo(p_socket->p_address_info);
        p_socket->p_address_info = NULL;
    }
}

enum SocketStatus socket_init(enum SocketType type, char *s_ip, char *s_port,
                              struct Socket *p_socket)
{
    memset(p_socket, 0, sizeof(struct Socket));

    p_socket->hints.ai_family = AF_INET;
    p_socket->hints.ai_flags = AI_NUMERICHOST;

    switch (type)
    {
        case SOCKET_TYPE_TCP:
            p_socket->hints.ai_socktype = SOCK_STREAM;
            break;

        case SOCKET_TYPE_UDP:
            p_socket->hints.ai_socktype = SOCK_DGRAM;
            break;

        default:
            p_socket->status = SOCKET_STATUS_UNKNOWN_SOCKET_TYPE;
            return p_socket->status;
    }

    if (getaddrinfo(s_ip, s_port, &p_socket->hints, &p_socket->p_address_info) !=
        GETADDRINFO_SUCCESSFUL)
    {
        p_socket->status = SOCKET_STATUS_FAILED_GET_ADDRESS_INFO;
        return p_socket->status;
    }

    p_socket->descriptor =
        socket(p_socket->p_address_info->ai_family, p_socket->p_address_info->ai_socktype,
               p_socket->p_address_info->ai_protocol);
    if (p_socket->descriptor < 0)
    {
        socket_free_address_info(p_socket);
        p_socket->status = SOCKET_STATUS_FAILED_CREATE_SOCKET;
        return p_socket->status;
    }
    p_socket->status = SOCKET_STATUS_OK;
    return p_socket->status;
}

void socket_bind(struct Socket *p_socket)
{
    if (p_socket->status != SOCKET_STATUS_OK)
    {
        return;
    }
    if (bind(p_socket->descriptor, p_socket->p_address_info->ai_addr,
             p_socket->p_address_info->ai_addrlen) < 0)
    {
        p_socket->status = SOCKET_STATUS_FAILED_BIND;
    }
    socket_free_address_info(p_socket);
    return;
}