// Bluetooth接続スレーブ側プログラム
#include <M5StickCPlus.h>
#include "BluetoothSerial.h"
BluetoothSerial SerialBT;

#include <CircularBuffer.h>

// 変数宣言
String name = "M5StickC_Plus_2"; // 接続名を設定
int btn_pw = 0;           // 電源ボタン状態取得用
String data = "";         // 受信データ格納用

float accX = 0.0F;
float accY = 0.0F;
float accZ = 0.0F;
float accX_old = 0.0F;
float accY_old = 0.0F;
float accZ_old = 0.0F;
float accX_dif = 0.0F;
float accY_dif = 0.0F;
float accZ_dif = 0.0F;

float gyroX = 0.0F;
float gyroY = 0.0F;
float gyroZ = 0.0F;

float pitch = 0.0F;
float roll  = 0.0F;
float yaw   = 0.0F;

unsigned long t_new, t_old, dt, t_now;
unsigned long dt_target = 50000; // us
unsigned long t_delay = dt_target; // us

unsigned int measure_count = 0;
unsigned int send_count = 0;
unsigned int moveflag = 0;
unsigned int is_started = 0;
unsigned int is_finished = 0;
unsigned int is_started_count = 0;
unsigned int is_finished_count = 0;

unsigned int buf_size = 512;
CircularBuffer<float, 512> accX_buf, accY_buf, accZ_buf, gyroX_buf, gyroY_buf, gyroZ_buf;
CircularBuffer<int, 512> t_buf, moveflag_buf;

// String data_buf="";

// 初期設定 -----------------------------------------------
void setup() {


    M5.begin();             // 本体初期化
    M5.Imu.Init();          // IMU初期化
    M5.IMU.SetGyroFsr(M5.IMU.GFS_2000DPS);
    M5.IMU.SetAccelFsr(M5.IMU.AFS_4G); 
    Serial.begin(115200);     // シリアル通信初期化

    // 出力設定
    pinMode(10, OUTPUT);    //本体LED赤
    digitalWrite(10, HIGH); //本体LED初期値OFF（HIGH）
    
    // LCD表示設定
    M5.Lcd.setTextFont(2);
    M5.Lcd.println(name); // 接続名表示

    // Bluetooth接続開始
    SerialBT.begin(name); // 接続名を指定して初期化。第2引数にtrueを書くとマスター、省略かfalseでスレーブ
    // MACアドレスの取得と表示
    uint8_t macBT[6];
    esp_read_mac(macBT, ESP_MAC_BT);  // MACアドレス取得
    M5.Lcd.printf("%02X:%02X:%02X:%02X:%02X:%02X\r\n", macBT[0], macBT[1], macBT[2], macBT[3], macBT[4], macBT[5]);
    // Serial.printf("%02X:%02X:%02X:%02X:%02X:%02X\r\n", macBT[0], macBT[1], macBT[2], macBT[3], macBT[4], macBT[5]);
    
    // 電源ON時のシリアルデータが無くなるまで待つ
    while (Serial.available()) {data = Serial.read();}
    


    t_old = micros();
}

void measure_data(){
    is_started = 0;
    is_started_count = 0;
    is_finished = 0;
    is_finished_count = 0;


    t_buf.clear();
    accX_buf.clear();
    accY_buf.clear();
    accZ_buf.clear();
    gyroX_buf.clear();
    gyroY_buf.clear();
    gyroZ_buf.clear();

    Serial.println("measurement start");

    while (1){

        M5.update();  // 本体のボタン状態更新
        // 再起動（リスタート）処理
        if (btn_pw == 2) {ESP.restart();}   //電源ボタン短押し（1秒以下）なら再起動
        btn_pw = M5.Axp.GetBtnPress();  // 電源ボタン状態取得（1秒以下のONで「2」1秒以上で「1」すぐに「0」に戻る）

        // IMUデータ取得
        M5.IMU.getGyroData(&gyroX, &gyroY, &gyroZ);
        M5.IMU.getAccelData(&accX, &accY, &accZ);
        t_now = millis();

        // リングバッファへのデータの追加
        t_buf.push(t_now);
        accX_buf.push(accX);
        accY_buf.push(accY);
        accZ_buf.push(accZ);
        gyroX_buf.push(gyroX);
        gyroY_buf.push(gyroY);
        gyroZ_buf.push(gyroZ);

        // 加速度の差分
        accX_dif = abs(accX - accX_old);
        accY_dif = abs(accY - accY_old);
        accZ_dif = abs(accZ - accZ_old);

        // 動いているか判定して
        if ((accX_dif > 0.1)|(accY_dif > 0.1)|(accZ_dif > 0.1)){moveflag = 1;}
        else {moveflag=0;}

        if (moveflag==1){
            is_started_count += 1;
            is_finished_count = 0;
        }
        else{
            is_started_count = 0;
            if (is_started==1){is_finished_count += 1;}
        }

        if (is_started_count >= 10){is_started = 1;} //0.5s以上連続して動いているなら動き出したと判定
        if (is_finished_count >= 20){is_finished = 1;} //1.0s以上連続して止まっているなら動きが停止したと判定

        moveflag_buf.push(moveflag);
        Serial.printf("%d\t%d\t%d\t%d\t%d\t%d\t%5.2f\t%5.2f\t%5.2f\n",
                        t_now,
                        moveflag,
                        is_started_count,
                        is_started,
                        is_finished_count,
                        is_finished,
                        accX_dif,
                        accY_dif,
                        accZ_dif);

        accX_old = accX;
        accY_old = accY;
        accZ_old = accZ;

        // 遅延時間の計測と調整
        t_new = micros();
        dt = t_new - t_old;
        t_old = t_new;

        if (dt > 1.001*dt_target){t_delay *= 0.999;}
        else if (dt < 0.999*dt_target){t_delay *= 1.001;}

        delayMicroseconds(t_delay);  // 遅延時間 us

        measure_count += 1;
        if (measure_count == 20){
            digitalWrite(10, LOW); //LED ON
            measure_count = 0;
            }
        else {digitalWrite(10, HIGH);} //LED OFF

        // finish measurement
        if (is_finished){break;}
    }
    Serial.println("measurement finished\n");

}

void ble_send_data(){
    digitalWrite(10, LOW); //LED ON
    SerialBT.printf("start\n");
    for (int i=0; i<buf_size; i++){
        SerialBT.printf("%d\t%d\t%d\t%5.2f\t%5.2f\t%5.2f\t%6.2f\t%6.2f\t%6.2f\n",
                        i,
                        moveflag_buf.shift(),
                        t_buf.shift(),
                        accX_buf.shift(),
                        accY_buf.shift(),
                        accZ_buf.shift(),
                        gyroX_buf.shift(),
                        gyroY_buf.shift(),
                        gyroZ_buf.shift());

        send_count += 1;
        if (send_count == 10){
            digitalWrite(10, LOW); //LED ON
            send_count = 0;
            }
        else {digitalWrite(10, HIGH);} //LED OFF
        delay(10);

    }
    digitalWrite(10, HIGH); //LED OFF
    SerialBT.printf("end\n\n");
}

// メイン -------------------------------------------------
void loop() {
    measure_data();
    ble_send_data();
}