

void initaliseBlinkerInterupt()
{
  noInterrupts();           // disable all interrupts
  TCCR1A = 0;
  TCCR1B = 0;
  
  TCNT1 = TIMER1_COUNTER;   // preload timer 65536-16MHz/256/4Hz
  TCCR1B |= (1 << CS12);    // 256 prescaler 
  TIMSK1 |= (1 << TOIE1);   // enable timer overflow interrupt
  interrupts();             // enable all interrupts
}

void InitialiseBtnInterrupt(){
  cli();		// switch interrupts off while messing with their settings  
  PCICR =0x02;          // Enable PCINT1 interrupt
  PCMSK1 = 0b00011110;  // Enable interupt on A1 - A4
  sei();		// turn interrupts back on
}

ISR(TIMER1_OVF_vect)        // interrupt service routine 
{
  TCNT1 = TIMER1_COUNTER;   // preload timer
  
  //SLOW blink
  if(blink_count == 0)
  {
    if(status_led_status == LED_BLINK_SLOW)
      digitalWrite(status_led_pin, digitalRead(status_led_pin) ^ 1);
      
    if(error_led_status == LED_BLINK_SLOW)
      digitalWrite(error_led_pin, digitalRead(error_led_pin) ^ 1);
      
    if(led_1_state == LED_BLINK_SLOW)
      digitalWrite(led_1_pin, digitalRead(led_1_pin) ^ 1);
    if(led_2_state == LED_BLINK_SLOW)
      digitalWrite(led_2_pin, digitalRead(led_2_pin) ^ 1);
    if(led_3_state == LED_BLINK_SLOW)
      digitalWrite(led_3_pin, digitalRead(led_3_pin) ^ 1);
  }

  //FAST blink  
  if(status_led_status == LED_BLINK_FAST)
    digitalWrite(status_led_pin, digitalRead(status_led_pin) ^ 1);

  if(error_led_status == LED_BLINK_FAST)
    digitalWrite(error_led_pin, digitalRead(error_led_pin) ^ 1);

  if(led_1_state == LED_BLINK_FAST)
    digitalWrite(led_1_pin, digitalRead(led_1_pin) ^ 1);
  if(led_2_state == LED_BLINK_FAST)
    digitalWrite(led_2_pin, digitalRead(led_2_pin) ^ 1);
  if(led_3_state == LED_BLINK_FAST)
    digitalWrite(led_3_pin, digitalRead(led_3_pin) ^ 1);
    
  //ON
  if(status_led_status == LED_ON)
    digitalWrite(status_led_pin, HIGH);

  if(error_led_status == LED_ON)
    digitalWrite(error_led_pin, HIGH);
    
  if(led_1_state == LED_ON)
    digitalWrite(led_1_pin, HIGH);
  if(led_2_state == LED_ON)
    digitalWrite(led_2_pin, HIGH);
  if(led_3_state == LED_ON)
    digitalWrite(led_3_pin, HIGH);
  
  //OFF
  if(status_led_status == LED_OFF)
    digitalWrite(status_led_pin, LOW);

  if(error_led_status == LED_OFF)
    digitalWrite(error_led_pin, LOW);
  
  if(led_1_state == LED_OFF)
    digitalWrite(led_1_pin, LOW);
  if(led_2_state == LED_OFF)
    digitalWrite(led_2_pin, LOW);
  if(led_3_state == LED_OFF)
    digitalWrite(led_3_pin, LOW);
  
  blink_count = (blink_count + 1) % BLINK_MOD;
}

ISR(PCINT1_vect)
{
  unsigned long current = millis();
  int port = PINC;
  int btn1 = (port & 0b00000010) == 0b00000010;
  int btn2 = (port & 0b00000100) == 0b00000100;
  int btn3 = (port & 0b00001000) == 0b00001000;
  int btn4 = (port & 0b00010000) == 0b00010000;
  
  if(updateButton(btn1, btn_1_state, btn_1_last, chan_1_state, current))
  {
    chan_change = true;
    if(mode == MODE_EXCLUSIVE)
    {
      chan_2_state = CLICK_NONE;
      chan_3_state = CLICK_NONE;
    }
  }
  
  if(updateButton(btn2, btn_2_state, btn_2_last, chan_2_state, current))
  {
    chan_change = true;
    if(mode == MODE_EXCLUSIVE)
    {
      chan_1_state = CLICK_NONE;
      chan_3_state = CLICK_NONE;
    }
  }
  
  if(updateButton(btn3, btn_3_state, btn_3_last, chan_3_state, current))
  {
    chan_change = true;
    if(mode == MODE_EXCLUSIVE)
    {
      chan_1_state = CLICK_NONE;
      chan_2_state = CLICK_NONE;
    }
  }
  
  updateButton(btn4, btn_4_state, btn_4_last, chan_4_state, current);
  
  if(chan_1_state > 0) led_1_state = LED_ON; else led_1_state = LED_OFF;
  if(chan_2_state > 0) led_2_state = LED_ON; else led_2_state = LED_OFF;
  if(chan_3_state > 0) led_3_state = LED_ON; else led_3_state = LED_OFF;
  
}
