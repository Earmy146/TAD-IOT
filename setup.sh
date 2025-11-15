#!/bin/bash

echo "🚀 Đang thiết lập Smart Irrigation System..."

# Kiểm tra thư mục hiện tại
if [ ! -d "src" ]; then
    echo "❌ Không tìm thấy thư mục src. Bạn có đang ở đúng thư mục không?"
    exit 1
fi

# Tạo các thư mục cần thiết
echo "📁 Tạo cấu trúc thư mục..."
mkdir -p include
mkdir -p lib
mkdir -p test
mkdir -p .vscode
mkdir -p web-dashboard/public

# Tạo platformio.ini
echo "📝 Tạo platformio.ini..."
cat > platformio.ini << 'EOF'
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino

; Cấu hình Serial Monitor
monitor_speed = 115200
monitor_filters = esp32_exception_decoder

; Thư viện cần thiết
lib_deps = 
    adafruit/DHT sensor library@^1.4.4
    adafruit/Adafruit Unified Sensor@^1.1.9
    bblanchon/ArduinoJson@^6.21.3
    witnessmenow/UniversalTelegramBot@^1.3.0

; Cấu hình upload (dành cho hardware thật)
upload_speed = 921600

; Cấu hình build
build_flags = 
    -D CORE_DEBUG_LEVEL=3
    -D CONFIG_ARDUHAL_LOG_COLORS

; Wokwi Simulator
[env:wokwi]
platform = espressif32
board = esp32dev
framework = arduino
lib_deps = 
    adafruit/DHT sensor library@^1.4.4
    adafruit/Adafruit Unified Sensor@^1.1.9
    bblanchon/ArduinoJson@^6.21.3
    witnessmenow/UniversalTelegramBot@^1.3.0
EOF

# Tạo .vscode/settings.json
echo "⚙️  Tạo VS Code settings..."
cat > .vscode/settings.json << 'EOF'
{
  "files.associations": {
    "*.ino": "cpp",
    "*.h": "cpp"
  },
  "editor.formatOnSave": true,
  "editor.tabSize": 2,
  "C_Cpp.errorSquiggles": "enabled",
  "platformio-ide.useBuiltinPIOCore": true,
  "platformio-ide.autoRebuildAutocompleteIndex": true,
  "files.exclude": {
    "**/.git": true,
    "**/.pio": true,
    "**/node_modules": true
  },
  "files.autoSave": "afterDelay",
  "files.autoSaveDelay": 1000
}
EOF

# Tạo .gitignore
echo "🔒 Tạo .gitignore..."
cat > .gitignore << 'EOF'
.pio
.vscode/.browse.c_cpp.db*
.vscode/c_cpp_properties.json
.vscode/launch.json
.vscode/ipch
node_modules/
.env
*.log
.DS_Store
EOF

# Kiểm tra file config.h
if [ ! -f "include/config.h" ]; then
    echo "⚠️  Chưa có file config.h, tạo template..."
    cat > include/config.h << 'EOF'
#ifndef CONFIG_H
#define CONFIG_H

// ==================== WIFI ====================
#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASSWORD ""

// ==================== THINGSPEAK ====================
#define THINGSPEAK_SERVER "http://api.thingspeak.com/update"
#define THINGSPEAK_API_KEY "YOUR_WRITE_API_KEY"  // <<<< THAY ĐỔI

// ==================== TELEGRAM ====================
#define BOT_TOKEN "YOUR_BOT_TOKEN"  // <<<< THAY ĐỔI
#define CHAT_ID "YOUR_CHAT_ID"      // <<<< THAY ĐỔI

// ==================== HARDWARE PINS ====================
#define DHT_PIN 15
#define SOIL_PIN 34
#define RELAY_PIN 4
#define LED_WARNING 2
#define LED_OK 5
#define BUZZER_PIN 18

// ==================== THRESHOLDS ====================
#define SOIL_DRY_THRESHOLD 40
#define SOIL_WET_THRESHOLD 70
#define UPDATE_INTERVAL 20000
#define SENSOR_READ_INTERVAL 2000

// ==================== ALARM ====================
#define BUZZER_ENABLED true
#define BUZZER_BEEP_DURATION 200

#endif
EOF
fi

# Cài đặt web-dashboard dependencies
if [ -d "web-dashboard" ] && [ -f "web-dashboard/package.json" ]; then
    echo "📦 Cài đặt Node.js dependencies..."
    cd web-dashboard
    npm install
    cd ..
fi

echo ""
echo "✅ Hoàn thành! Cấu trúc dự án:"
tree -L 2 -I 'node_modules|.pio'

echo ""
echo "🎯 Các bước tiếp theo:"
echo "1. Sửa file include/config.h (thêm API keys)"
echo "2. Chạy: pio run"
echo "3. Mô phỏng Wokwi trong VS Code"
echo "4. Chạy web dashboard: cd web-dashboard && npm start"
echo ""
echo "🚀 Chúc may mắn!"
