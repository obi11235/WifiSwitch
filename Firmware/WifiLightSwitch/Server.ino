#include "utility/socket.h"

#define LISTEN_PORT 80

Adafruit_CC3000_Server webServer(LISTEN_PORT);

void setupWebServer()
{
  webServer.begin();
}

void processServerClient()
{
  Adafruit_CC3000_ClientRef client = webServer.available();
  if (client) {
    int status_led_orig = status_led_status;
    status_led_status = LED_BLINK_FAST;

    String buffer = "";
    String data = "";
    int char_cursor = 0;
    int char_count = 0;
    
    uint8_t data_length = 0;
    uint8_t method = 0;
    String page = "";
    while(client.available() > 0)
    {
      char c = client.read();
      if(char_cursor > -1) buffer += c;
      else data += c;
      //Serial.print(c);
      char_count = (char_count + 1) % 10;
      if(char_count > 0 && char_cursor > -1) char_cursor = processHeaders(buffer, data, char_cursor, data_length, method, page);
    }

    //debugHTTP(data_length, method, page, data);
    
    boolean error = false; 
    String output_buffer = "";
    if(page == "/status" && method == HTTP_GET)
      error = !statusPage(output_buffer);
    else if(page.substring(0,7)  == "/button" && method == HTTP_PUT)
      error = !updatePage(page, output_buffer, data);
    else
      error = true;
    
    String content_length = String(output_buffer.length());

    if(!error)
      client.fastrprint(F("HTTP/1.1 200 OK\r\n"));
    else
      client.fastrprint(F("HTTP/1.1 404 Not Found\r\n"));
    
    //client.fastrprint(F("Date: Mon, 21 Apr 2014 19:43:46 GMT\r\n"));
    client.fastrprint(F("Server: AVR\r\nCache-Control: no-store, no-cache, must-revalidate, post-check=0, pre-check=0\r\n"));
    client.fastrprint(F("Pragma: no-cache\r\n"));
    client.fastrprint(F("Content-Length: "));
    outputStringObj(client, content_length);
    client.fastrprint(F("\r\nContent-Type: text/html\r\n\r\n"));
    outputStringObj(client, output_buffer);
    
    buffer = NULL;
    status_led_status = status_led_orig;
  }
}

boolean statusPage(String &page)
{
  page += "BUTTON1=";
  if(chan_1_state) page.concat("1\n");
  else page.concat("0\n");
  page += "BUTTON2=";
  if(chan_2_state) page.concat("1\n");
  else page.concat("0\n");
  page += "BUTTON3=";
  if(chan_3_state) page.concat("1\n");
  else page.concat("0\n");
  
  return true;
}

boolean updatePage(String &page, String &output, String &data)
{
  if(page.length() == 7 || page == "/button/")
  {
    return false;
  }
  else if(page.length() == 9 && page.substring(0,8) == "/button/")
  {
    char btn = page.charAt(8);
    if(data.length() != 8) return false;
    
    if(data.substring(0,7) != "BUTTON=") return false;
 
    int state;
    switch(data.charAt(7))
    {
      case '0' : state = CLICK_NONE; break;
      case '1' : state = CLICK_SHORT; break;
      case '2' : state = CLICK_LONG; break;
      default : return false; break;
    }
 
    switch(btn)
    {
      case '1':
        chan_1_state = state; 
        if(mode == MODE_EXCLUSIVE)
        {
          chan_2_state = CLICK_NONE;
          chan_3_state = CLICK_NONE;
        }
        break;
      case '2':
        chan_2_state = state; 
        if(mode == MODE_EXCLUSIVE)
        {
          chan_1_state = CLICK_NONE;
          chan_3_state = CLICK_NONE;
        }
        break;
      case '3':
        chan_3_state = state; 
        if(mode == MODE_EXCLUSIVE)
        {
          chan_1_state = CLICK_NONE;
          chan_2_state = CLICK_NONE;
        }
        break;
      default : return false; break;
    }
    
    if(chan_1_state > 0) led_1_state = LED_ON; else led_1_state = LED_OFF;
    if(chan_2_state > 0) led_2_state = LED_ON; else led_2_state = LED_OFF;
    if(chan_3_state > 0) led_3_state = LED_ON; else led_3_state = LED_OFF;
  }
  else
    return false;
  
  return true;
}

void outputStringObj(Adafruit_CC3000_ClientRef &client, String &buff)
{
  for (int c = 0; c < buff.length(); c++)
    client.write(buff.charAt(c));
}
