#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Script to generate a high-resolution, publication-quality Architecture Diagram
for the 'Adaptive Vitals Monitor' project (IoT Health Wristband).
"""

import matplotlib.pyplot as plt
import matplotlib.patches as patches
from pathlib import Path

# Setup canvas: 16:9 aspect ratio, 300 DPI
fig, ax = plt.subplots(figsize=(18, 10.5), dpi=300)
ax.set_xlim(0, 100)
ax.set_ylim(0, 100)
ax.axis('off')

# Set background color
fig.patch.set_facecolor('#f8fafc')
ax.set_facecolor('#f8fafc')

# Color Palette
COLORS = {
    "edge_bg": "#f0f9ff",
    "edge_border": "#0284c7",
    "edge_header": "#0369a1",
    "comm_bg": "#faf5ff",
    "comm_border": "#7c3aed",
    "comm_header": "#6d28d9",
    "cloud_bg": "#f0fdf4",
    "cloud_border": "#059669",
    "cloud_header": "#047857",
    "app_bg": "#fffbeb",
    "app_border": "#d97706",
    "app_header": "#b45309",
    "card_bg": "#ffffff",
    "card_border": "#cbd5e1",
    "text_dark": "#0f172a",
    "text_muted": "#475569",
    "arrow": "#2563eb",
    "arrow_back": "#e11d48",
}

# Helper to draw rounded rectangle
def draw_rounded_rect(ax, x, y, w, h, r, facecolor, edgecolor, linewidth=1.5, zorder=1):
    box = patches.FancyBboxPatch(
        (x, y), w, h,
        boxstyle=f"round,pad={r}",
        facecolor=facecolor,
        edgecolor=edgecolor,
        linewidth=linewidth,
        zorder=zorder
    )
    ax.add_patch(box)
    return box

# ─── 0. MAIN TITLE ───
ax.text(50, 97.5, "KIẾN TRÚC TỔNG THỂ HỆ THỐNG ADAPTIVE VITALS MONITOR",
        fontsize=16, fontweight='bold', ha='center', va='center', color='#0f172a', fontfamily='Liberation Sans')
ax.text(50, 95.2, "Mô hình 4 tầng xử lý phân tán từ thiết bị đeo biên (Edge) đến nền tảng đám mây (Cloud) và ứng dụng Web",
        fontsize=10.5, fontstyle='italic', ha='center', va='center', color='#475569', fontfamily='Liberation Sans')

# ─── 1. LAYER 1: TẦNG THIẾT BỊ ĐEO (EDGE LAYER) ───
# Box: x=2, y=5, w=22, h=87
draw_rounded_rect(ax, 2, 5, 22, 87, 0.8, COLORS["edge_bg"], COLORS["edge_border"], linewidth=2, zorder=2)
# Header
draw_rounded_rect(ax, 2.5, 87.5, 21, 4, 0.4, COLORS["edge_header"], COLORS["edge_header"], zorder=3)
ax.text(13, 89.5, "TẦNG 1: THIẾT BỊ ĐEO (EDGE)", fontsize=11, fontweight='bold', color='#ffffff', ha='center', va='center')
ax.text(13, 85.8, "Vi điều khiển trung tâm: ESP32-WROOM-32", fontsize=9, fontweight='bold', color=COLORS["edge_header"], ha='center', va='center')

# Sub-card 1.1: Khối Cảm biến
draw_rounded_rect(ax, 3.5, 68, 19, 16.5, 0.4, COLORS["card_bg"], COLORS["card_border"], linewidth=1, zorder=3)
ax.text(4.5, 82.5, "1.1. Cảm biến sinh học & vận động", fontsize=9.5, fontweight='bold', color='#0369a1')
sensors = [
    "• MAX30102: Nhịp tim (HR) & SpO2 (I2C)",
    "• MAX30205: Thân nhiệt lâm sàng (I2C)",
    "• MPU6050: Gia tốc 3 trục (Chuyển động)",
    "• Biến trở: Giả lập nhịp tim bất thường"
]
for i, s in enumerate(sensors):
    ax.text(5, 79.5 - i * 3.2, s, fontsize=8.2, color=COLORS["text_dark"])

# Sub-card 1.2: Khối Xử lý biên (Edge Processing)
draw_rounded_rect(ax, 3.5, 33, 19, 33, 0.4, COLORS["card_bg"], COLORS["card_border"], linewidth=1, zorder=3)
ax.text(4.5, 63.8, "1.2. Xử lý dữ liệu tại biên (Edge AI)", fontsize=9.5, fontweight='bold', color='#0369a1')
processing_items = [
    "• Lọc nhiễu Moving Average:",
    "   - HR (N=10), SpO2 (N=8), Temp (N=5)",
    "• Nhận diện trạng thái vận động:",
    "   - Phân loại REST / ACTIVE (MPU6050)",
    "• Thuật toán Baseline 2 pha (EWMA):",
    "   - Warm-up α = 0.15 → Stable α = 0.01",
    "   - Tách biệt baseline REST & ACTIVE",
    "• Phân loại cảnh báo thích nghi:",
    "   - Đánh giá Z-score 4 cấp độ cảnh báo",
    "   - Tích lũy hệ số nguy cơ theo bệnh nền"
]
for i, item in enumerate(processing_items):
    is_sub = item.startswith("   -")
    ax.text(5 if not is_sub else 6.5, 60.8 - i * 2.8, item, fontsize=7.8 if is_sub else 8.2,
            color=COLORS["text_muted"] if is_sub else COLORS["text_dark"],
            fontweight='normal' if is_sub else '600')

# Sub-card 1.3: Khối Tương tác tại chỗ
draw_rounded_rect(ax, 3.5, 6.5, 19, 24.5, 0.4, COLORS["card_bg"], COLORS["card_border"], linewidth=1, zorder=3)
ax.text(4.5, 29, "1.3. Phản hồi tại chỗ & Điều khiển", fontsize=9.5, fontweight='bold', color='#0369a1')
io_items = [
    "• Màn hình OLED SSD1306 0.96\" (I2C):",
    "   - Hiển thị HR, SpO2, Temp, Baseline",
    "• Motor rung Haptic / LED cảnh báo:",
    "   - Báo rung theo 4 cấp độ tức thì",
    "• Nút bấm vật lý phục vụ demo:",
    "   - GPIO 25: Báo khẩn cấp SOS",
    "   - GPIO 26: Kích hoạt chế độ tập luyện"
]
for i, item in enumerate(io_items):
    is_sub = item.startswith("   -")
    ax.text(5 if not is_sub else 6.5, 26.2 - i * 2.7, item, fontsize=7.8 if is_sub else 8.2,
            color=COLORS["text_muted"] if is_sub else COLORS["text_dark"],
            fontweight='normal' if is_sub else '600')


# ─── 2. LAYER 2: TẦNG TRUYỀN THÔNG (COMMUNICATION LAYER) ───
# Box: x=27, y=5, w=21, h=87
draw_rounded_rect(ax, 27, 5, 21, 87, 0.8, COLORS["comm_bg"], COLORS["comm_border"], linewidth=2, zorder=2)
draw_rounded_rect(ax, 27.5, 87.5, 20, 4, 0.4, COLORS["comm_header"], COLORS["comm_header"], zorder=3)
ax.text(37.5, 89.5, "TẦNG 2: TRUYỀN THÔNG", fontsize=11, fontweight='bold', color='#ffffff', ha='center', va='center')
ax.text(37.5, 85.8, "Giao thức: MQTT / HiveMQ Cloud", fontsize=9, fontweight='bold', color=COLORS["comm_header"], ha='center', va='center')

# Sub-card 2.1: MQTT Broker
draw_rounded_rect(ax, 28.5, 53, 18, 31, 0.4, COLORS["card_bg"], COLORS["card_border"], linewidth=1, zorder=3)
ax.text(29.5, 81.5, "2.1. HiveMQ Cloud Broker (TLS)", fontsize=9.5, fontweight='bold', color='#6d28d9')
mqtt_items = [
    "• Kiến trúc Publish / Subscribe",
    "• Giao thức MQTT v3.1.1 (Cổng 8883)",
    "• Đảm bảo gói tin QoS 1 (At least once)",
    "• Topic Hierarchy chuẩn hóa:",
    "   - wristband/{userId}/data",
    "     (Telemetry định kỳ mỗi 1-3s)",
    "   - wristband/{userId}/alerts",
    "     (Cảnh báo bất thường thời gian thực)",
    "   - wristband/{userId}/commands",
    "     (Nhận lệnh điều khiển / cấu hình)"
]
for i, item in enumerate(mqtt_items):
    is_sub = item.startswith("   -")
    ax.text(30 if not is_sub else 31.5, 78.5 - i * 2.8, item, fontsize=7.8 if is_sub else 8.2,
            color=COLORS["text_muted"] if is_sub else COLORS["text_dark"],
            fontweight='normal' if is_sub else '600')

# Sub-card 2.2: Kênh truyền thông vật lý
draw_rounded_rect(ax, 28.5, 28, 18, 23, 0.4, COLORS["card_bg"], COLORS["card_border"], linewidth=1, zorder=3)
ax.text(29.5, 48.5, "2.2. Kênh truyền vật lý", fontsize=9.5, fontweight='bold', color='#6d28d9')
phy_items = [
    "• Kênh chính: Wi-Fi 802.11 b/g/n",
    "   - Băng tần 2.4 GHz kết nối trực tiếp",
    "   - Auto-reconnect khi gián đoạn mạng",
    "• Kênh dự phòng: Bluetooth LE (BLE)",
    "   - Dùng khi mất tín hiệu Wi-Fi",
    "   - Ghép nối trực tiếp thiết bị di động",
    "• Định dạng gói tin: JSON nén gọn"
]
for i, item in enumerate(phy_items):
    is_sub = item.startswith("   -")
    ax.text(30 if not is_sub else 31.5, 45.5 - i * 2.7, item, fontsize=7.8 if is_sub else 8.2,
            color=COLORS["text_muted"] if is_sub else COLORS["text_dark"],
            fontweight='normal' if is_sub else '600')

# Sub-card 2.3: Đặc tả Payload
draw_rounded_rect(ax, 28.5, 6.5, 18, 19.5, 0.4, COLORS["card_bg"], COLORS["card_border"], linewidth=1, zorder=3)
ax.text(29.5, 23.8, "2.3. Cấu trúc Gói tin JSON", fontsize=9.5, fontweight='bold', color='#6d28d9')
json_items = [
    "• user_id, timestamp",
    "• metrics: { hr, spo2, temp }",
    "• activity: \"REST\" | \"ACTIVE\"",
    "• alert_level: NORMAL .. CRITICAL",
    "• baseline: { mean, std, phase }"
]
for i, item in enumerate(json_items):
    ax.text(30.5, 21 - i * 2.7, item, fontsize=8.0, color="#1e293b", fontfamily='DejaVu Sans Mono')


# ─── 3. LAYER 3: TẦNG CLOUD & BACKEND ───
# Box: x=51, y=5, w=22, h=87
draw_rounded_rect(ax, 51, 5, 22, 87, 0.8, COLORS["cloud_bg"], COLORS["cloud_border"], linewidth=2, zorder=2)
draw_rounded_rect(ax, 51.5, 87.5, 21, 4, 0.4, COLORS["cloud_header"], COLORS["cloud_header"], zorder=3)
ax.text(62, 89.5, "TẦNG 3: CLOUD & BACKEND", fontsize=11, fontweight='bold', color='#ffffff', ha='center', va='center')
ax.text(62, 85.8, "Node.js Server & Firebase Realtime DB", fontsize=9, fontweight='bold', color=COLORS["cloud_header"], ha='center', va='center')

# Sub-card 3.1: Backend Server
draw_rounded_rect(ax, 52.5, 49, 19, 35, 0.4, COLORS["card_bg"], COLORS["card_border"], linewidth=1, zorder=3)
ax.text(53.5, 81.5, "3.1. Node.js Backend Server", fontsize=9.5, fontweight='bold', color='#047857')
be_items = [
    "• MQTT Subscriber Service:",
    "   - Lắng nghe /data và /alerts",
    "• Data Pipeline & Validation:",
    "   - Kiểm tra tính hợp lệ dữ liệu",
    "   - Ghi chuỗi thời gian SensorData",
    "• Dịch vụ cảnh báo (Alert Service):",
    "   - Lọc trùng cảnh báo (de-bouncing)",
    "   - Ghi nhận nhật ký cảnh báo khẩn",
    "• Bộ đồng bộ Baseline dài hạn:",
    "   - Tổng hợp chỉ số tuần/tháng",
    "   - Hiệu chỉnh profile cá nhân"
]
for i, item in enumerate(be_items):
    is_sub = item.startswith("   -")
    ax.text(54 if not is_sub else 55.5, 78.5 - i * 2.8, item, fontsize=7.8 if is_sub else 8.2,
            color=COLORS["text_muted"] if is_sub else COLORS["text_dark"],
            fontweight='normal' if is_sub else '600')

# Sub-card 3.2: Cơ sở dữ liệu Firebase
draw_rounded_rect(ax, 52.5, 6.5, 19, 40.5, 0.4, COLORS["card_bg"], COLORS["card_border"], linewidth=1, zorder=3)
ax.text(53.5, 44.5, "3.2. Firebase Realtime Database", fontsize=9.5, fontweight='bold', color='#047857')
db_items = [
    "• /users/{userId}:",
    "   - Họ tên, tuổi, tiền sử bệnh nền",
    "   - Ngưỡng cảnh báo tùy chỉnh",
    "• /baseline/{userId}:",
    "   - Giá trị trung bình & độ lệch chuẩn",
    "   - Baseline phân tách REST / ACTIVE",
    "• /sensor_data/{userId}/{pushId}:",
    "   - Lịch sử đo nhịp tim, SpO2, thân nhiệt",
    "   - Nhãn vận động & thời gian đo",
    "• /alerts/{userId}/{pushId}:",
    "   - Lịch sử vi phạm ngưỡng bất thường",
    "   - Mức độ cảnh báo & nguyên nhân"
]
for i, item in enumerate(db_items):
    is_sub = item.startswith("   -")
    ax.text(54 if not is_sub else 55.5, 41.5 - i * 2.7, item, fontsize=7.8 if is_sub else 8.2,
            color=COLORS["text_muted"] if is_sub else COLORS["text_dark"],
            fontweight='normal' if is_sub else '600')


# ─── 4. LAYER 4: TẦNG ỨNG DỤNG (APPLICATION LAYER) ───
# Box: x=76, y=5, w=22, h=87
draw_rounded_rect(ax, 76, 5, 22, 87, 0.8, COLORS["app_bg"], COLORS["app_border"], linewidth=2, zorder=2)
draw_rounded_rect(ax, 76.5, 87.5, 21, 4, 0.4, COLORS["app_header"], COLORS["app_header"], zorder=3)
ax.text(87, 89.5, "TẦNG 4: ỨNG DỤNG (CLIENT)", fontsize=11, fontweight='bold', color='#ffffff', ha='center', va='center')
ax.text(87, 85.8, "Giao diện Web Dashboard thời gian thực", fontsize=9, fontweight='bold', color=COLORS["app_header"], ha='center', va='center')

# Sub-card 4.1: Web Dashboard Realtime
draw_rounded_rect(ax, 77.5, 49, 19, 35, 0.4, COLORS["card_bg"], COLORS["card_border"], linewidth=1, zorder=3)
ax.text(78.5, 81.5, "4.1. Dashboard Giám sát (index.html)", fontsize=9.5, fontweight='bold', color='#b45309')
dash_items = [
    "• Công nghệ: HTML5, CSS3, JavaScript",
    "• Biểu đồ thời gian thực (Chart.js):",
    "   - Xu hướng Nhịp tim (HR - bpm)",
    "   - Nồng độ oxy trong máu (SpO2 - %)",
    "   - Thân nhiệt cơ thể (Temp - °C)",
    "• Thẻ tình trạng sức khỏe tổng thể:",
    "   - 4 màu sắc trực quan theo cấp độ",
    "   - Hiển thị nhãn Nghỉ ngơi / Vận động",
    "• Hộp cảnh báo Banner & Còi báo khẩn"
]
for i, item in enumerate(dash_items):
    is_sub = item.startswith("   -")
    ax.text(79 if not is_sub else 80.5, 78.5 - i * 2.8, item, fontsize=7.8 if is_sub else 8.2,
            color=COLORS["text_muted"] if is_sub else COLORS["text_dark"],
            fontweight='normal' if is_sub else '600')

# Sub-card 4.2: Quản lý Hồ sơ & Bệnh nền
draw_rounded_rect(ax, 77.5, 6.5, 19, 40.5, 0.4, COLORS["card_bg"], COLORS["card_border"], linewidth=1, zorder=3)
ax.text(78.5, 44.5, "4.2. Hồ sơ cá nhân (profile.html)", fontsize=9.5, fontweight='bold', color='#b45309')
profile_items = [
    "• Khai báo thông tin cá nhân:",
    "   - Độ tuổi, giới tính, chỉ số cơ bản",
    "• Đánh dấu bệnh nền lâm sàng:",
    "   - Cao huyết áp (hệ số 0.90)",
    "   - Hen suyễn (hệ số SpO2 0.95)",
    "   - Tiểu đường (hệ số 0.95)",
    "   - Bệnh tim mạch (hệ số 0.85)",
    "   - Người cao tuổi, hạn chế vận động",
    "• Cơ chế tự động cập nhật:",
    "   - Ghi Firebase → Đồng bộ xuống ESP32",
    "   - Tự động hiệu chỉnh ngưỡng cảnh báo"
]
for i, item in enumerate(profile_items):
    is_sub = item.startswith("   -")
    ax.text(79 if not is_sub else 80.5, 41.5 - i * 2.7, item, fontsize=7.8 if is_sub else 8.2,
            color=COLORS["text_muted"] if is_sub else COLORS["text_dark"],
            fontweight='normal' if is_sub else '600')


# ─── 5. DATA FLOW ARROWS & LABELS ───

# Arrow 1: Edge -> Communication (Publish Telemetry & Alerts)
ax.annotate('', xy=(27, 68), xytext=(24, 68),
            arrowprops=dict(facecolor=COLORS["arrow"], edgecolor=COLORS["arrow"],
                            width=3, headwidth=8, headlength=7, zorder=5))
ax.text(25.5, 70.5, "MQTT Publish\n(/data, /alerts)", fontsize=7.5, fontweight='bold',
        color=COLORS["arrow"], ha='center', va='bottom', bbox=dict(boxstyle="round,pad=0.2", fc="#eff6ff", ec="#bfdbfe", lw=0.8, zorder=6))

# Arrow 1_rev: Communication -> Edge (Subscribe Commands / Profile)
ax.annotate('', xy=(24, 16), xytext=(27, 16),
            arrowprops=dict(facecolor=COLORS["arrow_back"], edgecolor=COLORS["arrow_back"],
                            width=2.5, headwidth=7, headlength=6, zorder=5))
ax.text(25.5, 18.5, "MQTT Sub\n(/commands)", fontsize=7.5, fontweight='bold',
        color=COLORS["arrow_back"], ha='center', va='bottom', bbox=dict(boxstyle="round,pad=0.2", fc="#fff1f2", ec="#fecdd3", lw=0.8, zorder=6))

# Arrow 2: Communication -> Backend (Subscribe)
ax.annotate('', xy=(51, 68), xytext=(48, 68),
            arrowprops=dict(facecolor=COLORS["arrow"], edgecolor=COLORS["arrow"],
                            width=3, headwidth=8, headlength=7, zorder=5))
ax.text(49.5, 70.5, "Subscribe\n(Worker)", fontsize=7.5, fontweight='bold',
        color=COLORS["arrow"], ha='center', va='bottom', bbox=dict(boxstyle="round,pad=0.2", fc="#eff6ff", ec="#bfdbfe", lw=0.8, zorder=6))

# Arrow 2_rev: Backend -> Communication (Publish Command / Sync)
ax.annotate('', xy=(48, 16), xytext=(51, 16),
            arrowprops=dict(facecolor=COLORS["arrow_back"], edgecolor=COLORS["arrow_back"],
                            width=2.5, headwidth=7, headlength=6, zorder=5))
ax.text(49.5, 18.5, "Publish\nCommands", fontsize=7.5, fontweight='bold',
        color=COLORS["arrow_back"], ha='center', va='bottom', bbox=dict(boxstyle="round,pad=0.2", fc="#fff1f2", ec="#fecdd3", lw=0.8, zorder=6))

# Arrow 3: Cloud -> App (Realtime Sync 2-way)
ax.annotate('', xy=(76, 68), xytext=(73, 68),
            arrowprops=dict(facecolor=COLORS["arrow"], edgecolor=COLORS["arrow"],
                            width=3, headwidth=8, headlength=7, zorder=5))
ax.text(74.5, 70.5, "Realtime\nData Sync", fontsize=7.5, fontweight='bold',
        color=COLORS["arrow"], ha='center', va='bottom', bbox=dict(boxstyle="round,pad=0.2", fc="#eff6ff", ec="#bfdbfe", lw=0.8, zorder=6))

# Arrow 3_rev: App -> Cloud (Profile update)
ax.annotate('', xy=(73, 20), xytext=(76, 20),
            arrowprops=dict(facecolor=COLORS["arrow_back"], edgecolor=COLORS["arrow_back"],
                            width=2.5, headwidth=7, headlength=6, zorder=5))
ax.text(74.5, 22.5, "Cập nhật\nHồ sơ bệnh", fontsize=7.5, fontweight='bold',
        color=COLORS["arrow_back"], ha='center', va='bottom', bbox=dict(boxstyle="round,pad=0.2", fc="#fff1f2", ec="#fecdd3", lw=0.8, zorder=6))

# Save image
out_path = Path("/home/duyle/coding/IOT/iot-health-wristband/architecture_diagram.png")
fig.savefig(out_path, dpi=300, bbox_inches='tight', facecolor=fig.get_facecolor(), edgecolor='none')
plt.close(fig)

print(f"Successfully generated: {out_path} ({out_path.stat().st_size} bytes)")
