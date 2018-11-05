#include <SoftwareSerial.h>     //Communications Library for Serial Monitor and Bluetooth applications.
#include <SPI.h>               //From line 2-3 are necessary Pixy Libraries
#include <Pixy.h>  
           
SoftwareSerial BTserial(8, 11); // RX | TX

Pixy heart;                     // Name for Pixy input data (For this project we called it "Heart" since it's the main component for our project.)

bool debug=false;                                      //Switch ON debug mode if "1",Switches OFF if "0"
bool remote=false;                                     //Switch ON remote mode if "1",Switches OFF if "0"
int count = 0;                                         //represents the debug counter
int MP1 = 2,MP2 = 4,MP3 = 7,MP4 = 9,Speed= 10;         //H Bridge Pin Assignments
int RLED=3;                                            //Diagnostics LED
int GLED=5;
int BLED =6; 
int id;                                                //Pixy Cam Object ID
static int i= 0;                                       //Used to obtain data from Pixy Cam
unsigned int x , y , width=0 , height=0;               //Holds Pixy Obj specific data for processing
char monit = ' ',phone;                                //This is a Buffer for the serial monitor and bluetooth
double prevvalue=0,value=0;                            //This line and the following line are used to correct an error caused by the PixyCam
double errorcorrection=0;                              //where the last seen object's data gets continuously sent to the Arduino when the object is no longer visible.

void setup() 
  {
  BTserial.begin(9600);                                 //Sets our BTserial monitor to output at a 9600 Baud Rate
  Serial.begin(9600);                                   //Sets our serial monitor to output at a 9600 Baud Rate
  pinMode(MP1, OUTPUT);                                 //From lines 27-33 sets H Bridge's Pinmodes 
  pinMode(MP2, OUTPUT);
  pinMode(MP3, OUTPUT);
  pinMode(MP4, OUTPUT);
  pinMode(RLED,OUTPUT);
  pinMode(GLED,OUTPUT); 
  pinMode(BLED,OUTPUT);
  heart.init();                                         //Initializes our Pixy Camera
  Serial.println("Booting up the system. Please wait....");
  void stopall();                      //From line 36-45 Declares a function for Arduino to use
  void obtain();
  void boot();
  void forward();
  void backward();
  void right();
  void left();
  void bluetooth_remote_mode();
  void phoneinput();
  void search();
  boot();
  obtain();
  }
  
void loop() 
  {
  phoneinput();                         //retrieves an input from the phone using bluetooth
  Serial.available();                   //checks the serial monitor for an entered command/text
  monit=Serial.read();                  //sets monit to the data read form the serial monitor
  if (phone=='Y'||monit=='d')           //Test whether it's able to activate/deactivate debug mode
  {
  debug=true;
  digitalWrite(RLED,LOW);
  }
  else if (phone=='A'||monit=='o')
  {
  debug=false;
  digitalWrite(RLED,HIGH);
  }
  if (phone=='X'||monit=='r')           //Test whether it's able to activate/deactivate remote mode
  {
  remote=true;
  Serial.println("REMOTE ON");
  digitalWrite(BLED,LOW);
  digitalWrite(GLED,HIGH);
  }
  else if (phone=='B'||monit=='a')
  {
  remote=false;
  Serial.println("REMOTE OFF");
  digitalWrite(BLED,HIGH);
  digitalWrite(GLED,LOW);
  }
  if (remote==true)                     //Test whether it's able to go into remote mode or automatic mode
  bluetooth_remote_mode();
   
  else
  {
    obtain();
    int area=width*height;
          
    if ((x<=100)&&(x!=0))                  //Threshold for left movement
      {
      left();
      }
    else if(x >= 220)                      //Threshold for right movement
      {
      right();
      }  
    else                                   //STOP
      {
      stopall();
      }  
    delay(80);

    if (  area<=2000 && area!=0 )          //Threshold for forward movement
      {
              
        forward();
      
      }
    else if (area>=10000 || id == 2)       //Threshold for backward movement
      { 
            
      backward();
      }   
    else                                   //STOP
      {
      stopall();
      }
    delay(175);     
  }
  
}
void boot()                             //Runs a quick diagnostic test with our device to make- 
  {                                     //sure all motors and sensors are operating correctly.
  stopall();                            //Blinking Light Symbolizes the progress of the test
  debug=true;
  digitalWrite(RLED,HIGH);
  digitalWrite(GLED,HIGH);
  digitalWrite(BLED,HIGH);
  digitalWrite (RLED, LOW);
  delay(200);
  digitalWrite(RLED,HIGH);
  digitalWrite (GLED, LOW);
  delay(200);
  digitalWrite(GLED,HIGH);
  digitalWrite (BLED, LOW);
  delay(200);
  digitalWrite(BLED,HIGH);
  for ( int c=0;c<=7;c++)
  {  
    for (int l = 255; l >= 0; l--)
      {
      analogWrite (RLED, l);            //Pulsing RLED, GLED, BLED 
      analogWrite (GLED, l);
      analogWrite (BLED, l);
      delay(1);
      }
    for (int l = 0; l <= 255; l++)
      {
      analogWrite (RLED, l);            //Pulsing RLED, GLED, BLED 
      analogWrite (GLED, l);
      analogWrite (BLED, l);
      delay(1);
      }
      if (c==0)
      forward();
      if (c==1)
      backward();
      if (c==2)
      left();
      if (c==3)
      right();
      if (c==4)
      stopall();
      
    }
  Serial.println("Finished Booting\nENTER \"d\" to enable debug mode\nENTER \"r\" to enable remote mode\nENTER \"o\" to turn off debug mode\nENTER \"a\" to turn off remote mode");
  debug=false;
  analogWrite(GLED,LOW);
  }
  
void forward()                       //Make vehical go forward
  { 
  digitalWrite(MP1, LOW);                
  digitalWrite(MP2, HIGH);
  digitalWrite(MP3, LOW);
  digitalWrite(MP4, HIGH);
  analogWrite(Speed,200);
  if(debug==true)                    //Prints out following debug info in serial monitor
    Serial.println("Forward");
  }
  
void backward()                      //Make vehical go backward
  { 
  digitalWrite(MP1, HIGH);                
  digitalWrite(MP2, LOW);
  digitalWrite(MP3, HIGH);
  digitalWrite(MP4, LOW);
  analogWrite(Speed,200);
  if(debug==true)                    //Prints out following debug info in serial monitor
  Serial.println("Backward");
  }

void left()                          //Make vehical go left
  {
  digitalWrite(MP1, HIGH);                
  digitalWrite(MP2, LOW);
  digitalWrite(MP3, LOW);
  digitalWrite(MP4, HIGH);
  analogWrite(Speed,235);
  if(debug==true)                    //Prints out following debug info in serial monitor
  Serial.println("Left");
  }

void right()                         //Make vehical go right
  {
  digitalWrite(MP1, LOW);                
  digitalWrite(MP2, HIGH);
  digitalWrite(MP3, HIGH);
  digitalWrite(MP4, LOW);
  analogWrite(Speed,235);
  if(debug==true)                    //Prints out following debug info in serial monitor
  Serial.println("Right");
  }
  
void stopall()                       //Make vehical STOP
  {
  digitalWrite(MP1, LOW);
  digitalWrite(MP2, LOW);
  digitalWrite(MP3, LOW);
  digitalWrite(MP4, LOW);
  if(debug==true)                    //Prints out following debug info in serial monitor
  Serial.println("Stop");
  }
  
void obtain()                         //Gets Data from the Pixy Cam to use for object tracking.
  {                      
  uint16_t blocks=heart.getBlocks();  //creates a structure called "blocks" that will hold multiple types of data from the Pixy Cam
  
  id=heart.blocks[i].signature;       //Sets "id" to Pixy's signiture
  x=heart.blocks[i].x;                //Sets "x" to Pixy's object x coordinate
  y=heart.blocks[i].y;                //Sets "y" to Pixy's object y coordinate
  width=heart.blocks[i].width;        //Sets "width" to Pixy's object width
  height=heart.blocks[i].height;      //Sets "height" to Pixy's object height
  value=x+y+width+height;
  if (id<0||id>7)
    {
    id=0;
    x=0;
    y=0;
    width=0;
    height=0;
    if (debug==true)
      Serial.println("READ-ERROR");  
    }
  if (prevvalue==value)
  {
    errorcorrection++;
    if (errorcorrection>=2)
    {
      if(debug==true)
        Serial.println("NO OBJECT IN SIGHT");
      id=0;
      x=0;
      y=0;
      width=0;
      height=0;
    }
  }
  else
  errorcorrection=0;
  prevvalue=value;
  count++;
  if (debug==true && count >= 1)                    //Prints out following debug info in serial monitor
    {
    Serial.print("id ");
    Serial.print(id);
    Serial.print(", x ");
    Serial.print(x);
    Serial.print(", y ");
    Serial.print(y);
    Serial.print(", width ");
    Serial.print(width);
    Serial.print(", height ");
    Serial.print(height);
    Serial.print(", area ");
    Serial.println(height*width);
    count = 0;
    }
  }

void bluetooth_remote_mode()                         //Runs the remote mode that will get turn by turn directions from the phone thorugh bluetooth.
  {
  phoneinput();
  if ((phone=='1')||(phone=='2')||(phone=='3')||(phone=='4')||(phone=='?')){
    switch (phone)
    {
    case '1':
    { 
    forward();
    break;
    }
    case '2':
    { 
    backward();
    break;
    }
    case '3':
    {
    left();
    break;
    }
    case '4':
    {
    right();
    break;
    }
    case '?':
    {
    stopall();
    break;
    }
    default:
    {
    stopall();
    break;
    }
 }
 }
  else
    {
    stopall();
    delay(10);
    }
  phone=' ';
  delay(100);  
  }

void phoneinput()               //retrieves an input from the phone using bluetooth
  {
  if (BTserial.available())     //BTserial checks if serial is available
    {   
    phone = BTserial.read();    //takes the input from the bluetoth and puts it into [hone buffer
    if(debug==true)
    Serial.write(phone);        //outputs the input from the phone into serial monitor
    }    
  }









