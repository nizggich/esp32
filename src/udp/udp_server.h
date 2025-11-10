#pragma once

#include "socket/socket.h"
#include <string.h>

enum UdpServerStatus
{
    UDP_SERVER_STATUS_DISABLED = 0,
    UDP_SERVER_STATUS_ENABLED,
    UDP_SERVER_STATUS_WAITING_NETWORK,
    UDP_SERVER_STATUS_WAITING_IP,
    UDP_SERVER_STATUS_INITIALIZING_SOCKET,
    UDP_SERVER_STATUS_FAILED_INITIALIZING_SOCKET,
    UDP_SERVER_STATUS_FAILED_SOCKET_SET_OPTIONS,
    UDP_SERVER_STATUS_BINDING,
    UDP_SERVER_STATUS_FAILED_BIND,
    UDP_SERVER_STATUS_LISTENING,
    UDP_SERVER_STATUS_GOT_DATA,
    UDP_SERVER_STATUS_CLOSED,
    UDP_SERVER_STATUS_RECEIVE_ERROR,
};

struct UdpServer
{
    enum UdpServerStatus status;
    char *s_ip;
    char *s_port;
    bool *p_network_is_available;
    uint8_t *p_buffer;
    size_t buffer_size;
    size_t bytes_received;
    int errno_value;
    struct sockaddr_storage address;

    enum SocketStatus socket_status;
    struct Socket socket;
};

void udp_server_init(char *s_ip, char *s_port, bool *p_network_is_available, size_t buffer_size, struct UdpServer *p_server);
void udp_server_tick(struct UdpServer *p_server);
void udp_server_set_listening(struct UdpServer *p_server);
bool udp_server_is_online(struct UdpServer *p_server);
bool udp_server_has_data(struct UdpServer *p_server);