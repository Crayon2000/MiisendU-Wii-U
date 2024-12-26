#include "udp.h"
#include <coreinit/thread.h>
#include <sys/socket.h>
#include <sys/unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>

static int udp_socket = -1;
static volatile bool udp_lock = false;

/**
 * Initialize the UDP socket.
 * @param ipString The IP address to connect to.
 * @param ipport The port to connect to.
 */
void udp_init(std::string_view ipString, uint16_t ipport)
{
    udp_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(udp_socket < 0) {
        return;
    }

    struct sockaddr_in connect_addr;
    std::memset(&connect_addr, 0, sizeof(connect_addr));
    connect_addr.sin_family = AF_INET;
    connect_addr.sin_port = ipport;
    inet_aton(ipString.data(), &connect_addr.sin_addr);

    if(connect(udp_socket, reinterpret_cast<struct sockaddr*>(&connect_addr), sizeof(connect_addr)) < 0) {
        close(udp_socket);
        udp_socket = -1;
    }
}

/**
 * Deinitialize the UDP socket.
 */
void udp_deinit()
{
    if(udp_socket >= 0) {
        close(udp_socket);
        udp_socket = -1;
    }
}

/**
 * Print a string to the UDP socket.
 * @param str The string to send.
 * @return Returns true on success, false otherwise.
 */
bool udp_print(std::string_view str)
{
    // socket is always 0 initially as it is in the BSS
    if(udp_socket < 0) {
        return false;
    }

    while(udp_lock == true) {
        OSSleepTicks(OSMillisecondsToTicks(1));
    }
    udp_lock = true;

    bool result = true;

    while (str.empty() == false) {
        const size_t block = std::min(str.size(), static_cast<size_t>(1400)); // take max 1400 bytes per UDP packet
        const ssize_t ret = send(udp_socket, str.data(), block, 0);
        if(ret < 0) {
            result = false;
            break;
        }

        str.remove_prefix(ret);
    }

    udp_lock = false;

    return result;
}
