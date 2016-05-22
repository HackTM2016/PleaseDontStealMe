#include <Servo.h>

long long systemClickTime = 0;
int DELAYTIME = 1;
//                                                                       A SI VEZI CA NE TREBE SI FRONT LIGHT
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
//de adaugat cand terminam                                                             MA COAE ADAUGA AICi
const short semnalizat_dr=35;
const short semnalizat_stg=47;



const short RGB[2][3] = 
{
  {
    6,7,8
  },
  {
    11,12,3
  }
};
//



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

void displayDr(int r=0, int g=0, int b=0)
{
  r = 255-r;
  g = 255-g;
  b = 255-b;
    analogWrite(RGB[0][0], r);
    analogWrite(RGB[0][1], g);
    analogWrite(RGB[0][2], b);
}

void displayStg(int r=0, int g=0, int b=0)
{
  r = 255-r;
  g = 255-g;
  b = 255-b;
    analogWrite(RGB[1][0], r);
    analogWrite(RGB[1][1], g);
    analogWrite(RGB[1][2], b);
}

void displayRandom()
{
  displayAll(random(0, 255), random(0, 255), random(0, 255));
}

const unsigned short wheel = 2080;

void writeServos(int right=0, int left=0)
{
  servo_dreapta.write(90-5+right);
  servo_stanga.write(90-14-((float)left*0.85));
}

void writeDr(int right=0)
{
  servo_dreapta.write(90-5+right);
}

void writeStg(int left=0)
{
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
void semn(char a, int timp, int epoca){                                                                        // VERIFICA
  if(a=='d'){
      if(timp%epoca>=epoca/2)
        displayDr(255,100,0);
      else
        displayDr();
  }
  else{
      if(timp%epoca>=epoca/2)
        displayStg(255,100,0);
      else
        displayStg();
  }
}

void semnAll(int timp, int epoca){                                                                        // VERIFICA
  if(timp%epoca>=epoca/2)
    displayAll(255,100,0);
  else
    displayAll();
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
bool stgTog=0, drTog=0;
bool lastStg = 1, lastDr = 1;

void setup() {
  Serial.begin(9600);
  pinMode(vitezometru, INPUT_PULLUP);
  pinMode(Toggle, INPUT_PULLUP);
  pinMode(ToggleStatus, OUTPUT);
  
  pinMode(semnalizat_stg,INPUT_PULLUP);
  pinMode(semnalizat_dr, INPUT_PULLUP);
   
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
  systemClickTime++;

  // Sectiunea de vitezometru
  
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
      displayRandom();
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
  //cod pt semnalizat                                                                 ASTA TEORETIC DOAR VERIFICA
  int stg=digitalRead(semnalizat_stg);
  int dr=digitalRead(semnalizat_dr);
  Serial.print(stg);
  Serial.print("-");
  Serial.println(dr);
  Serial.print("+");
  if(stg==0&&lastStg==1)
  {
    stgTog = !stgTog;
    if(stgTog == 0)
    {
      writeStg();
      displayStg();
    }
    else if (drTog)
    {
      displayAll();
      writeStg(75);
    }
    else
    {
      displayAll();
      writeDr();
      writeStg(75);
    }
  }
  if(dr==0&&lastDr==1)
  {
    drTog = !drTog;
    if(drTog == 0)
    {
      writeDr();
      displayDr();
    }
    else if (stgTog)
    {
      displayAll();
      writeDr(75);
    }
    else
    {
      displayAll();
      writeStg();
      writeDr(75);
    }
  }

  lastStg = stg;
  lastDr = dr;

  if(drTog&&stgTog)
  {
    semnAll(systemClickTime, (500/DELAYTIME)/8);
  }
  else if(stgTog)
  {
    semn('s', systemClickTime, (500/DELAYTIME)/8);
  }
  else if(drTog)
  {
    semn('d', systemClickTime, (500/DELAYTIME)/8);
  }
  
  
  if(ReturnToBase)
  {
    writeServos();
    ReturnToBase = !ReturnToBase;
    displayAll();
  }
  CLICKLOOP = 0;
  viteza_anterioara = viteza;
  
  
  
  delay(DELAYTIME); // "daca da eroare maresti delayu;" Pava -2016
}
