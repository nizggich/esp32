#include "udp_server.h"

void udp_server_init(char *s_ip, char *s_port, bool *p_network_is_available, size_t buffer_size, struct UdpServer *p_server)
{
    memset(p_server, 0, sizeof(struct UdpServer));
    p_server->s_ip = s_ip;
    p_server->s_port = s_port;
    p_server->p_network_is_available = p_network_is_available;
    p_server->buffer_size = buffer_size;
    p_server->p_buffer = calloc(p_server->buffer_size, sizeof(uint8_t));
    p_server->status = UDP_SERVER_STATUS_DISABLED;
}

void udp_server_tick(struct UdpServer *p_server)
{
    while (1)
    {
        switch (p_server->status)
        {
        case UDP_SERVER_STATUS_DISABLED:
            return;

        case UDP_SERVER_STATUS_ENABLED:
            p_server->status = UDP_SERVER_STATUS_WAITING_NETWORK;
            continue;

        case UDP_SERVER_STATUS_WAITING_NETWORK:
            if (!(*(p_server->p_network_is_available)))
            {
                return;
            }
            p_server->status = UDP_SERVER_STATUS_WAITING_IP;
            continue;

        case UDP_SERVER_STATUS_WAITING_IP:
            if (!p_server->s_ip) {
                return;
            }
            p_server->status = UDP_SERVER_STATUS_INITIALIZING_SOCKET;
            continue;

        case UDP_SERVER_STATUS_INITIALIZING_SOCKET:
            p_server->socket_status = socket_init(SOCKET_TYPE_UDP, p_server->s_ip, p_server->s_port, &p_server->socket);
            if (p_server->socket_status != SOCKET_STATUS_OK)
            {
                p_server->status = UDP_SERVER_STATUS_FAILED_INITIALIZING_SOCKET;
                socket_free_address_info(&p_server->socket);
                return;
            }

            int flags = fcntl(p_server->socket.descriptor, F_GETFL);
            if (fcntl(p_server->socket.descriptor, F_SETFL, flags | O_NONBLOCK) == -1)
            {
                p_server->status = UDP_SERVER_STATUS_FAILED_INITIALIZING_SOCKET;
                socket_free_address_info(&p_server->socket);
                return;
            }

            p_server->status = UDP_SERVER_STATUS_BINDING;
            continue;

        case UDP_SERVER_STATUS_BINDING:
            socket_bind(&p_server->socket);
            if (p_server->socket_status != SOCKET_STATUS_OK)
            {
                p_server->status = UDP_SERVER_STATUS_FAILED_BIND;
                return;
            }
            p_server->status = UDP_SERVER_STATUS_LISTENING;
            continue;

        case UDP_SERVER_STATUS_LISTENING:
            socklen_t address_length = sizeof(struct sockaddr);
            int bytes_received = recvfrom(p_server->socket.descriptor, p_server->p_buffer, p_server->buffer_size, 0, (struct sockaddr *)&p_server->address, &address_length);
            if (bytes_received < 0)
            {
                if (errno == EINPROGRESS || errno == EAGAIN || errno == EWOULDBLOCK)
                {
                    return;
                }
                p_server->status = UDP_SERVER_STATUS_RECEIVE_ERROR;
                p_server->errno_value = errno;
                continue;
            }

            if (!bytes_received)
            {
                return;
            }

            p_server->bytes_received = (size_t)bytes_received;
            p_server->status = UDP_SERVER_STATUS_GOT_DATA;
            continue;

        case UDP_SERVER_STATUS_RECEIVE_ERROR:
        case UDP_SERVER_STATUS_CLOSED:
            p_server->status = UDP_SERVER_STATUS_LISTENING;
            return;

        default:
            return;
        }
    }
}

void udp_server_set_listening(struct UdpServer *p_server)
{
    if (p_server->status == UDP_SERVER_STATUS_GOT_DATA)
    {
        p_server->status = UDP_SERVER_STATUS_LISTENING;
    }
}

bool udp_server_is_online(struct UdpServer *p_server)
{
    return p_server->status == UDP_SERVER_STATUS_LISTENING || p_server->status == UDP_SERVER_STATUS_GOT_DATA;
}

bool udp_server_has_data(struct UdpServer *p_server)
{
    return p_server->status == UDP_SERVER_STATUS_GOT_DATA;
}

// #include "udp_server.h"

// bool is_udp_server_enabled = false;

// int udp_server_value = 0;
// uint8_t udp_server_buttons = 0;

// char udp_server_buffer[128];

// void udp_server_task(void *param)
// {
//     char rx_buffer[128];
//     char addr_str[128];
//     int addr_family = AF_INET;
//     int ip_protocol = 0;
//     struct sockaddr_in6 dest_addr;

//     while (1) {
//         if (!is_udp_server_enabled) {
//             vTaskDelay(pdMS_TO_TICKS(100));
//             continue;
//         }

//         struct sockaddr_in *dest_addr_ip4 = (struct sockaddr_in *)&dest_addr;

//         dest_addr_ip4->sin_addr.s_addr = htonl(INADDR_ANY);
//         dest_addr_ip4->sin_family = AF_INET;

//         dest_addr_ip4->sin_port = htons(UDP_PORT);
//         ip_protocol = IPPROTO_IP;

//         int sock = socket(addr_family, SOCK_DGRAM, ip_protocol);
//         if (sock < 0) {
//             ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
//             break;
//         }
//         ESP_LOGI(TAG, "Socket created");

//         // Set timeout
//         struct timeval timeout;
//         timeout.tv_sec = 60;
//         timeout.tv_usec = 0;
//         setsockopt (sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof timeout);

//         int err = bind(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
//         if (err < 0) {
//             ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
//         }
//         ESP_LOGI(TAG, "Socket bound, port %d", UDP_PORT);

//         struct sockaddr_storage source_addr;
//         socklen_t socklen = sizeof(source_addr);

//         while (1) {
//             // ESP_LOGI(TAG, "Waiting for data");
//             int len = recvfrom(sock, rx_buffer, sizeof(rx_buffer) - 1, 0, (struct sockaddr *)&source_addr, &socklen);
//             // Error occurred during receiving
//             if (len < 0) {
//                 ESP_LOGI(TAG, "recvfrom failed: errno %d", errno);
//                 break;
//             }
//             // Data received
//             else {
//                 // Get the sender's ip address as string
//                 if (source_addr.ss_family == PF_INET) {
//                     inet_ntoa_r(((struct sockaddr_in *)&source_addr)->sin_addr, addr_str, sizeof(addr_str) - 1);
//                 }
//                 rx_buffer[len] = 0; // Null-terminate whatever we received and treat like a string...
//                 // ESP_LOGI(TAG, "Received %d bytes from %s:", len, addr_str);
//                 // ESP_LOGI(TAG, "%s", rx_buffer);

//                 memcpy(udp_server_buffer, rx_buffer, 128);
//             }
//             vTaskDelay(pdMS_TO_TICKS(UDP_SERVER_TASK_DELAY_MS));
//         }

//         if (sock != -1) {
//             ESP_LOGI(TAG, "Shutting down socket and restarting...");
//             shutdown(sock, 0);
//             close(sock);
//         }
//     }
//     vTaskDelete(NULL);
// }
