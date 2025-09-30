# thetemperatureproject

Temperature sensor data collection

Register board with UI_DeviceNet

## Files

### arduino_code

`blink.ino` Used to test the Arduino board and the upload / download process. This sketch blinks the red light on the board.  
`get_mac_addr.ino` Used to get the mac address from the board.  
`test_wifi_connect.ino` Used to ensure the board can connect to the wifi using WPA2 or WPA3.  
`i2c_scanner.ino` Used to get the hardware address of the STEMMA-QT attachments.

\*\* `bme280_api_post.ino` Used to read temperature, humidity, and barometric pressure and send to API server. Also, displays readings on OLED screen attached to device.
