import can

def receive_can_messages(channel='vcan0'):
    bus = can.interface.Bus(channel=channel, bustype='socketcan')
    
    try:

        while True:
            message = bus.recv()  #receive message
            print(f"Received Message on Channel {channel}:")
            print(message)
    except KeyboardInterrupt:
        print("Receiver terminated by user")

print("Python receiver starts execution")

receive_can_messages()



