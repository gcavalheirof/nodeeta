//Por enquanto em teste com o arduino. Futuramente implementar junto com o NoteETA_pH


#include <Ultrasonic.h>
#define pino_trigger 12
#define pino_echo 11

float cmMsec, old;
int T = 20;
float mm[20];
float media = 0.0;
int i = 0;

Ultrasonic ultrasonic(pino_trigger, pino_echo);
 
void setup()
{
  Serial.begin(9600);
}
 
void loop()
{
  long microsec = ultrasonic.timing();
  cmMsec = ultrasonic.convert(microsec, Ultrasonic::CM);
  
  if(cmMsec > 5 && cmMsec < (old + 100)){     //100 - offset
      mm[i] = cmMsec;
      old = cmMsec;
      
    if(i == (T-1)){
      i = 0;
      media = 0;
      for (int j = 0; j<T; j++){
        media += mm[j];
      }
      media = media / T;
      Serial.println(media);
    }else{
      if(i > T){
        old = cmMsec;
      }
      i++;
    }
 }
 //Serial.println(cmMsec);
  delay(120);
}
