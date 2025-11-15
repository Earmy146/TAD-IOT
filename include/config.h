#ifndef CONFIG_H
#define CONFIG_H

// ==================== WIFI ====================
#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASSWORD ""

// ==================== THINGSPEAK ====================
#define THINGSPEAK_SERVER "http://api.thingspeak.com/update"
#define THINGSPEAK_API_KEY "MO6KC0KZO13I8YQS" // <<<< THAY ĐỔI

// ==================== TELEGRAM ====================
#define BOT_TOKEN "8538704959:AAGVo4C2m9czrnvQQy0GGn9130q7lHOIqWM" // <<<< THAY ĐỔI

// Mật khẩu để đăng ký chat mới (thay đổi để bảo mật)
#define REGISTER_PASSWORD "irrigation2024"

// Bot sẽ tự động lưu các nhóm đã đăng ký

// ==================== HARDWARE PINS ====================
#define DHT_PIN 15    // DHT22 sensor
#define SOIL_PIN 34   // Soil moisture (analog)
#define RELAY_PIN 4   // Water pump relay
#define LED_WARNING 2 // Red LED (dry soil)
#define LED_OK 5      // Green LED (wet soil)
#define BUZZER_PIN 18 // Buzzer alarm

// LCD I2C
#define LCD_ADDR 0x27 // Địa chỉ I2C LCD (thử 0x3F nếu không hoạt động)
#define LCD_COLS 16   // Số cột
#define LCD_ROWS 2    // Số hàng

// ==================== THRESHOLDS ====================
#define SOIL_DRY_THRESHOLD 40     // If soil < 40% → water ON
#define SOIL_WET_THRESHOLD 70     // If soil > 70% → water OFF
#define UPDATE_INTERVAL 20000     // Send to ThingSpeak every 20s
#define SENSOR_READ_INTERVAL 2000 // Read sensors every 2s

// ==================== ALARM ====================
#define BUZZER_ENABLED true      // Enable/disable buzzer
#define BUZZER_BEEP_DURATION 200 // Beep duration (ms)

#endif