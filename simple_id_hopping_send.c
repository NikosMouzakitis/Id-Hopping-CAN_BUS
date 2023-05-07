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
#define MESSAGE_DELAY_MIN 100000     // Minimum delay between messages in microseconds
#define MESSAGE_DELAY_MAX 500000     // Maximum delay between messages in microseconds


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
    unsigned int index = 0;
    unsigned int id_list[] = {0x100, 0x200, 0x300, 0x400, 0x500};  // List of valid CAN message IDs
    unsigned int id_list_size = sizeof(id_list) / sizeof(id_list[0]);
    unsigned char data[8] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
    unsigned char dlc = sizeof(data);
    unsigned int delay;

    // Initialize the CAN socket
    if ((sock = initSocket(CAN_INTERFACE)) < 0) {
        return 1;
    }

    srand(time(NULL));  // Seed the random number generator

    while (1) {
        // Send the CAN message
        sendMessage(sock, id_list[index], data, dlc);

        // Increment the index
        index++;

        // Wrap around to the beginning if the index exceeds the list size
        if (index >= id_list_size) {
            index = 0;
        }

        // Generate a random delay between messages
        delay = rand() % (MESSAGE_DELAY_MAX - MESSAGE_DELAY_MIN + 1) + MESSAGE_DELAY_MIN;

        // Delay between messages
        usleep(delay);
    }

    return 0;
}
