#include <Servo.h>
Servo servo_dreapta;
Servo servo_stanga;

//constante
const short vitezometru = 2;
const short Stop = 3;
const short servo_dreapta_pin = 4;
const short servo_stanga_pin = 5;
const short controlX = A0;
const short controlY = A1;
const short Toggle = 22;
const short ToggleStatus = 23;



const unsigned short wheel = 2080;

void writeServos(int right=0, int left=0)
{
  servo_dreapta.write(90-5+right);
  servo_stanga.write(90-13-left);
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

void setup() {
  Serial.begin(9600);
  pinMode(vitezometru, INPUT_PULLUP);
  pinMode(Toggle, INPUT_PULLUP);
  pinMode(Stop, OUTPUT);
  pinMode(ToggleStatus, OUTPUT);
  
  servo_dreapta.attach(servo_dreapta_pin);
  servo_stanga.attach(servo_stanga_pin);

  ServoSeq();
  
}

void loop() {

  // Sectiunea de vitezometru
  
  int clic = digitalRead(vitezometru);
  int ToggleVal = digitalRead(Toggle);
  clicTime = millis() - lastClic;
  
  
  if(clic == 0 && lastTypeClic == 1)
  {
    viteza = vitezaCalc(clicTime);
    lastClic = millis();
    clicTime = 0;
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
    writeServos();
  }
  //END Sectiunea de Control Manual al FLAPS
  
  //Sectiunea de detectie de frana
  if(CLICKLOOP || !viteza)
  {
    if(viteza != viteza_anterioara && ((viteza_anterioara - viteza) >= 6))
      analogWrite(Stop, 255);
    else if( ( (viteza_anterioara - viteza) <= 6) && ( (viteza_anterioara - viteza) > 2) )
      analogWrite(Stop, 128);
    else if(!viteza)
      analogWrite(Stop, 128);
    else
      analogWrite(Stop, 0);
  
    viteza_anterioara = viteza;
  }
  //END Sectiunea de detectie de frana
  
  Serial.print(viteza);
  Serial.print(" , ");
  Serial.print(viteza_anterioara);
  //Serial.print(" , ");
  //Serial.println(analogRead(A0));
  Serial.print(" , ");
  Serial.println(clic);
  
  CLICKLOOP = 0;
  
  delay(1); // "daca da eroare maresti delayu;" Pava -2016
}
