# Tiến độ Dự án Adaptive Vitals Monitor (IoT Health Wristband)

Tài liệu này ghi lại chi tiết các đầu việc đã thực hiện, các file mã nguồn tương ứng và các hạng mục còn lại của đồ án môn học IoT.

---

## 1. Báo cáo Giai đoạn 2 (Word)
- [x] **File báo cáo:** `Nhom3-Detai31_GD2.docx` (đã sao lưu bản gốc `Nhom3-Detai31_GD2.docx.bak`)
- [x] **Mục 2.2 – Thiết kế kiến trúc hệ thống tổng thể:**
  - Mô hình 4 tầng (Tầng thiết bị đeo $\to$ Tầng truyền thông Gateway/MQTT $\to$ Tầng Cloud Backend/Firebase $\to$ Tầng ứng dụng Web).
  - 3 luồng dữ liệu chính: Telemetry định kỳ (1s), Alert bất thường thời gian thực, Đồng bộ Profile/Baseline.
  - **Hình 2.1 – Sơ đồ kiến trúc tổng thể hệ thống:** Đã vẽ sơ đồ 4 tầng trực quan, độ phân giải cao (300 DPI), đầy đủ các khối linh kiện, xử lý biên, MQTT topics, Firebase và Web Dashboard; chèn trực tiếp vào báo cáo.
- [x] **Mục 2.3 – Phương thức truyền thông:**
  - So sánh phân tích MQTT vs HTTP/WebSocket.
  - Cấu trúc Topic Hierarchy chuẩn IoT (`wristband/{userId}/data`, `wristband/{userId}/alerts`, `wristband/{userId}/commands`).
  - Đặc tả Payload định dạng JSON cho dữ liệu cảm biến và cảnh báo.
- [x] **Định dạng văn bản chuẩn:** Times New Roman 13pt, line spacing 1.5 (360), space after 200, bullet `numId` chuẩn, bảng biểu căn đều.

---

## 2. Firmware ESP32 & Mô phỏng Wokwi (`firmware/`)
- [x] **Cấu hình mô phỏng:**
  - `firmware/diagram.json`: Sơ đồ Wokwi (ESP32-DevKit-V1, OLED SSD1306, LED/Rung, nút SOS GPIO 25, nút Tập luyện GPIO 26, biến trở nhịp tim GPIO 34).
  - `firmware/wokwi.toml`: Cấu hình tích hợp trình mô phỏng Wokwi.
  - `firmware/platformio.ini`: Môi trường build PlatformIO, quản lý thư viện ArduinoJson, Adafruit SSD1306, PubSubClient.
- [x] **Cấu hình hệ thống:** `firmware/src/config.h` (ngưỡng Z-score, thông số Wi-Fi, HiveMQ broker).
- [x] **Thuật toán xử lý tại biên (Edge Processing):**
  - `firmware/src/processing/filter.h`: Bộ lọc trung bình trượt (Moving Average) làm mịn nhiễu tín hiệu.
  - `firmware/src/processing/baseline.h` & `.cpp`: Thuật toán học Baseline cá nhân 2 pha (Warm-up $\alpha=0.15 \to 0.01$, phân tách baseline `REST` và `ACTIVE`).
  - `firmware/src/processing/classifier.h` & `.cpp`: Phân loại 4 cấp độ cảnh báo (Z-score + tích lũy hệ số bệnh nền + Max-rule + ức chế báo giả giai đoạn ấm máy).
- [x] **Cảm biến & Điều khiển Demo:**
  - `firmware/src/sensors/heart_rate.h` & `.cpp`: Giả lập nhịp tim cơ sở, hỗ trợ biến trở chiết áp ghi đè giá trị tức thời.
  - `firmware/src/sensors/temperature.h` & `.cpp`: Giả lập thân nhiệt theo dao động sinh học.
  - `firmware/src/sensors/motion.h` & `.cpp`: Giả lập gia tốc 3 trục phát hiện vận động/nghỉ ngơi.
  - `firmware/src/sensors/demo_controls.h` & `.cpp`: Xử lý 2 nút nhấn mô phỏng khẩn cấp và chuyển chế độ tập luyện.
- [x] **Hiển thị & Cảnh báo vật lý:**
  - `firmware/src/display/oled_display.h` & `.cpp`: Giao diện hiển thị trực quan thông số và trạng thái trên màn hình OLED.
  - `firmware/src/display/vibration.h` & `.cpp`: Điều khiển rung haptic/LED theo 4 cấp độ cảnh báo.
- [x] **Truyền thông & Vòng lặp chính:**
  - `firmware/src/communication/mqtt_client.h` & `.cpp`: Kết nối MQTT HiveMQ, tự động reconnect và gửi payload JSON.
  - `firmware/src/main.cpp`: Vòng lặp chính điều phối đọc cảm biến, tính toán baseline, hiển thị và truyền tin.

---

## 3. Backend Dịch vụ Node.js (`backend/`)
- [x] **Cấu hình & Điểm vào:** `backend/package.json`, `backend/.env.example`, `backend/src/index.js`.
- [x] **Tích hợp Firebase Realtime Database:**
  - `backend/src/firebase/config.js` & `database.js`: Kết nối Firebase Admin SDK và cơ chế dự phòng Firebase REST API khi chưa có file key.
- [x] **MQTT Subscriber:**
  - `backend/src/mqtt/subscriber.js`: Lắng nghe dữ liệu telemetry và alerts từ broker MQTT.
- [x] **Dịch vụ phân tích & Cảnh báo:**
  - `backend/src/services/data-processor.js`: Lưu dữ liệu chuỗi thời gian, phân tích thống kê và cập nhật baseline dài hạn.
  - `backend/src/services/alert-service.js`: Xử lý luồng thông báo cảnh báo và kích hoạt trạng thái khẩn cấp.
- [x] **Script kiểm thử:**
  - `backend/scripts/mock-publisher.js`: Giả lập thiết bị gửi dữ liệu lên MQTT để test backend/frontend độc lập mà không cần bật ESP32.

---

## 4. Web App Frontend (`web-app/`)
- [x] **Giao diện Dashboard chính:** `web-app/index.html` (Thanh điều hướng, banner cảnh báo, thẻ tổng quan, 3 thẻ chỉ số sinh tồn có khung Chart.js, lịch sử cảnh báo).
- [ ] *Giao diện CSS (Dark theme / Glassmorphism): `web-app/css/style.css` (Chưa làm)*
- [ ] *Cấu hình Firebase Frontend: `web-app/js/firebase-config.js` (Chưa làm)*
- [ ] *Vẽ biểu đồ thời gian thực: `web-app/js/dashboard.js` (Chưa làm)*
- [ ] *Xử lý cảnh báo & âm thanh: `web-app/js/alerts.js` (Chưa làm)*
- [ ] *Bộ điều phối dữ liệu Frontend: `web-app/js/app.js` (Chưa làm)*
- [ ] *Trang hồ sơ cá nhân & điều chỉnh bệnh nền: `web-app/profile.html` & `profile.js` (Chưa làm)*

---

## 5. Tài liệu kiến trúc & Thiết kế (`docs/`)
- [ ] *Sơ đồ kiến trúc tổng thể: `docs/architecture.md` (Chưa làm)*
- [ ] *Thiết kế cơ sở dữ liệu Firebase: `docs/database-schema.md` (Chưa làm)*
- [ ] *Đặc tả thuật toán Baseline & Phân loại: `docs/algorithms.md` (Chưa làm)*
