Brazo brazo;
#define SERVO_V 9
#define SERVO_R 10
#define SERVO_G 11


void setup() {
  Serial.begin(9600);
  brazo.velH(1000);
  brazo.velC(1000);
  brazo.accH(1000);
  brazo.accC(1000);
  brazo.attachall();
}

void loop() {
  // put your main code here, to run repeatedly:
  while (!Serial) delay(1); // Espera que el puerto serie esté listo
  Serial.println("Inserte movermanual si quiere calcular el angulo con coordenadas dichas");
  Serial.println("Inserte stop en cualquier momento para detener el brazo");
  Serial.println("Inserte moverauto para iniciar el movimiento con un angulo de la muñeca");
  Serial.println("Inserte moverbrazo para insertar los angulos del brazo y moverlos ahí");
  Serial.println("Inserte moververtical para insertar la posición que quiera subir el brazo (en cm de 0-40)");
  while(Serial.available() == 0) {}
  if (Serial.available() > 0) {
    String input = Serial.readString();
    if (input == "movermanual\n") {
      Serial.println("inserte el x");
      while(Serial.available() == 0) {}
      input = Serial.readString();
      float px = input.toInt();
      Serial.println("inserte el y");
      while(Serial.available() == 0) {}
      input = Serial.readString();
      float py = input.toInt();
      brazo.angulosM(22.0, 19.0, px, py);
      Serial.print("angulo del hombro: ");
      Serial.println(brazo.getHombro());
      Serial.print("angulo del codo: ");
      Serial.println(brazo.getCodo());
      Serial.print("posición del brazo: ");
      Serial.println(brazo.getBrazo());
    }
    if (input == "moverbrazo\n") {
      Serial.println("inserte el angulo del hombro");
      while(Serial.available() == 0) {}
      input = Serial.readString();
      float h = input.toInt();
      brazo.setHombro(h);
      Serial.println("inserte el angulo del codo");
      while(Serial.available() == 0) {}
      input = Serial.readString();
      float c = input.toInt();
      brazo.setCodo(c);
      brazo.moverB();
      brazo.setBrazo(20);
    }
    if (input == "moververtical\n") {
      Serial.println("inserte la altura donde quiere llevar el brazo");
      while(Serial.available() == 0) {}
      input = Serial.readString();
      float altura = input.toInt();
      brazo.setBrazo(altura);
      brazo.moverV();
      brazo.setHombro(-90);
      brazo.setCodo(90);
      Serial.println(brazo.getBrazo());
    }
    if (input == "moverauto\n") {
      Serial.println("inserte el angulo de la muñeca");
      while(Serial.available() == 0) {}
      input = Serial.readString();
      float pm = input.toInt();
      Serial.println("inserte el l1");
      while(Serial.available() == 0) {}
      input = Serial.readString();
      float largo1 = input.toInt();
      Serial.println("inserte el l2");
      while(Serial.available() == 0) {}
      input = Serial.readString();
      float largo2 = input.toInt();
      brazo.angulosA(largo1, largo2, pm);
      Serial.print("angulo del hombro: ");
      Serial.println(" grados");
      Serial.println(brazo.getHombro());
      Serial.print("angulo del codo: ");
      Serial.println(" grados");
      Serial.println(brazo.getCodo());
      Serial.print("posición del brazo: ");
      Serial.println(" cm");
      Serial.println(brazo.getBrazo());
      Serial.print("angulo de la muñeca: ");
      Serial.println(" grados");
      Serial.println(brazo.getMuneca());
      brazo.mover();
      delay(5000);
    }
    if (input == "auto2\n") {
      brazo.angulosA(22.0, 19.0, 0);
      Serial.print("angulo del hombro: ");
      Serial.println(brazo.getHombro());
      Serial.print("angulo del codo: ");
      Serial.println(brazo.getCodo());
      Serial.print("posición del brazo: ");
      Serial.println(brazo.getBrazo());
      Serial.print("angulo de la muñeca: ");
      Serial.println(brazo.getMuneca());
      brazo.mover();
      delay(2000);
    }
    if(input == "stop\n"){
      brazo.stop();
    }
    if (!stepperh.isRunning() && !stepperc.isRunning()) {
      delay(10000);
      Serial.println("Motor ha completado los pasos.");
      brazo.inicio();
    }

  }
}
 

 
