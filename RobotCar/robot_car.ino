//////// -- TIC-RBT1 -- ////////
// | corde_t | judea_d | kingki_n | brouar_l | //
// rendu : 19 Apr 2026 //
// Wall - E avance, recule, detecte les objets proches de lui, et drifts.

#include <Servo.h>
Servo head;
#define SERVO_PIN A3

//  Capteur ultrason HC-SR04
#define TRIG_PIN 7 //Broche qui envoie le signal
#define ECHO_PIN 8 //Broche qui reçois le signal


//  Moteur l298n
//coté gauche
#define ENA 3 //Vitesse du côté gauche
#define IN1 5
#define IN2 6

//coté droit
#define ENB 11 //Vitesse du côté droit
#define IN3 9 
#define IN4 10

//  ROBOT
#define STOP_THRESHOLD 20 //Objet à moins de 20cm = stop
#define SLOW_THRESHOLD 50 //Ralentissement progressif par rapport à la distance

#define FULL_SPEED 120//Vitesse max 
#define SLOW_SPEED 100 //Vitesse réduite

#define BACKUP_TIME 400 //Combien de temps le robot recul
#define TURN_TIME 800  //Combien de temps le robot tourne

//  Fonction pour bouger le robot

//  POUR AVANCER
void  moveForward(int speed) {
  analogWrite(ENA, speed); //Vitesse moteur côté gauche
  analogWrite(ENB, speed); //Vitesse moteur côté droit

 //Moteur gauche en avant
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);

 //Moteur droit en avant
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
}

//  POUR RECULER
void moveBackward(int speed) {
    analogWrite(ENA, speed);
    analogWrite(ENB, speed);

  //Moteur gauche pour reculer (inverser IN1 et IN2)
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);

  //Moteur droit pour reculer (inverser IN3 et IN4)
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
}

// TOURNER A DROITE (Un côté avance l'autre recul, gauche en avant et droit recul)
void turnRight(int speed){
    analogWrite(ENA, speed);
    analogWrite(ENB, speed);

  //Moteur gauche en avant
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);

  //Moteur droit recul
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    }

//TOURNER A GAUCHE (Un côté recul l'autre avance, gauche recul et droit en avant)
void turnLeft(int speed){
    analogWrite(ENA, speed);
    analogWrite(ENB, speed);

  //Moteur gauche recul
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);

  //Moteur droit en avant
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
}

//  TOUT ARRETER
void stopMotors() {
  //Moteur coupé
    analogWrite(ENA, 0);
    analogWrite(ENB, 0);
}

void setup() {
    Serial.begin(9600);

    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);

    pinMode(ENA, OUTPUT);
    pinMode(ENB, OUTPUT);
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);


// Le capteur est centré au démarrage (regarde devant)
    head.attach(SERVO_PIN);
    head.write(90);

    stopMotors();

    Serial.println(" Initialisation WALL-E !");
    Serial.println("Demarrage...");

    delay(2000);

    Serial.println("C'est parti !");
}

// MESURER LA DISTANCE
long measureDistance() {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    long duree = pulseIn(ECHO_PIN, HIGH, 30000);

    if (duree == 0) {
    return 999;
    }

  long distance = duree * 0.034 / 2;
    return distance;
}

// BOUCLE PRINCIPALE
void loop() {
    long distance = measureDistance();

    Serial.print("Distance : ");
    Serial.print(distance);
    Serial.println(" cm");

    if (distance < STOP_THRESHOLD) {
    Serial.println("OBSTACLE ! Arret + evitement");
    stopMotors();
    delay(300);
    moveBackward(FULL_SPEED);
    delay(BACKUP_TIME);
    stopMotors();
    delay(300);

// Le servo oriente le capteur pour analyser les deux côtés
    long leftDistance;
    long rightDistance;

    head.write(150);
    delay(300);
    leftDistance = measureDistance();

    head.write(30);
    delay(300);
    rightDistance = measureDistance();

    head.write(90);
    delay(200);

// Le robot choisit le côté le plus libre
    if (leftDistance > rightDistance) {
        Serial.println(">> Tourne a GAUCHE");
        turnLeft(220);
    } else {
        Serial.println(">> Tourne a DROITE");
        turnRight(220);
    }

    delay(TURN_TIME);
    stopMotors();
    delay(100);
    }
    else if (distance < SLOW_THRESHOLD) {
    //Utilisation de map pour le calcul progressif de la vitesse
    int vitesse = map(distance, STOP_THRESHOLD, SLOW_THRESHOLD, SLOW_SPEED, FULL_SPEED);
     // distance = 20 cm → vitesse = SLOW_SPEED
    // distance = 50 cm → vitesse = FULL_SPEED

    Serial.print("Ralentissement progressif, vitesse: ");
    Serial.println(vitesse);

    moveForward(vitesse);
    }   
    else {
    moveForward(FULL_SPEED);
    }

delay(70);
} 