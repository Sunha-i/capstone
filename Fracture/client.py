import socket
import struct
import numpy as np
import time

HOST = '127.0.0.1'
PORT = 5050

client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
try:
    client.connect((HOST, PORT))
    print("successfully connected")
except socket.error:
    print("couldn't connect", socket.error)

start = time.time()

# Receive Array
array_size_bytes = client.recv(4)
array_size = struct.unpack('!I', array_size_bytes)[0]
array_data_bytes = client.recv(array_size * 4)
array_data = np.frombuffer(array_data_bytes, dtype=np.float32)
print("Received Array: ", array_data)

# Write vertices (COM data)
filename = "hourglass.txt"
with open(filename, 'w') as obj_file:
    num_vertices = len(array_data) // 3
    for i in range(num_vertices):
        obj_file.write(f"{array_data[i*3]} {array_data[i*3+1]} {array_data[i*3+2]}\n")
    print(f"Written {filename}")

# Send Array - tmp
clusteredIdx = \
[0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 1, 2, 2, 2, 2, 0, 0, 2, 0, 1, 0, 2, 2, 1, 1, 0, 2, 3, 2, 1, 1, 0, 1, 2, 0, 0, 1, 2, 2, 0, 0, 0, 0, 2, 2, 1, 2, 2, 2, 1, 2, 0, 1, 2, 1, 2, 1, 2, 2, 2, 2, 2, 0, 0, 0, 2, 0, 1, 1, 0, 0, 0, 1, 0, 0, 2, 2, 0, 0, 2, 2, 2, 2, 2, 1, 1, 1, 0, 2, 2, 2, 1, 2, 2, 2, 2, 0, 1, 2, 2, 0, 2, 1, 2, 2, 0, 0, 2, 1, 1, 0, 0, 1, 0, 2, 2, 2, 0, 0, 2, 1, 1, 2, 0, 0, 1, 2, 2, 0, 0, 0, 2, 0, 2, 0, 2, 0, 2, 1, 2, 2, 2, 2, 0, 1, 0, 2, 0, 2, 1, 3, 2, 0, 2, 1, 1, 0, 0, 2, 2, 0, 1, 2, 1, 2, 2, 0, 2, 2, 1, 0, 1, 2, 2, 1, 1, 1, 1, 0, 2, 2, 1, 2, 0, 1, 2, 3, 0, 2, 2, 2, 2, 0, 1, 1, 1, 0, 2, 1, 1, 0, 1, 2, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 0, 0, 2, 2, 0, 0, 1, 1, 2, 1, 1, 1, 2, 2, 2, 0, 2, 1, 2, 2, 2, 2, 2, 0, 2, 1, 0, 2, 2, 2, 2, 1, 1, 1, 2]
array = np.array(clusteredIdx, dtype=np.int32)
array = np.insert(array, 0, len(array))
array_bytes = array.tobytes()
client.sendall(array_bytes)

end = time.time()
print(f"total time: {end-start} ms")

client.close()