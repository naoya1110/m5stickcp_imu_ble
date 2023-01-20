import numpy as np
import matplotlib.pyplot as plt
import serial

elapsed_time = 0
t_list = []
gyroX_list = []
gyroY_list = []
gyroZ_list = []

fig = plt.figure(figsize=(8,6))        # グラフのサイズを指定
plt.rcParams["font.size"] = 10    # フォントサイズ
plt.grid()                        # グリッドを表示

ax_gyro = fig.add_subplot(2, 1, 1)
ax_acc = fig.add_subplot(2, 1, 2)


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
            
            
            

            
            plt.ylim(-2500, 2500)
            plt.yticks(np.arange(-2500, 2501, 500))
            
            if len(t_list) > 200:
                del(t_list[0])
                del(gyroX_list[0])
                del(gyroY_list[0])
                del(gyroZ_list[0])
                plt.xlim(min(t_list), max(t_list))
            else:
                plt.xlim(0, 10)
                
            plt.xlabel("Time (s)")
            plt.ylabel("Gyro (deg/s)")

            
            plt.plot(t_list, gyroX_list, label="gyroX")
            plt.plot(t_list, gyroY_list, label="gyroY")
            plt.plot(t_list, gyroZ_list, label="gyroZ")
            plt.grid()
            plt.legend(loc=1)
            #plt.tight_layout()
            
            plt.pause(0.01) 
            plt.cla()
        
        
        except ValueError:
            pass

        except KeyboardInterrupt:
            break