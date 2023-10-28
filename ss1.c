#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <fcntl.h>
#include <pthread.h>

#define CAN_INTERFACE "vcan0"        // Virtual CAN interface name


// Function to initialize the CAN socket
pthread_mutex_t vcan0Mutex;  // Mutex for vcan0

//Necessary to read the data to transmit on the virtual CAN.
FILE *data_file;
char *sine_data="sine_test_data.txt.txt";
unsigned int sdata[360];


//Dynamic ID Generation part.(DIDG)
uint32_t didg_key1 = 0x4432; // key used to initialize the
// algorithm for the 1st secure node.


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

	printf("Tx success\n");
}

// Thread function for receiving CAN messages
void* receiveThread(void* arg) {
	printf("receiveThread runs on sender app.\n");
	int sock = *((int*)arg);
	int ret;
	struct can_frame frame;
	// Set the file descriptor to non-blocking mode
	int flags = fcntl(sock, F_GETFL, 0);
	fcntl(sock, F_SETFL, flags | O_NONBLOCK);

	while (1) {

		if(!pthread_mutex_trylock(&vcan0Mutex)) {
			//thread got the lock.

			ret = read(sock, &frame, sizeof(struct can_frame));
			//unlock the mutex.
			pthread_mutex_unlock(&vcan0Mutex);

			if(ret == -1)
				continue;

			printf("Received CAN message: ID=0x%03X, DLC=%d, Data=", frame.can_id, frame.can_dlc);

			for (int i = 0; i < frame.can_dlc; ++i)
				printf("%02X ", frame.data[i]);
			printf("\n");

		}
	}
	printf("Debug receiver thread exits..\n");
	return NULL;
}

// Thread function for sending CAN messages
void* sendThread(void* arg) {

	int sock = *((int*)arg);
	unsigned int index = 0;
	//structure holding offsets specific for the nodes ID Hopping.
	//utilization of the DIDG scheme.

	// List of valid CAN message IDs to be filled.
	unsigned int id_list[] = {0x100, 0x200, 0x300, 0x400, 0x500};
	unsigned int id_list_size = sizeof(id_list) / sizeof(id_list[0]);
	// data sent on the vcan0
	unsigned char data[8] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
	unsigned char dlc = sizeof(data);
	unsigned int delay;
	uint32_t id_prefix_ss1 = 0x13;

	/*IDs start with 0b'110'*/
	uint8_t upper_part3bit = 0x6;
	uint8_t lower_part8bit = 0x0;
	/* will be used modulo 360 in this demonstration */
	int data_index = 0 ;

	printf("sendThread runs on sender app.\n");


	//Calculate IDs based on the Dynamic ID Generation algorithm.

	for(int i = 0; i < id_list_size; i++)
	{
		//a function to create 8bit lower part of the id.
		lower_part8bit = (((didg_key1 << i)) );
		id_list[i] = (upper_part3bit << 8) | lower_part8bit;
	}
        
	while (1) {
		// fill the data field.
		for(int i = 0; i < 8; i++)
		{
			data[7-i] =  (unsigned char) ( (sdata[data_index] >> (i*4) ) & 0xf);
		}

		// Send the CAN message
		pthread_mutex_lock(&vcan0Mutex);
		sendMessage(sock, id_prefix_ss1 + id_list[index], data, dlc);
		pthread_mutex_unlock(&vcan0Mutex);
		// Increment the index and the data_index
		index++;
		data_index++;
		// Wrap around to the beginning if the index exceeds the list size
		if(index >= id_list_size)
			index = 0;
		

		// Delay between messages
		usleep(50000);
	}
	return NULL;
}

int initialize_test_data(void)
{

	data_file = fopen(sine_data,"r");

	if(data_file == NULL) {
		printf("Error opening the test data file\n");
		return 1;
	}

	int cnt = 0;

	while(cnt < 360 && fscanf(data_file, "%d",&sdata[cnt]) == 1) {
		cnt++;
	}

	fclose(data_file);
	/*
	    printf("read %d integers\n",cnt);
	    for(int i = 0; i < cnt; i++)
		    printf("%d   \n", sdata[i]);
	*/
	return 0;
}


int main() {
	int sock;

	pthread_t receiveThreadId, sendThreadId;

	// fetch the data to transmit.
	initialize_test_data();

	// Initialize the CAN socket
	if ((sock = initSocket(CAN_INTERFACE)) < 0) {
		return 1;
	}

	// Create the send thread
	if (pthread_create(&sendThreadId, NULL, sendThread, &sock) != 0) {
		perror("Failed to create send thread");
		return 1;
	}

	// Create the receive thread , must ensure is non-blocking call.
	if (pthread_create(&receiveThreadId, NULL, receiveThread, &sock) != 0) {
		perror("Failed to create receive thread");
		return 1;
	}
	// Wait for the threads to finish
	pthread_join(receiveThreadId, NULL);
	pthread_join(sendThreadId, NULL);

	return 0;
}

