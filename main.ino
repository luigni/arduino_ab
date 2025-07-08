

Brazo* brazo = new Brazo();


void setup() {
  Serial.begin(9600);
  brazo->velH(1000);
  brazo->velC(1000);
  brazo->accH(1000);
  brazo->accC(1000);
}

void loop() {
  // put your main code here, to run repeatedly:
  
  Serial.println("Inserte calculo si quiere calcular el angulo");
  Serial.println("Inserte mover si quiere iniciar el movimiento del brazo");
  Serial.println("Inserte stop en cualquier momento para detener el brazo");
  while(Serial.available() == 0) {}
  if (Serial.available() > 0) {
    String input = Serial.readString();
    if (input == "calculo\n") {
      Serial.println("inserte el x");
      while(Serial.available() == 0) {}
      input = Serial.readString();
      float px = input.toInt();
      Serial.println("inserte el y");
      while(Serial.available() == 0) {}
      input = Serial.readString();
      float py = input.toInt();
      Serial.println("inserte el l1");
      while(Serial.available() == 0) {}
      input = Serial.readString();
      float largo1 = input.toInt();
      Serial.println("inserte el l2");
      while(Serial.available() == 0) {}
      input = Serial.readString();
      float largo2 = input.toInt();
      brazo->angulos(largo1, largo2, px, py);
      Serial.print("angulo del hombro: ");
      Serial.println(brazo->getHombro());
      Serial.print("angulo del codo: ");
      Serial.println(brazo->getCodo());
      Serial.print("posición del brazo: ");
      Serial.println(brazo->getBrazo());
    }
    if (input == "mover\n") {
      brazo->mover();
    }
    if(input == "stop\n"){
      brazo->stop();
    }
    if (!stepperh.isRunning() && !stepperc.isRunning()) {
      delay(2000);
      Serial.println("Motor ha completado los pasos.");
      brazo->inicio();
    }

  }
}
