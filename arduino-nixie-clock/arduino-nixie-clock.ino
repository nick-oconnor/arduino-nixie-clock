#define DECODER_C   0
#define DECODER_B   1
#define DECODER_D   2
#define DECODER_A   3

#define SET_HOUR    4
#define SET_MINUTE  5

#define HOUR_10     6
#define HOUR_1      7
#define MINUTE_10   8
#define MINUTE_1    9
#define SECOND_10   10
#define SECOND_1    11

#define DISPLAY_MS  2
#define BLANKING_US 200

extern volatile unsigned long timer0_millis;
const unsigned long millisMax = (unsigned long)12 * 60 * 60 * 1000,
                    secondDivisor = 1000,
                    minuteDivisor = secondDivisor * 60,
                    hourDivisor = minuteDivisor * 60;
bool hourIncremented = false,
     minuteIncremented = false;

void addMillis(unsigned long offset)
{
  uint8_t oldSREG = SREG;
  
  cli();
  timer0_millis += offset;
  SREG = oldSREG;
}

void displayDigit(unsigned short digit, unsigned short pin)
{
  digitalWrite(DECODER_A, (1 << 0) & digit);
  digitalWrite(DECODER_B, (1 << 1) & digit);
  digitalWrite(DECODER_C, (1 << 2) & digit);
  digitalWrite(DECODER_D, (1 << 3) & digit);
  digitalWrite(pin, HIGH);
  delay(DISPLAY_MS);
  digitalWrite(pin, LOW);
  delayMicroseconds(BLANKING_US);
}

void setup()
{
  pinMode(DECODER_A, OUTPUT);
  pinMode(DECODER_B, OUTPUT);
  pinMode(DECODER_C, OUTPUT);
  pinMode(DECODER_D, OUTPUT);
  
  pinMode(SET_HOUR, INPUT_PULLUP);
  pinMode(SET_MINUTE, INPUT_PULLUP);
  
  pinMode(HOUR_10, OUTPUT);
  pinMode(HOUR_1, OUTPUT);
  pinMode(MINUTE_10, OUTPUT);
  pinMode(MINUTE_1, OUTPUT);
  pinMode(SECOND_10, OUTPUT);
  pinMode(SECOND_1, OUTPUT);
}

void loop()
{ 
  bool setHourPressed = !digitalRead(SET_HOUR),
       setMinutePressed = !digitalRead(SET_MINUTE);
  
  if (setHourPressed && !hourIncremented)
  {
    addMillis(hourDivisor);
    hourIncremented = true;
  }
  else if (setMinutePressed && !minuteIncremented)
  {
    addMillis(minuteDivisor - millis() % minuteDivisor);
    minuteIncremented = true;
  }
  else if (!setHourPressed && hourIncremented)
  {
    hourIncremented = false;
  }
  else if (!setMinutePressed && minuteIncremented)
  {
    minuteIncremented = false;
  }

  if (millis() >= millisMax)
  {
    addMillis(-millisMax);
  }
  
  unsigned short hour = millis() / hourDivisor % 12,
                 minute = millis() / minuteDivisor % 60,
                 second = millis() / secondDivisor % 60;
  
  if (millis() < 10 * secondDivisor)
  {
    hour = minute = second = second * 11;
  }
  else if (hour == 0)
  {
    hour = 12;
  }
  
  displayDigit(hour / 10, HOUR_10);
  displayDigit(hour % 10, HOUR_1);
  displayDigit(minute / 10, MINUTE_10);
  displayDigit(minute % 10, MINUTE_1);
  displayDigit(second / 10, SECOND_10);
  displayDigit(second % 10, SECOND_1);
}
