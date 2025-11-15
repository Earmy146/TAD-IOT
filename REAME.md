# 🌱 Smart Irrigation System

Hệ thống tưới cây tự động thông minh sử dụng ESP32, cảm biến độ ẩm đất, DHT22, kết nối ThingSpeak và Telegram Bot.

## 📋 Tính năng

- ✅ Đo độ ẩm đất real-time
- ✅ Đo nhiệt độ & độ ẩm không khí (DHT22)
- ✅ Tự động bật/tắt bơm nước dựa trên ngưỡng
- ✅ Gửi cảnh báo qua Telegram
- ✅ Lưu dữ liệu lên ThingSpeak Cloud
- ✅ Web Dashboard hiển thị real-time
- ✅ Biểu đồ theo dõi lịch sử
- ✅ Mô phỏng hoàn chỉnh trên Wokwi

## 🛠️ Cấu trúc dự án

```
smart-irrigation-system/
├── src/main.cpp              # Code ESP32
├── include/config.h          # File cấu hình
├── platformio.ini            # Cấu hình PlatformIO
├── wokwi.toml               # Cấu hình Wokwi
├── diagram.json             # Sơ đồ mạch Wokwi
└── web-dashboard/           # Web Dashboard
    ├── server.js            # Node.js backend
    ├── package.json
    ├── .env                 # Biến môi trường
    └── public/
        └── index.html       # Frontend
```

## 🚀 Hướng dẫn cài đặt

### Bước 1: Cài đặt môi trường

#### VS Code + PlatformIO
```bash
# Cài extension trong VS Code:
1. PlatformIO IDE
2. Wokwi Simulator (optional)
```

#### Node.js
```bash
# Tải và cài đặt Node.js từ: https://nodejs.org
node --version  # Kiểm tra cài đặt
```

### Bước 2: Thiết lập ThingSpeak

1. Truy cập: https://thingspeak.com
2. Tạo tài khoản miễn phí
3. Tạo Channel mới:
   - **Field 1**: Soil Moisture (%)
   - **Field 2**: Temperature (°C)
   - **Field 3**: Humidity (%)
   - **Field 4**: Pump Status (0/1)
4. Lưu lại:
   - **Channel ID**
   - **Write API Key**
   - **Read API Key**

### Bước 3: Thiết lập Telegram Bot

1. Mở Telegram, tìm `@BotFather`
2. Gửi lệnh: `/newbot`
3. Đặt tên và username cho bot
4. Lưu **Bot Token**
5. Tạo nhóm, thêm bot vào
6. Lấy **Chat ID** từ: 
   ```
   https://api.telegram.org/bot<BOT_TOKEN>/getUpdates
   ```

### Bước 4: Cấu hình dự án

#### ESP32 - File `include/config.h`
```cpp
#define THINGSPEAK_API_KEY "YOUR_WRITE_API_KEY"
#define BOT_TOKEN "YOUR_BOT_TOKEN"
#define CHAT_ID "YOUR_CHAT_ID"
```

#### Web Dashboard - File `web-dashboard/.env`
```bash
THINGSPEAK_CHANNEL_ID=YOUR_CHANNEL_ID
THINGSPEAK_READ_API_KEY=YOUR_READ_API_KEY
```

### Bước 5: Cài đặt dependencies

#### PlatformIO (tự động)
```bash
# PlatformIO sẽ tự động tải các thư viện khi build
pio run
```

#### Web Dashboard
```bash
cd web-dashboard
npm install
```

## 🎮 Chạy dự án

### Mô phỏng Wokwi (trong VS Code)

1. Mở dự án trong VS Code
2. Nhấn `Ctrl+Shift+P` (hoặc `Cmd+Shift+P` trên Mac)
3. Gõ: `Wokwi: Start Simulator`
4. Hoặc click vào nút Wokwi ở thanh trạng thái

### Upload lên ESP32 thật

```bash
# Kết nối ESP32 qua USB
pio run --target upload

# Xem Serial Monitor
pio device monitor
```

### Chạy Web Dashboard

```bash
cd web-dashboard
npm start

# Hoặc dùng nodemon (auto-reload)
npm run dev
```

Truy cập: **http://localhost:3000**

## 📊 API Endpoints

| Endpoint | Method | Mô tả |
|----------|--------|-------|
| `/api/health` | GET | Health check |
| `/api/latest` | GET | Dữ liệu mới nhất |
| `/api/history?results=100` | GET | Lịch sử dữ liệu |
| `/api/stats` | GET | Thống kê tổng quan |
| `/api/range?start=...&end=...` | GET | Dữ liệu theo khoảng thời gian |

## ⚙️ Ngưỡng điều khiển

```cpp
#define SOIL_DRY_THRESHOLD 40   // Độ ẩm < 40% → BẬT BƠM
#define SOIL_WET_THRESHOLD 70   // Độ ẩm > 70% → TẮT BƠM
```

## 🔧 Hardware

### Linh kiện cần thiết

- ESP32 DevKit V1
- Cảm biến DHT22 (hoặc DHT11)
- Cảm biến độ ẩm đất (capacitive)
- Relay module 5V
- Bơm nước DC 5V
- LED (đỏ, xanh)
- Buzzer
- Breadboard + dây nối

### Sơ đồ kết nối

| Component | ESP32 Pin |
|-----------|-----------|
| DHT22 Data | GPIO 15 |
| Soil Sensor | GPIO 34 (ADC) |
| Relay | GPIO 4 |
| LED Warning | GPIO 2 |
| LED OK | GPIO 5 |
| Buzzer | GPIO 18 |

## 📱 Telegram Commands

### Lệnh cơ bản
- `/start` - Menu lệnh
- `/status` - Xem trạng thái hiện tại
- `/history` - Xem lịch sử 24h
- `/help` - Trợ giúp chi tiết

### Điều khiển bơm
- `/manual` - Chuyển sang chế độ thủ công
- `/pump on` - Bật bơm (chế độ thủ công)
- `/pump off` - Tắt bơm (chế độ thủ công)
- `/auto` - Chuyển về chế độ tự động

### Cài đặt
- `/settings` - Xem cài đặt hiện tại
- `/set dry <value>` - Đặt ngưỡng khô (10-60%)
- `/set wet <value>` - Đặt ngưỡng ẩm (50-90%)
- `/reset` - Khởi động lại ESP32

**📖 Xem hướng dẫn chi tiết:** [TELEGRAM_COMMANDS.md](./TELEGRAM_COMMANDS.md)

## 🐛 Troubleshooting

### ESP32 không kết nối WiFi
- Kiểm tra SSID và password trong `config.h`
- Đảm bảo WiFi là 2.4GHz (ESP32 không hỗ trợ 5GHz)

### ThingSpeak không nhận dữ liệu
- Kiểm tra Write API Key
- Đảm bảo update interval >= 15 giây (Free tier)

### Telegram Bot không gửi tin nhắn
- Kiểm tra Bot Token và Chat ID
- Đảm bảo bot đã được thêm vào nhóm
- Bot phải có quyền gửi tin nhắn

### Web Dashboard không hiển thị
- Kiểm tra file `.env` có đúng thông tin
- Đảm bảo server đang chạy (`npm start`)
- Kiểm tra console log trong browser (F12)

## 📈 Nâng cấp tương lai

- [ ] Thêm cảm biến mực nước
- [ ] Tích hợp RTC cho lịch tưới
- [ ] ML/AI dự đoán lịch tưới
- [ ] Mobile App (React Native)
- [ ] Nhiều khu vực tưới
- [ ] Báo cáo tuần/tháng qua email

## 📝 License

MIT License

## 👨‍💻 Author

Your Name - [GitHub](https://github.com/yourusername)

## 🙏 Credits

- ThingSpeak - IoT Platform
- Telegram Bot API
- PlatformIO
- Wokwi Simulator
- Chart.js

---

**Made with ❤️ for Smart Agriculture**