# Id-Hopping-CAN_BUS
* In order to defend against masquerade and replay attacks, a proposed method is implemented using ID Hopping for secure nodes in CAN BUS, in conjuction with an authentication signal following secure messages unique for each secure transmitter.


#Install can library for python3 on Ubuntu > 18.04
sudo apt update
sudo apt install python3-can

# Dynamic ID Generation Scheme and ID Hopping
The DIDG scheme generates unique 11-bit message identifiers
for secure nodes, incorporating fixed upper bits and encrypted
lower bits. This scheme facilitates ID hopping between secure
nodes, enhancing communication efficiency and minimizing the
risk of collision and unauthorized access.

# Signal Generation Algorithm (SGA) for Trustworthiness

To further enhance the trustworthiness of communication and
prevent masquerade and replay attacks, we integrate the Signal
Generation Algorithm (SGA). The SGA generates a unique and
recognizable signal that secure nodes periodically transmit using
the same ID derived from the DIDG scheme. 
This validation mechanism ensures that nodes have not been 
impersonated since the last
periodic transmission, 
enhancing trust and preventing unauthorized
access.

# Demo of a sine wave transmission
a node which follows the DIDG algorithm, transmits a sine wave over the vcan0, and a python program listening to 'vcan0' animates the data.
![img](https://github.com/NikosMouzakitis/Id-Hopping-CAN_BUS/blob/main/%CE%A3%CF%84%CE%B9%CE%B3%CE%BC%CE%B9%CF%8C%CF%84%CF%85%CF%80%CE%BF%20%CE%BF%CE%B8%CF%8C%CE%BD%CE%B7%CF%82%20(190).png)
