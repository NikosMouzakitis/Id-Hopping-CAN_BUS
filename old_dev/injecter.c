#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <time.h>

#define CAN_INTERFACE "vcan0"        // Virtual CAN interface name


// Function to initialize the CAN socket
int initSocket(const char* ifname) {
    int s;
    struct sockaddr_can addr;
    struct ifreq ifr;

    if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
        perror("Socket creation failed");
        return -1;
    }

    strcpy(ifr.ifr_name, ifname);
    ioctl(s, SIOCGIFINDEX, &ifr);

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(s, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("Socket bind failed");
        return -1;
    }

    return s;
}

// Function to send a CAN message
void sendMessage(int sock, unsigned int id, unsigned char* data, unsigned char dlc) {

    struct can_frame frame;

    frame.can_id = id;
    frame.can_dlc = dlc;
    memcpy(frame.data, data, dlc);

    if (write(sock, &frame, sizeof(struct can_frame)) != sizeof(struct can_frame)) {
        perror("Socket write failed");
    }
}

int main() {

    int sock;

    unsigned int id = 0x66;
    unsigned char data[8] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    unsigned char dlc = sizeof(data);
	//delay in us.
    unsigned int delay;
    delay = 0x20000;

    // Initialize the CAN socket
    if ((sock = initSocket(CAN_INTERFACE)) < 0) {
        return 1;
    }

    while (1) {
        // Send the CAN message
        sendMessage(sock, id, data, dlc);
        usleep(delay);
    }

    return 0;
}
