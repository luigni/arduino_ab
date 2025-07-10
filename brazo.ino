#include <AccelStepper.h>
#include <cmath>
#include <Wire.h>
#include <Adafruit_VL53L0X.h>
#include <Servo.h>

#define X_STEP_PIN 2
#define X_DIR_PIN 5
#define Y_STEP_PIN 3
#define Y_DIR_PIN 6
#define X_STEP_PIN_2 2
#define X_DIR_PIN_2 5
#define Y_STEP_PIN_2 3
#define Y_DIR_PIN_2 6
#define SERVO_V 9
#define SERVO_R 10
#define SERVO_G 11


// Crea una instancia del sensor

AccelStepper stepperh(AccelStepper::DRIVER, X_STEP_PIN, X_DIR_PIN);
AccelStepper stepperc(AccelStepper::DRIVER, Y_STEP_PIN, Y_DIR_PIN);
AccelStepper stepper1(AccelStepper::DRIVER, X_STEP_PIN_2, X_DIR_PIN_2);
AccelStepper stepper2(AccelStepper::DRIVER, Y_STEP_PIN_2, Y_DIR_PIN_2);


class Brazo{

  private:
  
  const float yIdeal = -4; //cada objeto debe estar idealmente 4 cm por abajo del hombro
  const float hombroI = -90.0; //angulo inicial del hombro 
  const float codoI = 90.0; //angulo inicial del codo 
  const float brazoI = 10; //punto inicia del brazo en cm
  const float angporpaso = 0.1; //el angulo que se mueve un nema por cada paso
  const float cmporpaso = 1; // cuantos cm sube el brazo por cada paso de los nemas
  const float xH = 40; // cuantos cm puede subir el brazo
  const float dML = 17; // distancia (en centimetros) entre la base de la garra y el laser
  float posM; //angulo a mover de la muñeca vertical
  float posB; //posicion del brazo en cm
  float posH; //angulo a mover del hombro
  float posC; //angulo a mover del codo
  Adafruit_VL53L0X lox; 
  Servo munecav;
  Servo munecar;
  Servo gripper; 
  VL53L0X_RangingMeasurementData_t measure; 
  public:
  //valor del angulo del hombro
  float getHombro(){
    return posH;
  }
  //valor del angulo del codo
  float getCodo(){
    return posC;
  }
  //valor de la altura del brazo
  float getBrazo(){
    return posB;
  }
  //valor del angulo de la muñeca
  float getMuneca(){
    return posM;
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
  bool iniciarSensorVL53L0X() {
    Serial.println("Intentando iniciar sensor VL53L0X...");
    if (!lox.begin()) {
        Serial.println(F("¡Error! No se pudo iniciar el sensor VL53L0X."));
        return false; // Retorna false si falla la inicialización
    }
    Serial.println("Sensor VL53L0X iniciado correctamente.");
    return true; // Retorna true si la inicialización fue exitosa
  }
  float getD() {
    float distancia_cm = -1.0; // Valor por defecto en caso de error

    // Realiza una medición. El 'false' significa que no se imprimirá depuración por serial.
    // La estructura 'measure' es un miembro de la clase, no una variable local de la función.
    lox.rangingTest(&measure, false); 

    // Verifica si la medición fue exitosa (el estado 4 significa "fuera de rango" o error)
    if (measure.RangeStatus != 4) {
        // Convierte la distancia de milímetros a centímetros
        distancia_cm = (float)measure.RangeMilliMeter / 10.0; 
    } else {
        // Si hay un error, puedes imprimir un mensaje (opcional)
        Serial.println("VL53L0X: Error al medir o fuera de rango.");
    }
    
    return distancia_cm; // Devuelve el valor medido (o -1.0 si hubo error)
}
  //calcular angulos manualmente (coordenadas dichas)
  void angulosM(float l1, float l2, float x, float y){
    //posición ideal del brazo
    posB = (y-yIdeal)+brazoI;
    //si es que el brazo está más abajo de su altura máxima
    if (posB <= xH){
      //angulo codo 
      float d = pitagoras(x,yIdeal);  //distancia de hombro al punto
      float angc = leycos(l1,l2,d); //angulo interior del triangulo entre l1 y l2
      posC = 180 - angc;

      //angulo vertical del hombro.
      float angh = leycos(l1,d,l2); //angulo interior del triangulo entre l1 y d
      float angb = atan2dgr(yIdeal,x); //angulo entre Py y Px
      posH = angb-angh;
    }
    //si es que el brazo está más arriba de su altura máxima
    else if (posB >= xH){
      posB = xH;
      float d = pitagoras(x,(y-xH));  //distancia de hombro al punto
      float angc = leycos(l1,l2,d); //angulo interior del triangulo entre l1 y l2
      posC = -(180 - angc);

      //angulo vertical del hombro.
      float angh = leycos(l1,d,l2); //angulo interior del triangulo entre l1 y d
      float angb = atan2dgr((y-xH),x); //angulo entre Py y Px
      posH = -(angb-angh);
    }
    //si es que el brazo está más abajo de la posición posible
    else if (posB <= 0){
      posB = 10;
      float d = pitagoras(x,(y-xH));  //distancia de hombro al punto
      float angc = leycos(l1,l2,d); //angulo interior del triangulo entre l1 y l2
      posC = -(180 - angc);

      //angulo vertical del hombro.
      float angh = leycos(l1,d,l2); //angulo interior del triangulo entre l1 y d
      float angb = atan2dgr((y-xH),x); //angulo entre Py y Px
      posH = -(angb-angh);
    }
  }
  //calcular angulos automaticamente (angulo de la muñeca y laser)
  void angulosA(float l1, float l2, float m){
    //mover el servo al angulo (lo haré después)
    posM = m;
    muneca();


    //sacar la distancia entre la muñeca y el punto
    iniciarSensorVL53L0X();
    float r = getD();
    Serial.print(r);
    Serial.println(" cm");

    //sacar coordenadas del punto (x,y) con el hombro como (0,0)
    float alfa = (PI/180)*hombroI;
    float l1x = (l1 * cos(alfa));
    float l1y = (l1 * sin(alfa));
    float beta = alfa+((PI/180)*codoI);
    float l2x = (l2 * cos(beta));
    float l2y = (l2 * sin(beta));
    float gamma = beta+((PI/180)*m);
    float rx = (r * cos(gamma));
    float ry = (r * sin(gamma));
    float x = l1x+l2x+rx;
    float y = l1y+l2y+ry; 
    Serial.print(x);
    Serial.println(" cm");
    Serial.print(y);
    Serial.println(" cm");

    //posición ideal del brazo
    posB = (y-yIdeal)+brazoI;
    //si es que el brazo está más abajo de su altura máxima
    if (posB <= xH){
      //angulo codo 
      float d = pitagoras(x,yIdeal);  //distancia de hombro al punto
      float angc = leycos(l1,l2,d); //angulo interior del triangulo entre l1 y l2
      posC = 180 - angc;

      //angulo vertical del hombro.
      float angh = leycos(l1,d,l2); //angulo interior del triangulo entre l1 y d
      float angb = atan2dgr(yIdeal,x); //angulo entre Py y Px
      posH = angb-angh;
    }
    //si es que el brazo está más arriba de su altura máxima
    else if (posB >= xH){
      posB = xH;
      float d = pitagoras(x,(y-xH));  //distancia de hombro al punto
      float angc = leycos(l1,l2,d); //angulo interior del triangulo entre l1 y l2
      posC = -(180 - angc);

      //angulo vertical del hombro.
      float angh = leycos(l1,d,l2); //angulo interior del triangulo entre l1 y d
      float angb = atan2dgr((y-xH),x); //angulo entre Py y Px
      posH = -(angb-angh);
    }
    //si es que el brazo está más abajo de la posición posible
    else if (posB <= 0){
      posB = 10;
      float d = pitagoras(x,(y-xH));  //distancia de hombro al punto
      float angc = leycos(l1,l2,d); //angulo interior del triangulo entre l1 y l2
      posC = -(180 - angc);

      //angulo vertical del hombro.
      float angh = leycos(l1,d,l2); //angulo interior del triangulo entre l1 y d
      float angb = atan2dgr((y-xH),x); //angulo entre Py y Px
      posH = -(angb-angh);
    }
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
  void muneca(){
    munecav.write(90);
    munecar.write(90);
    gripper.write(180);
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
