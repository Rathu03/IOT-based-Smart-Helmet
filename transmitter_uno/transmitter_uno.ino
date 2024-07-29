#include <SPI.h>
#include <RH_RF95.h>
#include <DHT.h>

#define RFM95_CS 10
#define RFM95_RST 9
#define RFM95_INT 2

#define RF95_FREQ 915.0

#define DHTPIN 8     // Pin which is connected to the DHT sensor.
#define DHTTYPE DHT11   // DHT 11 or DHT22 (choose according to your sensor)

RH_RF95 rf95(RFM95_CS, RFM95_INT);

const int trigPin = 7;
const int echoPin = 6;
const int buzPin = 4;
const int mq4Pin = A0;
const int mq7Pin = A1;

long duration;
int distance;
int mq4Value;
int mq7Value;
int temperature;
int humidity;

DHT dht(DHTPIN, DHTTYPE);

void setup() 
{
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(mq4Pin, INPUT);
  pinMode(mq7Pin, INPUT);
  pinMode(buzPin, OUTPUT);

  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  Serial.begin(9600);
  while (!Serial);

  dht.begin();
  Serial.println("Arduino LoRa TX Test!");

  // Manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  if (!rf95.init()) 
  {
    Serial.println("LoRa radio init failed");
    while (1);
  }
  Serial.println("LoRa radio init OK!");

  if (!rf95.setFrequency(RF95_FREQ)) 
  {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);

  rf95.setTxPower(13, false);
}

void warning(){
  digitalWrite(buzPin, HIGH);
  delay(500);
  digitalWrite(buzPin, LOW);  
}

void loop() 
{
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);

  // Calculating the distance
  distance = duration * 0.034 / 2;

  // Read MQ-4 and MQ-7 sensor values
  mq4Value = analogRead(mq4Pin);
  mq7Value = analogRead(mq7Pin);

  // Read DHT sensor values
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(humidity) || isnan(temperature)) 
  {
    Serial.println("Failed to read from DHT sensor!");
    delay(2000); // Wait a bit before trying again
    return;
  }

  // obj.mq4 < 500 && obj.mq7 < 500 && (obj.humidity >= 30 && obj.humidity <= 70) && (obj.temperature >=25 && obj.temperature < 50)
  if(mq4Value < 500 && mq7Value < 500 && (humidity >= 30 && humidity <= 70) && (temperature >= 25 && temperature < 50)){
    // No warning  
  }
  else{
    warning();  
  }
  // Send distance, MQ-4, MQ-7, temperature, and humidity values
  char msg[100];
  sprintf(msg, "D:%d,MQ4:%d,MQ7:%d,T:%d,H:%d", distance, mq4Value, mq7Value, temperature, humidity);
  rf95.send((uint8_t *)msg, strlen(msg));
  rf95.waitPacketSent();

  Serial.print("Sent: ");
  Serial.println(msg);

  delay(2000);
}
