import numpy as np
import matplotlib.pyplot as plt
import serial

elapsed_time = 0
t_list = []
moveflag_list = []
gyroX_list = []
gyroY_list = []
gyroZ_list = []
accX_list = []
accY_list = []
accZ_list = []

fig = plt.figure(figsize=(8,6))        # グラフのサイズを指定
plt.rcParams["font.size"] = 10    # フォントサイズ
#plt.grid()                        # グリッドを表示

ax_gyro = fig.add_subplot(2, 1, 1)
ax_acc = fig.add_subplot(2, 1, 2)
line_gyroX, = ax_gyro.plot(t_list, gyroX_list, lw=1, label="gyroX", color="#1f77b4")
line_gyroY, = ax_gyro.plot(t_list, gyroY_list, lw=1, label="gyroY", color="#ff7f0e")
line_gyroZ, = ax_gyro.plot(t_list, gyroZ_list, lw=1, label="gyroZ", color="#2ca02c")
line_accX, = ax_acc.plot(t_list, accX_list, lw=1, label="accX", color="#1f77b4")
line_accY, = ax_acc.plot(t_list, accY_list, lw=1, label="accY", color="#ff7f0e")
line_accZ, = ax_acc.plot(t_list, accZ_list, lw=1, label="accZ", color="#2ca02c")
ax_gyro.grid(True)
ax_acc.grid(True)
plt.pause(0.01)

is_receiving_ble = False

ser = serial.Serial('COM5',9600,timeout=1)
# clear receive buffer
ser.reset_input_buffer()

while True:
    try:
        line = ser.readline()   # read a '\n' terminated line
        text_data = line.decode().split("\n")[0]
        print(line)
        #print(text_data)

        if text_data == "end":
            is_receiving_ble = False
            
            for i_start, moveflag in enumerate(moveflag_list):
                if moveflag == 1:
                    print(i_start)
                    i_start = max(0, i_start-20)
                    break
            
            i_max = t_list.index(max(t_list))
            t_list = list(np.array(t_list)-t_list[i_start])
            

            
            line_gyroX.remove()
            line_gyroY.remove()
            line_gyroZ.remove()
            line_accX.remove()
            line_accY.remove()
            line_accZ.remove()
            
            line_gyroX, = ax_gyro.plot(t_list[i_start:i_max], gyroX_list[i_start:i_max], lw=1, label="gyroX", color="#1f77b4")
            line_gyroY, = ax_gyro.plot(t_list[i_start:i_max], gyroY_list[i_start:i_max], lw=1, label="gyroY", color="#ff7f0e")
            line_gyroZ, = ax_gyro.plot(t_list[i_start:i_max], gyroZ_list[i_start:i_max], lw=1, label="gyroZ", color="#2ca02c")
            line_accX, = ax_acc.plot(t_list[i_start:i_max], accX_list[i_start:i_max], lw=1, label="accX", color="#1f77b4")
            line_accY, = ax_acc.plot(t_list[i_start:i_max], accY_list[i_start:i_max], lw=1, label="accY", color="#ff7f0e")
            line_accZ, = ax_acc.plot(t_list[i_start:i_max], accZ_list[i_start:i_max], lw=1, label="accZ", color="#2ca02c")
            
            ax_gyro.set_xlim(0, max(t_list))
            ax_acc.set_xlim(0, max(t_list))
            ax_gyro.set_ylim(-2500, 2500)
            ax_gyro.set_yticks(np.arange(-2500, 2501, 500))
            ax_acc.set_ylim(-5, 5)
            ax_acc.set_yticks(np.arange(-5, 5.1, 1))
            ax_gyro.set_ylabel("Gyro (deg/s)")
            ax_acc.set_ylabel("Accel (g)")
            ax_acc.set_xlabel("Time (s)")
            plt.pause(0.01)

        
        elif is_receiving_ble:
            count, moveflag, elapsed_time, accX, accY, accZ, gyroX, gyroY, gyroZ = text_data.split("\t")
            moveflag = int(moveflag)
            elapsed_time = float(elapsed_time)
            accX = float(accX)
            accY = float(accY)
            accZ = float(accZ)
            gyroX = float(gyroX)
            gyroY = float(gyroY)
            gyroZ = float(gyroZ.split("\n")[0])
            
            elapsed_time = elapsed_time/1E3
            t_list.append(elapsed_time)
            moveflag_list.append(moveflag)
            gyroX_list.append(gyroX)
            gyroY_list.append(gyroY)
            gyroZ_list.append(gyroZ)
            accX_list.append(accX)
            accY_list.append(accY)
            accZ_list.append(accZ)
        
        elif text_data == "start":
            is_receiving_ble = True
            t_list = []
            moveflag_list = []
            gyroX_list = []
            gyroY_list = []
            gyroZ_list = []
            accX_list = []
            accY_list = []
            accZ_list = []
            #plt.close()


            



     
    except ValueError:
        pass

    except KeyboardInterrupt:
        # print(t_list)
        print("Keyboard Interrupt")
        print(len(t_list))
        break
