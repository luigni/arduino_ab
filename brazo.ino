#include <AccelStepper.h>
#include <cmath>

#define X_STEP_PIN 2
#define X_DIR_PIN 5
#define Y_STEP_PIN 3
#define Y_DIR_PIN 6
#define X_STEP_PIN_2 2
#define X_DIR_PIN_2 5
#define Y_STEP_PIN_2 3
#define Y_DIR_PIN_2 6
AccelStepper stepperh(AccelStepper::DRIVER, X_STEP_PIN, X_DIR_PIN);
AccelStepper stepperc(AccelStepper::DRIVER, Y_STEP_PIN, Y_DIR_PIN);
AccelStepper stepper1(AccelStepper::DRIVER, X_STEP_PIN_2, X_DIR_PIN_2);
AccelStepper stepper2(AccelStepper::DRIVER, Y_STEP_PIN_2, Y_DIR_PIN_2);

class Brazo{

  private:
  
  const float yIdeal = -4; //cada objeto debe estar idealmente 4 cm por abajo del hombro
  const float hombroI = -60.0; //angulo inicial del hombro
  const float codoI = 90.0; //angulo inicial del codo
  const float brazoI = 20; //punto inicia del brazo
  const float angporpaso = 0.1; //el angulo que se mueve un nema por cada paso
  const float cmporpaso = 1; // cuantos cm sube el brazo por cada paso de los nemas
  const float xH = 40; // cuantos cm puede subir el brazo
  float posB; //posicion del brazo en cm
  float posH; //angulo a mover del hombro
  float posC; //angulo a mover del codo

  public:
  //valor del angulo del hombro
  float getHombro(){
    return posH;
  }
  //valor del angulo del codo
  float getCodo(){
    return posC;
  }
  //valor del angulo del codo
  float getBrazo(){
    return posB;
  }
  //se explica por si solo
  float pitagoras(float n1, float n2){
    return sqrt(pow(n1, 2)+ pow(n2, 2));
  }
  //Formula simple para el angulo entre dos lados
  float atan2dgr(float n1, float n2){
    return (180/PI)*atan2(n1,n2);
  }
  //ley del coseno
  float leycos(float n1,float n2, float n3){
    return (180/PI)*(acos((pow(n1,2)+pow(n2,2)-pow(n3,2)) / (2*n1*n2)));
  }
  void angulos(float l1, float l2, float x, float y){
    //angulo codo 
    float d = pitagoras(x,yIdeal);  //distancia de hombro al punto
    float angc = leycos(l1,l2,d); //angulo interior del triangulo entre l1 y l2
    posC = 180 - angc;

    //angulo vertical del hombro.
    float angh = leycos(l1,d,l2); //angulo interior del triangulo entre l1 y d
    float angb = atan2dgr(yIdeal,x); //angulo entre Py y Px
    posH = angb-angh;

    posB = y-yIdeal;
  }
  void velH(int valv){
	  stepperh.setMaxSpeed(valv); 
    Serial.println(valv);
  }
  void velC(int valv){
	  stepperc.setMaxSpeed(valv); 
    Serial.println(valv);
  }
  void accH(int vala){
    stepperh.setAcceleration(vala); 
    Serial.println(vala);
  }
  void accC(int vala){
    stepperc.setAcceleration(vala); 
    Serial.println(vala);
  }
  void mas (float p, AccelStepper s){
    s.moveTo(s.currentPosition() + p);
  }
  void menos (float p, AccelStepper s){
    s.moveTo(s.currentPosition() - p);
  }
  void inicio(){
    //numero de pasos del brazo hasta lo más arriba
    float npb = (xH-posB)/cmporpaso;
    //numero de pasos del hombro
    float nph = (hombroI-posH)/angporpaso;
    //numero de pasos del codo
    float npc = (codoI-posC)/angporpaso;
    //mover arriba o abajo el hombro
    if (npb >= 0){
      mas(abs(npb), stepper1);
      mas(abs(npb), stepper2);
      Serial.println(npb);
    }
    else{
      menos(abs(npb), stepper1);
      menos(abs(npb), stepper2);
      Serial.println(npb);
    }
    delay(2000);
    //mover adelante o atras hombro
    if (nph >= 0){
      mas(abs(nph), stepperh);
      Serial.println(nph);
    }
    else{
      menos(abs(nph), stepperh);
      Serial.println(nph);
    }
    //mover adelante o atras codo
    if (npc >= 0){
      mas(abs(npc), stepperc);
      Serial.println(npc);
    }
    else{
      menos(abs(npc), stepperc);
      Serial.println(npc);
    }
    //volver a la posición inicial del brazo
    npb = (brazoI-xH)/cmporpaso;
    //mover el brazo a su posición inicial
    if (npb >= 0){
      mas(abs(npb), stepper1);
      mas(abs(npb), stepper2);
      Serial.println(npb);
    }
    else{
      menos(abs(npb), stepper1);
      menos(abs(npb), stepper2);
      Serial.println(npb);
    }
  }
  
  void mover(){
    //numero de pasos del brazo
    float npb = (posB-brazoI)/cmporpaso;
    //numero de angporpasos del hombro
    float nph = (posH-hombroI)/angporpaso;
    //numero de angporpasos del codo
    float npc = (posC-codoI)/angporpaso;
    //mover arriba o abajo el hombro
    if (npb >= 0){
      mas(abs(npb), stepper1);
      mas(abs(npb), stepper2);
      Serial.println(npb);
    }
    else{
      menos(abs(npb), stepper1);
      menos(abs(npb), stepper2);
      Serial.println(npb);
    }
    delay(2000);
    //mover adelante o atras hombro
    if (nph >= 0){
      mas(abs(nph), stepperh);
      Serial.println(nph);
    }
    else{
      menos(abs(nph), stepperh);
      Serial.println(nph);
    }
    //mover adelante o atras codo
    if (npc >= 0){
      mas(abs(npc), stepperc);
      Serial.println(npc);
    }
    else{
      menos(abs(npc), stepperc);
      Serial.println(npc);
    }
  }
  void stop(){
      stepperh.moveTo(stepperh.currentPosition());
      stepperh.stop(); // Detiene el motor inmediatamente
      posH = stepperh.currentPosition();
      stepperc.moveTo(stepperc.currentPosition());
      stepperc.stop(); // Detiene el motor inmediatamente
      posC = stepperc.currentPosition();
  }
  
};
