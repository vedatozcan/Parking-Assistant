/*
  Outher: Vedat Özcan 
  -> Reference_1: http://www.arduino.cc/en/Tutorial/LiquidCrystal
  -> Reference_2: https://forum.arduino.cc/t/rtc-library-with-day-number-of-year/401450
  -> Reference_3: https://forum.arduino.cc/t/using-millis-for-timing-a-beginners-guide/483573/2
  -> Reference_4: https://www.arduino.cc/reference/en/language/functions/advanced-io/tone/
  -> Reference_5: https://www.arduino.cc/en/Tutorial/BuiltInExamples/toneMelody
  -> Reference_6: https://www.youtube.com/watch?v=UKj6qtSBXMQ (Game Over Melody)
  
  Intervals are:
  -> Distance > 1 m → “Too Far”
  -> Distance ϵ [51 cm, 100 cm] → “Far”
  -> Distance ϵ [21 cm, 50 cm] → “Close”
  -> Distance ϵ [5 cm, 20cm] → “Too Close”
  -> Distance <5 cm → “Crashed”
  
  The circuit:
    LCD:
      -> LCD RS pin to digital pin 12
      -> LCD Enable pin to digital pin 11
      -> LCD D4 pin to digital pin 6
      -> LCD D5 pin to digital pin 5
      -> LCD D6 pin to digital pin 4
      -> LCD D7 pin to digital pin 3
      -> LCD R/W pin to ground
      -> LCD VSS pin to ground
      -> LCD VCC pin to 5V
      -> ends to +5V and ground   
    Ultrasonic Sensor(HC-SR04):
      -> HC-SR04 GND pin to ground
      -> HC-SR04 echo pin to digital pin 10
      -> HC-SR04 trig pin to digital pin 9
      -> HC-SR04 VCC pin to 5V
    Buzzer:
      -> Buzzer GND pin to ground
      -> Buzzer pin to digital pin 8
    
*/


#include <LiquidCrystal.h>  // include library for LCD

/* defining some melodies from arduino references */
#define NOTE_DS8 4978
#define NOTE_FS7 2960
#define NOTE_B6  1976
#define NOTE_A4  440
#define NOTE_A3  220
#define NOTE_E1  41

/* defining the LCD screen, Ultrasonik Sensör(HC-SR04) and Buzzer pins*/
#define RS 12 
#define EN 11 
#define D4 6
#define D5 5  
#define D6 4 
#define D7 3 

#define buzzerPin 8
#define trigPin 9
#define echoPin 10

LiquidCrystal lcd(RS, EN, D4, D5, D6, D7); // initialize the library with the numbers of the interface pins

int maxDistance = 100; // max. distance must be 100 cm.
int minDistance = 0;  // min. distance must be 0 cm.
int rangeValue = 0;
String distanceDescription = "";

String CURRENT_TIME = "2021.06.14 - 02:00:12";  

/* Parsing date and converting to integer */
int year = CURRENT_TIME.substring(0,4).toInt();
int month = CURRENT_TIME.substring(5,7).toInt();
int day = CURRENT_TIME.substring(8,10).toInt(); 
  
/* Parsing time and converting to integer */
int hour = CURRENT_TIME.substring(13,15).toInt();
int minute = CURRENT_TIME.substring(16,18).toInt();
int sec = CURRENT_TIME.substring(19,21).toInt();  

/* These variables were kept to increment the seconds value 
   one by one and update the time and date */
unsigned long startMillis;
unsigned long currentMillis;
const unsigned long period = 1000;

/* These variables were kept to switch screens 
   at intervals of 2 seconds */
unsigned long currentTime;
const unsigned long interval = 2000;



void setup() {
  Serial.begin(9600);
  pinMode(buzzerPin, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  lcd.begin(16, 2);  // set up the LCD's number of columns and rows
  lcd.clear();
}

void loop() {
  currentTime = millis(); // take the start time 
  while(millis() - currentTime <= interval){
    updateSerialMonitor(); // update the time and show information after 2 seconds
    parkSensor();
  }
  printSerialMonitor();
}

/* Function that calculates the distance. */
int range(int maxr, int minr){
  long duration;
  long distance;
  digitalWrite(trigPin, LOW); // stop the signal
  delayMicroseconds(2);      // wait 2 microseconds
  digitalWrite(trigPin, HIGH);  // send the signal 
  delayMicroseconds(10);     // wait 10 microseconds 
  digitalWrite(trigPin, LOW); // stop the signal 

  duration = pulseIn(echoPin, HIGH); // reads a pulse HIGH on a pin.
  distance = (duration/2)/29.1;  // it takes 29.1 microseconds for the audio signal to move 1 cm (from the formula for the speed of sound) 
  delay(50);

  if(distance >= maxDistance || distance <= minDistance)
    return 0;
  else
    return distance;
 }


/* Function that runs various melodies based on distance. */
int melody(int d, int tones){
  tone(buzzerPin, tones); // tone(pin, frequency)
  delay(d); 
  noTone(buzzerPin); // stop the waveform generation before the next note.
  delay(d);
}


/* The game over melody that will play when the distance falls below 5 cm */
int gameOverMelody(int delayTime){
  int noteDuration = 1000/8; // eight note
  int pauseBetweenNotes = noteDuration * 0.2;
  
  // determine the number of siren cycles to play
  for(int i=0; i<4 ; i++){ 
    
    // play low to high 
    for(int j=25; j<30 ; j++){
       tone(buzzerPin, 20*j, noteDuration); // tone(pin, frequency, duration)
       delay(pauseBetweenNotes);
    }
    // play high to low 
    for(int k=30; k>=25 ; k--){
       tone(buzzerPin, 20*k, noteDuration); 
       delay(pauseBetweenNotes);
    }
  }  
  delay(300); 
  tone(buzzerPin, 500); // tone(pin, frequency)
  delay(delayTime);
  tone(buzzerPin, 1200);
  delay(delayTime);
  tone(buzzerPin, 300);
  delay(delayTime);
  tone(buzzerPin, 1000);
  delay(delayTime);
  tone(buzzerPin, 400);
  delay(delayTime);
  tone(buzzerPin, 1100);
  delay(delayTime);
}

/* Function that prints information on Serial Monitor screen 
   and LCD display screen according to the calculated distance.*/
void parkSensor(){
  rangeValue = range(maxDistance, minDistance);
  if(rangeValue > 100){
    melody(rangeValue*10, NOTE_DS8);
    distanceDescription = "Too Far";
    lcd.setCursor(0,0);
    lcd.print("Distance -> ");
    lcd.print(rangeValue);
    lcd.setCursor(0,1);
    lcd.print(distanceDescription);
  }
  else if(rangeValue >= 51 && rangeValue <= 100){
    melody(rangeValue*10, NOTE_FS7);
    distanceDescription = "Far";
    lcd.setCursor(0,0);
    lcd.print("Distance -> ");
    lcd.print(rangeValue);
    lcd.setCursor(0,1);
    lcd.print(distanceDescription);
  }
  else if(rangeValue >= 21 && rangeValue <= 50){
    melody(rangeValue*10, NOTE_B6);
    distanceDescription = "Close";
    lcd.setCursor(0,0);
    lcd.print("Distance -> ");
    lcd.print(rangeValue);
    lcd.setCursor(0,1);
    lcd.print(distanceDescription);
  }
    
  else if(rangeValue >= 5 && rangeValue <= 20){
    melody(rangeValue*10, NOTE_A4);
    distanceDescription = "Too Close";
    lcd.setCursor(0,0);
    lcd.print("Distance -> ");
    lcd.print(rangeValue);
    lcd.setCursor(0,1);
    lcd.print(distanceDescription);
  }
  else if(rangeValue < 5){
    gameOverMelody(120); // game over melody
    distanceDescription = "Crashed";
    lcd.setCursor(0,0);
    lcd.print("Distance -> ");
    lcd.print(rangeValue);
    lcd.setCursor(0,1);
    lcd.print(distanceDescription);
  }
}

/* Function of printing information to serial monitor */
void printSerialMonitor(){
  Serial.print("Date: ");
  if(day < 10) 
    Serial.print("0");
  Serial.print(day);
  Serial.print(".");
  if(month < 10) 
     Serial.print("0");
  Serial.print(month);
  Serial.print(".");
  Serial.print(year);
  Serial.print(" - ");

  Serial.print("Hour: ");
  if(hour < 10)
    Serial.print("0");
  Serial.print(hour);
  Serial.print(":");
  if(minute < 10) 
     Serial.print("0");
  Serial.print(minute);
  Serial.print(":");
  if(sec < 10) 
     Serial.print("0");
  Serial.print(sec);
  Serial.print(" - ");
  
  Serial.print("Distance: ");
  Serial.print(rangeValue);
  Serial.println(" cm.");
  Serial.print("It is ");
  Serial.print(distanceDescription);
  Serial.println(".");
  Serial.println("------------------------------------");
}

/* The function that updates the Serial Monitor */
void updateSerialMonitor(){
  currentMillis = millis(); 
  if(currentMillis - startMillis >= period){
    // increase 1 second every 1000 milliseconds
    sec++; 
    if(sec == 60){
      sec = 0;
        // when 60 seconds have passed, increase the minute by 1
      minute++; 
    }
    if(minute == 60){
      minute = 0;
        // when 60 minutes have passed, increase the hour by 1
      hour++; 
    }
    if(hour == 24){
      hour = 0;
       /* Look at what day of the month after 24 hours.
          If it is the last day of any month, set the day to 1. 
          Otherwise increase 1 */
      if(day == getDaysInMonth(month,year)){ 
          day = 1;
           /* If it is the end of the 12th month, increase 
              the year by 1 and make the month value 1.
              Otherwise, increase the month value by 1 */
          if(month == 12){
        month = 1;
          year++;
          }
            else
              month++;
      }
        else
          day++;
    }
    startMillis = currentMillis; 
  }
}

/* Function that returns the days of the 
   week using the Gregorian Calendar formula */
int getWeekDay(int day, int month, int year){
  if (month <= 2){
    month += 12;
    --year;
  }
  int j = year % 100;
  int e = year / 100;
  return (((day + (((month + 1) * 26) / 10) + j + (j / 4) + (e / 4) - (2 * e)) - 2) % 7);
}

/* The function that returns the total 
   number of days of the relevant year */
int getDayOfYear(const int day, const int month, const int year){
  int d = day;
  int m = month;
  while(--m) 
    d += getDaysInMonth(m, year);
  return d;
}

/* Function that returns whether February is 29 days or not */
bool isLeapYear(const int year){
  return ((year % 400 == 0) || (year % 4 == 0 && year % 100 != 0));
}

/* Function that returns how many days the months are */
int getDaysInMonth(const int month, const int year){
  if (month == 2)  
    return isLeapYear(year) ? 29 : 28;
  else if (month == 4 || month == 6 || month == 9 || month == 11)  
      return 30;   
  return 31;
}
