import random
import can
import time

# Define the number of CAN messages you want to generate
num_messages = 1000
exclude_list = [ 0x6A3,0x633,0x645,0x677,0x6DB]
channel='vcan0'
bus = can.interface.Bus(channel=channel, bustype='socketcan')

# Open a file for writing
for _ in range(num_messages):
   # Generate a random CAN message ID (11 bits or 29 bits)
   message_id = random.randint(0, 0x7FF)  # 11-bit ID

   if message_id in exclude_list:
       message_id = message_id + 0x1  # don't let ID be a secure one.

   message=can.Message(arbitration_id=message_id, data = [0x1, 0x2, 0x3,0x4], extended_id = False)
   try:
       bus.send(message)
   except can.CanError:
       print("Can error")
   time.sleep(0.4)



