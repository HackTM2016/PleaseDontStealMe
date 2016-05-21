#include <Servo.h>


Servo servo_dreapta;
Servo servo_stanga;

//constante
const short vitezometru = 2;
const short servo_dreapta_pin = 4;
const short servo_stanga_pin = 5;
const short controlX = A0;
const short controlY = A1;
const short Toggle = 22;
const short ToggleStatus = 23;

const short RGB[2][3] = 
{
  {
    6,7,8
  },
  {
    11,12,3
  }
};

void displayAll(int r=0, int g=0, int b=0)
{
  r = 255-r;
  g = 255-g;
  b = 255-b;
  for(int i = 0;i<2;i++)
  {
    analogWrite(RGB[i][0], r);
    analogWrite(RGB[i][1], g);
    analogWrite(RGB[i][2], b);
  }
}


const unsigned short wheel = 2080;

void writeServos(int right=0, int left=0)
{
  servo_dreapta.write(90-5+right);
  servo_stanga.write(90-14-((float)left*0.85));
}

float vitezaCalc(int Timp)
{
  return (((float )wheel * 36) / (10 * (float)Timp));
}

void ServoSeq()
{
  writeServos();
  delay(500);
  for(int j = 0; j < 3; j++)
  {
    for(int i = 0;i<10;i++)
    {
      writeServos(i, i);
      delay(10);
    }
    for(int i = 10;i>0;i--)
    {
      writeServos(i, i);
      delay(10);
    }
  }
  writeServos();
  for(int i = 0;i<70;i++)
  {
    writeServos(i, i);
    delay(30);
  }
  writeServos();
}

//variabile
long lastClic = 0;
int clicTime = 0;
float viteza = 0;             // v = d/t
float viteza_anterioara = 0;  // pentru delta v
float acceleratie = 0;        // a =  v/t 

bool lastTypeClic = 1;
bool prevToggle = 1;
bool CLICKLOOP = 0;
bool MANUALFLAPS = 0;
bool MFToggle = 0;
bool ReturnToBase = 0;
bool ConnectedToBl = 0;

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  pinMode(vitezometru, INPUT_PULLUP);
  pinMode(Toggle, INPUT_PULLUP);
  pinMode(ToggleStatus, OUTPUT);
  for(int i = 0;i<2;i++)
  {
    for(int j = 0;j<3;j++)
    {
      pinMode(RGB[i][j], OUTPUT);
    }
  }
  
  servo_dreapta.attach(servo_dreapta_pin);
  servo_stanga.attach(servo_stanga_pin);

  ServoSeq();
  
}

void loop() {

  // Sectiunea de vitezometru
  if(Serial1.available() > 0)
  {
      str = Serial1.readStringUntil('~');
      Serial.println(str);
      if(str && !ConnectedToBl)
      {
        ConnectedToBl = 1;
      }
  }
  int clic = digitalRead(vitezometru);
  int ToggleVal = digitalRead(Toggle);
  clicTime = millis() - lastClic;
  
  
  if(clic == 0 && lastTypeClic == 1)
  {
    viteza = vitezaCalc(clicTime);
    lastClic = millis();
    lastTypeClic = clic;
    CLICKLOOP = 1; //IMPORTANT
  }
  else if(clic == 1 && lastTypeClic == 0)
    lastTypeClic = 1;
  if(clicTime > 2000)
    viteza = 0;

  // END Sectiunea de vitezometru

  //Sectiunea de Control Manual al FLAPS

  if(ToggleVal == 0 && prevToggle == 1)
  {
    MANUALFLAPS = !MANUALFLAPS;
    
    prevToggle = ToggleVal;
    if(MANUALFLAPS == 0)
    {
      ReturnToBase = 1;
    }
    else
      displayAll(0,0,255);
  }
  else if(ToggleVal == 1 && prevToggle == 0)
    prevToggle = 1;


  if(MANUALFLAPS)
  {
    int valoareX = analogRead(controlX);
    int writeValue = map(valoareX, 0, 1024, 75, 0);
    writeServos(writeValue, writeValue);
    digitalWrite(ToggleStatus, HIGH);
    
  }
  else
  {
    digitalWrite(ToggleStatus, LOW);
  }
  //END Sectiunea de Control Manual al FLAPS
  
  //Sectiunea de detectie de frana
  if(CLICKLOOP)
  {
    if(viteza != viteza_anterioara && ((viteza_anterioara - viteza) >= 4))
    {
      writeServos(75, 75);
      displayAll(255, 0, 0);
    }
    else if( ( (viteza_anterioara - viteza) < 4) && ( (viteza_anterioara - viteza) > 1.65) )
    {
      writeServos(38, 38);
      displayAll(128, 0, 0);
    }
    else
      displayAll(); 
  }
  if(!viteza && viteza_anterioara)
  {
    ReturnToBase = 1;
  }
  /*if(!viteza)
  {
    displayAll(128, 0, 0);
  }*/
  //END Sectiunea de detectie de frana
  
  Serial.println(viteza);
  /*Serial.print(" , ");
  Serial.print(viteza_anterioara);
  //Serial.print(" , ");
  //Serial.println(analogRead(A0));
  Serial.print(" , ");
  Serial.println(clic);
  */


  if(ReturnToBase)
  {
    writeServos();
    ReturnToBase = !ReturnToBase;
  }
  CLICKLOOP = 0;
  viteza_anterioara = viteza;
  delay(1); // "daca da eroare maresti delayu;" Pava -2016
}
