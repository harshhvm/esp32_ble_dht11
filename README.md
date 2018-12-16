***********************************************************************************************************
          Use DHT 11 with BLE and ESP32  
***********************************************************************************************************
The program basically sets the UUID of the UART communication service, it reads the humidity and temperature of the DHT sensor, and transmits this data to the application on the mobile phone. Data is sent in a single variable, but in a CSV type, with temperature and humidity separated by a comma.
folow these steps:
1. First you need to INSTALL all Required Libraries in Arduino IDE.
2. Burn the ".ino" file to your ESP32.
3. Now install BLE scanner app or Similar app in your Mobile phone.
4. Now you can see sensor value in app by Notify UUID.
