import numpy as np
import matplotlib.pyplot as plt
import serial

elapsed_time = 0
t_list = []
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

ax_gyro.grid(True)
ax_acc.grid(True)





with serial.Serial('COM4', 9600, timeout=1) as ser:
    while True:
        try:
            line = ser.readline()   # read a '\n' terminated line
            count, dt, accX, accY, accZ, gyroX, gyroY, gyroZ = line.decode().split("\t")
            count = int(count)
            dt = float(dt)
            accX = float(accX)
            accY = float(accY)
            accZ = float(accZ)
            gyroX = float(gyroX)
            gyroY = float(gyroY)
            gyroZ = float(gyroZ.split("\n")[0])
            
            # print(count, dt, accX, accY, accZ, gyroX, gyroY, gyroZ)
            
            elapsed_time += dt/1E6
            t_list.append(elapsed_time)
            gyroX_list.append(gyroX)
            gyroY_list.append(gyroY)
            gyroZ_list.append(gyroZ)
            accX_list.append(accX)
            accY_list.append(accY)
            accZ_list.append(accZ)
            
            
            

            
            ax_gyro.set_ylim(-2500, 2500)
            ax_gyro.set_yticks(np.arange(-2500, 2501, 500))

            ax_acc.set_ylim(-10, 10)
            ax_acc.set_yticks(np.arange(-10, 10.1, 2))
            
            if len(t_list) > 100:
                del(t_list[0])
                del(gyroX_list[0])
                del(gyroY_list[0])
                del(gyroZ_list[0])
                del(accX_list[0])
                del(accY_list[0])
                del(accZ_list[0])
                ax_gyro.set_xlim(min(t_list), max(t_list))
                ax_acc.set_xlim(min(t_list), max(t_list))
            else:
                ax_gyro.set_xlim(0, 10)
                ax_acc.set_xlim(0, 10)
                
            
            ax_gyro.set_ylabel("Gyro (deg/s)")
            ax_acc.set_ylabel("Accel (g)")
            ax_acc.set_xlabel("Time (s)")

            
            line_gyroX, = ax_gyro.plot(t_list, gyroX_list, lw=1, label="gyroX", color="#1f77b4")
            line_gyroY, = ax_gyro.plot(t_list, gyroY_list, lw=1, label="gyroY", color="#ff7f0e")
            line_gyroZ, = ax_gyro.plot(t_list, gyroZ_list, lw=1, label="gyroZ", color="#2ca02c")
            line_accX, = ax_acc.plot(t_list, accX_list, lw=1, label="accX", color="#1f77b4")
            line_accY, = ax_acc.plot(t_list, accY_list, lw=1, label="accY", color="#ff7f0e")
            line_accZ, = ax_acc.plot(t_list, accZ_list, lw=1, label="accZ", color="#2ca02c")
            
            ax_gyro.legend(loc=1)
            ax_acc.legend(loc=1)

            #plt.tight_layout()
            
            plt.pause(0.01) 
            line_gyroX.remove()
            line_gyroY.remove()
            line_gyroZ.remove()
            line_accX.remove()
            line_accY.remove()
            line_accZ.remove()
            #plt.cla()
        
        
        except ValueError:
            pass

        except KeyboardInterrupt:
            break