# thetemperatureproject

Temperature sensor data collection

Register board with UI_DeviceNet

## Hardware

## Arduino_code

`blink.ino` Used to test the Arduino board and the upload / download process. This sketch blinks the red light on the board.

`get_mac_addr.ino` Used to get the mac address from the board.

`test_wifi_connect.ino` Used to ensure the board can connect to the wifi using WPA2 or WPA3.

`i2c_scanner.ino` Used to get the hardware address of the STEMMA-QT attachments.

`bme280_reader_autodetect.ino` Used to see temperature, humidity, and pressure output in the serial monitor.

`bme280_reader_autodetect_oled_display.ino` Used to see temperature, humidity, and pressure output on the OLED display.

`bme280_reader_autodetect_oled_display_with_icons.ino` and `bme280_reader_oled_with_time.ino` are incremental to get the display looking like I wanted.

`bme280_api_post.ino` Used to read temperature, humidity, and barometric pressure and send to AWS API server. Also, displays readings on OLED screen attached to device.

\*\* `final.ino`\*\* Used to read temperature, humidity, and barometric pressure and send to [Azure API server](https://github.com/mikecolbert/thetemperatureproject-api). Also, displays readings on OLED screen attached to device.
