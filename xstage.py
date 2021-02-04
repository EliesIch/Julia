import serial
serl = serial.Serial("COM6","9600",timeout=1)
serl.write(b'M:1-P1000\r\n')
serl.write(b'G\r\n')