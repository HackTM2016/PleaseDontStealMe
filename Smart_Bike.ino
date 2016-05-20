
//constante
const short vitezometru = 2;
const short Stop = 3;


const unsigned short wheel = 2080;

float vitezaCalc(int Timp)
{
  return (((float )wheel * 36) / (10 * (float)Timp));
}

//variabile
long lastClic = 0;
int clicTime = 0;
float viteza = 0;             // v = d/t
float viteza_anterioara = 0;  // pentru delta v
float acceleratie = 0;        // a =  v/t 

bool lastTypeClic = 1;
bool CLICKLOOP = 0;

void setup() {
  Serial.begin(9600);
  pinMode(vitezometru, INPUT_PULLUP);
  pinMode(Stop, OUTPUT);
}

void loop() {

  // Sectiunea de vitezometru
  
  int clic = digitalRead(vitezometru);
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
  {
    lastTypeClic = 1;
  }
  if(clicTime > 3000)
  {
    viteza = 0;
  }

  // END Sectiunea de vitezometru

  //Sectiunea de detectie de frana
  if(CLICKLOOP)
  {
    if(viteza != viteza_anterioara && (viteza_anterioara - viteza) > 6)
    {
      analogWrite(Stop, 255);
    }
    else if( (viteza - viteza_anterioara <= 6) && (viteza - viteza_anterioara > 0) )
    {
      analogWrite(Stop, 128);
    }
    else
    {
      analogWrite(Stop, 0);
    }
  
    viteza_anterioara = viteza;
  }
  //END Sectiunea de detectie de frana
  
  Serial.print(viteza);
  Serial.print(" , ");
  Serial.println(clic);
  
  CLICKLOOP = 0;
  
  delay(1); // "daca da eroare maresti delayu;" Pava -2016
}
