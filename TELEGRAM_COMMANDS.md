# 📱 Hướng dẫn sử dụng Telegram Bot - Hệ thống tưới cây thông minh

## 📋 Mục lục

1. [Đăng ký sử dụng Bot](#đăng-ký-sử-dụng-bot)
2. [Các lệnh cơ bản](#các-lệnh-cơ-bản)
3. [Điều khiển bơm tưới](#điều-khiển-bơm-tưới)
4. [Cài đặt ngưỡng tưới](#cài-đặt-ngưỡng-tưới)
5. [Quản lý và bảo trì](#quản-lý-và-bảo-trì)
6. [Câu hỏi thường gặp (FAQ)](#câu-hỏi-thường-gặp)

---

## 🚀 Đăng ký sử dụng Bot

### Bước 1: Tìm Bot trên Telegram

1. Mở ứng dụng Telegram
2. Tìm kiếm tên bot hoặc username (ví dụ: `@YourIrrigationBot`)
3. Nhấn **Start** hoặc gửi tin nhắn đầu tiên

### Bước 2: Đăng ký Chat ID

```
/register irrigation2024
```

**Lưu ý:**

- Mật khẩu mặc định là: `irrigation2024`
- Liên hệ admin để lấy mật khẩu nếu đã thay đổi
- Mỗi hệ thống chỉ hỗ trợ tối đa **10 chat** đăng ký

### Bước 3: Xác nhận đăng ký thành công

Nếu thành công, bạn sẽ nhận được:

```
✅ Đăng ký thành công!
Chat này đã được thêm vào danh sách.
Gửi /start để xem lệnh.
```

### Bước 4: Xem danh sách lệnh

```
/start
```

---

## 📊 Các lệnh cơ bản

### 1. Xem trạng thái hệ thống

```
/status
```

**Thông tin hiển thị:**

```
📊 TRẠNG THÁI HỆ THỐNG
━━━━━━━━━━━━━━━━
💧 Độ ẩm đất: 45%
🌡️ Nhiệt độ: 28.5°C
💦 Độ ẩm KK: 65.2%
⚙️ Bơm: 🔴 TẮT
🎮 Chế độ: TỰ ĐỘNG
📡 WiFi: ✅
⏱️ Uptime: 5h 23m
🔋 IP: 192.168.1.100
```

**Giải thích:**

- **Độ ẩm đất**: % độ ẩm đất (0-100%)
- **Nhiệt độ**: Nhiệt độ môi trường (°C)
- **Độ ẩm KK**: Độ ẩm không khí (%)
- **Bơm**: 🟢 BẬT hoặc 🔴 TẮT
- **Chế độ**: TỰ ĐỘNG hoặc THỦ CÔNG
- **WiFi**: ✅ Kết nối / ❌ Mất kết nối
- **Uptime**: Thời gian hoạt động liên tục
- **IP**: Địa chỉ IP của ESP32

---

### 2. Xem lịch sử hoạt động

```
/history
```

**Thông tin hiển thị:**

```
📈 LỊCH SỬ 24H
━━━━━━━━━━━━━━━━
💧 Độ ẩm đất:
   • Trung bình: 52.3%
   • Tối thiểu: 38%
   • Tối đa: 72%
⚙️ Số lần tưới: 5
⏱️ Uptime: 18h
```

**Hữu ích khi:**

- Theo dõi xu hướng độ ẩm đất
- Kiểm tra tần suất tưới
- Đánh giá hiệu quả hệ thống

---

### 3. Trợ giúp chi tiết

```
/help
```

Hiển thị hướng dẫn nhanh về:

- Các lệnh cơ bản
- Cách điều khiển bơm
- Cách cài đặt ngưỡng
- Lưu ý quan trọng

---

## 🚰 Điều khiển bơm tưới

### ⚠️ LƯU Ý QUAN TRỌNG

**Bạn PHẢI bật chế độ THỦ CÔNG trước khi điều khiển bơm!**

### Quy trình điều khiển bơm

#### Bước 1: Chuyển sang chế độ thủ công

```
/manual
```

**Phản hồi:**

```
🎮 Đã chuyển sang chế độ THỦ CÔNG
Dùng /pump on hoặc /pump off để điều khiển
```

#### Bước 2: Bật bơm

```
/pump on
```

hoặc

```
/pump_on
```

**Phản hồi:**

```
🚨 BẬT BƠM TƯỚI
━━━━━━━━━━━━━━━━
💧 Độ ẩm đất: 35%
🌡️ Nhiệt độ: 28.0°C
💦 Độ ẩm KK: 60.0%
⚙️ Chế độ: THỦ CÔNG
```

#### Bước 3: Tắt bơm

```
/pump off
```

hoặc

```
/pump_off
```

**Phản hồi:**

```
✅ TẮT BƠM
━━━━━━━━━━━━━━━━
💧 Độ ẩm đất: 68%
🌡️ Nhiệt độ: 27.5°C
💦 Độ ẩm KK: 62.0%
⚙️ Chế độ: THỦ CÔNG
```

#### Bước 4: Quay lại chế độ tự động

```
/auto
```

**Phản hồi:**

```
🤖 Đã chuyển sang chế độ TỰ ĐỘNG
```

---

### Chế độ hoạt động

#### 🤖 Chế độ TỰ ĐỘNG (Auto Mode) - MẶC ĐỊNH

**Cách hoạt động:**

- Hệ thống tự động đo độ ẩm đất mỗi 2 giây
- Nếu độ ẩm **< Ngưỡng khô** → **Bật bơm**
- Nếu độ ẩm **> Ngưỡng ẩm** → **Tắt bơm**

**Ví dụ với ngưỡng mặc định:**

- Ngưỡng khô = 40%
- Ngưỡng ẩm = 70%

| Độ ẩm hiện tại | Hành động                   |
| -------------- | --------------------------- |
| 35%            | 🔴 Bật bơm tưới             |
| 55%            | ⏸️ Không làm gì (đang tưới) |
| 72%            | 🟢 Tắt bơm                  |

**Ưu điểm:**

- ✅ Tiết kiệm nước
- ✅ Tưới đúng lúc
- ✅ Không cần giám sát

**Nhược điểm:**

- ❌ Không linh hoạt với nhu cầu đột xuất

---

#### 🎮 Chế độ THỦ CÔNG (Manual Mode)

**Cách hoạt động:**

- Hệ thống KHÔNG tự động bật/tắt bơm
- Người dùng điều khiển hoàn toàn qua Telegram

**Khi nào nên dùng:**

- 🔧 Bảo trì hệ thống
- 💧 Tưới đột xuất (thời tiết nóng)
- 🚫 Tạm ngưng tưới (đổ phân, xới đất)
- 🧪 Thử nghiệm hệ thống

**Lưu ý an toàn:**

- ⚠️ Không để bơm chạy quá 30 phút liên tục
- ⚠️ Kiểm tra độ ẩm trước khi bật bơm
- ⚠️ Nhớ tắt bơm sau khi tưới
- ⚠️ Quay lại chế độ tự động khi xong

---

## ⚙️ Cài đặt ngưỡng tưới

### Xem cài đặt hiện tại

```
/settings
```

**Hiển thị:**

```
⚙️ CÀI ĐẶT HỆ THỐNG
━━━━━━━━━━━━━━━━
🔴 Ngưỡng khô: 40%
   (Bật bơm khi < 40%)

🟢 Ngưỡng ẩm: 70%
   (Tắt bơm khi > 70%)

🎮 Chế độ: TỰ ĐỘNG
🔊 Buzzer: BẬT

💡 Thay đổi:
/set dry 35 - Đặt ngưỡng khô 35%
/set wet 75 - Đặt ngưỡng ẩm 75%
```

---

### Thay đổi ngưỡng khô

```
/set dry <giá_trị>
```

**Ví dụ:**

```
/set dry 35
```

**Phản hồi:**

```
✅ Đã đặt ngưỡng khô: 35%
```

**Phạm vi cho phép:** 10% - 60%

**Giải thích:**

- Ngưỡng khô là mức độ ẩm mà hệ thống sẽ BẬT bơm
- Càng **thấp** = Đất phải khô hơn mới tưới
- Càng **cao** = Tưới sớm hơn, tưới nhiều hơn

**Khuyến nghị theo loại cây:**

| Loại cây              | Ngưỡng khô đề xuất |
| --------------------- | ------------------ |
| Xương rồng, sen đá    | 20-25%             |
| Cây văn phòng         | 30-35%             |
| Rau ăn lá             | 40-45%             |
| Hoa hồng, cây ăn trái | 35-40%             |

---

### Thay đổi ngưỡng ẩm

```
/set wet <giá_trị>
```

**Ví dụ:**

```
/set wet 75
```

**Phản hồi:**

```
✅ Đã đặt ngưỡng ẩm: 75%
```

**Phạm vi cho phép:** 50% - 90%

**Giải thích:**

- Ngưỡng ẩm là mức độ ẩm mà hệ thống sẽ TẮT bơm
- Càng **cao** = Tưới nhiều nước hơn
- Càng **thấp** = Tưới ít nước hơn

**Khuyến nghị:**

| Mùa/Điều kiện        | Ngưỡng ẩm đề xuất |
| -------------------- | ----------------- |
| Mùa hè, nắng nóng    | 75-80%            |
| Mùa đông, ít nắng    | 60-65%            |
| Trong nhà, điều hòa  | 65-70%            |
| Ngoài trời, tự nhiên | 70-75%            |

---

### ⚠️ Quy tắc quan trọng

**NGƯỠNG ẨM PHẢI LỚN HƠN NGƯỠNG KHÔ!**

❌ **SAI:**

```
/set dry 50
/set wet 45
```

→ Bơm sẽ bật tắt liên tục!

✅ **ĐÚNG:**

```
/set dry 40
/set wet 70
```

→ Bơm hoạt động ổn định.

**Khoảng cách tối thiểu:** 20%

---

### Ví dụ cài đặt theo tình huống

#### Tình huống 1: Mùa hè, cây cần nhiều nước

```
/set dry 45
/set wet 80
```

→ Tưới sớm (45%) và tưới nhiều (80%)

#### Tình huống 2: Mùa đông, cây ít cần nước

```
/set dry 30
/set wet 60
```

→ Đợi khô hơn (30%) và tưới ít (60%)

#### Tình huống 3: Tiết kiệm nước tối đa

```
/set dry 25
/set wet 55
```

→ Chỉ tưới khi thực sự cần

#### Tình huống 4: Cây ưa ẩm (rau ăn lá)

```
/set dry 50
/set wet 85
```

→ Giữ đất luôn ẩm

---

## 🔧 Quản lý và bảo trì

### Xem danh sách chat đã đăng ký

```
/chats
```

**Hiển thị:**

```
📋 DANH SÁCH CHAT ĐĂNG KÝ
━━━━━━━━━━━━━━━━
Tổng: 3/10

1. 123456789
2. 987654321
3. 555666777
```

**Lưu ý:**

- Chỉ admin mới nên xem lệnh này
- Giới hạn 10 chat để tránh spam

---

### Hủy đăng ký chat

```
/unregister
```

**Phản hồi:**

```
✅ Đã hủy đăng ký chat này.
Bot sẽ không gửi thông báo nữa.
```

**Khi nào nên dùng:**

- Không còn sử dụng hệ thống
- Chuyển sang số điện thoại mới
- Muốn dừng nhận thông báo

**Để đăng ký lại:**

```
/register irrigation2024
```

---

### Khởi động lại hệ thống

```
/reset
```

**Phản hồi:**

```
🔄 Đang khởi động lại ESP32...
```

**Khi nào nên dùng:**

- ❌ Hệ thống không phản hồi
- ❌ Cảm biến đọc sai
- ❌ WiFi mất kết nối lâu
- ⚙️ Sau khi thay đổi cài đặt phần cứng

**Lưu ý:**

- Mất kết nối bot khoảng 30 giây
- Tất cả cài đặt đã lưu được giữ nguyên
- Không ảnh hưởng đến dữ liệu ThingSpeak

---

## 🔔 Thông báo tự động

Hệ thống sẽ tự động gửi thông báo đến TẤT CẢ chat đã đăng ký khi:

### 1. Khởi động hệ thống

```
🌱 HỆ THỐNG TƯỚI CÂY ONLINE
━━━━━━━━━━━━━━━━
✅ ESP32 đã khởi động
📡 WiFi: Kết nối
📍 IP: 192.168.1.100
🎮 Chế độ: TỰ ĐỘNG
👥 Đang phục vụ: 3 chat
━━━━━━━━━━━━━━━━
Gõ /help để xem lệnh
```

### 2. Bật bơm tưới (Tự động)

```
🚨 BẬT BƠM TƯỚI
━━━━━━━━━━━━━━━━
💧 Độ ẩm đất: 35%
🌡️ Nhiệt độ: 28.0°C
💦 Độ ẩm KK: 60.0%
⚙️ Chế độ: TỰ ĐỘNG
```

### 3. Tắt bơm (Tự động)

```
✅ TẮT BƠM
━━━━━━━━━━━━━━━━
💧 Độ ẩm đất: 72%
🌡️ Nhiệt độ: 27.5°C
💦 Độ ẩm KK: 62.0%
⚙️ Chế độ: TỰ ĐỘNG
```

### 4. Điều khiển thủ công

```
🚨 BẬT BƠM TƯỚI
━━━━━━━━━━━━━━━━
💧 Độ ẩm đất: 45%
🌡️ Nhiệt độ: 28.5°C
💦 Độ ẩm KK: 65.0%
⚙️ Chế độ: THỦ CÔNG
```

**Lợi ích:**

- 👀 Giám sát từ xa mọi lúc mọi nơi
- 📊 Biết chính xác khi nào tưới
- 🔔 Phát hiện sự cố nhanh chóng
- 📱 Nhiều người cùng theo dõi

---

## ❓ Câu hỏi thường gặp

### Q1: Bot không phản hồi lệnh?

**A:** Kiểm tra theo thứ tự:

1. ✅ ESP32 đã bật chưa?
2. ✅ WiFi có kết nối không?
3. ✅ Chat đã đăng ký chưa? (`/register`)
4. ✅ Thử lệnh `/reset` để khởi động lại

---

### Q2: Làm sao để thêm nhiều người dùng?

**A:**

1. Gửi mật khẩu cho người khác: `irrigation2024`
2. Họ tìm bot trên Telegram
3. Họ gửi: `/register irrigation2024`
4. Tối đa 10 chat có thể đăng ký

---

### Q3: Quên mật khẩu đăng ký?

**A:**

- Mật khẩu mặc định: `irrigation2024`
- Nếu đã thay đổi, xem trong file `config.h`
- Liên hệ người cài đặt hệ thống

---

### Q4: Làm sao biết bơm đang chạy?

**A:** Có 3 cách:

1. Gửi `/status` - Xem trạng thái bơm
2. Quan sát LED đỏ nhấp nháy trên ESP32
3. Nghe tiếng buzzer bíp khi bật/tắt

---

### Q5: Bơm chạy quá lâu, làm sao dừng?

**A:**

```
/manual      # Bật chế độ thủ công
/pump off    # Tắt bơm ngay lập tức
```

---

### Q6: Muốn tạm ngưng tưới tự động?

**A:**

```
/manual      # Chuyển sang thủ công
/pump off    # Đảm bảo bơm tắt
```

Khi muốn bật lại:

```
/auto        # Quay lại tự động
```

---

### Q7: Độ ẩm đất hiển thị sai?

**A:**

1. Kiểm tra cảm biến có cắm đúng không
2. Gửi `/reset` để khởi động lại
3. Đợi 5 phút để cảm biến ổn định
4. Nếu vẫn sai, cần hiệu chỉnh phần cứng

---

### Q8: Nhận quá nhiều thông báo?

**A:**

- Điều chỉnh ngưỡng để giảm tần suất tưới:

```
/set dry 35   # Giảm ngưỡng khô
/set wet 75   # Tăng ngưỡng ẩm
```

- Hoặc hủy đăng ký: `/unregister`

---

### Q9: Làm sao xem dữ liệu lâu dài?

**A:**

- Truy cập ThingSpeak để xem biểu đồ
- ThingSpeak lưu trữ dữ liệu nhiều tháng
- Link được cung cấp khi cài đặt hệ thống

---

### Q10: Điện mất, hệ thống có mất cài đặt?

**A:**
**KHÔNG!** Các thông tin sau được lưu vào Flash:

- ✅ Danh sách chat đã đăng ký
- ✅ Ngưỡng tưới tùy chỉnh
- ✅ Chế độ hoạt động (Auto/Manual)

---

## 🎯 Tips & Tricks

### 💡 Mẹo 1: Kiểm tra hàng ngày

Gửi `/status` vào mỗi sáng để:

- Biết độ ẩm đất hiện tại
- Đảm bảo hệ thống hoạt động
- Phát hiện sự cố sớm

### 💡 Mẹo 2: Điều chỉnh theo mùa

**Mùa hè:**

```
/set dry 45
/set wet 80
```

**Mùa đông:**

```
/set dry 30
/set wet 60
```

### 💡 Mẹo 3: Trước khi đi du lịch

1. Kiểm tra hệ thống: `/status`
2. Đảm bảo chế độ: `/auto`
3. Điều chỉnh tưới nhiều hơn:

```
/set dry 50
/set wet 85
```

### 💡 Mẹo 4: Tiết kiệm nước

```
/set dry 25    # Chỉ tưới khi rất khô
/set wet 55    # Tưới vừa đủ
```

### 💡 Mẹo 5: Cây ưa ẩm

```
/set dry 55    # Tưới sớm
/set wet 90    # Giữ ẩm cao
```

---

## 🆘 Xử lý sự cố

### Sự cố 1: Bot offline

**Triệu chứng:** Bot không phản hồi mọi lệnh

**Giải pháp:**

1. Kiểm tra ESP32 có đèn nguồn sáng không
2. Kiểm tra WiFi router có hoạt động không
3. Khởi động lại ESP32 (rút nguồn 10 giây)
4. Nếu vẫn không được, kiểm tra cáp và nguồn điện

---

### Sự cố 2: Bơm không bật tự động

**Triệu chứng:** Đất khô nhưng bơm không chạy

**Giải pháp:**

1. Kiểm tra chế độ: `/settings`

   - Phải ở chế độ **TỰ ĐỘNG**
   - Nếu MANUAL, gửi `/auto`

2. Kiểm tra ngưỡng:

   - Ngưỡng khô có phù hợp không?
   - Thử giảm: `/set dry 45`

3. Kiểm tra relay:
   - Có tiếng "click" khi chuyển đổi không?

---

### Sự cố 3: Bơm chạy không ngừng

**Triệu chứng:** Bơm bật mãi không tắt

**Giải pháp:**

1. **Tắt ngay bằng tay:**

```
/manual
/pump off
```

2. Kiểm tra cảm biến đất:

   - Có cắm đúng không?
   - Có bị oxy hóa/hỏng không?

3. Kiểm tra ngưỡng ẩm:
   - Có quá cao không? (>90%)
   - Thử giảm: `/set wet 70`

---

### Sự cố 4: Thông báo liên tục

**Triệu chứng:** Nhận hàng chục tin nhắn trong vài phút

**Nguyên nhân:** Ngưỡng ẩm và ngưỡng khô quá gần nhau

**Giải pháp:**

```
/set dry 35
/set wet 75
```

→ Khoảng cách ít nhất 20%

---

## 📞 Liên hệ hỗ trợ

Nếu gặp vấn đề không thể tự xử lý:

1. **Ghi lại thông tin:**

   - Gửi `/status` và chụp màn hình
   - Mô tả chính xác sự cố
   - Ghi lại thời gian xảy ra

2. **Kiểm tra Serial Monitor:**

   - Kết nối ESP32 với máy tính
   - Mở Serial Monitor (115200 baud)
   - Chụp màn hình log lỗi

3. **Liên hệ:**
   - Gửi thông tin cho người cài đặt
   - Hoặc liên hệ admin hệ thống

---

## ✅ Checklist sử dụng hàng ngày

### Mỗi ngày

- [ ] Gửi `/status` kiểm tra hệ thống
- [ ] Đọc thông báo từ bot (nếu có)
- [ ] Quan sát cây có dấu hiệu khô/úng không

### Mỗi tuần

- [ ] Gửi `/history` xem thống kê
- [ ] Kiểm tra số lần tưới có hợp lý không
- [ ] Làm sạch cảm biến đất (lau nhẹ)

### Mỗi tháng

- [ ] Xem biểu đồ trên ThingSpeak
- [ ] Điều chỉnh ngưỡng theo mùa
- [ ] Kiểm tra phần cứng có hỏng hóc không

---

## 🎓 Kết luận

Telegram Bot giúp bạn:

- ✅ Điều khiển tưới từ xa mọi lúc mọi nơi
- ✅ Giám sát hệ thống real-time
- ✅ Nhận thông báo tức thì
- ✅ Tùy chỉnh linh hoạt
- ✅ Tiết kiệm nước và thời gian

**Lưu các lệnh hay dùng:**

```
/status      # Xem trạng thái
/history     # Xem lịch sử
/settings    # Xem cài đặt
/manual      # Bật thủ công
/auto        # Bật tự động
/pump on     # Bật bơm (thủ công)
/pump off    # Tắt bơm (thủ công)
```

---

**🌱 Chúc bạn có những vườn cây xanh tươi!**

_Tài liệu này được cập nhật thường xuyên. Nếu có câu hỏi, vui lòng liên hệ admin hệ thống._
