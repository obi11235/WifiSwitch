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


