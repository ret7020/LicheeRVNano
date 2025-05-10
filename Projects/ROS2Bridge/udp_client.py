import socket
import struct

class Parser:
    def __init__(self):
        self.formats = [
            "",       # 0 - Reserved
            "ffffff", # 1 - TF: x, y, z, roll, pitch, yaw
        ]
    def __call__(self, data):
        header = struct.unpack("HH", data[:4]) # msg_type, msg_id
        return header[0], header[1], struct.unpack(self.formats[header[0]], data[4:])
        
        

def main():
    parser = Parser()

    udp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    udp_socket.bind(("0.0.0.0", 8888))

    print("Listening for UDP broadcasts...")

    while True:
        data, addr = udp_socket.recvfrom(1024)
        msg_type, msg_id, msg = parser(data)
        print(msg_type, msg_id, msg)


if __name__ == "__main__":
    main()
