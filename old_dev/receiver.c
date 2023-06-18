#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <sys/time.h>

#define CAN_INTERFACE "vcan0"  // Virtual CAN interface name

int msg_cnt = 0; // id hop index.
int tot_cnt = 0; // total id hop messages.
struct timeval start_time, tmp,  end_time;
// Function to get the current timestamp
void getCurrentTime(struct timeval* timestamp) {
    gettimeofday(timestamp, NULL);
}

// Function to calculate the delta between two timestamps in microseconds
long long calculateDelta(struct timeval* start, struct timeval* end) {
    long long delta_sec = end->tv_sec - start->tv_sec;
    long long delta_usec = end->tv_usec - start->tv_usec;
    long long rv =  delta_sec * 1000000LL + delta_usec;
//    printf("Doing delta between values:\n");
//    printf(" start_sec = %lld   start_usec = %lld \n", start->tv_sec, start->tv_usec);
//    printf(" end_sec = %lld   end_usec = %lld \n", end->tv_sec, end->tv_usec);
//    printf(" delta_sec = %lld   delta_usec = %lld \n", delta_sec, delta_usec);
    return rv;
}

unsigned int id_list[] = {0x100, 0x200, 0x300, 0x400, 0x500};  
// List of valid CAN message IDs
// hopping thought those addresses of IDs.

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
// Function to receive and print CAN messages
void receiveMessages(int sock) {

    struct can_frame frame;
    struct timeval timeout;

    while (1) {
        if (read(sock, &frame, sizeof(struct can_frame)) < 0) {
            perror("Socket read failed");
            break;
        }

        // Process received CAN message
        //printf("Received CAN message: ID=0x%03X, DLC=%d, Data=", frame.can_id, frame.can_dlc);
        for (int i = 0; i < frame.can_dlc; ++i) {
            //printf("%02X ", frame.data[i]);
        }
        //printf("\n");

	if(frame.can_id == id_list[msg_cnt]) {
		tot_cnt++;

		getCurrentTime(&tmp);

        	//printf("Received ID-HOP CAN message: ID=0x%03X, DLC=%d, Data=", frame.can_id, frame.can_dlc);
		//printf("at %lld sec %lld us\n", start_time.tv_sec, start_time.tv_usec);
	
		//printing out the deltas between the arrivals of correct ID Hopping messages.	
		if( tot_cnt > 1) {
		
			msg_cnt = (msg_cnt + 1) % 5;
		    	// Get the end timestamp
			end_time = tmp;	
    			// Calculate the delta in microseconds
    			long long delta_microsec = calculateDelta(&start_time, &end_time);

	    		// Print the delta
			
    			printf("Delta: %lld microseconds\n", delta_microsec);
		}
		start_time = tmp;
	}
    }
}

int main() {
    int sock;

    // Initialize the CAN socket
    if ((sock = initSocket(CAN_INTERFACE)) < 0) {
        return 1;
    }

    // Receive and print CAN messages
    receiveMessages(sock);

    close(sock);
    return 0;
}
