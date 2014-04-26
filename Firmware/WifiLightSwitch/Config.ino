#include <EEPROM.h>

void loadConfig()
{
  readConfigIntVal(CFG_MODE, mode);
  
  int ip_a, ip_b, ip_c, ip_d;
  
  readConfigIntVal(CFG_HOST_IP_A, ip_a);
  readConfigIntVal(CFG_HOST_IP_B, ip_b);
  readConfigIntVal(CFG_HOST_IP_C, ip_c);
  readConfigIntVal(CFG_HOST_IP_D, ip_d);
  
  ip = cc3000.IP2U32(ip_a, ip_b, ip_c, ip_d);
  //cc3000.printIPdotsRev(ip);
}

void writeConfigVal(uint8_t var, char *value)
{
  uint8_t addr = 0;
  uint8_t len = 0;
  switch(var)
  {
    case CFG_SSID: addr = 0; len = 32; break;
    case CFG_PWD: addr = 32; len = 32; break;
    case CFG_SECURITY: addr = 64; len = 1; break;
    case CFG_MODE: addr = 65; len = 1; break;
    case CFG_HOST_IP_A: addr = 66; len = 1; break;
    case CFG_HOST_IP_B: addr = 67; len = 1; break;
    case CFG_HOST_IP_C: addr = 68; len = 1; break;
    case CFG_HOST_IP_D: addr = 69; len = 1; break;
    case CFG_HOST_NAME: addr = 70; len = 32; break;
    case CFG_HOST_PATH_STATUS: addr = 102; len = 32; break;
    case CFG_HOST_PATH_REGISTER: addr = 134; len = 32; break;
  }
  
  boolean done = false;
  for (int c = 0; c < len; c++)
  {
    if(value[c] == '\0') done = true;
    
    char l = '\0';
    if(!done) l = value[c];
    
    if(!done) EEPROM.write(c + addr, value[c]);
    else EEPROM.write(c + addr, '\0');
  }
}

void readConfigStringVal(uint8_t var, char *value)
{
  uint8_t addr = 0;
  uint8_t len = 0;
  switch(var)
  {
    case CFG_SSID: addr = 0; len = 32; break;
    case CFG_PWD: addr = 32; len = 32; break;
    case CFG_HOST_NAME: addr = 70; len = 32; break;
    case CFG_HOST_PATH_STATUS: addr = 102; len = 32; break;
    case CFG_HOST_PATH_REGISTER: addr = 134; len = 32; break;
    default: return;
  }
  
  for (int c = 0; c < len; c++)
  {
    value[c] = EEPROM.read(c+addr);
    if(value[c] == '\0') return;
  }
}

void readConfigIntVal(uint8_t var, int &value)
{
  uint8_t addr = 0;
  switch(var)
  {
    case CFG_SECURITY: addr = 64; break;
    case CFG_MODE: addr = 65; break;
    case CFG_HOST_IP_A: addr = 66; break;
    case CFG_HOST_IP_B: addr = 67; break;
    case CFG_HOST_IP_C: addr = 68; break;
    case CFG_HOST_IP_D: addr = 69; break;
  }
  
  value = EEPROM.read(addr);
}

void loadConfigFromSerial()
{
  char tmp_str[32] = "";
  int16_t tmp_int = 0;
  int rtn_int = 0;
  tmp_str[0] ='\0';
  
  Serial.println("Serial Mode");
  Serial.println("SSID:");
  readConfigLine(tmp_str, 32);
  if(tmp_str[0] != '\0') writeConfigVal(CFG_SSID, tmp_str);
  readConfigStringVal(CFG_SSID, tmp_str);
  Serial.println(tmp_str);
  
  Serial.println("Password:");
  readConfigLine(tmp_str, 32);
  if(tmp_str[0] != '\0') writeConfigVal(CFG_PWD, tmp_str);
  readConfigStringVal(CFG_PWD, tmp_str);
  Serial.println(tmp_str);
  
  Serial.println("Security:");
  tmp_int = readConfigInt();
  if(tmp_int != -1)
  {
    tmp_str[0] = tmp_int;
    tmp_str[1] = '\0';
    writeConfigVal(CFG_SECURITY, tmp_str);
  }
  readConfigIntVal(CFG_SECURITY, rtn_int);
  Serial.println(rtn_int);
  
  Serial.println("Mode:");
  tmp_int = readConfigInt();
  if(tmp_int != -1)
  {
    tmp_str[0] = tmp_int;
    tmp_str[1] = '\0';
    writeConfigVal(CFG_MODE, tmp_str);
  }
  readConfigIntVal(CFG_MODE, rtn_int);
  Serial.println(rtn_int);
  
  Serial.println("Host IP A:");
  tmp_int = readConfigInt();
  if(tmp_int != -1)
  {
    tmp_str[0] = tmp_int;
    tmp_str[1] = '\0';
    writeConfigVal(CFG_HOST_IP_A, tmp_str);
  }
  readConfigIntVal(CFG_HOST_IP_A, rtn_int);
  Serial.println(rtn_int);
  
  Serial.println("Host IP B:");
  tmp_int = readConfigInt();
  if(tmp_int != -1)
  {
    tmp_str[0] = tmp_int;
    tmp_str[1] = '\0';
    writeConfigVal(CFG_HOST_IP_B, tmp_str);
  }
  readConfigIntVal(CFG_HOST_IP_B, rtn_int);
  Serial.println(rtn_int);
  
  Serial.println("Host IP C:");
  tmp_int = readConfigInt();
  if(tmp_int != -1)
  {
    tmp_str[0] = tmp_int;
    tmp_str[1] = '\0';
    writeConfigVal(CFG_HOST_IP_C, tmp_str);
  }
  readConfigIntVal(CFG_HOST_IP_C, rtn_int);
  Serial.println(rtn_int);
  
  Serial.println("Host IP D:");
  tmp_int = readConfigInt();
  if(tmp_int != -1)
  {
    tmp_str[0] = tmp_int;
    tmp_str[1] = '\0';
    writeConfigVal(CFG_HOST_IP_D, tmp_str);
  }
  readConfigIntVal(CFG_HOST_IP_D, rtn_int);
  Serial.println(rtn_int);
  
  Serial.println("Host Name:");
  readConfigLine(tmp_str, 32);
  if(tmp_str[0] != '\0') writeConfigVal(CFG_HOST_NAME, tmp_str);
  readConfigStringVal(CFG_HOST_NAME, tmp_str);
  Serial.println(tmp_str);
  
  Serial.println("Status Path:");
  readConfigLine(tmp_str, 32);
  if(tmp_str[0] != '\0') writeConfigVal(CFG_HOST_PATH_STATUS, tmp_str);
  readConfigStringVal(CFG_HOST_PATH_STATUS, tmp_str);
  Serial.println(tmp_str);
  
  Serial.println("Reg Path:");
  readConfigLine(tmp_str, 32);
  if(tmp_str[0] != '\0') writeConfigVal(CFG_HOST_PATH_REGISTER, tmp_str);
  readConfigStringVal(CFG_HOST_PATH_REGISTER, tmp_str);
  Serial.println(tmp_str);
  
  loadConfig();
}

void readConfigLine(char *line, int max_chars)
{
  boolean done = false;
  int chars = 0;
  line[0] = '\0';
  while (!done)
  {
    while (Serial.available() > 0)
    {
      char c = Serial.read();
      if(c == '\n') done = true;
      else if(chars < max_chars && c != '\n')
      {
        line[chars] = c;
        line[chars+1] = '\0';
      }
      chars ++;
    }
  }
}
  
int16_t readConfigInt()
{
  boolean done = false;
  String in = "";
  while (!done)
  {
    while (Serial.available() > 0)
    {
      char c = Serial.read();
      if(c == '\n')
      {
        done = true;
        if(in.length() == 0) return -1;
        else return in.toInt();
      }
      else in += c;
    }
  }
}

void configurationState()
{
  Serial.println("Config Mode");
  int chan_1_state_orig = chan_1_state;
  int chan_2_state_orig = chan_2_state;
  int chan_3_state_orig = chan_3_state;
  int chan_4_state_orig = chan_4_state;
  
  
  int led_1_state_orig = led_1_state;
  int led_2_state_orig = led_2_state;
  int led_3_state_orig = led_3_state;
  
  boolean chan_change_orig = chan_change;
  
  chan_1_state = CLICK_NONE;
  chan_2_state = CLICK_NONE;
  chan_3_state = CLICK_NONE;
  chan_4_state = CLICK_NONE;
  
  led_1_state = LED_BLINK_FAST;
  led_2_state = LED_BLINK_FAST;
  led_3_state = LED_BLINK_FAST;
  
  while(chan_1_state == CLICK_NONE &&
  chan_2_state == CLICK_NONE &&
  chan_3_state == CLICK_NONE &&
  chan_4_state == CLICK_NONE) delay(1000);
  
  if(chan_1_state == CLICK_SHORT || chan_1_state == CLICK_LONG) loadConfigFromSerial();
  //else if(chan_2_state == CLICK_SHORT || chan_2_state == CLICK_LONG) load web config...
  else if(chan_3_state == CLICK_SHORT || chan_3_state == CLICK_LONG) resetWifi();
  else if(chan_4_state == CLICK_SHORT || chan_4_state == CLICK_LONG) Serial.println("Exit");
  
  chan_1_state = chan_1_state_orig;
  chan_2_state = chan_2_state_orig;
  chan_3_state = chan_3_state_orig;
  chan_4_state = chan_4_state_orig;
  
  led_1_state = led_1_state_orig;
  led_2_state = led_2_state_orig;
  led_3_state = led_3_state_orig;
  
  chan_change = chan_change_orig;
  
  Serial.println("Config Mode Done");
}
