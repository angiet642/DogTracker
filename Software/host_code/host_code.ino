// for forwarding run ./ngrok tcp 172.20.10.5:8888 --authtoken <token>
// Load Wi-Fi library
#include <WiFi.h>

// Replace with your network credentials
const char* ssid = "SSID";
const char* password = "PASSWORD";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;


void setup() {
  Serial.begin(115200);
  
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients
  
    if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) 
    {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            client.println("<!DOCTYPE html>\n");
            client.println("<html>\n");
            client.println("<body>\n");
            client.println("<center>\n");
            client.println("<h1>Kylee & Angie's Dawg Tracker </h1>\n");
            client.println("<div id=\"googleMap\" style=\"width:100%;height:400px;\"></div>\n");
            client.println("<script>\n");
            client.println("function myMap() {\n");
            client.println("var mapProp= {\n");
            client.println("center:new google.maps.LatLng(51.508742,-0.120850),\n");
            client.println("zoom:5,\n");
            client.println("};\n");
            client.println("var map = new google.maps.Map(document.getElementById(\"googleMap\"),mapProp);\n");
            client.println("}\n");
            client.println("</script>\n");
            client.println("<script src=\"https://maps.googleapis.com/maps/api/js?key=<PUT KEY HERE>&callback=myMap\"></script>\n");
            client.println("</center>\n");
            client.println("</body>\n");
            client.println("</html>");
            break;
           
          } 
          else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
