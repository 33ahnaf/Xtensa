#include <Arduino.h>
#include <WiFi.h>
#include <NetworkClient.h>
#include <WiFiAP.h>

#define L1 13
#define L2 12
#define L3 14
#define L4 27
#define L5 26
#define L6 25
#define L7 33
#define L8 32
#define L9 22
#define L10 23

int LED_PINs[10] = {L1, L2, L3, L4, L5, L6, L7, L8, L9, L10};
const char *ssid = "Im ESP32";
const char *password = "esp323232";

NetworkServer server(80);

void setup()
{
  for (unsigned short i = 0; i < 10; i++)
  {
    pinMode(LED_PINs[i], OUTPUT);
  }

  Serial.begin(9600);
  Serial.println();
  Serial.println("Configuring access point...");

  // You can remove the password parameter if you want the AP to be open.
  // a valid password must have more than 7 characters
  if (!WiFi.softAP(ssid, password))
  {
    log_e("Soft AP creation failed.");
    while (1)
      ;
  }
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.begin();

  Serial.println("Server started");
}

void loop()
{
  NetworkClient client = server.accept(); // listen for incoming clients

  if (client)
  {                                // if you get a client,
    Serial.println("New Client."); // print a message out the serial port
    String currentLine = "";       // make a String to hold incoming data from the client
    while (client.connected())
    { // loop while the client's connected
      if (client.available())
      {                         // if there's bytes to read from the client,
        char c = client.read(); // read a byte, then
        Serial.write(c);        // print it out the serial monitor
        if (c == '\n')
        { // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0)
          {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            client.print("<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>ESP32 Switching</title></head><body><h2>Click <a href=\"H1\">here</a> to turn ON LED 1\"</h2><br><h2>Click <a href=\"H2\">here</a> to turn ON LED 2\"</h2><br><h2>Click <a href=\"H3\">here</a> to turn ON LED 3\"</h2><br><h2>Click <a href=\"H4\">here</a> to turn ON LED 4\"</h2><br><h2>Click <a href=\"H5\">here</a> to turn ON LED 5\"</h2><br><h2>Click <a href=\"H6\">here</a> to turn ON LED 6\"</h2><br><h2>Click <a href=\"H7\">here</a> to turn ON LED 7\"</h2><br><h2>Click <a href=\"H8\">here</a> to turn ON LED 8\"</h2><br><h2>Click <a href=\"H9\">here</a> to turn ON LED 9\"</h2><br><h2>Click <a href=\"H10\">here</a> to turn ON LED 10\"</h2><br><h2>Click <a href=\"L1\">here</a> to turn OFF LED 1\"</h2><br><h2>Click <a href=\"L2\">here</a> to turn OFF LED 2\"</h2><br><h2>Click <a href=\"L3\">here</a> to turn OFF LED 3\"</h2><br><h2>Click <a href=\"L4\">here</a> to turn OFF LED 4\"</h2><br><h2>Click <a href=\"L5\">here</a> to turn OFF LED 5\"</h2><br><h2>Click <a href=\"L6\">here</a> to turn OFF LED 6\"</h2><br><h2>Click <a href=\"L7\">here</a> to turn OFF LED 7\"</h2><br><h2>Click <a href=\"L8\">here</a> to turn OFF LED 8\"</h2><br><h2>Click <a href=\"L9\">here</a> to turn OFF LED 9\"</h2><br><h2>Click <a href=\"L10\">here</a> to turn OFF LED 10\"</h2><br></body></html>");

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          }
          else
          { // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        }
        else if (c != '\r')
        {                   // if you got anything else but a carriage return character,
          currentLine += c; // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /H1"))
        {
          digitalWrite(L1, HIGH); // GET /H turns the LED on
        }
        if (currentLine.endsWith("GET /L1"))
        {
          digitalWrite(L1, LOW); // GET /L turns the LED off
        }
        if (currentLine.endsWith("GET /H2"))
        {
          digitalWrite(L2, HIGH); // GET /H turns the LED on
        }
        if (currentLine.endsWith("GET /L2"))
        {
          digitalWrite(L2, LOW); // GET /L turns the LED off
        }
        if (currentLine.endsWith("GET /H3"))
        {
          digitalWrite(L3, HIGH); // GET /H turns the LED on
        }
        if (currentLine.endsWith("GET /L3"))
        {
          digitalWrite(L3, LOW); // GET /L turns the LED off
        }
        if (currentLine.endsWith("GET /H4"))
        {
          digitalWrite(L4, HIGH); // GET /H turns the LED on
        }
        if (currentLine.endsWith("GET /L4"))
        {
          digitalWrite(L4, LOW); // GET /L turns the LED off
        }
        if (currentLine.endsWith("GET /H5"))
        {
          digitalWrite(L5, HIGH); // GET /H turns the LED on
        }
        if (currentLine.endsWith("GET /L5"))
        {
          digitalWrite(L5, LOW); // GET /L turns the LED off
        }
        if (currentLine.endsWith("GET /H6"))
        {
          digitalWrite(L6, HIGH); // GET /H turns the LED on
        }
        if (currentLine.endsWith("GET /L6"))
        {
          digitalWrite(L6, LOW); // GET /L turns the LED off
        }
        if (currentLine.endsWith("GET /H7"))
        {
          digitalWrite(L7, HIGH); // GET /H turns the LED on
        }
        if (currentLine.endsWith("GET /L7"))
        {
          digitalWrite(L7, LOW); // GET /L turns the LED off
        }
        if (currentLine.endsWith("GET /H8"))
        {
          digitalWrite(L8, HIGH); // GET /H turns the LED on
        }
        if (currentLine.endsWith("GET /L8"))
        {
          digitalWrite(L8, LOW); // GET /L turns the LED off
        }
        if (currentLine.endsWith("GET /H9"))
        {
          digitalWrite(L9, HIGH); // GET /H turns the LED on
        }
        if (currentLine.endsWith("GET /L9"))
        {
          digitalWrite(L9, LOW); // GET /L turns the LED off
        }
        if (currentLine.endsWith("GET /H10"))
        {
          digitalWrite(L10, HIGH); // GET /H turns the LED on
        }
        if (currentLine.endsWith("GET /L10"))
        {
          digitalWrite(L10, LOW); // GET /L turns the LED off
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}
