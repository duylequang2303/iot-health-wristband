# Adaptive Vitals Monitor – Vòng tay theo dõi sức khỏe cá nhân

Đồ án môn học **Internet of Things** – Đề tài 31: Vòng tay theo dõi sức khỏe cá nhân.
Trường Đại học Công Thương TP. Hồ Chí Minh – Khoa Công nghệ Thông tin.

**GVHD:** Đinh Huy Hoàng

**Nhóm thực hiện:**
- Đặng Nguyễn Minh Đăng – 2001230175
- Lê Quang Duy – 2001230123
- Trương Phước Thịnh – 2033230291

---

## 1. Giới thiệu

Hệ thống vòng tay IoT thu thập các chỉ số sinh tồn cơ bản (nhịp tim, SpO2, nhiệt độ cơ thể, mức độ vận động), xử lý và cảnh báo cho người dùng. Điểm khác biệt của hệ thống là **không phân loại cứng "bình thường / bất thường"**, mà:

- Học **baseline cá nhân** riêng cho từng người dùng thay vì dùng một ngưỡng chung.
- Cho phép khai báo **hồ sơ cá nhân** (bệnh nền, hạn chế vận động...) để điều chỉnh ngưỡng cảnh báo phù hợp với từng đối tượng.
- Phân loại theo **nhiều mức độ** (Bình thường – Cần chú ý – Cảnh báo – Khẩn cấp) thay vì hai nhóm nhị phân, giúp hệ thống linh hoạt hơn với nhiều tình trạng sức khỏe khác nhau.

Do giới hạn về thiết bị, dự án sử dụng **linh kiện mô phỏng (simulation)** thay vì phần cứng vật lý.

## 2. Kiến trúc hệ thống

Hệ thống gồm 3 tầng:

1. **Tầng thiết bị (Edge)** – vi điều khiển đọc cảm biến, lọc nhiễu, tính baseline cá nhân, cảnh báo rung tức thời khi khẩn cấp.
2. **Tầng truyền thông** – WiFi + MQTT là kênh chính, Bluetooth (BLE) là kênh dự phòng.
3. **Tầng Backend / App** – nhận dữ liệu, lưu trữ, xử lý, hiển thị và đẩy thông báo về ứng dụng người dùng.

```
[Cảm biến] -> [ESP32: lọc nhiễu + baseline cá nhân] -> [MQTT/BLE] -> [Backend + Database] -> [App hiển thị/cảnh báo]
```

## 3. Danh sách linh kiện (mô phỏng)

| Linh kiện | Vai trò |
|---|---|
| ESP32-WROOM-32 | Vi điều khiển trung tâm, tích hợp WiFi/BLE |
| MAX30102 | Cảm biến nhịp tim và SpO2 |
| MAX30205 | Cảm biến nhiệt độ cơ thể |
| MPU6050 | Cảm biến chuyển động (gia tốc kế + con quay hồi chuyển) |
| SSD1306 OLED | Hiển thị chỉ số tại chỗ |
| Motor rung | Cảnh báo trực tiếp trên vòng tay |
| Pin Li-ion 3.7V + TP4056 | Nguồn và mạch sạc |

Mô phỏng mạch bằng **Wokwi** / **Fritzing** — xem link project mô phỏng tại: `[chèn link Wokwi tại đây]`

## 4. Cơ sở dữ liệu

Sử dụng **Firebase Realtime Database / Firestore**, gồm các bảng:

- `Users` – hồ sơ người dùng, bệnh nền khai báo, ngưỡng cảnh báo tùy chỉnh
- `Baseline` – trung bình và độ lệch chuẩn từng chỉ số theo từng người dùng
- `SensorData` – lịch sử đo theo thời gian
- `Alerts` – lịch sử cảnh báo theo mức độ

## 5. Quy trình xử lý dữ liệu

1. Thu thập dữ liệu từ cảm biến (mỗi 2–5 giây)
2. Lọc nhiễu bằng trung bình động
3. Học baseline cá nhân trong những ngày đầu sử dụng
4. Áp dụng hồ sơ cá nhân để điều chỉnh ngưỡng cảnh báo
5. Phân loại theo 4 mức: **Bình thường → Cần chú ý → Cảnh báo → Khẩn cấp**
6. Lưu trữ dữ liệu và thực hiện hành động hỗ trợ tương ứng với từng mức

## 6. Cấu trúc thư mục (đề xuất)

```
├── firmware/           # Code cho ESP32 (đọc cảm biến, xử lý, gửi MQTT)
├── simulation/         # File mô phỏng mạch (Wokwi/Fritzing)
├── backend/            # Xử lý dữ liệu, kết nối Firebase
├── app/                # Ứng dụng/giao diện hiển thị
├── docs/               # Báo cáo, sơ đồ, hình ảnh
└── README.md
```

## 7. Phân công

| Thành viên | Phụ trách |
|---|---|
| Đặng Nguyễn Minh Đăng | Danh sách phần cứng, sơ đồ đấu nối mô phỏng |
| Lê Quang Duy | Kiến trúc hệ thống, phương thức truyền thông |
| Trương Phước Thịnh | Cơ sở dữ liệu, quy trình xử lý, mục lục báo cáo |

## 8. Hướng phát triển

- Bổ sung phân tích dữ liệu lớn, dự đoán bệnh lý bằng trí tuệ nhân tạo
- Mở rộng hỗ trợ theo dõi tập trung cho nhiều người dùng (bệnh viện, doanh nghiệp)
- Tối ưu phần cứng thực tế (kiểu dáng, thời lượng pin) cho mục đích thương mại hóa

---

## 9. Tiến độ thực hiện

Chi tiết tiến độ thực hiện mã nguồn, mô phỏng và báo cáo được cập nhật tại [CHECKLIST.md](CHECKLIST.md).

