from serial import Serial
from serial.tools import list_ports

import json
import sys

baud = 9600

def select_controller_port():
    for port, desc, hwid in list_ports.comports():
        # print(f'{ port }, { desc }, { hwid }')
        if 'ACM' in port:
            print(f'Using { port }')
            return port

def get_method():
    if len(sys.argv) > 1:
        return sys.argv[1].lower()

def new_port() -> Serial:
    port = select_controller_port()
    return Serial(port, baud, timeout=10)


def serial_port(func: callable):
    def wrapper():
        with new_port() as ser:
            ser.flush()
            is_done = False
            while not is_done:
                try:
                    # Expects function to return True when finished
                    is_done = func(ser)
                    
                except json.decoder.JSONDecodeError or UnicodeDecodeError:
                    pass

    return wrapper

@serial_port
def read_card(ser: Serial) -> str:
    line = ser.readline()
    card = json.loads(line)
    if card:
        print(json.dumps(card))
        return True
    
    return False

@serial_port
def clear_card(ser: Serial) -> bool:
    line = ser.readline()
    card = json.loads(line)
    if card:
        ser.write(b'{ "clear": true }')
        output = json.dumps({ 'message': 'OK' })
        print(output)

        return True
    
    return False


@serial_port
def write_card(ser: Serial) -> bool:
    line = ser.readline()
    card = json.loads(line)
    if card:
        print("Writing card")
        ser.write(b'{ "write": true }')
        data = ser.readline().decode().strip("\n\r")    
        output = json.dumps({ 'message': 'OK', 'id': data })
        print(output)

        return True

    return False

if __name__ == "__main__":
    method = get_method()
    if method == "--read" or method == "-r":
        read_card()

    elif method == "--write" or method == "-w":
        write_card()

    elif method == "--clear" or method == "-c":
        clear_card()
    
    else:
        print('No method selected.')
        print('Use --read, --write, or --clear')