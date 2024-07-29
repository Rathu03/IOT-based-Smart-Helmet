#include <SPI.h>
#include <RH_RF95.h>

#define RFM95_CS 10
#define RFM95_RST 9
#define RFM95_INT 2

#define RF95_FREQ 915.0

RH_RF95 rf95(RFM95_CS, RFM95_INT);

void setup() 
{
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  Serial.begin(9600);
  while (!Serial);

  //Serial.println("Arduino LoRa RX Test!");

  // Manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  if (!rf95.init()) 
  {
    //Serial.println("LoRa radio init failed");
    while (1);
  }
  //Serial.println("LoRa radio init OK!");

  if (!rf95.setFrequency(RF95_FREQ)) 
  {
    //Serial.println("setFrequency failed");
    while (1);
  }
  //Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);

  rf95.setTxPower(13, false);
}

void loop() 
{
  if (rf95.available()) 
  {
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);

    if (rf95.recv(buf, &len)) 
    {
      buf[len] = '\0';  // Null-terminate the received data

      // Parse and print the received values
      int distance, mq4Value, mq7Value;
      int temperature, humidity;
      sscanf((char*)buf, "D:%d,MQ4:%d,MQ7:%d,T:%d,H:%d", &distance, &mq4Value, &mq7Value, &temperature, &humidity);

      Serial.println(String(distance) + "," + String(mq4Value) + "," + String(mq7Value) + "," + String(temperature) + "," + String(humidity) + ",");
      
    } 
    else 
    {
      Serial.println("Receive failed");
      Serial.println();
    }
  }
}
