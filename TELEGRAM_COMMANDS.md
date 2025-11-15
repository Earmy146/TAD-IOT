# 📱 Hướng dẫn sử dụng Telegram Bot

## 📋 Danh sách lệnh đầy đủ

### 🔍 Giám sát

#### `/start`
Hiển thị menu lệnh và hướng dẫn sử dụng

**Ví dụ:**
```
/start
```

---

#### `/status`
Xem trạng thái hiện tại của hệ thống

**Thông tin hiển thị:**
- 💧 Độ ẩm đất (%)
- 🌡️ Nhiệt độ (°C)
- 💦 Độ ẩm không khí (%)
- ⚙️ Trạng thái bơm (BẬT/TẮT)
- 🎮 Chế độ hoạt động (TỰ ĐỘNG/THỦ CÔNG)
- 📡 Trạng thái WiFi
- ⏱️ Thời gian hoạt động
- 🔋 Địa chỉ IP

**Ví dụ:**
```
/status
```

---

#### `/history`
Xem lịch sử hoạt động 24h

**Thông tin hiển thị:**
- Độ ẩm đất trung bình
- Độ ẩm đất tối thiểu
- Độ ẩm đất tối đa
- Số lần tưới
- Thời gian hoạt động

**Ví dụ:**
```
/history
```

---

### ⚙️ Điều khiển bơm

#### `/manual`
Chuyển sang chế độ THỦ CÔNG

**Lưu ý:** Phải bật chế độ thủ công trước khi điều khiển bơm

**Ví dụ:**
```
/manual
```

---

#### `/pump on`
Bật bơm tưới (chỉ hoạt động ở chế độ thủ công)

**Cách sử dụng:**
1. Gõ `/manual` để chuyển sang chế độ thủ công
2. Gõ `/pump on` để bật bơm

**Ví dụ:**
```
/manual
/pump on
```

---

#### `/pump off`
Tắt bơm tưới (chỉ hoạt động ở chế độ thủ công)

**Ví dụ:**
```
/pump off
```

---

#### `/auto`
Chuyển về chế độ TỰ ĐỘNG

Hệ thống sẽ tự động bật/tắt bơm dựa trên ngưỡng độ ẩm đất

**Ví dụ:**
```
/auto
```

---

### 🔧 Cài đặt

#### `/settings`
Xem cài đặt hiện tại

**Thông tin hiển thị:**
- 🔴 Ngưỡng khô (%)
- 🟢 Ngưỡng ẩm (%)
- 🎮 Chế độ hoạt động
- 🔊 Trạng thái buzzer

**Ví dụ:**
```
/settings
```

---

#### `/set dry <giá trị>`
Thay đổi ngưỡng khô (10-60%)

Khi độ ẩm đất < ngưỡng này → hệ thống sẽ BẬT bơm

**Giá trị hợp lệ:** 10 - 60

**Ví dụ:**
```
/set dry 35
/set dry 40
/set dry 30
```

**Giải thích:**
- `/set dry 35` → Bật bơm khi độ ẩm đất < 35%
- `/set dry 40` → Bật bơm khi độ ẩm đất < 40%

---

#### `/set wet <giá trị>`
Thay đổi ngưỡng ẩm (50-90%)

Khi độ ẩm đất > ngưỡng này → hệ thống sẽ TẮT bơm

**Giá trị hợp lệ:** 50 - 90

**Ví dụ:**
```
/set wet 70
/set wet 75
/set wet 80
```

**Giải thích:**
- `/set wet 70` → Tắt bơm khi độ ẩm đất > 70%
- `/set wet 75` → Tắt bơm khi độ ẩm đất > 75%

---

#### `/reset`
Khởi động lại ESP32

**Lưu ý:** Tất cả cài đặt đã lưu sẽ được giữ nguyên

**Ví dụ:**
```
/reset
```

---

#### `/help`
Hiển thị trợ giúp chi tiết

**Ví dụ:**
```
/help
```

---

## 🎯 Kịch bản sử dụng phổ biến

### Kịch bản 1: Tưới cây thủ công ngay lập tức

```
Bước 1: /manual          → Chuyển sang chế độ thủ công
Bước 2: /pump on         → Bật bơm
Bước 3: (Chờ 5 phút)
Bước 4: /pump off        → Tắt bơm
Bước 5: /auto            → Quay về chế độ tự động
```

---

### Kịch bản 2: Điều chỉnh ngưỡng tưới cho mùa khô

```
# Mùa khô → Cần tưới sớm hơn, nhiều hơn
/set dry 45    → Tăng ngưỡng khô (bật bơm sớm hơn)
/set wet 80    → Tăng ngưỡng ẩm (tưới lâu hơn)
/settings      → Kiểm tra cài đặt
```

---

### Kịch bản 3: Điều chỉnh ngưỡng cho mùa mưa

```
# Mùa mưa → Giảm tưới
/set dry 30    → Giảm ngưỡng khô (chỉ tưới khi thực sự khô)
/set wet 65    → Giảm ngưỡng ẩm (tưới ít hơn)
/settings      → Kiểm tra cài đặt
```

---

### Kịch bản 4: Kiểm tra sức khỏe hệ thống

```
/status        → Xem trạng thái hiện tại
/history       → Xem lịch sử 24h
/settings      → Xem cài đặt
```

---

## ⚠️ Lưu ý quan trọng

### 🔴 Ngưỡng khô (Dry Threshold)
- **Giá trị:** 10 - 60%
- **Ý nghĩa:** Khi độ ẩm đất < giá trị này → BẬT bơm
- **Khuyến nghị:**
  - Cây ưa ẩm: 40-50%
  - Cây trung bình: 30-40%
  - Cây chịu hạn: 20-30%

### 🟢 Ngưỡng ẩm (Wet Threshold)
- **Giá trị:** 50 - 90%
- **Ý nghĩa:** Khi độ ẩm đất > giá trị này → TẮT bơm
- **Khuyến nghị:**
  - Cây ưa ẩm: 75-85%
  - Cây trung bình: 65-75%
  - Cây chịu hạn: 55-65%

### ⚖️ Quy tắc vàng
**Ngưỡng ẩm PHẢI LỚN HƠN ngưỡng khô ít nhất 10-15%**

**Ví dụ hợp lệ:**
```
Dry: 35% | Wet: 70% ✅ (Chênh lệch 35%)
Dry: 40% | Wet: 65% ✅ (Chênh lệch 25%)
```

**Ví dụ KHÔNG hợp lệ:**
```
Dry: 50% | Wet: 55% ❌ (Chênh lệch quá nhỏ - bơm sẽ bật/tắt liên tục)
Dry: 60% | Wet: 55% ❌ (Ngược ngược)
```

---

## 🔐 Bảo mật

- ✅ Bot chỉ nhận lệnh từ Chat ID đã cấu hình
- ✅ Tất cả lệnh điều khiển đều có xác thực
- ✅ Cài đặt được lưu vào bộ nhớ Flash (không mất khi tắt nguồn)
- ✅ Log mọi hoạt động ra Serial Monitor

---

## 🎉 Tips & Tricks

### 1. Xem IP để truy cập Dashboard
```
/status
→ Sao chép IP address
→ Mở browser: http://<IP>:3000
```

### 2. Kiểm tra hoạt động định kỳ
Đặt alarm trong Telegram để gửi `/status` mỗi ngày

### 3. Chế độ tiết kiệm năng lượng
```
# Giảm số lần tưới
/set dry 25
/set wet 65
```

### 4. Chế độ "nghỉ phép"
```
# Tắt hệ thống tạm thời
/manual
/pump off
```

### 5. Khôi phục cài đặt gốc
```
# Sửa file config.h và reset
/reset
```

---

## 📞 Hỗ trợ

Nếu có lỗi, kiểm tra:
1. ✅ Bot Token và Chat ID đúng chưa?
2. ✅ ESP32 đã kết nối WiFi chưa?
3. ✅ Serial Monitor có báo lỗi gì không?
4. ✅ Thử gửi `/start` để kiểm tra kết nối

---

**Chúc bạn sử dụng hiệu quả! 🌱**