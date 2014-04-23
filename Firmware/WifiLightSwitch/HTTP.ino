
#define HTTP_NONE 0
#define HTTP_GET 1
#define HTTP_DELETE 2
#define HTTP_PUT 3
#define HTTP_POST 4

int processHeaders(String &response, String &body, int char_cursor, uint8_t &data_length, uint8_t &method, String &page)
{
  int line_begining = 0;
  int line_end = 0;
  int index = 0;
  boolean done = false;
  
  while(line_end < response.length() && !done)
  {
    index = response.indexOf('\r', char_cursor);
    
    if(index == -1)
      return char_cursor;
    else if(index + 1 == response.length())
      return char_cursor;
    else if(response.charAt(index + 1) == '\n')
    {
      line_end = index;
      if(line_begining == line_end)
        done = true;
      else
      {
        //Process header line here
        String line = response.substring(line_begining, line_end);
        Serial.println(line);
        

        if(line.substring(0, 3) == "GET")
        {
          method = HTTP_GET;
          extractPage(line, page);
        }
        else if(line.substring(0, 3) == "PUT")
        {
          method = HTTP_PUT;
          extractPage(line, page);
        }
        else if(line.substring(0, 4) == "POST")
        {
          method = HTTP_POST;
          extractPage(line, page);
        }
        else if(line.substring(0, 6) == "DELETE")
        {
          method = HTTP_DELETE;
          extractPage(line, page);
        }
        else if(line.substring(0, 15) == "Content-Length:")
        {
          String tmp = line.substring(16);
          data_length = tmp.toInt();
        }

        
        //Remove line from buffer
        response = response.substring(line_end + 2);
      
        line_begining = 0;
        char_cursor = 0;
        line_end = 0;
      }
    }
    else
      char_cursor = index + 1;
  }
  
  if(done)
  {
    body = response.substring(char_cursor + 2);
    return -1;
  }
  
  return char_cursor;
}

void extractPage(String &line, String &page)
{
  int first = line.indexOf(' ');
  page = line.substring(first+1, line.indexOf(' ',first + 1));
}

/*
void debugHTTP(uint8_t &data_length, uint8_t &method, String &page, String &data)
{
  Serial.println(F("-------------------------------------"));
  Serial.print("Method: ");
  switch(method)
  {
    case HTTP_POST: Serial.println("POST"); break;
    case HTTP_GET: Serial.println("GET"); break;
    case HTTP_PUT: Serial.println("PUT"); break;
    case HTTP_DELETE: Serial.println("DELETE"); break;
    case HTTP_NONE: Serial.println("NONE"); break;
  }
  Serial.print("Data Length: ");Serial.println(data_length);
  Serial.print("Page: ");Serial.println(page);
  Serial.print("Data: ");Serial.println(data);
  Serial.println(F("-------------------------------------"));
}
*/
