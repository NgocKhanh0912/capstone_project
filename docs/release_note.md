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
- Improve button event detection.

#### 3.2. Application
- Improve the stream speed of PPG signals.

### 4. Contributors
- Giang Phan Truong
- Khanh Nguyen Ngoc
- Viet Hoang Xuan

## **v1.0.1**

### 1. Features

#### 1.1. Firmware
- Estimate the heart rate using the TERMA framework.
- Manage button click and hold events.
- Display heart rate, graph, thresholds, and notifications on SSD1306.
- RTC, UART, and buzzer sound effects work out-of-the-box.
- The button works smoothly.

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

#### 2.2. Application
- PPG signals are slowly streamed.

### 3. In Progress

#### 3.1. Firmware
- Improve the peak detection algorithm.
- Handle the SSD1306 OLED display speed.

#### 3.2. Application
- Improve the stream speed of PPG signals.

### 4. Contributors
- Khanh Nguyen Ngoc

## **v1.0.2**

### 1. Features

#### 1.1. Firmware
- Peak detection algorithm is stable and has high accuracy.
- Estimate the heart rate using the TERMA framework.
- Manage button click and hold events.
- Display heart rate, graph, thresholds, and notifications on SSD1306.
- RTC, UART, and buzzer sound effects work out-of-the-box.
- The button works smoothly.
- OLED streaming is good.

#### 1.2. Application
- Support serial port connection: automatically detect newly connected COM Ports in real-time, detect if the COM Port currently selected in the GUI is being used by another software, and detect if the COM Port currently connected is suddenly physically disconnected.
- Plot heart rate data and clear the graph.
- Fast streaming speed of PPG signals.
- Get/clear heart rate records.
- Set the interval for collecting heart rate data from the device.
- Set thresholds: low/high heart rate levels.
- Set the time for the RTC in three modes: 12-hour mode, 24-hour mode, and epoch time mode.
- Notify errors from the firmware and alert when the heart rate is too high or too low based on the threshold.

#### 1.3. Data analyze
- The peak detection algorithm has been validated with multiple databases (mimic normal and afib dataset, real-world dataset), achieving nearly 100% accuracy in detecting the peaks of PPG waves.
- The heart rate measurement results from the device I developed compared to the standard device show a mean error of 1% to 2%, with a maximum error of 4.225% in a single measurement.

### 2. Issues

#### 2.1. Firmware
- The peak detection algorithm has not yet eliminated spurious heart rate values when there is strong interference from finger movement, as the contact point between the sensor and the finger is unstable.

#### 2.2. Application
- Need a button to clear graph of streaming PPG signals.

### 3. In Progress

#### 3.1. Firmware
- Improve the peak detection algorithm.

#### 3.2. Application
- Add new feature to clear graph of streaming PPG signals.

### 4. Contributors
- Khanh Nguyen Ngoc

## **v1.1.0**

### 1. Features
- Complete Project 2.

#### 1.1. Firmware
- Peak detection algorithm is stable and has high accuracy.
- Estimate the heart rate using the TERMA framework.
- Manage button click and hold events.
- Display heart rate, graph, thresholds, and notifications on SSD1306.
- RTC, UART, and buzzer sound effects work out-of-the-box.
- The button works smoothly.
- OLED streaming is good.
- The peak detection algorithm has eliminated spurious heart rate values when there is strong interference from finger movement, as the contact point between the sensor and the finger is unstable by using FFT.

#### 1.2. Application
- Support serial port connection: automatically detect newly connected COM Ports in real-time, detect if the COM Port currently selected in the GUI is being used by another software, and detect if the COM Port currently connected is suddenly physically disconnected.
- Plot heart rate data and clear the graph.
- Fast streaming speed of PPG signals.
- Get/clear heart rate records.
- Set the interval for collecting heart rate data from the device.
- Set thresholds: low/high heart rate levels.
- Set the time for the RTC in three modes: 12-hour mode, 24-hour mode, and epoch time mode.
- Notify errors from the firmware and alert when the heart rate is too high or too low based on the threshold.

#### 1.3. Data analyze
- The peak detection algorithm has been validated with multiple databases (mimic normal and afib dataset, real-world dataset), achieving nearly 100% accuracy in detecting the peaks of PPG waves.
- The heart rate measurement results from the device I developed compared to the standard device show a mean error of 1% to 2%, with a maximum error of 4.225% in a single measurement.

### 2. Issues

#### 2.1. Firmware
- Improve the FFT heart rate result.

#### 2.2. Application
- Need a button to clear graph of streaming PPG signals.

### 3. In Progress

#### 3.1. Firmware
- Improve the FFT heart rate result.
- Start the Atrial Fibrillation detection.

#### 3.2. Application
- Add new feature to clear graph of streaming PPG signals.

### 4. Contributors
- Khanh Nguyen Ngoc
