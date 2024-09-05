# Release Note

## **v1.0.0**
### 1. Features
Initial release.

#### 1.1. Firmware
- Estimate the heart rate using the TERMA framework.
- Manage button click and hold events.
- Display heart rate, graph, thresholds, and notifications on SSD1306.
- RTC, UART, and buzzer sound effects work out-of-the-box.

#### 1.2. Application
- Support serial port connection: automatically detect newly connected COM Ports in real-time, detect if the COM Port currently selected in the GUI is being used by another software, and detect if the COM Port currently connected is suddenly physically disconnected.
- Plot heart rate data and clear the graph.
- Stream PPG signals.
- Get/clear heart rate records.
- Set the interval for collecting heart rate data from the device.
- Set thresholds: low/high heart rate levels.
- Set the time for the RTC in three modes: 12-hour mode, 24-hour mode, and epoch time mode.
- Notify errors from the firmware and alert when the heart rate is too high or too low based on the threshold.

### 2. Issues
#### 2.1. Firmware
- Peak detection algorithm is unstable and has low accuracy.
- OLED streaming is slow.
- The button does not work smoothly.

#### 2.2. Application
- PPG signals are slowly streamed.

### 3. In Progress
#### 3.1. Firmware
- Improve the peak detection algorithm.
- Handle the SSD1306 OLED display speed.
- Improved button event detection.

#### 3.2. Application
- Improve the stream speed of PPG signals.

## Contributors
- Giang Phan Truong
- Khanh Nguyen Ngoc
- Viet Hoang Xuan