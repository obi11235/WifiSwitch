#include <Adafruit_CC3000.h>
#include <ccspi.h>
#include <SPI.h>
#include <string.h>
#include "utility/debug.h"

//WIFI Config / Vars
// These are the interrupt and control pins
#define ADAFRUIT_CC3000_IRQ   3  // MUST be an interrupt pin!
// These can be any two pins
#define ADAFRUIT_CC3000_VBAT  5
#define ADAFRUIT_CC3000_CS    10
// Use hardware SPI for the remaining pins
// On an UNO, SCK = 13, MISO = 12, and MOSI = 11
Adafruit_CC3000 cc3000 = Adafruit_CC3000(ADAFRUIT_CC3000_CS, ADAFRUIT_CC3000_IRQ, ADAFRUIT_CC3000_VBAT,
                                         SPI_CLOCK_DIVIDER); // you can change this clock speed

#define IDLE_TIMEOUT_MS  10000      // Amount of time to wait (in milliseconds) with no data 
                                   // received before closing the connection.  If you know the server
                                   // you're accessing is quick to respond, you can reduce this value.

uint32_t ip;
//End WIFI Config / Vars

//Button Management Vars
#define DEBOUNCE_MILLS 25
#define LONG_CLICK_MILLS 1000

#define CLICK_NONE 0
#define CLICK_SHORT 1
#define CLICK_LONG 2

#define MODE_STANDARD 0
#define MODE_EXCLUSIVE 1

#define LED_OFF 0
#define LED_ON 1
#define LED_BLINK_SLOW 3
#define LED_BLINK_FAST 4

#define BLINK_MOD 10

#define TIMER1_COUNTER 59286

#define      CFG_SSID               0
#define      CFG_PWD                1
#define      CFG_SECURITY           2
#define      CFG_MODE               3
#define      CFG_HOST_IP_A          4
#define      CFG_HOST_IP_B          5
#define      CFG_HOST_IP_C          6
#define      CFG_HOST_IP_D          7
#define      CFG_HOST_NAME          8
#define      CFG_HOST_PATH_STATUS   9
#define      CFG_HOST_PATH_REGISTER 10


volatile boolean chan_change = false;

volatile unsigned long btn_1_last = NULL;
volatile int chan_1_state = false;
volatile int led_1_state = LED_OFF;
volatile int btn_1_state = LOW;

volatile unsigned long btn_2_last = NULL;
volatile int chan_2_state = false;
volatile int led_2_state = LED_OFF;
volatile int btn_2_state = LOW;

volatile unsigned long btn_3_last = NULL;
volatile int chan_3_state = false;
volatile int led_3_state = LED_OFF;
volatile int btn_3_state = LOW;

volatile unsigned long btn_4_last = NULL;
volatile int chan_4_state = false;
volatile int btn_4_state = LOW;

const uint8_t led_1_pin = 8;
const uint8_t led_2_pin = 7;
const uint8_t led_3_pin = 6;

const uint8_t btn_1_pin = A1;
const uint8_t btn_2_pin = A2;
const uint8_t btn_3_pin = A3;
const uint8_t btn_4_pin = A4;

int mode = MODE_STANDARD;
//End Button Vars

const uint8_t status_led_pin = 2;
const uint8_t error_led_pin = 4;
volatile uint8_t status_led_status = LED_BLINK_SLOW;
volatile uint8_t error_led_status = LED_BLINK_FAST;

volatile uint8_t blink_count = 0;

unsigned long wifi_last_check = 0;
//30 seconds
#define WIFI_CHECK_INTERVAL 30000



void setup()
{
  unsigned long start = millis();
  Serial.begin(115200);
  Serial.println("WiFi Switch");
  
  //Setup Buttons / LEDs
  InitialiseBtnIO();
  initaliseBlinkerInterupt();
  InitialiseBtnInterrupt();
  loadConfig();
  
  //Setup WiFi Module
  initaliseWiFi();
  
  setupWebServer();
  
  #ifdef _CC3000_DEBUG
  Serial.print("Free RAM: "); Serial.println(getFreeRam(), DEC);
  #endif
  
  status_led_status = LED_ON;
  error_led_status = LED_OFF;
  Serial.print("Boot Time: "); Serial.println((millis() - start)/1000);
}

void loop() {
    
    if(chan_4_state == CLICK_LONG)
    {
      status_led_status = LED_BLINK_SLOW;
      chan_4_state = CLICK_NONE;
      configurationState();
      status_led_status = LED_ON;
    }
    
    if(chan_4_state == CLICK_SHORT)
    {
      //status_led_status = LED_BLINK_SLOW;
      //resetWifi();
      chan_4_state = CLICK_NONE;
      //status_led_status = LED_ON;
    }
    
    if(chan_change)
    {
      chan_change = false;
      sendUpdate();
    }
    
    if(millis() - wifi_last_check > WIFI_CHECK_INTERVAL)
    {
      checkWifiStatus();
      wifi_last_check = millis();
    }
    
    processServerClient();
}

void checkWifiStatus()
{
  if(!cc3000.checkConnected())
  {
    error_led_status = LED_BLINK_SLOW;
    Serial.println("Wifi connection lost...");
    resetWifi();
    if(cc3000.checkConnected()) error_led_status = LED_OFF;
  }
}

void initaliseWiFi()
{
  /* Initialise the module */
  Serial.println(F("\nInitializing Wifi..."));
  if (!cc3000.begin())
  {
    Serial.println(F("Couldn't begin()!"));
    while(1);
  }
  
  char ssid[32] = "";
  char pwd[32] = "";
  int security = 0;
  
  readConfigStringVal(CFG_SSID, ssid);
  readConfigStringVal(CFG_PWD, pwd);
  readConfigIntVal(CFG_SECURITY, security);
  
  if (!cc3000.connectToAP(ssid, pwd, security)) {
    Serial.println(F("Failed!"));
    while(1);
  }
  
  Serial.println(F("Connected!"));
  
  /* Wait for DHCP to complete */
  Serial.println(F("Request DHCP"));
  while (!cc3000.checkDHCP())
  {
    delay(100); // ToDo: Insert a DHCP timeout!
  }  

  /* Display the IP address DNS, Gateway, etc. */  
  while (! displayConnectionDetails()) {
    delay(1000);
  }

/*
  ip = 0;
  // Try looking up the website's IP address
  Serial.print(WEBSITE); Serial.print(F(" -> "));
  while (ip == 0) {
    if (! cc3000.getHostByName(WEBSITE, &ip)) {
      Serial.println(F("Couldn't resolve!"));
    }
    delay(500);
  }
*/
  
  Serial.println(' ');
}

void InitialiseBtnIO()
{
  pinMode(led_1_pin, OUTPUT);
  pinMode(led_2_pin, OUTPUT);
  pinMode(led_3_pin, OUTPUT);
  
  pinMode(status_led_pin, OUTPUT);
  pinMode(error_led_pin, OUTPUT);
  
  pinMode(btn_1_pin, INPUT);
  pinMode(btn_2_pin, INPUT);
  pinMode(btn_3_pin, INPUT);
  pinMode(btn_4_pin, INPUT);
}

boolean updateButton(boolean btn, volatile int &btn_state, volatile unsigned long &btn_last, volatile int &chan_state, unsigned long current)
{
  int cur_state;
  unsigned long duration = current - btn_last;
  boolean retval = false;
  
  if(btn == LOW && btn_state == HIGH)
  {
    if(duration > DEBOUNCE_MILLS)
    {
      if(LONG_CLICK_MILLS > duration)
        cur_state = CLICK_SHORT;
      else
        cur_state = CLICK_LONG;
    
      if(chan_state == CLICK_NONE)
        chan_state = cur_state;
      else
        chan_state = CLICK_NONE;
        
      retval = true;
    }
  }
  
  if(btn != btn_state)
  {
    btn_state = btn;
    btn_last = current;
  }
  
  return retval;
}

void resetWifi()
{
  cc3000.disconnect();
  initaliseWiFi();
}

void sendUpdate()
{
  int old_led_status = status_led_status;
  status_led_status = LED_BLINK_FAST;
  
  Adafruit_CC3000_Client www = cc3000.connectTCP(ip, 80);
  //Serial.print("Free RAM: "); Serial.println(getFreeRam(), DEC);
  
  char host[32] = "";
  char webpage[32] = "";
  
  readConfigStringVal(CFG_HOST_NAME, host);
  readConfigStringVal(CFG_HOST_PATH_STATUS, webpage);
  //readConfigStringVal(CFG_HOST_PATH_REGISTER, value);
  if (www.connected()) {
    www.fastrprint(F("POST "));
    www.fastrprint(webpage);
    www.fastrprint(F(" HTTP/1.1\r\nAccept: */*\r\nHost: "));
    www.fastrprint(host); www.fastrprint(F("\r\n"));
    //POST
    www.fastrprint(F("Content-Length: 29\r\n"));
    www.fastrprint(F("Content-Type: application/x-www-form-urlencoded\r\n"));
    www.fastrprint(F("\r\n"));
    www.fastrprint(F("BUTTON1="));
    if(chan_1_state) www.fastrprint(F("1"));
    else www.fastrprint(F("0"));
    www.fastrprint(F("&BUTTON2="));
    if(chan_2_state) www.fastrprint(F("1"));
    else www.fastrprint(F("0"));
    www.fastrprint(F("&BUTTON3="));
    if(chan_3_state) www.fastrprint(F("1"));
    else www.fastrprint(F("0"));

    www.fastrprint(F("\r\n"));
    www.println();
  } else {
    Serial.println(F("Connection failed"));
    return;
  }
  
  /* Read data until either the connection is closed, or the idle timeout is reached. */ 
  String response = "";
  String body = "";
  int char_cursor = 0;
  int char_count = 0;
  boolean done = false;
  
  uint8_t data_length = -1;
  uint8_t method = 0;
  String page = "";
  unsigned long lastRead = millis();
  while (www.connected() && (millis() - lastRead < IDLE_TIMEOUT_MS) && !done)
  {
    while (www.available())
    {
      char c = www.read();
      if(char_cursor > -1) response += c;
      else 
      {
        body += c;
        if( data_length >= 0 && body.length() >= data_length) done = true;
      }
      //Serial.print(c);
      lastRead = millis();
      char_count = (char_count + 1) % 10;
      if(char_count > 0 && char_cursor > -1) char_cursor = processHeaders(response, body, char_cursor, data_length, method, page);
    }
    if(char_count > 0 && char_cursor > -1)
    {
      char_cursor = processHeaders(response, body, char_cursor, data_length, method, page);
      char_count = 0;
    }
  }
  
  Serial.println(body);
  www.close();
  
  //debugHTTP(data_length, method, page, body);
  
  status_led_status = old_led_status;
}

/**************************************************************************/
/*!
    @brief  Tries to read the IP address and other connection details
*/
/**************************************************************************/
bool displayConnectionDetails(void)
{
  uint32_t ipAddress, netmask, gateway, dhcpserv, dnsserv;
  
  if(!cc3000.getIPAddress(&ipAddress, &netmask, &gateway, &dhcpserv, &dnsserv))
  {
    Serial.println(F("Unable to retrieve the IP Address!\r\n"));
    return false;
  }
  else
  {
    Serial.print(F("\nIP Addr: ")); cc3000.printIPdotsRev(ipAddress);
    Serial.print(F("\nNetmask: ")); cc3000.printIPdotsRev(netmask);
    Serial.print(F("\nGateway: ")); cc3000.printIPdotsRev(gateway);
    Serial.print(F("\nDHCPsrv: ")); cc3000.printIPdotsRev(dhcpserv);
    Serial.print(F("\nDNSserv: ")); cc3000.printIPdotsRev(dnsserv);
    Serial.println();
    return true;
  }
}
