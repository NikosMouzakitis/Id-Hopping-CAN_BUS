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
#include <pthread.h>

#define CAN_INTERFACE "vcan0"        // Virtual CAN interface name

// Function to initialize the CAN socket

pthread_mutex_t vcan0Mutex;  // Mutex for vcan0
pthread_mutex_t vcan1Mutex;  // Mutex for vcan1

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

// Thread function for receiving CAN messages
void* receiveThread(void* arg) {
    printf("receiveThread runs on sender app.\n");
    int sock = *((int*)arg);
    struct can_frame frame;

    while (1) {

	pthead_mutex_lock(&vcan0Mutex);
        if (read(sock, &frame, sizeof(struct can_frame)) > 0) {
            // Process received CAN message
            perror("Socket read failed");
            break;
        }

        printf("Received CAN message: ID=0x%03X, DLC=%d, Data=", frame.can_id, frame.can_dlc);
	
	for (int i = 0; i < frame.can_dlc; ++i) 
            printf("%02X ", frame.data[i]);
        printf("\n");
        
    }
    return NULL;
}

// Thread function for sending CAN messages
void* sendThread(void* arg) {

    int sock = *((int*)arg);
    unsigned int index = 0;
    unsigned int id_list[] = {0x100, 0x200, 0x300, 0x400, 0x500};  // List of valid CAN message IDs
    unsigned int id_list_size = sizeof(id_list) / sizeof(id_list[0]);
    unsigned char data[8] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
    unsigned char dlc = sizeof(data);
    unsigned int delay;

    printf("sendThread runs on sender app.\n");

    while (1) {
        // Send the CAN message
	pthead_mutex_lock(&vcan0Mutex);
        sendMessage(sock, id_list[index], data, dlc);
	pthead_mutex_unlock(&vcan0Mutex);
        // Increment the index
        index++;

        // Wrap around to the beginning if the index exceeds the list size
        if (index >= id_list_size) {
            index = 0;
        }

        // Delay between messages
        usleep(500000);
    }
    return NULL;
}

int main() {
    int sock;
    pthread_t receiveThreadId, sendThreadId;

    // Initialize the CAN socket
    if ((sock = initSocket(CAN_INTERFACE)) < 0) {
        return 1;
    }

    // Create the receive thread
    if (pthread_create(&receiveThreadId, NULL, receiveThread, &sock) != 0) {
        perror("Failed to create receive thread");
        return 1;
    }

    // Create the send thread
    if (pthread_create(&sendThreadId, NULL, sendThread, &sock) != 0) {
        perror("Failed to create send thread");
        return 1;
    }

    // Wait for the threads to finish
    pthread_join(receiveThreadId, NULL);
    pthread_join(sendThreadId, NULL);

    return 0;
}

