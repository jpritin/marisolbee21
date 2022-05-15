/*
 * Marisol.bee - Bicicleta eléctrica
 */
#include <Arduino.h>
#include <SimpleTimer.h>

// Pins
const unsigned char Rele_Pin = 2;       // GPIO2 - Relay
const unsigned char Passive_buzzer = 3; // GPIO3 - Buzzer
// ultrasonic
const unsigned char trigPin = 4;        // GPIO4 - Trigger
const unsigned char echoPin = 5;        // GPIO5 - Echo

// SimpleTimers
SimpleTimer stToneIni;  // Temporizador inicio
SimpleTimer stToneDur;  // Temporizador duración sonido
SimpleTimer stToneSil;  // Temporizador duración silencio

// isMute?
bool isMute = true;

// Distancia
int lastDistance = -1;

//==============================================================
// Function measureDistance
//==============================================================
unsigned int measureDistance() {
  // defines variables
  long duration;
  int distance, media; 
  // Clears the trigPin
  // Serial.print("Clear pin... "); 
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  // Serial.print("trigger... ");
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  // Serial.print("trigger low... ");
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  // Serial.print("lee... ");
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance= duration*0.034/2;
/*
  Serial.print("lastDistance: ");
  Serial.print(lastDistance);

  Serial.print(" Distance: ");
  Serial.print(distance);
*/
  // Calculo de la media de las dos últimas distancias
  if(lastDistance < 0)
    media = distance;
  else
    media = (lastDistance + distance) / 2;
  lastDistance = distance;

  // Prints the distance on the Serial Monitor

  Serial.print("Distancia: ");
  Serial.println(media);
  return media;
}

//==============================================================
//     Función Buzzer - Activa el zumbador
//==============================================================
void buzzerFunction() {
  const unsigned int NOINICIADO = 0, SONIDO = 1, SILENCIO = 2;
  static unsigned int estado = NOINICIADO;
  static unsigned int maxNumSec = 3;    
  static unsigned int numSec = 0;  
  //Serial.println("Función buzzer...");
  // iniciar contador st
  if(!isMute){
    if (stToneIni.isReady() and estado == NOINICIADO) {
      //Serial.println("Comienza sonido...");
      stToneDur.reset(); // reIniciamos a contar
      tone(Passive_buzzer, 523); //DO note 523 Hz
      estado = SONIDO;
    } else if (stToneDur.isReady() and estado == SONIDO) {
      //Serial.println("Comienza silencio...");    
      stToneSil.reset();  
      noTone(Passive_buzzer); //Silence... 
      estado = SILENCIO;    
    } else if (stToneSil.isReady() and estado == SILENCIO) {
      //Serial.println("Siguiente secuencia...");    
      // Comenzamos nueva secuencia
      numSec++;     
      estado = NOINICIADO;    
      if (numSec >= maxNumSec) {
        //Serial.println("Reinicamos secuencia...");
        // Reiniciamos secuencia
        numSec = 0;            
        stToneIni.reset();
      }
    }
  } else // isMute
    noTone(Passive_buzzer); //Silence...  
}

void handleDistance() {
  String txt;
  unsigned int dur = 0;
  dur = measureDistance();
  // txt += "Millis =" + String(millis());
  // txt +=" - Distancia =" + dur;
  // comprobamos medida válida
  if ( dur > 5 && dur < 195) {
    isMute = false;
    // SimpleTimers
    stToneIni.setInterval(600);  
    stToneDur.setInterval(100);
    stToneSil.setInterval(100);
    //Serial.println("intervalos - 600-100-100");        
    if (dur > 100 and dur < 150 ) {
      // SimpleTimers // Tope y paramos motor
      stToneIni.setInterval(500);  
      stToneDur.setInterval(500);
      stToneSil.setInterval(1);
      digitalWrite(Rele_Pin, HIGH);             
      //Serial.println("intervalos - 500-500-1 - PARA MOTOR");      
    } 
    if (dur <= 100){
      // SimpleTimers // Volvemos a poner a cero y encendemos motor
      stToneIni.setInterval(3000);  
      stToneDur.setInterval(0);
      stToneSil.setInterval(0);
      isMute = true;
      digitalWrite(Rele_Pin, LOW);
      //Serial.println("intervalos - 3000-0-0");            
    }
  } else {
    isMute = true;
    digitalWrite(Rele_Pin, LOW);
  }
  // Serial.println(txt);
}


//===============================================================
//                  SETUP
//===============================================================
void setup(void){
  Serial.begin(9600);
  Serial.println("Iniciando...");

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(Rele_Pin, OUTPUT);
  // Buzzer
  pinMode (Passive_buzzer,OUTPUT);  
  // ultrasonic
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input  
  

  // Set rele on
  // Rele is on when output is HIGH
  digitalWrite(LED_BUILTIN, LOW);  
  digitalWrite(Rele_Pin, LOW); 

  // SimpleTimers
  stToneIni.setInterval(3000);  
  stToneDur.setInterval(0);
  stToneSil.setInterval(0);
  isMute = true;
  lastDistance = -1;
}

//===============================================================
//                     LOOP
//===============================================================
void loop(void){
  // comprobamos distancia
  handleDistance();
  // función Buzzer
  buzzerFunction(); 
  // Delay
  delay(10);
}

