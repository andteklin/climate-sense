#include <dht_nonblocking.h>

/* ClimateSense
   
   Visualizing Temperature & Humidity

   Displays the ambient temperature on the serial monitor
   as well as the LCD display. Includes the LiquidCrystal library
   to display content on LCD. Thermistor is used, changing
   resistance based on temperature.

   modified 21 July 2022
   by Andrew H. Lin & Antony S. Langley
   contributions by Joshua Kim, Charlotte Lin, and Sahil Shah
*/

//include library
#include <LiquidCrystal.h>


// defining pins and initializing LCD and DHT sensor
#define dhtPin 2
#define DHT_SENSOR_TYPE DHT_TYPE_11
DHT_nonblocking dht_sensor( dhtPin, DHT_SENSOR_TYPE );
#define BLUE 3
#define GREEN 5
#define RED 6
#define DIR 13
#define fanSwitch A1
#define pinTherm A0
#define buttonPin 4

//defining variables for inputs and calculation]
boolean isHot = false; 
boolean switcher = true;
unsigned long previousTime;
int redVal = 0; int greenVal = 0; int blueVal = 0;
float r1 = 10000;   // known resistor value, 10K ohms
float inV = 1024;      // known voltage input, 5V or analog 1024 bits
float outV, r2, temp;
float a = 0.001129148; float b = 0.000234125; float c = 0.0000000876741;

LiquidCrystal lcd(7, 8, 9, 10, 11, 12);
void setup()
{
  Serial.begin(9600);
  lcd.begin(16, 2);
  // initializing pins as outputs
  pinMode(RED, OUTPUT); pinMode(GREEN, OUTPUT); pinMode(BLUE, OUTPUT);
  pinMode(DIR, OUTPUT); pinMode(fanSwitch, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
}

void loop()
{
  digitalWrite(fanSwitch, HIGH);    // enable fan
  outV = analogRead(pinTherm); // read voltage from thermistor
  r2 = r1 * ((inV/outV) - 1);       // using voltage divider equation
  /* using Steinhart-Hart equation, convert from voltage to temp in K */
  temp = (1.00 / (a + b * log(r2) + c * log(r2)*log(r2)*log(r2)));
  double tempK = temp;
  double tempC = temp - 273.15; // convert kelvin to celsius
  double tempF = (temp - 273.15) * (9.0/5.0) + 32; // convert celsius to fahrenheit

  // reading humidity
  int chk = analogRead(dhtPin);
  float humidity;

  // checking for button status to switch screen
  if (digitalRead(buttonPin) == LOW) {
    delay(500);
    switcher = !switcher;
    lcd.clear();
  }

  /* Following code divided into 7 parts, 5 parts relating values of 255
   *  in the RGB spectrum in a range of 72 to 92 degrees Fahrenheit.
   *  Other conditions relating to values outside the range.
   */
  if (tempF < 72) 
  {
    blueVal = 255; redVal = 0; greenVal = 0; isHot = false;
  }
  else if (tempF >= 72 && tempF <= 77)
  {
    blueVal = 255; redVal = 0;
    greenVal = 51.0 * (tempF - 72.0);
    isHot = false;
  }
  else if (tempF > 77 && tempF <= 82)
  {
    greenVal = 255; redVal = 0;
    blueVal = 255 - 51.0 * (tempF - 77.0);
    isHot = false;
  }
  else if (tempF > 82 && tempF <= 87)
  {
    greenVal = 255; blueVal = 0;
    redVal = 51.0 * (tempF - 82.0);
    isHot = false;
  }
  else if (tempF > 87 && tempF <= 92) 
  {
    redVal = 255; blueVal = 0;
    greenVal = 255 - 51.0 * (tempF - 87.0);
    isHot = true;
  }
  else if (tempF > 92)
  {
    redVal = 255; blueVal = 0; greenVal = 0;
    isHot = true;
  }

  // conditions checking for temperature to turn on fan when too hot
  if (isHot)
  {
    digitalWrite(DIR, HIGH);
  }
  else if (!isHot)
  {
    digitalWrite(DIR, LOW);
  }

  // writing binary values to RGB LEDs
  analogWrite(RED, redVal);
  analogWrite(GREEN, greenVal);
  analogWrite(BLUE, blueVal);

  unsigned long currentTime = millis();
  // by default, shows temperature in Fahrenheit
  if (switcher && (currentTime - previousTime >= 500)) {
  // Display Temperature in Fahrenheit
  lcd.setCursor(0, 0); lcd.print("Temp");
  lcd.setCursor(11, 0); lcd.print(" ");
  lcd.setCursor(6, 0); lcd.print(tempF);
  lcd.setCursor(12, 0); lcd.print(char(223)); //char 223 is degree symbol
  lcd.setCursor(13, 0); lcd.print("F");
  Serial.println(outV);
  lcd.setCursor(0, 1);
  String h = String(humidity, 1);
  lcd.print("Humidity: " + h + "%");
  previousTime = currentTime;
  }

  // Switch statement to show stemperature in Celsius
  else if (!switcher && (currentTime - previousTime >= 500)) {
    lcd.setCursor(0, 0); lcd.print("Temp");
    lcd.setCursor(11, 0); lcd.print(" ");
    lcd.setCursor(6, 0); lcd.print(tempC);
    lcd.setCursor(12, 0); lcd.print(char(223));
    lcd.setCursor(13, 0); lcd.print("C");
    lcd.setCursor(0, 1);
    String h = String(humidity, 1);
    lcd.print("Humidity: " + h + "%");
    previousTime = currentTime;
  }
}
