import tkinter as tk
from tkinter import ttk
import socket

IP = "192.168.1.106"
PORT = 9999

soc = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

root = tk.Tk()  
root.title("Controller Test")
root.geometry("400x250")
root.resizable(False, False)
root.iconbitmap(default="D:/work_space/fs42/controller_test/test_manager/favicon.ico")

frames = {}

group_value = tk.StringVar()

control_frame = ttk.Frame(root)
control_frame.pack()

test_cases = ["led", "buttons", "motor", "joystick"]
cmb_box = ttk.Combobox(control_frame, textvariable=group_value, values=test_cases, state = "readonly")
cmb_box.current(0)
cmb_box.pack()

def send_message(msg):
    try:
        soc.sendto(msg.encode('utf-8'), (IP, PORT))
        print("Sended:", msg)
    except Exception as e:
        print(f"Error: {e}")
        print("Msg:", msg)


frame_led = ttk.Frame(root)
btn1 = ttk.Button(frame_led, text="red", command= lambda: send_message("led_on_r")).pack()
btn2 = ttk.Button(frame_led, text="green", command= lambda: send_message("led_on_g")).pack()
btn3 = ttk.Button(frame_led, text="yellow", command= lambda: send_message("led_on_y")).pack()
btn4 = ttk.Button(frame_led, text="led_off", command= lambda: send_message("led_off")).pack()
frames["led"] = frame_led

frame_motor = ttk.Frame(root)
btn5 = ttk.Button(frame_motor, text="on", command= lambda: send_message("motor_on")).pack()
btn6 = ttk.Button(frame_motor, text="off", command= lambda: send_message("motor_off")).pack()
frames["motor"] = frame_motor

frame_buttons = ttk.Frame(root)
btn7 = ttk.Button(frame_buttons, text="log", command= lambda: send_message("btn_log")).pack()
btn8 = ttk.Button(frame_buttons, text="continous_log", command= lambda: send_message("btn_continous_log")).pack()
frames["buttons"] = frame_buttons

frame_joystick = ttk.Frame(root)
btn9 = ttk.Button(frame_joystick, text="log", command= lambda: send_message("joystick_log")).pack()
btn10 = ttk.Button(frame_joystick, text="continous_log", command= lambda: send_message("joystick_continous_log")).pack()
frames["joystick"] = frame_joystick

def on_combo_select(event):
    group = group_value.get()

    for frame in frames.values():
        frame.pack_forget()
    
    if (group == "led"):
        frames["led"].pack()
    if (group == "motor"):
        frames["motor"].pack()
    if (group == "buttons"):
        frames["buttons"].pack()
    if (group == "joystick"):
        frames["joystick"].pack()

def on_closing():
    soc.close()
    root.destroy()

cmb_box.bind("<<ComboboxSelected>>", on_combo_select)
on_combo_select(None)

root.mainloop()