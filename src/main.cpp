#include <Arduino.h>
#include <SoftwareSerial.h>
#define sensor_baud_rate 9600 //sensor only works with baud rate of 9600 
#define ESP_baud_rate 115200 //separate baud rate for ESP32 can be changed
#define LED1 2 //InBuilt led pin of ESP32

SoftwareSerial O2_serial (18,19); //Establising software 

byte O2_qa_mode[] = {0xFF, 0x01, 0x78, 0x04, 0x00, 0x00, 0x00, 0x00, 0x83}; //Command to start O2 sensor in QnA mode
byte O2_read[] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79}; //command to read O2 concentration under QnA mode
byte O2_received_bytes[9]; //Array to store sensor response

float read_O2(float *gas_con); //function to read o2 concentration 
float O2; //variable to store O2 value

void setup() {
  Serial.begin(ESP_baud_rate); //Initialise serial monitor from ESP32
  O2_serial.begin(sensor_baud_rate);  //Initialise Uart communication with O2 sensor
  pinMode(LED1, OUTPUT); //declaring esp32 inbuilt led pin as output

  delay(1000);

  O2_serial.write(O2_qa_mode, sizeof(O2_qa_mode));
  delay(250);
  O2_serial.readBytes(O2_received_bytes, sizeof(O2_received_bytes));
  
  
  //prints the received bytes in serial monitor
  for (int i = 0; i <= 8; i++)
  {
    Serial.print(O2_received_bytes[i], HEX); Serial.print(" ");
  }
  Serial.println();

  //sensor booting section
  Serial.println("Sensor is booting Please wait");
  int bootUP_time = 0;
  while(bootUP_time != 10)
  {
    Serial.print(".");
    delay(1000);
    bootUP_time += 1;
  }
  Serial.println();

}

void loop() {

  delay(1840);

  // O2_serial.write(O2_read, sizeof(O2_read));
  // O2_serial.readBytes(O2_received_bytes, sizeof(O2_received_bytes));
  // float O2 = O2_received_bytes[2] * 256 + O2_received_bytes[3]; 
  // Serial.printf("Raw O2 : %f",O2);

  // O2 = O2 * 0.1;
  // Serial.printf("\tDerived O2 : %f  ->->-> \t",O2); 
  
  read_O2(&O2); //passing O2 variable address to write the o2 gas concentration value in it 

  Serial.println(O2); //prints O2 concentration in a serial monitor

  //code is just to print loading dots
  for(int i = 0;i<=10;i++)
  {
    Serial.print(".");
    delay(10);
  }
  Serial.println("");
  //End of loading dots print function

  //simply just blinks Inbuilt LED in ESp32
  digitalWrite(LED1,HIGH);
  delay(150);
  digitalWrite(LED1,LOW);

}

float read_O2(float *gas_con)
{
   if(O2_serial.write(O2_read,sizeof(O2_read)) == 9) //send O2 read command to o2 sensor and checks whether it id fully sent
   {
    delay(250);
    O2_serial.readBytes(O2_received_bytes, sizeof(O2_received_bytes));
    *gas_con = O2_received_bytes[2] * 256 + O2_received_bytes[3]; //Substituting sensor responses to the gas concentration formula
    *gas_con = *gas_con * 0.1;
   } 

   /* //debug code -> to print received bytes into serial monitor */
   for(byte j = 0;j<9;j++)
   {
     Serial.print(O2_received_bytes[j]); //Prints received byte from O2 sensor response
     Serial.print(" ");
   }
  Serial.println();  // */
  return *gas_con; //returns the gas concentration of O2
}