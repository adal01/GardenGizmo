#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <TimeLib.h>    // ALLEN:: TODO:: Get this library on Github
#include <WiFiUdp.h>

const char *ssid = "NETGEAR34";  
const char *password = "sunnyspider459";  
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0); 
//IPAddress subnet(255, 255, 248, 0); 
IPAddress ip(192,168,1,134);

const int CENTURY = 2000;
ESP8266WebServer server (80); // Creaet new instance and set the port of the web server.

//*********************** Light Class ***************************
class Light
{
  protected:
    public: enum eLightStateType {eOn, eOff, eBlinkOn, eBlinkOff};

    protected:
    eLightStateType m_eLightState;
    int m_iBlinkDelay = 500;

  public: Light()
  {
    m_eLightState = eOff;
  }
  
  public: virtual void On() = 0;
  public: virtual void Off() = 0;
  public: virtual void BlinkOn() = 0;  
};


//*********************** Green Light Class Start ***************************
class GreenLight : Light
{
  public: GreenLight()
  {
    // Setup the IO to Output for the light.
    pinMode(2,OUTPUT);  //Set GPIO2 to output mode.
    SetState(eOff);
  }

  void Service()
  { 
    if ( m_eLightState == eBlinkOn)
    {
      BlinkOn();
    }
    else if ( m_eLightState == eBlinkOff)
    {
      BlinkOff();
    }
    else if (m_eLightState == eOff)
    {
      Off();
    }
    else if (m_eLightState == eOn)
    {
      On();
    }
  }
  
  void SetState(eLightStateType eLightState)
  {
    m_eLightState = eLightState;
  }
  
  public: void BlinkOn()
  {
    SetState(eBlinkOn);
    digitalWrite(2,HIGH); // turn the LED on (GPIO2 Voltage High)
    Serial.println("Blink ON");
    
    delay(m_iBlinkDelay);
    Serial.println("Just finished blink delay");
    
    digitalWrite(2,LOW);  // turn the LED off (GPIO2 Voltage Low)
    Serial.println("Blink OFF");
    delay(m_iBlinkDelay);    
  }
  
  public: void BlinkOff()
  {
    SetState(eBlinkOff);
    digitalWrite(2,LOW);  // turn the LED off (GPIO2 Voltage Low)
    Serial.println("Blink OFF");
  }
  
  public: void On()
  { 
    SetState(eOn);
    digitalWrite(2,HIGH); // turn the LED on (GPIO2 Voltage High)
    Serial.println("Light ON");
  }
  
  public: void Off()
  {
    SetState(eOff);
    digitalWrite(2,LOW);  // turn the LED off (GPIO2 Voltage Low)
    Serial.println("Light OFF");
  }
  
};
//*********************** Green Light Class End ***************************


GreenLight cGreenLight;


/* ######***** setup() ***** */
void setup ( void ) {
 
   // Setup the IO
  pinMode(2,OUTPUT);  //Set GPIO2 to output mode.
  cGreenLight.Off();
  // ALLEN:: OUT:: Serial.begin ( 9600 );
  Serial.begin(115200);
  Serial.println ( "Hello World" );
  WiFi.begin ( ssid, password );
  Serial.println ( "" );
  WiFi.config(ip, gateway, subnet);

  // Wait for connection
  while ( WiFi.status() != WL_CONNECTED ) 
  {
    delay ( 500 );
  Serial.print ( "." );
  }

  Serial.println ( "" );
  Serial.print ("Connected to ");
  Serial.println ( ssid );
  Serial.print ( "IP address: " );
  Serial.print("I");
  Serial.println (WiFi.localIP());

  //Set up the HTTP Server
  if( MDNS.begin("esp8266")) 
  {
    Serial.println("MDNS responder started");
  }

  server.on ( "/", handleRoot );
  //server.on ( "/test.svg", drawGraph );
  server.on ( "/message", []() {server.send (200, "text/plain", "ESP8266 I'm Alive!!"); } );  // ALLEN:: UNCOMMENTED
  
  server.onNotFound (handleNotFound);
  server.begin();
  Serial.println ( "HTTP server started" );

  //Set up the NTP time service using UDP
  //Udp.begin(localPort);
  // Serial.print("Local port: ");
  //Serial.println(Udp.localPort());
  //Serial.println("waiting for sync");
  //setSyncProvider(getNtpTime);
  // setSyncInterval(3600);
  // DST = set_DST();
}


/* ######***** loop() ***** */
void loop() 
{
  server.handleClient();
  cGreenLight.Service();
}


/* ######***** handleRoot() ***** */
void handleRoot() {

  char temp[2000];
  String serialMsg = "";
  String dateBuffer = "none";
  String timeBuffer = "none";
  String amPm = "none";

  Serial.print("URI: ");
  Serial.println(server.uri());

//Get any arguments from the call
  if (server.args() > 0) 
  {
    for (int x=0; x < server.args(); x++) 
    {      
      if (server.argName(x).equals("LightOn")) 
      {
          cGreenLight.On();
          break;
      }
      else if (server.argName(x).equals("LightOff")) 
      {
          cGreenLight.Off();
           break;
      }   
      else if (server.argName(x).equals("BlinkOn"))
      {  
          cGreenLight.BlinkOn();  
          break;
      }
      else if (server.argName(x).equals("BlinkOff"))
      {
          cGreenLight.BlinkOff();
          break;
      }
    }
  }

  bool On;

    snprintf ( temp, 2000,
    "<html>\
      <head>\
        <title>Garden Gizmo</title>\
        <style>\
          body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
          div.absolute {\
            position: absolute;\
            top: 100px;\
            left: 400;\
          }\
        </style>\
      </head>\
<body>\
<form action='/' method='POST'><p><input type='hidden' name='LightOn' value='true'>\
        <input type='submit' value='Light On' style='width:120px;height:40px;'/></form>\ 
    <form action='/' method='POST'><p><input type='hidden' name='LightOff' value='true'>\
        <input type='submit' value='LightOff' style='width:120px;height:40px;'/></form>\
    <form action='/' method='POST'> <input type='hidden' name='BlinkOn' value='true'>\
        <input type='submit' value='Blink On' style='width:120px;height:40px;'/></form>\
    <form action='/' method='POST'> <input type='hidden' name='BlinkOff' value='true'>\
        <input type='submit' value='Blink Off' style='width:120px;height:40px;'/></form>\
</body>\
</html>"
    
  );
 
  server.send ( 200, "text/html", temp );
}


/* ######***** handleNotFound() ***** */
void handleNotFound() {

  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for ( uint8_t i = 0; i < server.args(); i++ ) {
    message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
  }

  server.send ( 404, "text/plain", message );
}










