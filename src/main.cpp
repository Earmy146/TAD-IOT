#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <Preferences.h>
#include <LiquidCrystal_I2C.h>
#include "config.h"

// ==================== KHỞI TẠO ====================
DHT dht(DHT_PIN, DHT22);
WiFiClientSecure secureClient;
UniversalTelegramBot bot(BOT_TOKEN, secureClient);
Preferences preferences;
LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);

// ==================== BIẾN TOÀN CỤC ====================
unsigned long lastUpdate = 0;
unsigned long lastSensorRead = 0;
unsigned long lastBotCheck = 0;
bool pumpState = false;
bool manualMode = false;
bool alarmSent = false;

// Danh sách Chat ID được phép (tối đa 10 chat)
String allowedChats[10];
int chatCount = 0;

// Ngưỡng động
int soilDryThreshold = SOIL_DRY_THRESHOLD;
int soilWetThreshold = SOIL_WET_THRESHOLD;

struct SensorData
{
  int soilMoisture;
  float temperature;
  float humidity;
  unsigned long uptime;
} sensorData = {0, 0.0, 0.0, 0};

struct HistoryData
{
  int minSoil = 100;
  int maxSoil = 0;
  float avgSoil = 0;
  int pumpActivations = 0;
} history;

// ==================== FORWARD DECLARATIONS ====================
void saveChatList();
void loadChatList();
void saveSettings();
void loadSettings();
void broadcastTelegram(const String &message);
void sendTelegram(const String &chat_id, const String &message);
void updateLCD();

// ==================== HÀM TIỆN ÍCH ====================

void playBuzzer(int duration = BUZZER_BEEP_DURATION)
{
  if (BUZZER_ENABLED)
  {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(duration);
    digitalWrite(BUZZER_PIN, LOW);
  }
}

void sendToThingSpeak(int soil, float temp, float hum, int pump)
{
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("❌ WiFi không kết nối!");
    return;
  }

  HTTPClient http;
  String url = String(THINGSPEAK_SERVER) + "?api_key=" + String(THINGSPEAK_API_KEY);
  url += "&field1=" + String(soil);
  url += "&field2=" + String(temp, 1);
  url += "&field3=" + String(hum, 1);
  url += "&field4=" + String(pump);

  http.begin(url);
  int httpCode = http.GET();

  if (httpCode > 0)
  {
    Serial.println("✅ ThingSpeak: Gửi thành công");
  }
  else
  {
    Serial.println("❌ ThingSpeak: Lỗi gửi");
  }

  http.end();
}

// ==================== QUẢN LÝ CHAT ====================

bool isChatAllowed(const String &chat_id)
{
  for (int i = 0; i < chatCount; i++)
  {
    if (allowedChats[i] == chat_id)
      return true;
  }
  return false;
}

bool addChat(const String &chat_id)
{
  if (isChatAllowed(chat_id))
    return false;
  if (chatCount >= 10)
    return false;

  allowedChats[chatCount] = chat_id;
  chatCount++;
  saveChatList();
  return true;
}

bool removeChat(const String &chat_id)
{
  for (int i = 0; i < chatCount; i++)
  {
    if (allowedChats[i] == chat_id)
    {
      for (int j = i; j < chatCount - 1; j++)
      {
        allowedChats[j] = allowedChats[j + 1];
      }
      chatCount--;
      saveChatList();
      return true;
    }
  }
  return false;
}

void broadcastTelegram(const String &message)
{
  for (int i = 0; i < chatCount; i++)
  {
    bot.sendMessage(allowedChats[i], message, "");
    delay(100);
  }
  Serial.println("📱 Broadcast đến " + String(chatCount) + " chat");
}

void sendTelegram(const String &chat_id, const String &message)
{
  bot.sendMessage(chat_id, message, "");
}

// ==================== LƯU/ĐỌC CÀI ĐẶT ====================

void saveChatList()
{
  preferences.begin("irrigation", false);
  preferences.putInt("chatCount", chatCount);
  for (int i = 0; i < chatCount; i++)
  {
    String key = "chat" + String(i);
    preferences.putString(key.c_str(), allowedChats[i]);
  }
  preferences.end();
  Serial.println("💾 Đã lưu danh sách chat");
}

void loadChatList()
{
  preferences.begin("irrigation", false);
  chatCount = preferences.getInt("chatCount", 0);
  for (int i = 0; i < chatCount; i++)
  {
    String key = "chat" + String(i);
    allowedChats[i] = preferences.getString(key.c_str(), "");
  }
  preferences.end();
  Serial.println("📂 Đã tải " + String(chatCount) + " chat");
}

void saveSettings()
{
  preferences.begin("irrigation", false);
  preferences.putInt("dryThreshold", soilDryThreshold);
  preferences.putInt("wetThreshold", soilWetThreshold);
  preferences.putBool("manualMode", manualMode);
  preferences.end();
  Serial.println("💾 Đã lưu cài đặt");
}

void loadSettings()
{
  preferences.begin("irrigation", false);
  soilDryThreshold = preferences.getInt("dryThreshold", SOIL_DRY_THRESHOLD);
  soilWetThreshold = preferences.getInt("wetThreshold", SOIL_WET_THRESHOLD);
  manualMode = preferences.getBool("manualMode", false);
  preferences.end();
  Serial.println("📂 Đã tải cài đặt");

  loadChatList();
}

// ==================== ĐỌC CẢM BIẾN ====================

int readSoilMoisture()
{
  int rawValue = analogRead(SOIL_PIN);
  int moisture = map(rawValue, 0, 4095, 0, 100);
  moisture = constrain(moisture, 0, 100);

  if (moisture < history.minSoil)
    history.minSoil = moisture;
  if (moisture > history.maxSoil)
    history.maxSoil = moisture;

  return moisture;
}

void readSensors()
{
  sensorData.soilMoisture = readSoilMoisture();
  sensorData.temperature = dht.readTemperature();
  sensorData.humidity = dht.readHumidity();
  sensorData.uptime = millis() / 1000;

  if (isnan(sensorData.temperature) || isnan(sensorData.humidity))
  {
    Serial.println("⚠️  Cảnh báo: Lỗi đọc DHT22");
    sensorData.temperature = 0;
    sensorData.humidity = 0;
  }
}

// ==================== ĐIỀU KHIỂN BƠM ====================

void setPump(bool state, bool fromTelegram = false)
{
  if (pumpState == state)
    return;

  pumpState = state;
  digitalWrite(RELAY_PIN, state ? HIGH : LOW);
  digitalWrite(LED_WARNING, state ? HIGH : LOW);
  digitalWrite(LED_OK, state ? LOW : HIGH);

  if (state)
  {
    history.pumpActivations++;
    playBuzzer(500);
  }
  else
  {
    playBuzzer(200);
    delay(100);
    playBuzzer(200);
  }

  String msg = state ? "🚨 BẬT BƠM TƯỚI" : "✅ TẮT BƠM";
  msg += "\n━━━━━━━━━━━━━━━━\n";
  msg += "💧 Độ ẩm đất: " + String(sensorData.soilMoisture) + "%\n";
  msg += "🌡️  Nhiệt độ: " + String(sensorData.temperature, 1) + "°C\n";
  msg += "💦 Độ ẩm KK: " + String(sensorData.humidity, 1) + "%\n";

  if (fromTelegram)
  {
    msg += "⚙️  Chế độ: THỦ CÔNG";
  }
  else
  {
    msg += "⚙️  Chế độ: TỰ ĐỘNG";
  }

  broadcastTelegram(msg);

  Serial.println(state ? "\n🔴 BẬT BƠM\n" : "\n🟢 TẮT BƠM\n");
}

void controlPump()
{
  if (manualMode)
    return;

  if (sensorData.soilMoisture < soilDryThreshold && !pumpState)
  {
    setPump(true);
  }
  else if (sensorData.soilMoisture > soilWetThreshold && pumpState)
  {
    setPump(false);
  }
}

// ==================== TELEGRAM COMMANDS ====================

void handleTelegramCommand(String chat_id, String text, String username)
{
  text.toLowerCase();
  text.trim();

  Serial.println("📱 Nhận lệnh từ @" + username + " (" + chat_id + "): " + text);

  if (text == "/start")
  {
    String welcome = "🌱 *HỆ THỐNG TƯỚI CÂY THÔNG MINH*\n\n";

    if (!isChatAllowed(chat_id))
    {
      welcome += "⚠️  *Chat chưa được đăng ký!*\n\n";
      welcome += "Để sử dụng bot, gửi lệnh:\n";
      welcome += "`/register " + String(REGISTER_PASSWORD) + "`\n\n";
      welcome += "Liên hệ admin để lấy mật khẩu.";
      bot.sendMessage(chat_id, welcome, "Markdown");
      return;
    }

    welcome += "📋 *Danh sách lệnh:*\n";
    welcome += "/status - Trạng thái hiện tại\n";
    welcome += "/history - Lịch sử 24h\n";
    welcome += "/pump on - Bật bơm\n";
    welcome += "/pump off - Tắt bơm\n";
    welcome += "/auto - Chế độ tự động\n";
    welcome += "/manual - Chế độ thủ công\n";
    welcome += "/settings - Xem cài đặt\n";
    welcome += "/set dry <value> - Đặt ngưỡng khô\n";
    welcome += "/set wet <value> - Đặt ngưỡng ẩm\n";
    welcome += "/chats - Xem danh sách chat\n";
    welcome += "/unregister - Hủy đăng ký chat này\n";
    welcome += "/reset - Khởi động lại ESP32\n";
    welcome += "/help - Trợ giúp\n\n";
    welcome += "💡 Ví dụ: /set dry 35";
    bot.sendMessage(chat_id, welcome, "Markdown");
  }

  else if (text.startsWith("/register "))
  {
    String password = text.substring(10);
    password.trim();

    if (password == String(REGISTER_PASSWORD))
    {
      if (addChat(chat_id))
      {
        bot.sendMessage(chat_id, "✅ Đăng ký thành công!\nChat này đã được thêm vào danh sách.\nGửi /start để xem lệnh.");
        Serial.println("✅ Chat mới đăng ký: " + chat_id + " (@" + username + ")");
      }
      else
      {
        if (isChatAllowed(chat_id))
        {
          bot.sendMessage(chat_id, "⚠️  Chat này đã được đăng ký trước đó.");
        }
        else
        {
          bot.sendMessage(chat_id, "❌ Danh sách chat đã đầy (tối đa 10 chat).");
        }
      }
    }
    else
    {
      bot.sendMessage(chat_id, "❌ Mật khẩu không đúng!\nLiên hệ admin để lấy mật khẩu.");
      Serial.println("⚠️  Đăng ký thất bại từ: " + chat_id + " (@" + username + ")");
    }
    return;
  }

  else if (text == "/unregister")
  {
    if (removeChat(chat_id))
    {
      bot.sendMessage(chat_id, "✅ Đã hủy đăng ký chat này.\nBot sẽ không gửi thông báo nữa.");
      Serial.println("🗑️  Chat hủy đăng ký: " + chat_id);
    }
    else
    {
      bot.sendMessage(chat_id, "❌ Chat này chưa được đăng ký.");
    }
    return;
  }

  else if (text == "/chats")
  {
    if (!isChatAllowed(chat_id))
    {
      bot.sendMessage(chat_id, "❌ Bạn chưa đăng ký!");
      return;
    }

    String list = "📋 *DANH SÁCH CHAT ĐĂNG KÝ*\n";
    list += "━━━━━━━━━━━━━━━━\n";
    list += "Tổng: *" + String(chatCount) + "/10*\n\n";

    for (int i = 0; i < chatCount; i++)
    {
      list += String(i + 1) + ". `" + allowedChats[i] + "`\n";
    }

    bot.sendMessage(chat_id, list, "Markdown");
  }

  else if (!isChatAllowed(chat_id))
  {
    bot.sendMessage(chat_id, "❌ Chat chưa được đăng ký!\nGửi /start để biết cách đăng ký.");
    return;
  }

  else if (text == "/status")
  {
    String status = "📊 *TRẠNG THÁI HỆ THỐNG*\n";
    status += "━━━━━━━━━━━━━━━━\n";
    status += "💧 Độ ẩm đất: *" + String(sensorData.soilMoisture) + "%*\n";
    status += "🌡️  Nhiệt độ: *" + String(sensorData.temperature, 1) + "°C*\n";
    status += "💦 Độ ẩm KK: *" + String(sensorData.humidity, 1) + "%*\n";
    status += "⚙️  Bơm: *" + String(pumpState ? "🟢 BẬT" : "🔴 TẮT") + "*\n";
    status += "🎮 Chế độ: *" + String(manualMode ? "THỦ CÔNG" : "TỰ ĐỘNG") + "*\n";
    status += "📡 WiFi: *" + String(WiFi.status() == WL_CONNECTED ? "✅" : "❌") + "*\n";
    status += "⏱️  Uptime: *" + String(sensorData.uptime / 3600) + "h " + String((sensorData.uptime % 3600) / 60) + "m*\n";
    status += "🔋 IP: `" + WiFi.localIP().toString() + "`";
    bot.sendMessage(chat_id, status, "Markdown");
  }

  else if (text == "/history")
  {
    String hist = "📈 *LỊCH SỬ 24H*\n";
    hist += "━━━━━━━━━━━━━━━━\n";
    hist += "💧 Độ ẩm đất:\n";
    hist += "   • Trung bình: *" + String(history.avgSoil, 1) + "%*\n";
    hist += "   • Tối thiểu: *" + String(history.minSoil) + "%*\n";
    hist += "   • Tối đa: *" + String(history.maxSoil) + "%*\n";
    hist += "⚙️  Số lần tưới: *" + String(history.pumpActivations) + "*\n";
    hist += "⏱️  Uptime: *" + String(sensorData.uptime / 3600) + "h*";
    bot.sendMessage(chat_id, hist, "Markdown");
  }

  else if (text == "/pump on" || text == "/pump_on")
  {
    if (!manualMode)
    {
      bot.sendMessage(chat_id, "⚠️  Vui lòng bật chế độ /manual trước!");
      return;
    }
    setPump(true, true);
    bot.sendMessage(chat_id, "✅ Đã BẬT bơm (Chế độ thủ công)");
  }

  else if (text == "/pump off" || text == "/pump_off")
  {
    if (!manualMode)
    {
      bot.sendMessage(chat_id, "⚠️  Vui lòng bật chế độ /manual trước!");
      return;
    }
    setPump(false, true);
    bot.sendMessage(chat_id, "✅ Đã TẮT bơm (Chế độ thủ công)");
  }

  else if (text == "/auto")
  {
    manualMode = false;
    saveSettings();
    bot.sendMessage(chat_id, "🤖 Đã chuyển sang chế độ TỰ ĐỘNG");
    playBuzzer(100);
  }

  else if (text == "/manual")
  {
    manualMode = true;
    saveSettings();
    bot.sendMessage(chat_id, "🎮 Đã chuyển sang chế độ THỦ CÔNG\nDùng /pump on hoặc /pump off để điều khiển");
    playBuzzer(100);
    delay(50);
    playBuzzer(100);
  }

  else if (text == "/settings")
  {
    String settings = "⚙️  *CÀI ĐẶT HỆ THỐNG*\n";
    settings += "━━━━━━━━━━━━━━━━\n";
    settings += "🔴 Ngưỡng khô: *" + String(soilDryThreshold) + "%*\n";
    settings += "   _(Bật bơm khi < " + String(soilDryThreshold) + "%)_\n\n";
    settings += "🟢 Ngưỡng ẩm: *" + String(soilWetThreshold) + "%*\n";
    settings += "   _(Tắt bơm khi > " + String(soilWetThreshold) + "%)_\n\n";
    settings += "🎮 Chế độ: *" + String(manualMode ? "THỦ CÔNG" : "TỰ ĐỘNG") + "*\n";
    settings += "🔊 Buzzer: *" + String(BUZZER_ENABLED ? "BẬT" : "TẮT") + "*\n\n";
    settings += "💡 Thay đổi:\n";
    settings += "`/set dry 35` - Đặt ngưỡng khô 35%\n";
    settings += "`/set wet 75` - Đặt ngưỡng ẩm 75%";
    bot.sendMessage(chat_id, settings, "Markdown");
  }

  else if (text.startsWith("/set dry "))
  {
    int value = text.substring(9).toInt();
    if (value >= 10 && value <= 60)
    {
      soilDryThreshold = value;
      saveSettings();
      bot.sendMessage(chat_id, "✅ Đã đặt ngưỡng khô: *" + String(value) + "%*", "Markdown");
      playBuzzer(100);
    }
    else
    {
      bot.sendMessage(chat_id, "❌ Giá trị không hợp lệ! (10-60)");
    }
  }

  else if (text.startsWith("/set wet "))
  {
    int value = text.substring(9).toInt();
    if (value >= 50 && value <= 90)
    {
      soilWetThreshold = value;
      saveSettings();
      bot.sendMessage(chat_id, "✅ Đã đặt ngưỡng ẩm: *" + String(value) + "%*", "Markdown");
      playBuzzer(100);
    }
    else
    {
      bot.sendMessage(chat_id, "❌ Giá trị không hợp lệ! (50-90)");
    }
  }

  else if (text == "/reset")
  {
    bot.sendMessage(chat_id, "🔄 Đang khởi động lại ESP32...");
    delay(1000);
    ESP.restart();
  }

  else if (text == "/help")
  {
    String help = "📚 *TRỢ GIÚP*\n\n";
    help += "*Lệnh cơ bản:*\n";
    help += "/status - Xem trạng thái\n";
    help += "/history - Xem lịch sử\n\n";
    help += "*Điều khiển bơm:*\n";
    help += "1. Bật /manual\n";
    help += "2. Dùng /pump on hoặc /pump off\n";
    help += "3. Quay lại /auto\n\n";
    help += "*Cài đặt:*\n";
    help += "/settings - Xem cài đặt\n";
    help += "/set dry 40 - Đặt ngưỡng khô\n";
    help += "/set wet 70 - Đặt ngưỡng ẩm\n\n";
    help += "*Lưu ý:*\n";
    help += "• Ngưỡng khô: 10-60%\n";
    help += "• Ngưỡng ẩm: 50-90%\n";
    help += "• Ngưỡng ẩm > Ngưỡng khô";
    bot.sendMessage(chat_id, help, "Markdown");
  }

  else
  {
    bot.sendMessage(chat_id, "❌ Lệnh không hợp lệ!\nGõ /help để xem danh sách lệnh.");
  }
}

void checkTelegramMessages()
{
  int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

  for (int i = 0; i < numNewMessages; i++)
  {
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;
    String username = bot.messages[i].from_name;

    handleTelegramCommand(chat_id, text, username);
  }
}

// ==================== LCD DISPLAY ====================

void updateLCD()
{
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("S:");
  lcd.print(sensorData.soilMoisture);
  lcd.print("% T:");
  lcd.print(sensorData.temperature, 0);
  lcd.print("C");

  lcd.setCursor(0, 1);
  lcd.print("H:");
  lcd.print(sensorData.humidity, 0);
  lcd.print("% ");
  lcd.print(pumpState ? "PUMP:ON " : "PUMP:OFF");
}

// ==================== IN THÔNG TIN ====================

void printStatus()
{
  Serial.println("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
  Serial.println("📊 TRẠNG THÁI HỆ THỐNG");
  Serial.println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
  Serial.printf("💧 Độ ẩm đất:      %d%%\n", sensorData.soilMoisture);
  Serial.printf("🌡️  Nhiệt độ:       %.1f°C\n", sensorData.temperature);
  Serial.printf("💦 Độ ẩm không khí: %.1f%%\n", sensorData.humidity);
  Serial.printf("⚙️  Trạng thái bơm: %s\n", pumpState ? "🟢 BẬT" : "🔴 TẮT");
  Serial.printf("🎮 Chế độ:         %s\n", manualMode ? "THỦ CÔNG" : "TỰ ĐỘNG");
  Serial.printf("🔴 Ngưỡng khô:     %d%%\n", soilDryThreshold);
  Serial.printf("🟢 Ngưỡng ẩm:      %d%%\n", soilWetThreshold);
  Serial.printf("📡 WiFi:           %s\n", WiFi.status() == WL_CONNECTED ? "✅ Kết nối" : "❌ Mất kết nối");
  Serial.printf("⏱️  Uptime:         %luh %lum %lus\n",
                sensorData.uptime / 3600,
                (sensorData.uptime % 3600) / 60,
                sensorData.uptime % 60);
  Serial.println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
}

// ==================== SETUP ====================

void setup()
{
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n\n");
  Serial.println("╔════════════════════════════════════╗");
  Serial.println("║   🌱 SMART IRRIGATION SYSTEM 🌱   ║");
  Serial.println("║   ESP32 + ThingSpeak + Telegram   ║");
  Serial.println("╚════════════════════════════════════╝");
  Serial.println();

  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_WARNING, OUTPUT);
  pinMode(LED_OK, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(SOIL_PIN, INPUT);

  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(LED_WARNING, LOW);
  digitalWrite(LED_OK, HIGH);
  digitalWrite(BUZZER_PIN, LOW);

  loadSettings();

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Smart Irrigation");
  lcd.setCursor(0, 1);
  lcd.print("Starting...");
  delay(2000);

  dht.begin();
  Serial.println("✅ DHT22 đã khởi động");

  Serial.print("🔌 Đang kết nối WiFi");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connecting WiFi");

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20)
  {
    delay(500);
    Serial.print(".");
    lcd.setCursor(attempts % 16, 1);
    lcd.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("\n✅ Đã kết nối WiFi!");
    Serial.print("📍 IP Address: ");
    Serial.println(WiFi.localIP());

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WiFi Connected!");
    lcd.setCursor(0, 1);
    lcd.print(WiFi.localIP());
    delay(2000);
  }
  else
  {
    Serial.println("\n❌ Không thể kết nối WiFi!");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WiFi Failed!");
    delay(2000);
  }

  secureClient.setInsecure();
  Serial.println("✅ Telegram Bot đã sẵn sàng");

  playBuzzer(100);
  delay(100);
  playBuzzer(100);
  delay(100);
  playBuzzer(100);

  if (WiFi.status() == WL_CONNECTED && chatCount > 0)
  {
    String startMsg = "🌱 *HỆ THỐNG TƯỚI CÂY ONLINE*\n";
    startMsg += "━━━━━━━━━━━━━━━━\n";
    startMsg += "✅ ESP32 đã khởi động\n";
    startMsg += "📡 WiFi: Kết nối\n";
    startMsg += "📍 IP: `" + WiFi.localIP().toString() + "`\n";
    startMsg += "🎮 Chế độ: *" + String(manualMode ? "THỦ CÔNG" : "TỰ ĐỘNG") + "*\n";
    startMsg += "👥 Đang phục vụ: *" + String(chatCount) + " chat*\n";
    startMsg += "━━━━━━━━━━━━━━━━\n";
    startMsg += "Gõ /help để xem lệnh";
    broadcastTelegram(startMsg);
  }

  delay(2000);
  Serial.println("\n🚀 Hệ thống đã sẵn sàng!\n");
}

// ==================== LOOP ====================

void loop()
{
  unsigned long currentMillis = millis();

  if (currentMillis - lastSensorRead >= SENSOR_READ_INTERVAL)
  {
    lastSensorRead = currentMillis;

    readSensors();
    controlPump();
    printStatus();
    updateLCD();
  }

  if (currentMillis - lastUpdate >= UPDATE_INTERVAL)
  {
    lastUpdate = currentMillis;
    sendToThingSpeak(
        sensorData.soilMoisture,
        sensorData.temperature,
        sensorData.humidity,
        pumpState ? 1 : 0);
  }

  if (currentMillis - lastBotCheck >= 1000)
  {
    lastBotCheck = currentMillis;
    checkTelegramMessages();
  }

  if (pumpState)
  {
    static unsigned long lastBlink = 0;
    if (currentMillis - lastBlink > 500)
    {
      lastBlink = currentMillis;
      digitalWrite(LED_WARNING, !digitalRead(LED_WARNING));
    }
  }

  delay(100);
}