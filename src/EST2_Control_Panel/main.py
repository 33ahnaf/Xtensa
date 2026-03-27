# including libraries
import pyautogui
import time
import serial
import threading

# defining COM PORT and BAUD RATE
COM_PORT = "/dev/ttyUSB0"
BAUD_RATE = 9600

# function for simulating a key
def simulate_key(target_key):
    if(target_key != ''): # check if target is not a empty string
        if(target_key == 's'):
            target_key = 'shift' # s means shift, as esp32 will be sending a single character to maximize performance lol
        elif(target_key == '^'):
            target_key = 'ctrl' # ^ means ctrl
        elif(target_key == '!'):
            target_key = ' ' # ! used as ' ' because if esp32 sends a space then maybe if won't be recognized
        
        # simulating key
        pyautogui.keyDown(target_key)
        time.sleep(0.05)
        pyautogui.keyUp(target_key)

# main function
def listen_serial():
    with serial.Serial(COM_PORT, BAUD_RATE) as peripheral:
        while True:
            data = peripheral.readline().decode().strip()
            threading.Thread(target=simulate_key(data)).start()

# check if the runner is main
if __name__ == "__main__":
    listen_serial()