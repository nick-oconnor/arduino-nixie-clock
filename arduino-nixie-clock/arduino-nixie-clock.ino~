#define DECODER_A   2
#define DECODER_B   3
#define DECODER_C   4
#define DECODER_D   5

#define HOUR_10     6
#define HOUR_1      7
#define MINUTE_10   8
#define MINUTE_1    9
#define SECOND_10   10
#define SECOND_1    11

#define SET_HOUR    12
#define SET_MINUTE  13

#define DISPLAY_MS  2
#define BLANKING_US 200

volatile unsigned long milliseconds = 0;
const unsigned long millisecondReset = (unsigned long)12 * 60 * 60 * 1000 - 1,
                    secondDivisor = 1000,
                    minuteDivisor = secondDivisor * 60,
                    hourDivisor = minuteDivisor * 60;
bool hourIncremented = false, minuteIncremented = false;

ISR(TIMER2_OVF_vect)
{
  milliseconds++;
  
  if (milliseconds > millisecondReset)
  {
    milliseconds -= millisecondReset;
  }
  
  TCNT2 = 130;
  TIFR2 = 0x00;
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

  pinMode(HOUR_10, OUTPUT);
  pinMode(HOUR_1, OUTPUT);
  pinMode(MINUTE_10, OUTPUT);
  pinMode(MINUTE_1, OUTPUT);
  pinMode(SECOND_10, OUTPUT);
  pinMode(SECOND_1, OUTPUT);

  pinMode(SET_HOUR, INPUT_PULLUP);
  pinMode(SET_MINUTE, INPUT_PULLUP);

  TCCR2B = 0x00;
  TCNT2  = 130;
  TIFR2  = 0x00;
  TIMSK2 = 0x01;
  TCCR2A = 0x00;
  TCCR2B = 0x05;
}

void loop()
{
  unsigned short hour = milliseconds / hourDivisor % 12,
                 minute = milliseconds / minuteDivisor % 60,
                 second = milliseconds / secondDivisor % 60,
                 displayHour = hour;
  bool setHourPressed = !digitalRead(SET_HOUR),
       setMinutePressed = !digitalRead(SET_MINUTE);

  if (hour == 0)
  {
    displayHour = 12;
  }
  
  if (setHourPressed && !hourIncremented)
  {
    milliseconds += hourDivisor;
    hourIncremented = true;
  }
  else if (setMinutePressed && !minuteIncremented)
  {
    milliseconds += minuteDivisor;
    milliseconds -= milliseconds % minuteDivisor;
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
  
  displayDigit(displayHour / 10, HOUR_10);
  displayDigit(displayHour % 10, HOUR_1);
  displayDigit(minute / 10, MINUTE_10);
  displayDigit(minute % 10, MINUTE_1);
  displayDigit(second / 10, SECOND_10);
  displayDigit(second % 10, SECOND_1);
}
