import can
import matplotlib.pyplot as plt
import numpy as np
from matplotlib.animation import FuncAnimation
import threading

#list of ID Hopping Id's.
exclude_list = [ 0x6A3,0x633,0x645,0x677,0x6DB]

#update with new values
def update_data(new_value):
    global data
    data = np.roll(data,-1) #shift data to the left.
    data[-1] = new_value ##add the new random value to the end

def update_plot(frame):
  # 
    line.set_data(np.arange(360), data)
    return line,


def receive_can_and_update_animation(channel='vcan0'):
    bus = can.interface.Bus(channel=channel, bustype='socketcan')
    expected_id_index = 0
    expected_id = exclude_list[expected_id_index]
    print("next expected id: ")
    print(expected_id)
   
    try:

        while True:
            message = bus.recv()  #receive message
            print(f"Received Message on Channel {channel}:")
            print(message)

            received_id  = message.arbitration_id
            expected_id = exclude_list[expected_id_index]

            if (received_id == expected_id):
                #new_value = int.from_bytes(message.data[-2:0],byteorder='little') 
                new_value = message.data[7] + (2*2*2*2*message.data[6])
                print("new value is: ",new_value)
                ##ANIMATION UPDATE
                update_data(new_value)
                ani.event_source.start() #starts the animation update.
                expected_id_index = (expected_id_index + 1)%5 
    except KeyboardInterrupt:
        print("Receiver terminated by user")
        bus.shutdown()

print("Python receiver starts execution")
data = np.zeros(360)


# creation of the figure.
fig, ax = plt.subplots()
ax.set_xlim(0,360)
ax.set_ylim(0,256)
#creation of an empty line for the initial plot.
line, = ax.plot([],[])
ani = FuncAnimation(fig, update_plot, frames=None, blit=True, interval=50)
#receive_can_messages()
can_thread = threading.Thread(target=receive_can_and_update_animation)
can_thread.start()

plt.show()
