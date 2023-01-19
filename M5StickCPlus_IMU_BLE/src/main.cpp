// Bluetooth接続スレーブ側プログラム
#include <M5StickCPlus.h>
#include "BluetoothSerial.h"
BluetoothSerial SerialBT;

// 変数宣言
String name = "M5StickC_Plus"; // 接続名を設定
int btn_pw = 0;           // 電源ボタン状態取得用
String data = "";         // 受信データ格納用

float accX = 0.0F;
float accY = 0.0F;
float accZ = 0.0F;

float gyroX = 0.0F;
float gyroY = 0.0F;
float gyroZ = 0.0F;

float pitch = 0.0F;
float roll  = 0.0F;
float yaw   = 0.0F;

unsigned long t_new, t_old, dt;
unsigned long dt_target = 5000; // us
unsigned long t_delay = 5000; // us

unsigned int count = 0;


// 初期設定 -----------------------------------------------
void setup() {
    M5.begin();             // 本体初期化
    M5.Imu.Init();          // IMU初期化
    Serial.begin(9600);     // シリアル通信初期化

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
    // M5.Lcd.printf("%02X:%02X:%02X:%02X:%02X:%02X\r\n", macBT[0], macBT[1], macBT[2], macBT[3], macBT[4], macBT[5]);
    Serial.printf("%02X:%02X:%02X:%02X:%02X:%02X\r\n", macBT[0], macBT[1], macBT[2], macBT[3], macBT[4], macBT[5]);
    
    // 電源ON時のシリアルデータが無くなるまで待つ
    while (Serial.available()) {data = Serial.read();}
    
    // M5.Lcd.setTextFont(4);  // テキストサイズ変更


    t_old = micros();
}


// メイン -------------------------------------------------
void loop() {
    M5.update();  // 本体のボタン状態更新

    M5.IMU.getGyroData(&gyroX, &gyroY, &gyroZ);
    M5.IMU.getAccelData(&accX, &accY, &accZ);
    // M5.IMU.getAhrsData(&pitch, &roll, &yaw);
    
    t_new = micros();
    dt = t_new - t_old;
    t_old = t_new;
    
    // SerialBT.printf("%d\t%d\t%5.2f\t%5.2f\t%5.2f\t%6.2f\t%6.2f\t%6.2f\t%5.2f\t%5.2f\t%5.2f\n", 
    //                dt, t_delay, accX, accY, accZ, gyroX, gyroY, gyroZ, pitch, roll, yaw);

    SerialBT.printf("%d\t%d\t%5.2f\t%5.2f\t%5.2f\t%6.2f\t%6.2f\t%6.2f\n", 
                    dt, t_delay, accX, accY, accZ, gyroX, gyroY, gyroZ);

    if (dt > 1.01*dt_target){
        t_delay *= 0.99;
        }
    else if (dt < 0.99*dt_target){
        t_delay *= 1.01;
    }

    // 再起動（リスタート）処理
    if (btn_pw == 2) {          // 電源ボタン短押し（1秒以下）なら
        ESP.restart();
    }
    // 電源ボタン状態取得（1秒以下のONで「2」1秒以上で「1」すぐに「0」に戻る）
    btn_pw = M5.Axp.GetBtnPress();

    if (count==500){
        digitalWrite(10, LOW); //LED ON
        count = 0;
    }
    else {
        digitalWrite(10, HIGH); //LED OFF
    }
    count += 1;


    delayMicroseconds(t_delay);  // 遅延時間 us
}