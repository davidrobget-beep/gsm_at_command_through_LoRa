import tkinter as tk
from tkinter import scrolledtext, PhotoImage
from tkinter import ttk

import serial                       # ???
import serial.tools.list_ports      # ???

import threading
import time


import commands         as CMD
import parse            as PARSE
import crc32            as CRC32
import transmit_manager

#if Serial is opened and a user presses Disconnect button ( to change COM port # )
#for synchronization with the read_thread.
Close_Serial_Connection_Start = 0

TX_Manager = transmit_manager.Transmit_Manager()


# * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
#  * * * * * *  B U T T O N   H A N D L E R S  * * * * * * * * * *
# * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#
# com_ports_wg - ttk.Combobox
# serial - serial "module"
#
def RefreshCOM_PortsBTN():
    global com_ports_wg
    global serial

    # Get a list of all available COM ports
    com_ports_ = serial.tools.list_ports.comports()
    com_ports_list = [port.device for port in com_ports_]

    # Adding combobox drop down list
    com_ports_wg['values'] = com_ports_list
    if com_ports_list:
        com_ports_wg.set(com_ports_list[0])  # Auto-select the first port
    else:
        com_ports_wg.set("No ports found")


def ConnectCOM_PortsBTN():
    global com_ports_wg
    global ser
    global user_TX_text_area
    global connect_button_widget
    global Close_Serial_Connection_Start

    if ser.is_open:
        Close_Serial_Connection_Start = 1
    else:
        #item = com_ports_wg.current()
        item = com_ports_wg.get()
        #if item >= 0:
        if item :
            #ser.port = com_ports_wg['values'][item]
            try:
                ser.port = item
                ser.open()
                connect_button_widget.config(text= f"Disonnect ({item})")
            except Exception as e:
                user_TX_text_area.insert("1.0", f"Error: {e}\r\n")
        else:  # -1
            user_TX_text_area.insert("1.0", "Choose Port first\r\n")



def Send_Message_UserInput(add_CR):

    if not ser.is_open:
        user_TX_text_area.insert("1.0", f"Connect to port first\r\n")
        return

    text1 = UserInput.get("1.0", tk.END).strip('\n') # and remove trailing \n (auto added by python)
    BigMessage_bytearray = bytearray(text1, "utf-8")
    if ( add_CR ):
        BigMessage_bytearray.extend(b'\x0D')    # CR

    PacketsList = []
    _start = 0
    _length = len(BigMessage_bytearray)

    if _length == 0 :
        return

    while _length:
        if _length <= CMD.MESSAGE_LEN:
            _stop = _start + _length
            PacketsList.append(BigMessage_bytearray[_start: _stop])
            # print(BigMessage_bytearray[_start: _stop])
            # print( f"START={_start} STOP={_stop}")
            _length = 0
        else:  # > 40
            _stop = _start + CMD.MESSAGE_LEN
            PacketsList.append(BigMessage_bytearray[_start: _stop])
            # print(BigMessage[_start: _stop])
            # print( f"START={_start} STOP={_stop}")
            _length -= CMD.MESSAGE_LEN
            _start = _stop

    CRC32.add_tech_info_into_message(PacketsList)  # header, crc

    for packet in PacketsList:
        data_structure = transmit_manager.data_structure( packet, 1)
        TX_Manager.queue.enqueue(data_structure)


# big messages will have been split onto some packets
def SendMessageBTN_clicked():
    Send_Message_UserInput(0)

# adds Carriage Return to user input
def SendMessage_with_CR_BTN_clicked():
    Send_Message_UserInput(1)

def Decode_UCS2_BTN_clicked( bigEndian ):
    text1 = UserInput.get("1.0", tk.END).strip('\n')  # and remove trailing \n (auto added by python)

    # example: 000A0033002004200430043404300440000A003400200417043D0430043A043E043C0441044204320430000A00350020041C04430437044B043A0430000A00380020041504490435
    bytes_ = bytes.fromhex(text1)

    if ( bigEndian == 1 ):
        text3 = bytes_.decode('utf-16-be')
    else:
        text3 = bytes_.decode('utf-16-le')

    user_TX_text_area.insert("1.0", f"Decoded : \r\n{text3}\r\n")


def Decode_UCS2_BE_BTN_clicked():
    Decode_UCS2_BTN_clicked(1)

def Decode_UCS2_LE_BTN_clicked():
    Decode_UCS2_BTN_clicked(0)

def SendMessageBTN_2_clicked():     # "Turn modem ON. PHY."

    if not ser.is_open:
        user_TX_text_area.insert("1.0", f"Connect to port first\r\n")
        return
                                                                #   # CRC32 0x5B0E3C02         # __CMD_PHY_TURN_MODEM_ON from Node 2
    data_structure = transmit_manager.data_structure(b'\x02\x00\x14\x02\x3C\x0E\x5B', 1)
    TX_Manager.queue.enqueue(data_structure)


def SendMessageBTN_3_clicked():     # "Turn modem OFF. PHY."

    if not ser.is_open:
        user_TX_text_area.insert("1.0", f"Connect to port first\r\n")
        return

    #                                                           # 0x5AD69085               # __CMD_PHY_TURN_MODEM_OFF from Node 2
    data_structure = transmit_manager.data_structure( b'\x02\x00\x15\x85\x90\xD6\x5A', 1)
    TX_Manager.queue.enqueue(data_structure)


def SendMessageBTN_4_clicked():

    if not ser.is_open:
        user_TX_text_area.insert("1.0", f"Connect to port first\r\n")
        return

    #                                                           # 0x5967C98B             # __CMD_MODEM_POWER_SWITCH from Node 2
    data_structure = transmit_manager.data_structure( b'\x02\x00\x17\x8B\xC9\x67\x59', 1 )
    TX_Manager.queue.enqueue(data_structure)


def SendMessageBTN_5_clicked():

    if not ser.is_open:
        user_TX_text_area.insert("1.0", f"Connect to port first\r\n")
        return

    #                                                           # 0x537146A1             # __CMD_ADC_GET_DATA from Node 2
    data_structure = transmit_manager.data_structure( b'\x02\x00\x19\xA1\x46\x71\x53', 1 )
    TX_Manager.queue.enqueue(data_structure)


def SendMessageBTN_6_clicked():

    if not ser.is_open:
        user_TX_text_area.insert("1.0", f"Connect to port first\r\n")
        return

    #                                                           # 0x5C5B95F1             # __CMD_GET_STATISTICS from Node 1
    data_structure = transmit_manager.data_structure( b'\x01\x00\x1A\xF1\x95\x5B\x5C', 1 )
    TX_Manager.queue.enqueue(data_structure)


def SendMessageBTN_7_clicked():

    if not ser.is_open:
        user_TX_text_area.insert("1.0", f"Connect to port first\r\n")
        return

    #                                                           # 0x5118B328             # __CMD_GET_STATISTICS from Node 2
    data_structure = transmit_manager.data_structure( b'\x02\x00\x1A\x28\xB3\x18\x51', 1 )
    TX_Manager.queue.enqueue(data_structure)


def SendMessageBTN_8_clicked():

    if not ser.is_open:
        user_TX_text_area.insert("1.0", f"Connect to port first\r\n")
        return

    #                                                           # 0x740FF763             #
    data_structure = transmit_manager.data_structure( b'\x03\x00\x0B\x1A\x63\xF7\x0F\x74', 1 )
    TX_Manager.queue.enqueue(data_structure)



# * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
#  * * * * * *  TRANSMIT TO COM-PORT  * * * * * * * * * *
# * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

def write_to_serial(ser, data):
    """Thread function to write data to serial port."""
    if ser.is_open :
        ser.write(data)
        debug_TX_text_area.insert("1.0", f"Sent: {data}\r\n")


# * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
# * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
# * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

TIMEOUT_TO_RESET_VALUE = 2000

class Merging_messages:
    def __init__(self):
        self.Started = 0
        self.flags = 0
        self.Total_message = bytearray()
        self.TotalPackets = 0
        self.mess_list = []
        self.timeoutToReset = 0        #  to reset the process when not all messages was come
        self.stop_Timer = 0            #  safe stop

    # mes_struct.message - is without Headers and CRC
    # messages may come not in order
    # mes_struct.CurrentPacket - from 1
    def append_message(self, mes_struct):

        if self.Started == 0 :

            Merging_M.flags = 0
            Merging_M.TotalPackets = mes_struct.TotalPackets
            Merging_M.mess_list = [bytearray()]*Merging_M.TotalPackets     # { [bytearray()] [bytearray()] [bytearray()] [bytearray()] ,,, }


            for n in range(1, Merging_M.TotalPackets+1) :       # set all bits
                Merging_M.flags |= (1 << n)

            self.Started = 1

        Merging_M.mess_list[mes_struct.CurrentPacket-1] = mes_struct.message

        Merging_M.timeoutToReset = TIMEOUT_TO_RESET_VALUE

        Merging_M.flags &= ~(1 << mes_struct.CurrentPacket)     # reset glag

        if Merging_M.flags == 0 :	# all flags were reset/( all messages were added to buf_to_modem)
            Merging_M.Total_message = bytearray()
            for n in range(0, Merging_M.TotalPackets):
                Merging_M.Total_message.extend(Merging_M.mess_list[n])
            Merging_M.Started = 0
            Merging_M.stop_Timer = 1

            return Merging_M.Total_message

        return bytearray()


Merging_M = Merging_messages()


# * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
#  * * * * * *  RECEIVIG FROM COM-PORT  * * * * * * * * * *
# * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

# Configure the serial port
ser = serial.Serial()
ser.baudrate = 9600
ser.timeout = 0.3   # fix it
#ser.port will be set ..  later.  ( port='COM25' )


confirmation_data = b'\x01\x00\x0C\x93\xB6\x02\x45'                         # FIX IT

serial_input_buffer = bytearray()                                           # FIX IT


def read_from_serial(ser):
    """Thread function to continuously read from serial port."""

    global Close_Serial_Connection_Start
    global serial_input_buffer
    in_waiting_old_state = 0  # indicates The byte thread is stopped  # like "IDLE_FRAME" irq in USART
    num_to_read = 0

    # while running:
    while (1):
        if ser.is_open:
            if ( not Close_Serial_Connection_Start ):   # == 0

                num_to_read = ser.in_waiting

                if num_to_read > 0:
                    in_waiting_old_state += 1
                    data = ser.read(num_to_read)
                    serial_input_buffer.extend(data)
                    # print(f"Received: {data.decode('utf-8', errors='replace')}")  #adjust decoding (or remove .decode()) for raw bytes.
                    # print(f"Received: {data}")
                    # text_area.insert(tk.END, f"Received: {data}\r\n")
                elif in_waiting_old_state > 0:  # && (num_to_read == 0)  # nothing to read (no NEW bytes), but I have bytes gotten
                    in_waiting_old_state = 0
                    debug_RX_text_area.insert("1.0", f"Received: {serial_input_buffer}\r\n")

                    #   handle/parse message...
                    len_ = len(serial_input_buffer)
                    if (len_ > 6):
                        if ( CRC32.CheckData(serial_input_buffer, len_) == 1 ):
                            parseDataResult = PARSE.parseData(serial_input_buffer, len_)
                        else:
                            parseDataResult = CMD.__CMD_BADCRC

                        if (parseDataResult == CMD.__CMD_RETRANSMIT):       # from Modem
                            TX_Manager.period_waitForConfirmation = TX_Manager.CONST_PERIOD_WAIT_FOR_CONFIRMATION   # high priority ;   update a delay to transmit
                            TX_Manager.gettingModemMessage = 1      #
                            TX_Manager.I_must_confirm = 1
                            user_RX_text_area.insert("1.0", "__CMD_RETRANSMIT\r\n")
                            res_ = PARSE.parse_Modem_Data(serial_input_buffer, len_)
                            if ( res_.TotalPackets == 1 ):
                                user_RX_text_area.insert("1.0", f"* * * * MESSAGE from MODEM start * * * *\r\n{res_.message}\r\n")
                                TX_Manager.gettingModemMessage = 0  # all packets are received
                            else:
                                res2_ = Merging_M.append_message(res_)
                                if ( res2_ ) :  # not b''
                                    user_RX_text_area.insert("1.0",f"* * * * MESSAGE from MODEM start * * * *\r\n{res2_}\r\n")
                                    TX_Manager.gettingModemMessage = 0  # all packets are received


                        elif (parseDataResult == CMD.__CMD_CONFIRMATION):
                            user_RX_text_area.insert("1.0", "__CMD_CONFIRMATION\r\n")
                            if ( TX_Manager.confirmations.TX_Confirmation_waitingFor == 1 ):
                                TX_Manager.confirmations.TX_Confirmation_waitingFor = 0
                                TX_Manager.confirmations.TX_ConfirmationGotten = 1

                        elif (parseDataResult == CMD.__CMD_PHY_MODEM_SWITCH_RESULT):
                            user_RX_text_area.insert("1.0", "__CMD_PHY_MODEM_SWITCH_RESULT\r\n")
                            TX_Manager.I_must_confirm = 1

                        elif (parseDataResult == CMD.__CMD_MODEM_STATUS_STATE):
                            TX_Manager.I_must_confirm = 1
                            res_ = PARSE.parse_Modem_Status(serial_input_buffer)
                            user_RX_text_area.insert("1.0", f"__CMD_MODEM_STATUS_STATE: {res_}\r\n")

                        elif (parseDataResult == CMD.__CMD_ADC_GET_DATA):
                            user_RX_text_area.insert("1.0", "__CMD_ADC_GET_DATA\r\n")
                            TX_Manager.I_must_confirm = 1
                            # parse and print float values
                            res_ = PARSE.parse_ADC_float_data(serial_input_buffer)
                            user_RX_text_area.insert("1.0", res_)

                        elif (parseDataResult == CMD.__CMD_GET_STATISTICS):  # it is Result after I send similar command
                            user_RX_text_area.insert("1.0", "__CMD_GET_STATISTICS\r\n")
                            # parse and print float values
                            res_ = PARSE.parse_Statistics_data(serial_input_buffer)
                            user_RX_text_area.insert("1.0", res_)
                            TX_Manager.I_must_confirm = 1


                        elif (parseDataResult == CMD.__CMD_BADCRC):
                            user_RX_text_area.insert("1.0", "__CMD_BADCRC\r\n")
                        elif (parseDataResult == CMD.__CMD_UNKNOWN):
                            user_RX_text_area.insert("1.0", "__CMD_UNKNOWN\r\n")
                            # add for test
                            # write_thread = threading.Thread(target=write_to_serial, args=(ser, confirmation_data))
                            # write_thread.start()

                    else:
                        user_RX_text_area.insert("1.0", "__CMD_BADSIZE\r\n")

                    serial_input_buffer.clear()
            else:   # Close_Serial_Connection_Start == 1
                ser.close()
                connect_button_widget.config(text="Connect to Com port")
                Close_Serial_Connection_Start = 0
                TX_Manager.queue.clear()
                TX_Manager.attempts_to_send = TX_Manager.CONST_ATTEMPTS_TO_SEND
                time.sleep(0.1) # private case  # ? delete

        time.sleep(0.05)  # = = = = = = = 50 MILLISECONDS


# Start the reading thread
read_thread = threading.Thread(target=read_from_serial, args=(ser,))
read_thread.daemon = True  # Thread will exit when main program exits
read_thread.start()


#   ADD TRY-EXCEPT-FINALLY
""" ADD TRY-EXCEPT-FINALLY """

# * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
#  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
# * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

window = tk.Tk()
window.title("Modem Terminal")
window.geometry('800x600')

###############################################################
#                  UserInput_frame                            #
###############################################################
#                                      #                      #
#                                      #                      #
#                                      #    Menu_frame        #
#           TX_RX_frame                #                      #
#                                      ########################
#                                      #                      #
#                                      #                      #
#                                      #       Help_frame     #
#                                      #                      #
#                                      #                      #
###############################################################

TX_RX_frame = tk.Frame(window, bg="papayawhip")
#Menu_Status_frame  = tk.Frame(window)
Help_frame  = tk.Frame(window, bg="lemonchiffon")
Menu_frame  = tk.Frame(window, bg="lightblue")
UserInput_frame = tk.Frame(window, bg = "lightgrey")

COM_Ports_frame  = tk.Frame(Menu_frame)

# * * * * *
UserInput_frame.place( x = 0, y = 0, relwidth = 1, relheight = 0.2)
TX_RX_frame.place( x = 0, rely = 0.2, relwidth = 0.6, relheight = 0.8)
Menu_frame.place( relx = 0.6, rely = 0.2, relwidth = 0.4, relheight = 0.4)
Help_frame.place( relx = 0.6, rely = 0.6, relwidth = 0.4, relheight = 0.4 )
# * * * * *

UserInput_frame_sub_top    = tk.Frame(UserInput_frame, bg = "lightgrey")
UserInput_frame_sub_bottom = tk.Frame(UserInput_frame, bg = "lightgrey")

UserInput_frame_sub_top.pack(expand = 1, fill = tk.BOTH)
UserInput_frame_sub_bottom.pack(expand = 1, fill = tk.BOTH)

lbl_userInput = tk.Label(UserInput_frame_sub_top, text="User Input")
lbl_userInput.pack(expand = 0, fill = tk.NONE, side = tk.LEFT)
UserInput = tk.scrolledtext.ScrolledText(UserInput_frame_sub_top,width=50,height=2)
UserInput.pack(expand = 1, fill = tk.X, side = tk.LEFT)

tk.Button(UserInput_frame_sub_bottom, text="Send big Message", command=SendMessageBTN_clicked).pack(expand = 0, fill = tk.NONE, padx=5, side = tk.LEFT )
tk.Button(UserInput_frame_sub_bottom, text="Send with CR", command=SendMessage_with_CR_BTN_clicked).pack(expand = 0, fill = tk.NONE, padx=5, side = tk.LEFT )
tk.Button(UserInput_frame_sub_bottom, text="Decode UCS-2 big-endian", command=Decode_UCS2_BE_BTN_clicked).pack(expand = 0, fill = tk.NONE, padx=5, side = tk.LEFT )
tk.Button(UserInput_frame_sub_bottom, text="Decode UCS-2 little-endian", command=Decode_UCS2_LE_BTN_clicked).pack(expand = 0, fill = tk.NONE, padx=5, side = tk.LEFT )

# * * * * *

lbl_userTX = tk.Label(TX_RX_frame, text="User TX")
lbl_userTX.pack(expand = 0, fill = tk.NONE)
user_TX_text_area = tk.scrolledtext.ScrolledText(TX_RX_frame,width=80, height=5)
user_TX_text_area.pack(expand = 1, fill = tk.BOTH)

lbl_userRX = tk.Label(TX_RX_frame, text="User RX")
lbl_userRX.pack(expand = 0, fill = tk.NONE)
user_RX_text_area = tk.scrolledtext.ScrolledText(TX_RX_frame,width=80,height=5)
user_RX_text_area.pack(expand = 1, fill = tk.BOTH)

lbl_debugTX = tk.Label(TX_RX_frame, text="Debug TX")
lbl_debugTX.pack(expand = 0, fill = tk.NONE)
debug_TX_text_area = tk.scrolledtext.ScrolledText(TX_RX_frame,width=80,height=5)
debug_TX_text_area.pack(expand = 1, fill = tk.BOTH)

lbl_debugRX = tk.Label(TX_RX_frame, text="Debug RX")
lbl_debugRX.pack(expand = 0, fill = tk.NONE)
debug_RX_text_area = tk.scrolledtext.ScrolledText(TX_RX_frame,width=80,height=5)
debug_RX_text_area.pack(expand = 1, fill = tk.BOTH)

# * * * * *

Help_descrybing = ["Report Mobile Equipment Error (Extended info)",
                   "The phone number will be received when an incoming call is received.",
                   "Set SMS system into text mode",
                   "Select TE Character Set",
                   "Delete All SMS",
                   "Mobile wallet balance",
                   "Read SMS Message with index 20",
                   "Request TA Serial Number Identification(IMEI)",
                   "Signal Quality Report",
                   "Operator Selection.\n Display the current network operator that the handset is currently registered with.",
                   "Read Operator Names",
                   "SIM card presence and status",
                   "Network Registration"]

Help_values     = ["AT+CMEE=2",
                   "AT+CLIP=1",
                   "AT+CMGF=1",
                   'AT+CSCS="UCS2"',
                   'AT+CMGDA="DEL ALL"',
                   'AT+CUSD=1,"*100#"',
                   'AT+CMGR=20,0',
                   'AT+GSN',
                   'AT+CSQ',
                   'AT+COPS?',
                   'AT+COPN',
                   'AT+CPIN?',
                   'AT+CREG?']

html_res = "https://m2msupport.net\nhttp://codius.ru/articles/251"

def helper_list_selection_changed(event):
    selected_indices = event.widget.curselection()
    if selected_indices:
        index = selected_indices[0]
        Help_text_area.delete("1.0", tk.END)
        Help_text_area.insert("1.0", Help_descrybing[index])

        #label.config(text=f"{event.widget.get(index)} selected!")
        #selected_item_text = event.widget.get(index)


def helper_double_click_handler(event):
    # Get the index of the double-clicked item
    #selected_index = listbox.curselection()[0]
    selected_index = event.widget.curselection()[0]
    # Get the text of the double-clicked item
    selected_item_text = event.widget.get(selected_index)        #listbox.get(selected_index)
    UserInput.delete("1.0", tk.END)
    UserInput.insert("1.0", selected_item_text )


Help_listbox = tk.Listbox(Help_frame)
for item in Help_values:
    Help_listbox.insert(tk.END, item)
Help_listbox.bind("<<ListboxSelect>>", helper_list_selection_changed)
Help_listbox.bind("<Double-Button-1>", helper_double_click_handler)
Help_listbox.pack(padx=5, pady=5, fill=tk.NONE, expand=0)

Help_text_area = tk.scrolledtext.ScrolledText(Help_frame,width=30,height=3)
Help_text_area.pack(expand = 1, fill = tk.BOTH)
#
Help_text_area.insert("1.0", html_res)

# * * * * * * * * * * * * * *



# * * * * *
n01 = tk.StringVar()
com_ports_wg = ttk.Combobox(COM_Ports_frame, width = 13, textvariable = n01, state="readonly")
com_ports_wg.pack(expand = 0, fill = tk.NONE, padx=(15,5) , side = tk.LEFT )
#com_ports_.current()

image01 = PhotoImage(file="i_m_a_g_e_s/refr_20_20.png")
refresh_button_widget = tk.Button(COM_Ports_frame, text="Refresh Com ports", image= image01, command= lambda : RefreshCOM_PortsBTN())
refresh_button_widget.pack(expand = 0, fill = tk.NONE, padx=5, side = tk.LEFT )

connect_button_widget = tk.Button(COM_Ports_frame, text="Connect to Com port", command= lambda : ConnectCOM_PortsBTN())
connect_button_widget.pack(expand = 0, fill = tk.NONE, padx=5, side = tk.LEFT )

COM_Ports_frame.pack(expand = 0, fill = tk.NONE, pady=(5,40)) # put in Menu_frame

tk.Button(Menu_frame, text="Turn modem ON. PHY.", command=SendMessageBTN_2_clicked).pack(expand = 0, fill = tk.NONE, pady=5 )
tk.Button(Menu_frame, text="Turn modem OFF. PHY.", command=SendMessageBTN_3_clicked).pack(expand = 0, fill = tk.NONE, pady=5 )
tk.Button(Menu_frame, text="Switch Modem Power(phy) ", command=SendMessageBTN_4_clicked).pack(expand = 0, fill = tk.NONE, pady=5 )
tk.Button(Menu_frame, text=" Get Sensors' Values ", command=SendMessageBTN_5_clicked).pack(expand = 0, fill = tk.NONE, pady=5 )
tk.Button(Menu_frame, text=" Get STAT from Node 1 ", command=SendMessageBTN_6_clicked).pack(expand = 0, fill = tk.NONE, pady=5 )
tk.Button(Menu_frame, text=" Get STAT from Node 2 ", command=SendMessageBTN_7_clicked).pack(expand = 0, fill = tk.NONE, pady=5 )

tk.Button(Menu_frame, text=" toModem. Ctrl-Z (0x1A) ", command=SendMessageBTN_8_clicked).pack(expand = 0, fill = tk.NONE, pady=5 )



#class AppState:
#    def __init__(self):
#        self.counter = 0
#        self.button_pressed = False




TIMER_PERIOD = 50
def check_task():
    global Close_Serial_Connection_Start

    if (ser.is_open and not Close_Serial_Connection_Start):  # 1 and != 0

        if TX_Manager.period_waitForConfirmation > 0 :
            TX_Manager.period_waitForConfirmation -= TIMER_PERIOD


    # # #   TX
        if ( TX_Manager.I_must_confirm ):
            TX_Manager.I_must_confirm = 0
            write_thread = threading.Thread(target=write_to_serial, args=(ser, confirmation_data))
            write_thread.start()
        else:
            if not TX_Manager.period_waitForConfirmation > 0 :      # <= 0
                data_str = TX_Manager.queue.peek()   # only reading
                if (data_str is not None) :
                    if ( TX_Manager.attempts_to_send > 0 ) :
                        TX_Manager.attempts_to_send -= 1
                        write_thread = threading.Thread(target=write_to_serial, args=(ser, data_str.b_array))
                        write_thread.start()
                        TX_Manager.confirmations.TX_Confirmation_waitingFor = data_str.TX_Confirmation_waitingFor
                        TX_Manager.period_waitForConfirmation = TX_Manager.CONST_PERIOD_WAIT_FOR_CONFIRMATION
                    else:
                        TX_Manager.queue.clear()
                        TX_Manager.attempts_to_send = TX_Manager.CONST_ATTEMPTS_TO_SEND
                        user_TX_text_area.insert("1.0", f"[Transmit is STOPPED]. No answer received({TX_Manager.CONST_ATTEMPTS_TO_SEND}).\r\n")

    # # #   RX
        if ( TX_Manager.confirmations.TX_ConfirmationGotten ) :
            TX_Manager.confirmations.TX_ConfirmationGotten = 0
            TX_Manager.queue.dequeue()  # item = TX_Manager.queue.dequeue()     # delete first item
            TX_Manager.attempts_to_send = TX_Manager.CONST_ATTEMPTS_TO_SEND
            if ( not TX_Manager.gettingModemMessage ):
                TX_Manager.period_waitForConfirmation = 0


    # # # Merging
        if Merging_M.timeoutToReset :
            if Merging_M.stop_Timer == 0 :
                Merging_M.timeoutToReset -= TIMER_PERIOD
                if Merging_M.timeoutToReset <= 0:
                    Merging_M.Total_message = bytearray()
                    for n in range(0, Merging_M.TotalPackets):
                        Merging_M.Total_message.extend(Merging_M.mess_list[n])
                    user_RX_text_area.insert("1.0", f"* * * * ! bad MESSAGE from MODEM start * * * *\r\n{Merging_M.Total_message}\r\n")
                    Merging_M.Started = 0
                    TX_Manager.gettingModemMessage = 0
            else:   # stop_Timer == 1
                Merging_M.stop_Timer = 0
                Merging_M.timeoutToReset = 0
                Merging_M.Started = 0
                TX_Manager.gettingModemMessage = 0


    window.after(TIMER_PERIOD, check_task)  # Schedule again after TIMER_PERIOD ms


# Start the periodic task
check_task()

window.mainloop()

if ser.is_open:
    Close_Serial_Connection_Start = 1


#   print ( " TEST . FIX ")