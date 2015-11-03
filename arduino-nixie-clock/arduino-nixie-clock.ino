#define DECODER_A   0
#define DECODER_B   1
#define DECODER_C   2
#define DECODER_D   3

#define HOUR_10     4
#define HOUR_1      5
#define MINUTE_10   6
#define MINUTE_1    7
#define SECOND_10   8
#define SECOND_1    9

#define SET_HOUR    10
#define SET_MINUTE  11

#define BLANKING_US 100
#define DISPLAY_MS  1

volatile unsigned long milliseconds = 0;
unsigned short hour, minute, second;
bool hourIncremented = false, minuteIncremented = false;

ISR(TIMER2_OVF_vect)
{
  milliseconds++;
  
  if(milliseconds >= 12 * 60 * 60 * 1000)
  { 
    milliseconds = 0;
  }
  
  TCNT2 = 130;
  TIFR2 = 0x00;
}

void updateTime()
{
  unsigned short seconds = milliseconds / 1000;
  
  hour = seconds / 60 / 60 % 12;
  minute = seconds / 60 % 60;
  second = seconds % 60;
}

void setTime(unsigned short new_hour, unsigned short new_minute, unsigned short new_second)
{
  milliseconds = (new_hour * 60 * 60 + new_minute * 60 + new_second) * 1000;
  updateTime();
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
  unsigned short displayHour;
  bool setHourPressed = !digitalRead(SET_HOUR), setMinutePressed = !digitalRead(SET_MINUTE);

  updateTime();

  if (setHourPressed && !hourIncremented)
  {
    if (hour == 11)
    {
      setTime(0, minute, second);
    }
    else
    {
      setTime(hour + 1, minute, second);
    }
    
    hourIncremented = true;
  }
  else if (setMinutePressed && !minuteIncremented)
  {
    if (minute == 59)
    {
      setTime(hour, 0, second);
    }
    else
    {
      setTime(hour, minute + 1, second);
    }
    
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

  if (hour == 0)
  {
    displayHour = 12;
  }
  else
  {
    displayHour = hour;
  }
  
  displayDigit(displayHour / 10, HOUR_10);
  displayDigit(displayHour % 10, HOUR_1);
  displayDigit(minute / 10, MINUTE_10);
  displayDigit(minute % 10, MINUTE_1);
  displayDigit(second / 10, SECOND_10);
  displayDigit(second % 10, SECOND_1);
}
