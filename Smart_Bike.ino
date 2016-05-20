
//constante
int vitezometru = 2;


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

void setup() {
  Serial.begin(9600);
  pinMode(vitezometru, INPUT_PULLUP);
}

void loop() {
  int clic = digitalRead(vitezometru);
  clicTime = millis() - lastClic;
  if(clic == 0 && lastTypeClic == 1)
  {
    viteza = vitezaCalc(clicTime);
    lastClic = millis();
    clicTime = 0;
    lastTypeClic = clic;
  }
  else if(clic == 1 && lastTypeClic == 0)
  {
    lastTypeClic = 1;
  }
  if(clicTime > 3000)
  {
    viteza = 0;
  }

  
  Serial.print(viteza);
    Serial.print(" , ");
    Serial.println(clic);
  delay(1); // "daca da eroare maresti delayu;" Pava -2016
}
