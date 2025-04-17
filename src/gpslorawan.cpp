// #define BLYNK_TEMPLATE_ID "TMPL5s8VMQWfW"
// #define BLYNK_TEMPLATE_NAME "Lorawan"
// #define BLYNK_AUTH_TOKEN "Two9XaVXTERBXc_ZCLBJeusyBkujCUFn"
// #include <BlynkSimpleEsp32.h> // Include the Blynk library

#include <TinyGPS++.h>
#include <HardwareSerial.h>
#include <rn2xx3.h>         // Include the RN2xx3 library for LoRa communication
#include <Arduino.h>        // Include the Arduino core library

#define RESET 5  // Alias for the reset pin
// GPS Configuration
HardwareSerial gpsSerial(1); // UART1 (RX=GPIO22, TX=GPIO23)
TinyGPSPlus gps;

// LoRa Configuration
HardwareSerial loraSerial(2); // UART2 (RX=GPIO16, TX=GPIO17)
// Create an instance of the rn2xx3 library, passing the HardwareSerial instance for communication
rn2xx3 myLora(loraSerial);
// LoRaWAN Settings
const char *appEUI = "BE7A000000001465"; // Replace with your App EUI
const char *appKey = "930DA0D290A16E07BA21D3A45F221A35"; // Replace with your App Key

void initialize_radio() {
  // Reset the RN2xx3 module
  pinMode(RESET, OUTPUT);  // Set the reset pin as an output
  digitalWrite(RESET, LOW);  // Pull the reset pin low to reset the module
  delay(100);  // Wait for 100ms
  digitalWrite(RESET, HIGH);  // Pull the reset pin high to release the module

  delay(100);  // Wait for the RN2xx3's startup message
  loraSerial.flush();  // Clear the serial buffer

  // Check communication with the RN2xx3 module by reading its hardware EUI
  String hweui = myLora.hweui();  // Get the hardware EUI of the RN2xx3 module
  while (hweui.length() != 16) {  // If the EUI is not 16 characters long, communication has failed
    Serial.println("Communication with RN2xx3 unsuccessful. Power cycle the board.");
    Serial.println(hweui);  // Print the invalid EUI
    delay(10000);  // Wait for 10 seconds before retrying
    hweui = myLora.hweui();  // Attempt to read the EUI again
  }

  // Print the hardware EUI for registration on the TTN dashboard
  Serial.println("When using OTAA, register this DevEUI: ");
  Serial.println(hweui);

  // Print the firmware version of the RN2xx3 module
  Serial.println("RN2xx3 firmware version:");
  Serial.println(myLora.sysver());

  // Configure the device keys and join the TTN network
  Serial.println("Trying to join TTN");
  bool join_result = false;  // Variable to store the result of the join operation

  // Use ABP (Activation by Personalization) to join the network
  // Replace the address, AppSKey, and NwkSKey with your registered values
  // join_result = myLora.initABP("02017201", "8D7FFEF938589D95AAD928C2E2E7E48F", "AE17E567AECC8787F749A62F5541D522");

// 11:46:40.685 -> When using OTAA, register this DevEUI: 
// 11:46:40.685 -> 0004A30B010D0626
// jointEUI: BE7A000000001465
// AppKey Application Key (Device Key) 930DA0D290A16E07BA21D3A45F221A35

  // Alternatively, use OTAA (Over-The-Air Activation) to join the network
  // Uncomment the following line and replace the AppEUI and AppKey with your registered values
  join_result = myLora.initOTAA("BE7A000000001465", "930DA0D290A16E07BA21D3A45F221A35");
  // join_result = myLora.initOTAA("BE7A000000001465", "NNSXS.GZCYWTZ3D6YDU7NZCPNG7FE5EMLQYOHEGJQ7GUI.5ODOTSLVLTGEKLIXBNFGOLQG5V3K27CZ67L3SHMTMD57Q6GMK5XA");
  

  // Retry joining the network if the initial attempt fails
  while (!join_result) {
    Serial.println("Unable to join. Are your keys correct, and do you have TTN coverage?");
    Serial.println(join_result);
    delay(60000);  // Wait for 1 minute before retrying
    join_result = myLora.init();  // Attempt to join the network again
  }

  // Print a success message once the device has joined the network
  Serial.println("Successfully joined TTN");
  Serial.println(join_result);
}
// Function to turn on the LED
void led_on() {
  digitalWrite(2, 1);  // Set GPIO2 high to turn on the LED
}

// Function to turn off the LED
void led_off() {
  digitalWrite(2, 0);  // Set GPIO2 low to turn off the LED
}

void setup() {
  // Initialize Serial for debugging
  Serial.begin(115200);
  Serial.println("Initializing...");

  // Initialize GPS Serial
  gpsSerial.begin(9600, SERIAL_8N1, 22, 23); // RX=D22 (GPIO22), TX=D23 (GPIO23)
  Serial.println("Waiting for GPS data...");
 
   // LED pin is GPIO2, which is the ESP8266's built-in LED
   pinMode(2, OUTPUT);  // Set GPIO2 as an output
   led_on(); 
  // Initialize LoRa Serial
  loraSerial.setRxBufferSize(1024);  // Set the RX buffer size for the secondary serial port
  loraSerial.begin(57600, SERIAL_8N1, 16, 17);
  loraSerial.setTimeout(1000);  // Set a timeout of 1 second for serial communication

  delay(1000);  // Wait for the Arduino IDE's serial console to open

  Serial.println("Startup");  // Print a startup message to the Serial Monitor

  initialize_radio();  // Initialize the RN2xx3 module and join the TTN network

  // Transmit a startup message to the TTN network
  myLora.tx("TTN Mapper on ESP32 node");

  led_off();  // Turn off the LED
  delay(2000);  // Wait for 2 seconds
}

void loop() {
  static unsigned long lastGpsCheck = 0; // Timestamp for GPS data processing
  static unsigned long lastPayloadSent = 0; // Timestamp for payload transmission
  static bool locationUpdated = false; // Flag to track if location was updated
  const unsigned long gpsCheckInterval = 10000; // 20 seconds interval for GPS processing
  const unsigned long payloadIntervalUpdated = 10000; // 30 seconds interval if location is updated
  const unsigned long payloadIntervalNotUpdated = 10000; // 60 seconds interval if location is not updated

  unsigned long currentMillis = millis();

  // Process GPS data every 20 seconds
  if (currentMillis - lastGpsCheck >= gpsCheckInterval) {
    lastGpsCheck = currentMillis;

    while (gpsSerial.available() > 0) {
      gps.encode(gpsSerial.read());
    }

    // Check if the location has been updated
    if (gps.location.isUpdated()) {
      locationUpdated = true;
    } else {
      locationUpdated = false;
    }
  }

  // Send payload based on location update status
  unsigned long payloadInterval = locationUpdated ? payloadIntervalUpdated : payloadIntervalNotUpdated;
  if (currentMillis - lastPayloadSent >= payloadInterval) {
    lastPayloadSent = currentMillis;

    // Prepare and send payload
    if (gps.location.isValid()) {
      double latitude = gps.location.lat();
      double longitude = gps.location.lng();
      int year = gps.date.year();
      int month = gps.date.month();
      int day = gps.date.day();
      int hour = gps.time.hour();
      int minute = gps.time.minute();
      int second = gps.time.second();

      // Format timestamp
      char timestamp[25];
      snprintf(timestamp, sizeof(timestamp),
               "%04d-%02d-%02d %02d:%02d:%02d UTC",
               year, month, day, hour, minute, second);

      // Print GPS data to Serial
      Serial.println("-------------------");
      Serial.printf("Timestamp: %s\n", timestamp);
      Serial.printf("Latitude: %.6f\n", latitude);
      Serial.printf("Longitude: %.6f\n", longitude);

      // Send GPS data via LoRa
      char payload[64];
      snprintf(payload, sizeof(payload), "%.6f, %.6f, %s", latitude, longitude, timestamp);
      myLora.tx(payload); // Send the payload via LoRa
      Serial.printf("Sent to LoRa: %s\n", payload);
    } else {
      // Send a default payload if location is not valid
      Serial.println("Location not valid. Sending default payload.");
      myLora.tx("No valid GPS data");
    }
  }
  delay(payloadIntervalUpdated);
}

