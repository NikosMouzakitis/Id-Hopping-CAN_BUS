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
