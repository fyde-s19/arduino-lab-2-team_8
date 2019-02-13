// **************************************************************************
//
//      THIS CODE RUNS ON THE ESP8266 NOT ON THE MEGA!!
//
// **************************************************************************
//
//      This revision has the hardware handshake signals to interface 
//      to the Mega and TM4C123
//
// **************************************************************************

// Comment out the next line to disable verbose messages
// which will mess up the interface to the MEGA. Use only
// for debug on the 8266 programmer

//#define BLYNK_PRINT Serial

// ----------------------------------------------------------------------------
// DEFINEs section

#define DEBUG1 0                // First level of Debug
#define DEBUG2 0                // Second level of Debug
#define DEBUG3 0                // Third level of Debug
//#undef DEBUG1
//#undef DEBUG2
#undef DEBUG3

#define GPIO_0 0                // ACK signal from the MEGA
#define GPIO_2 2                // RDY signal to the MEGA
#define RDY 2
#define ACK 0

// ----------------------------------------------------------------------------
// INCLUDEs section

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <stdio.h>
#include <string.h>

// ----------------------------------------------------------------------------
// Variable assignments

int     Pin_Number  = 255;      // 255 is an invalid pin number
int     Pin_Integer = 0;
float   Pin_Float   = 1.234;
String  Pin_String  = "";       // Empty string

char    ssid[32]    = "EE-IOT-Platform-02"; 
char    pass[32]    = "dUQQE?&W44x7";

char    auth[256]   = "3249371193124d6ba26f0abe3f65ed2d";
char    http[1024]  = "blynk-cloud.com";
char    port[16]    = "8442";

// ----------------------------------------------------------------------------
// CODE section starts here

BlynkTimer timer;         // Start the timer

// ----------------------------------------------------------------------------
// This routine is called once a second to determine if the WiFi needs to be
// disconnected

void Disconnect_WiFi(void) {

  if (digitalRead(GPIO_0) == 0) { 
      WiFi.disconnect(); 
      #ifdef DEBUG1
      Serial.print("Disconnecting from WiFi");
      Serial.println();
      #endif
  }
}
// ----------------------------------------------------------------------------
// This routine sets up Wifi. First step is receive the SSID, Password and
// Authorization code using CSV format. Second step is to parse it and try
// to connect to the WiFi hotspot. Once the connection is made we then try to
// connect to Blynk server.
//

void Setup_Wifi(void) {
  
  char ser_buf[1024];
  static int bufpos = 0;              // starts the buffer back at the first position in the incoming serial.read

  WiFi.disconnect();                  // Disconnect the Wifi before re-enabling it

  delay(3000);                        // Wait for system to stabilize
  
  digitalWrite(RDY, HIGH);            // Set RDY to MEGA

  Serial.print("########");           // Indication to the Mega that Serial setup is ready
  Serial.println();                   // Mega waits for \n to continue


  while ((digitalRead(ACK) != 1) or (Serial.available() == 0)) 
    {       // Wait for MEGA to start sending data
    }
  
  while (Serial.available() > 0)   {

      char inchar = Serial.read();  // assigns one byte (as serial.read()'s only input one byte at a time

      if (inchar != '\n') {         // if the incoming character is not a newline then process byte
        ser_buf[bufpos] = inchar;   // the buffer position in the array get assigned to the current read
        bufpos++;                   // once that has happend the buffer advances, doing this over and over again until the end of package marker is read.
        delay(10);
      }
  } 

  // ----------------------------------------------------------------------------
  // This debug block double checks that the # of received 
  // characters matches the transmitted number
     
  #ifdef DEBUG1
  Serial.print("bufpos: ");
  Serial.println(bufpos);
  #endif

  // ----------------------------------------------------------------------------
  // Rip the 3 fields out of the CSV data. The sequence of data from the MEGA is:
  // authorization_code, ssid, password, 
  // Note: the string is terminate with a dangling comma
  //
  if (bufpos  > 0) {
    strcpy(auth, strtok(ser_buf, ","));
    strcpy(ssid, strtok(NULL, ","));  
    strcpy(pass, strtok(NULL, ","));
  }

  #ifdef DEBUG2

  Serial.print("Size of ssid: ");
  Serial.println(strlen(ssid));

  Serial.print("Size of pass: ");
  Serial.println(strlen(pass));

  Serial.print("Size of auth: ");
  Serial.println(strlen(auth));
  
  Serial.print("SSID: ");
  Serial.print(ssid);
  Serial.println();

  Serial.print("Password: ");
  Serial.print(pass);
  Serial.println();

  Serial.print("Auth: ");
  Serial.print(auth);
  Serial.println();

  Serial.print("Web page: ");
  Serial.print(http);
  Serial.println();
  #endif   

  // ----------------------------------------------------------------------------
  // Wait for the WiFi to connect. There is no timeout mechanism
  // so the timeout is up the Mega. The Mega would count the number
  // of '.' to determine when to reset the WiFi module
  //
  // Blynk.begin suppport 3 ways of specifying the website to connect to
  //      Blynk.begin(auth, ssid, pass);
  //      Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 8442);
  //      Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8442);
  
  WiFi.begin(ssid, pass);  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    
    if ((digitalRead(ACK) == 0))  { 
      ESP.restart(); 
    }
  }

  #ifdef DEBUG2
  Serial.println("");
  Serial.print("WiFi connected");  
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  
  #endif


  Blynk.config(auth);                 // in place of Blynk.begin(auth, ssid, pass);
  Blynk.connect(3333);                // timeout set to 10 seconds
  while (Blynk.connect() == false)    // Wait until connected or timeout occurs
  {     }

  digitalWrite(2, LOW);               // Negate READY to MEGA
  /*
  while ((digitalRead(0) != 0) or (Serial.available() == 0))  {       // Wait for ACK from MEGA before exiting setup
  }
*/
  
  #ifdef DEBUG1

  Serial.println("Connected to Blynk server");
  Serial.print("Leaving Setup_Wifi");
  Serial.println();
  #endif

  
}  // End of Setup_Wifi(void)

// --------------------------------------------------------
// This is the MACRO used to send control signals to the MEGA
// 

#define VirtualPin_Handler(pin_num) \
BLYNK_WRITE(pin_num)                \
{                                   \
  Pin_Number = pin_num;             \
  Pin_String  = param.asStr();      \
  Pin_Integer = param.asInt();      \
  Pin_Float   = param.asDouble();   \
  Serial.print(Pin_Number);         \
  Serial.print(",");                \
  Serial.print(Pin_Integer);        \
  Serial.print(",");                \
  Serial.print(Pin_Float);          \
  Serial.print("\n");               \
}                         


// ----------------------------------------------------------
// This section calls the Virtual Pin Handler (above) to
// determine which virtual pins on the Blynk App have been
// pressed. That information is then sent to the Mega.
//
// NOTE: Only 10 Virtual Pins are called. Add addtional pins
// as needed.

VirtualPin_Handler(V1)
VirtualPin_Handler(V2)
VirtualPin_Handler(V3)
VirtualPin_Handler(V4)
VirtualPin_Handler(V5)
VirtualPin_Handler(V6)
VirtualPin_Handler(V7)
VirtualPin_Handler(V8)
VirtualPin_Handler(V9)
VirtualPin_Handler(V10)


// ----------------------------------------------------------------------------
// This routine receives data from the Mega and parses it.
// The parsed data is then sent to the Blynk Server which 
// sends it to the Blynk App. 
// NOTE: Virtual pins from 50-89 are used to send data back
// to the Blynk app

void SerialInput_to_BlynkApp(void) { 
  while (Serial.available() > 0) {

    // look for the next valid integer in the incoming serial stream:

    int Pin_Number  = Serial.parseInt();
    int Pin_Integer = Serial.parseInt();
    float Pin_Float = Serial.parseFloat();
 
    // Look for the newline as the EOL indication
    if (Serial.read() == '\n') {

      #ifdef DEBUG3

      Serial.print("V");        
      Serial.print(Pin_Number);
      Serial.print(",");
      Serial.print(Pin_Integer);
      Serial.print(",");
      Serial.print(Pin_Float);
      Serial.print("\n");
      # endif

      // This section of code determines if integer or floating point
      // values are returned to the Blynk app. Virtual pins between 
      // 50 and 69 return floating point. Virtual pins between 70 and 
      // 89 return integers
      
      
      if((Pin_Number >49) && (Pin_Number < 70)) {
        Blynk.virtualWrite(Pin_Number, Pin_Float );
      }
      else if((Pin_Number >69) && (Pin_Number < 90)) {
        Blynk.virtualWrite(Pin_Number, Pin_Integer );
      }
      else {
        Blynk.virtualWrite(Pin_Number, "Help");
      }
    }
  }
}


// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// 
// THIS IS THE MAIN SETUP CODE. IT IS ONLY RUN ONCE
//
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void setup() {


Serial.begin(9600);               // Set up debug serial port
Serial.flush();                   // Flush the serial port

pinMode(GPIO_0, INPUT_PULLUP);    // Set GPIO_0 to an input  - ACK signal from the MEGA
pinMode(GPIO_2, OUTPUT);          // Set GPIO_2 to an output - RDY signal to the MEGA

digitalWrite(RDY, LOW);             // Set the GPIO_2 pin LOW

Setup_Wifi(); // This routine reads in the new SSID, Password and Authorization code


  
timer.setInterval(1000L, SerialInput_to_BlynkApp);      // This is a scheduler to read serial data
//timer.setInterval(1000L, Disconnect_WiFi);            // This looks for a 1->0 transistion on the GPIO0 Pin
                                                        // Disconnect the WiFi if GPIO0==0
  
}  // End of setup()
 

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// 
// THIS IS THE MAIN LOOP. IT RUNS CONTINOUSLY
//
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void loop()
{
  Blynk.run();
  timer.run();
  
  // You can inject your own code or combine it with other sketches.
  // Check other examples on how to communicate with Blynk. Remember
  // to avoid delay() function!
  
}   // End of loop()


