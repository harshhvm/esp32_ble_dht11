 #include <BLEDevice.h>
 #include <BLEServer.h>
 #include <BLEUtils.h>
 #include <BLE2902.h>

 #include <DHT.h>

 #include <iostream>
 #include <string>

 BLECharacteristic * pCharacteristic;

 bool deviceConnected = false;
 const int LED = 2;  // Could be different depending on the dev board.I used the DO32 ESP32

 
 //Definition of DHT11
 
 #define DHTPIN 23 // DHT11 data pin
 #define DHTTYPE DHT11 // sets the sensor type, in case DHT11

 DHT dht (DHTPIN, DHTTYPE);

 int humidity;
 int temperature;

 // Visit following link if you want to generate your own UUIDs:
 // https://www.uuidgenerator.net/

 #define SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
 #define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"  //for write
 #define DHTDATA_CHAR_UUID "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"   // for notify


 class MyServerCallbacks: public BLEServerCallbacks {
     void onConnect (BLEServer * pServer) {
       deviceConnected = true;
     };

     void onDisconnect (BLEServer * pServer) {
       deviceConnected = false;
     }
 };

 class MyCallbacks: public BLECharacteristicCallbacks {
     void onWrite (BLECharacteristic * pCharacteristic) {
       std :: string rxValue = pCharacteristic-> getValue ();
       Serial.println (rxValue [0]);

       if (rxValue.length ()> 0) {
         Serial.println ("*********");
         Serial.print ("Received Value:");

         for (int i = 0; i <rxValue.length (); i ++) {
           Serial.print (rxValue [i]);
         }
         Serial.println ();
         Serial.println ("*********");
       }

       // Do stuff based on the command received from the app
        if (rxValue.find("ON") != -1) {
          Serial.println("Turning ON!");
          digitalWrite(LED, HIGH);
        }
        else if (rxValue.find("OFF") != -1) {
          Serial.println("Turning OFF!");
          digitalWrite(LED, LOW);
        }       
       }
 };

 void setup () {
   Serial.begin (115200);

   pinMode (LED, OUTPUT);

   // Create the BLE Device
   BLEDevice :: init ("ESP32 DHT11");  // Device Name

   // Set the device as BLE Server
   BLEServer * pServer = BLEDevice :: createServer ();
   pServer-> setCallbacks (new MyServerCallbacks ());

   // Create the UART service
   BLEService * pService = pServer-> createService (SERVICE_UUID);

   // Create a BLE Feature to send the data
   pCharacteristic = pService-> createCharacteristic (
                       DHTDATA_CHAR_UUID,
                       BLECharacteristic :: PROPERTY_NOTIFY
                     );
                      
   pCharacteristic-> addDescriptor (new BLE2902 ());

   // creates a BLE characteristic to receive the data
   BLECharacteristic * pCharacteristic = pService-> createCharacteristic (
                                          CHARACTERISTIC_UUID_RX,
                                          BLECharacteristic :: PROPERTY_WRITE
                                        );

   pCharacteristic-> setCallbacks (new MyCallbacks ());

   // Start the service
   pService-> start ();

   // Starts the discovery of ESP32
   pServer-> getAdvertising () -> start ();
   Serial.println ("Waiting for a client to connect ...");
 }

 void loop () {
   if (deviceConnected) {

     humidity = dht.readHumidity ();
     temperature = dht.readTemperature ();
     // test if return is valid, otherwise something is wrong.
     if (isnan (temperature) || isnan (humidity))
     {
       Serial.println ("Failed to read from DHT");
     }
     else
     {
       Serial.print ("Humidity:");
       Serial.print (humidity);
       Serial.print ("% \ t");
       Serial.print ("Temperature:");
       Serial.print (temperature);
       Serial.println ("* C");
     }
    
     char humidityString [2];
     char temperatureString [2];
     dtostrf (humidity, 1, 2, humidityString);
     dtostrf (temperature, 1, 2, temperatureString);

     char dhtDataString [16];
     sprintf (dhtDataString, "% d,% d", temperature, humidity);
    
     pCharacteristic-> setValue (dhtDataString);
    
     pCharacteristic-> notify ();  // Send the value to the app!
     Serial.print ("*** Sent Data:");
     Serial.print (dhtDataString);
     Serial.println ("***");
   }
   delay (1000);
 }
