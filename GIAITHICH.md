# 🌱 Hệ thống Tưới Cây Thông Minh - ESP32

## 📋 Tổng quan dự án

Đây là hệ thống tưới cây tự động sử dụng ESP32, có khả năng:

- Đo độ ẩm đất, nhiệt độ và độ ẩm không khí
- Tự động bật/tắt bơm tưới dựa trên ngưỡng cài đặt
- Điều khiển từ xa qua Telegram Bot
- Gửi dữ liệu lên ThingSpeak để theo dõi
- Hiển thị thông tin trên LCD 16x2
- Lưu cài đặt vào bộ nhớ flash (không mất khi tắt nguồn)

---

## 🔧 Phần cứng sử dụng

### Linh kiện chính:

1. **ESP32 DevKit V1** - Vi điều khiển chính
2. **DHT22** - Cảm biến nhiệt độ và độ ẩm không khí
3. **Soil Moisture Sensor** (mô phỏng bằng potentiometer) - Đo độ ẩm đất
4. **Relay Module** - Điều khiển bơm nước
5. **LCD 16x2 I2C** - Hiển thị thông tin
6. **2 LED** (Đỏ/Xanh) - Báo trạng thái
7. **Buzzer** - Cảnh báo âm thanh
8. **2 điện trở 220Ω** - Hạn dòng cho LED

### Sơ đồ kết nối:

| Linh kiện        | Chân ESP32    | Ghi chú                 |
| ---------------- | ------------- | ----------------------- |
| DHT22 Data       | GPIO 15       | Cảm biến nhiệt độ/độ ẩm |
| Soil Sensor      | GPIO 34 (ADC) | Đọc analog 0-4095       |
| Relay IN         | GPIO 4        | Điều khiển bơm          |
| LED Đỏ (Warning) | GPIO 2        | Báo đất khô             |
| LED Xanh (OK)    | GPIO 5        | Báo đất ẩm              |
| Buzzer           | GPIO 18       | Cảnh báo âm thanh       |
| LCD SDA          | GPIO 21       | I2C Data                |
| LCD SCL          | GPIO 22       | I2C Clock               |

---

## 📦 Thư viện cần cài đặt

```cpp
#include <Arduino.h>
#include <WiFi.h>              // Kết nối WiFi
#include <HTTPClient.h>         // Gửi HTTP request
#include <DHT.h>                // Đọc DHT22
#include <WiFiClientSecure.h>   // HTTPS cho Telegram
#include <UniversalTelegramBot.h> // Telegram Bot API
#include <Preferences.h>        // Lưu dữ liệu vào Flash
#include <LiquidCrystal_I2C.h>  // Điều khiển LCD I2C
```

---

## ⚙️ Cấu hình (config.h)

### WiFi

```cpp
#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASSWORD ""
```

### ThingSpeak (IoT Platform)

```cpp
#define THINGSPEAK_SERVER "http://api.thingspeak.com/update"
#define THINGSPEAK_API_KEY "MO6KC0KZO13I8YQS"
```

ThingSpeak lưu 4 trường dữ liệu:

- Field 1: Độ ẩm đất (%)
- Field 2: Nhiệt độ (°C)
- Field 3: Độ ẩm không khí (%)
- Field 4: Trạng thái bơm (0/1)

### Telegram Bot

```cpp
#define BOT_TOKEN "8538704959:AAGVo4C2m9czrnvQQy0GGn9130q7lHOIqWM"
#define REGISTER_PASSWORD "irrigation2024"
```

Người dùng phải đăng ký chat ID bằng mật khẩu để sử dụng bot.

### Ngưỡng tưới

```cpp
#define SOIL_DRY_THRESHOLD 40  // Bật bơm khi < 40%
#define SOIL_WET_THRESHOLD 70  // Tắt bơm khi > 70%
```

---

## 🧩 Cấu trúc Code

### 1. Khởi tạo đối tượng

```cpp
DHT dht(DHT_PIN, DHT22);
UniversalTelegramBot bot(BOT_TOKEN, secureClient);
Preferences preferences;  // Lưu trữ Flash
LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);
```

### 2. Biến toàn cục

```cpp
unsigned long lastUpdate = 0;        // Lần gửi ThingSpeak cuối
unsigned long lastSensorRead = 0;    // Lần đọc cảm biến cuối
unsigned long lastBotCheck = 0;      // Lần check Telegram cuối

bool pumpState = false;     // Trạng thái bơm
bool manualMode = false;    // Chế độ thủ công
bool alarmSent = false;     // Đã gửi cảnh báo chưa

String allowedChats[10];    // Danh sách Chat ID được phép
int chatCount = 0;          // Số chat đã đăng ký
```

### 3. Cấu trúc dữ liệu

```cpp
struct SensorData {
  int soilMoisture;      // Độ ẩm đất (%)
  float temperature;     // Nhiệt độ (°C)
  float humidity;        // Độ ẩm KK (%)
  unsigned long uptime;  // Thời gian hoạt động (giây)
};

struct HistoryData {
  int minSoil = 100;     // Độ ẩm thấp nhất
  int maxSoil = 0;       // Độ ẩm cao nhất
  float avgSoil = 0;     // Độ ẩm trung bình
  int pumpActivations;   // Số lần bật bơm
};
```

---

## 🔄 Các chức năng chính

### 1. Đọc cảm biến

#### `readSoilMoisture()`

```cpp
int readSoilMoisture() {
  int rawValue = analogRead(SOIL_PIN);         // Đọc ADC (0-4095)
  int moisture = map(rawValue, 0, 4095, 0, 100); // Chuyển sang %
  moisture = constrain(moisture, 0, 100);       // Giới hạn 0-100

  // Cập nhật lịch sử
  if (moisture < history.minSoil) history.minSoil = moisture;
  if (moisture > history.maxSoil) history.maxSoil = moisture;

  return moisture;
}
```

#### `readSensors()`

```cpp
void readSensors() {
  sensorData.soilMoisture = readSoilMoisture();
  sensorData.temperature = dht.readTemperature();
  sensorData.humidity = dht.readHumidity();
  sensorData.uptime = millis() / 1000;

  // Kiểm tra lỗi đọc DHT22
  if (isnan(sensorData.temperature) || isnan(sensorData.humidity)) {
    sensorData.temperature = 0;
    sensorData.humidity = 0;
  }
}
```

### 2. Điều khiển bơm

#### `setPump(bool state, bool fromTelegram)`

```cpp
void setPump(bool state, bool fromTelegram = false) {
  if (pumpState == state) return;  // Không đổi nếu trạng thái giống nhau

  pumpState = state;
  digitalWrite(RELAY_PIN, state ? HIGH : LOW);
  digitalWrite(LED_WARNING, state ? HIGH : LOW);
  digitalWrite(LED_OK, state ? LOW : HIGH);

  if (state) {
    history.pumpActivations++;
    playBuzzer(500);  // Bíp dài khi bật
  } else {
    playBuzzer(200);  // Bíp 2 lần ngắn khi tắt
    delay(100);
    playBuzzer(200);
  }

  // Gửi thông báo Telegram
  broadcastTelegram(message);
}
```

#### `controlPump()` - Logic tự động

```cpp
void controlPump() {
  if (manualMode) return;  // Bỏ qua nếu đang ở chế độ thủ công

  // Bật bơm nếu đất khô
  if (sensorData.soilMoisture < soilDryThreshold && !pumpState) {
    setPump(true);
  }
  // Tắt bơm nếu đất đủ ẩm
  else if (sensorData.soilMoisture > soilWetThreshold && pumpState) {
    setPump(false);
  }
}
```

### 3. Gửi dữ liệu ThingSpeak

```cpp
void sendToThingSpeak(int soil, float temp, float hum, int pump) {
  if (WiFi.status() != WL_CONNECTED) return;

  String url = String(THINGSPEAK_SERVER) + "?api_key=" + THINGSPEAK_API_KEY;
  url += "&field1=" + String(soil);
  url += "&field2=" + String(temp, 1);
  url += "&field3=" + String(hum, 1);
  url += "&field4=" + String(pump);

  HTTPClient http;
  http.begin(url);
  int httpCode = http.GET();
  http.end();
}
```

### 4. Quản lý Chat Telegram

#### `isChatAllowed(chat_id)`

Kiểm tra xem Chat ID có trong danh sách cho phép không.

#### `addChat(chat_id)`

Thêm Chat ID mới (tối đa 10 chat).

#### `removeChat(chat_id)`

Xóa Chat ID khỏi danh sách.

#### `broadcastTelegram(message)`

Gửi tin nhắn tới tất cả chat đã đăng ký.

### 5. Lưu/Đọc cài đặt

```cpp
void saveSettings() {
  preferences.begin("irrigation", false);
  preferences.putInt("dryThreshold", soilDryThreshold);
  preferences.putInt("wetThreshold", soilWetThreshold);
  preferences.putBool("manualMode", manualMode);
  preferences.end();
}

void loadSettings() {
  preferences.begin("irrigation", false);
  soilDryThreshold = preferences.getInt("dryThreshold", SOIL_DRY_THRESHOLD);
  soilWetThreshold = preferences.getInt("wetThreshold", SOIL_WET_THRESHOLD);
  manualMode = preferences.getBool("manualMode", false);
  preferences.end();
}
```

Dữ liệu được lưu vào **Flash memory** của ESP32, không bị mất khi tắt nguồn.

---

## 🤖 Lệnh Telegram Bot

### Đăng ký sử dụng bot

```
/register irrigation2024
```

### Lệnh cơ bản

| Lệnh       | Chức năng               |
| ---------- | ----------------------- |
| `/start`   | Xem danh sách lệnh      |
| `/status`  | Xem trạng thái hiện tại |
| `/history` | Xem lịch sử 24h         |
| `/help`    | Trợ giúp chi tiết       |

### Điều khiển bơm (cần bật Manual Mode)

```
/manual         # Chuyển sang chế độ thủ công
/pump on        # Bật bơm
/pump off       # Tắt bơm
/auto           # Quay lại chế độ tự động
```

### Cài đặt ngưỡng

```
/settings              # Xem cài đặt hiện tại
/set dry 35           # Đặt ngưỡng khô = 35%
/set wet 75           # Đặt ngưỡng ẩm = 75%
```

### Quản lý

```
/chats          # Xem danh sách chat đã đăng ký
/unregister     # Hủy đăng ký chat này
/reset          # Khởi động lại ESP32
```

---

## 📊 Hiển thị LCD

```
S:45% T:28C      // Độ ẩm đất 45%, Nhiệt độ 28°C
H:65% PUMP:OFF   // Độ ẩm KK 65%, Bơm tắt
```

---

## 🔁 Vòng lặp chính (loop)

```cpp
void loop() {
  unsigned long currentMillis = millis();

  // 1. Đọc cảm biến mỗi 2 giây
  if (currentMillis - lastSensorRead >= 2000) {
    readSensors();
    controlPump();
    printStatus();
    updateLCD();
  }

  // 2. Gửi ThingSpeak mỗi 20 giây
  if (currentMillis - lastUpdate >= 20000) {
    sendToThingSpeak(...);
  }

  // 3. Kiểm tra Telegram mỗi 1 giây
  if (currentMillis - lastBotCheck >= 1000) {
    checkTelegramMessages();
  }

  // 4. Nhấp nháy LED cảnh báo khi bơm bật
  if (pumpState) {
    digitalWrite(LED_WARNING, !digitalRead(LED_WARNING));
  }
}
```

---

## 🎯 Logic hoạt động

### Chế độ tự động (Auto Mode)

1. Đọc độ ẩm đất mỗi 2 giây
2. Nếu độ ẩm **< 40%** → Bật bơm
3. Nếu độ ẩm **> 70%** → Tắt bơm
4. Gửi thông báo Telegram mỗi khi bật/tắt bơm

### Chế độ thủ công (Manual Mode)

- Hệ thống không tự động bật/tắt bơm
- Người dùng điều khiển qua Telegram: `/pump on` hoặc `/pump off`

### Cảnh báo

- **Buzzer bíp** khi bật/tắt bơm
- **LED đỏ nhấp nháy** khi bơm đang chạy
- **LED xanh sáng** khi đất đủ ẩm

---

## 🔒 Bảo mật

1. **Xác thực Chat ID**: Chỉ chat đã đăng ký mới điều khiển được
2. **Mật khẩu đăng ký**: Cần mật khẩu để đăng ký chat mới
3. **Giới hạn 10 chat**: Tránh lạm dụng hệ thống
4. **Lưu danh sách vào Flash**: Không mất khi mất điện

---

## 📈 Tính năng nâng cao

### Lưu trữ lâu dài với Preferences

ESP32 lưu các thông tin sau vào Flash:

- Danh sách Chat ID đã đăng ký
- Ngưỡng tưới tùy chỉnh
- Chế độ hoạt động (Auto/Manual)

### Lịch sử thống kê

Hệ thống theo dõi:

- Độ ẩm đất: Min/Max/Average
- Số lần bật bơm trong ngày
- Thời gian hoạt động (Uptime)

---

## 🐛 Xử lý lỗi

### Lỗi WiFi

```cpp
if (WiFi.status() != WL_CONNECTED) {
  Serial.println("❌ WiFi không kết nối!");
  // Không gửi ThingSpeak và Telegram
}
```

### Lỗi DHT22

```cpp
if (isnan(temperature) || isnan(humidity)) {
  Serial.println("⚠️ Lỗi đọc DHT22");
  temperature = 0;
  humidity = 0;
}
```

### Chat chưa đăng ký

```cpp
if (!isChatAllowed(chat_id)) {
  bot.sendMessage(chat_id, "❌ Chat chưa được đăng ký!");
  return;
}
```

---

## 📝 Lưu ý quan trọng

1. **Địa chỉ LCD I2C**: Thử `0x27` hoặc `0x3F` nếu không hiển thị
2. **Ngưỡng ẩm > Ngưỡng khô**: Tránh bơm bật tắt liên tục
3. **Khoảng cách gửi ThingSpeak**: Ít nhất 15-20 giây/lần
4. **Bảo mật Bot Token**: Không chia sẻ công khai
5. **Test trên Wokwi**: Dùng potentiometer mô phỏng cảm biến đất

---

## 🚀 Hướng dẫn sử dụng

### Bước 1: Cài đặt phần cứng

Kết nối các linh kiện theo sơ đồ trên.

### Bước 2: Cấu hình

Sửa file `config.h`:

- WiFi SSID/Password
- ThingSpeak API Key
- Telegram Bot Token

### Bước 3: Upload code

Nạp code lên ESP32 qua Arduino IDE hoặc PlatformIO.

### Bước 4: Đăng ký Telegram

1. Tìm bot trên Telegram
2. Gửi: `/register irrigation2024`
3. Gửi: `/start` để xem lệnh

### Bước 5: Theo dõi

- Xem dữ liệu trên ThingSpeak
- Nhận thông báo qua Telegram
- Điều khiển từ xa qua bot

---

## 🎓 Kiến thức học được

1. **ESP32**: GPIO, ADC, I2C, WiFi
2. **Sensors**: DHT22, Soil Moisture
3. **IoT Platforms**: ThingSpeak, Telegram Bot
4. **Data Persistence**: ESP32 Preferences (Flash)
5. **Real-time Control**: Remote automation
6. **Error Handling**: Robust system design

---

## 🔧 Mở rộng trong tương lai

- [ ] Thêm cảm biến ánh sáng
- [ ] Tưới theo lịch trình
- [ ] Web Dashboard riêng
- [ ] Nhiều vùng tưới độc lập
- [ ] Dự báo thời tiết tích hợp
- [ ] Pin sạc dự phòng
- [ ] Cảnh báo qua Email/SMS

---

## 📧 Hỗ trợ

Nếu có lỗi hoặc cần hỗ trợ:

1. Kiểm tra Serial Monitor để debug
2. Kiểm tra kết nối WiFi
3. Verify Bot Token và API Key
4. Test từng module riêng lẻ

---

**🌱 Chúc bạn thành công với dự án tưới cây thông minh!**
