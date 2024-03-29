
#define left_dir1 2//defining all the motor pins for the motor driver
#define left_dir2 3
#define left_enable 6

#define right_dir1 4
#define right_dir2 7
#define right_enable 5

#define trig  10  //defining the Trigger and Echo pin of Ultrasonic Sensor
#define echo  11

#define rack1 12
#define rack2 13  //This part is for handling the Rack and Pinion Mechanism


const int irPins[6] = {A1, A2, A3, A4, A5, 8}; //Though 8 sensors are there but 6 sensor are used in this code
int irSensorDigital[6] = {0, 0, 0, 0, 0, 0}; //Set the basic value to 0 for all the sensors
int irSensors = B000000; //The value of the sensors are stored in the variable isSensors
int count = 0; //For PID calculation
int error = 0; //For PID calculation
int errorLast = 0;  //For PID calculation
int correction = 0; //For PID calculation


int maxSpeed = 200; //adjust for oyur own bot, if your bot is driving too fast reduce this else increase this
int motorLSpeed = 0;
int motorRSpeed = 0;

unsigned int dist;
unsigned int obstkl = 10;


void setup() {
  pinMode(left_dir1, OUTPUT); //Set the pins for controlling the motor as OUTPUT
  pinMode(left_dir2, OUTPUT);
  pinMode(right_dir1, OUTPUT);
  pinMode(right_dir2, OUTPUT);
  pinMode(left_enable, OUTPUT);
  pinMode(right_enable, OUTPUT);
  pinMode(rack1, OUTPUT);
  pinMode(rack2, OUTPUT);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);

  digitalWrite(trig, LOW);
  digitalWrite(rack1, LOW);
  digitalWrite(rack2, LOW);
  

  dist = 0;
  dist -= 1;

  /* Set-up IR sensor pins as input */
  for (int i = 0; i < 6; i++)
  {
    pinMode(irPins[i], INPUT);
  }

}

void loop() {
  ultrasonic(); //For measuring the distance
  Scan(); //for scanning the sensors
  UpdateError(); //for PID calculation
  UpdateCorrection();////for PID calculation
  if (dist < obstkl) {
    motorLSpeed = 0;
    motorRSpeed = 0;
  }

  Drive(); //For driving the motor
  rack(); // For movement of rack and pinion

}

void Scan()
{
  count = 0;

  irSensors = B000000;

  for (int i = 0; i < 6; i++)
  {

    if (digitalRead(irPins[i]) == 1)
      irSensorDigital[i] = 0;
    if (digitalRead(irPins[i]) == 0)
      irSensorDigital[i] = 1;

    count = count + irSensorDigital[i];
    int b = 5 - i;
    irSensors = irSensors + (irSensorDigital[i] << b); //For summing up the sensor value as a binary input
    //Serial.print(irSensorDigital[i]);
  }
  //Serial.println();
}

void UpdateError() { //This whole part is for PID calculation, the error is minimized, and the speed of motor is determined

  errorLast = error;

  switch (irSensors) {

    case B000000:                         //No sensor is on the line
      if (errorLast < 0) {
        error = -180;
      }
      else if (errorLast > 0) {
        error = 180;
      }
      break;

    case B100000: // leftmost sensor on the line
      error = -150;
      break;

    case B010000: //2nd left sensor on the line
      error = -90;
      break;

    case B001000: // 3rd left sensor on the line
      error = -30;
      break;

    case B000100: // 3rd right sensor on the line
      error = 30;
      break;

    case B000010: // 2nd right sensor on the line
      error = 90;
      break;

    case B000001: // rightmost sensor on the line
      error = 150;
      break;

    /* 2 Sensors on the line */

    case B110000:
      error = -120;
      break;

    case B011000:
      error = -60;
      break;

    case B001100:
      error = 0;
      break;

    case B000110:
      error = 60;
      break;

    case B000011:
      error = 120;
      break;

    /* 3 Sensors on the line */

    case B111000:
    case B011100:
      error = -150;
      break;

    case B000111:
    case B001110:
      error = 150;
      break;

    /* 4 Sensors on the line */
    case B111100:
      error = -150;
      break;

    case B111010:
      error = -150;
      break;

    case B001111:
      error = 150;
      break;

    case B010111:
      error = 150;
      break;

    /* 5 Sensors on the line */
    case B111110:
      error = -150;
      break;

    case B011111:
      error = +150;
      break;

    case B111111:
      error = 0;
      break;

    default:
      error = errorLast;
  }
}

void UpdateCorrection() { //error are corrected here

  if (error >= 0 && error < 30) {
    correction = 0;
  }

  else if (error >= 30 && error < 60) {
    correction = 15;
  }

  else if (error >= 60 && error < 90) {
    correction = 40;
  }

  else if (error >= 90 && error < 120) {
    correction = 55;
  }

  else if (error >= 120 && error < 150) {
    correction = 75;
  }

  else if (error >= 150 && error < 180) {
    correction = 255;
  }

  else if (error >= 180) {
    correction = 305;
  }

  if (error <= 0 && error > -30) {
    correction = 0;
  }

  else if (error <= -30 && error > -60) {
    correction = -15;
  }

  else if (error <= -60 && error > -90) {
    correction = -40;
  }

  else if (error <= -90 && error > -120) {
    correction = -55;
  }

  else if (error <= -120 && error > -150) {
    correction = -75;
  }

  else if (error <= -150 && error > -180) {
    correction = -255;
  }

  else if (error <= -180) {
    correction = -305;
  }

  if (correction >= 0) {
    motorRSpeed = maxSpeed;
    motorLSpeed = maxSpeed - correction;
  }

  else if (correction < 0) {
    motorRSpeed = maxSpeed + correction;
    motorLSpeed = maxSpeed;
  }
}

void Drive() { //This function will drive the motor
  if (motorRSpeed > 255) {
    motorRSpeed = 255;
  }
  else if (motorRSpeed < 0) {
    motorRSpeed = 0;
  }

  if (motorLSpeed > 255) {
    motorLSpeed = 255;
  }
  else if (motorLSpeed < 0) {
    motorLSpeed = 0;
  }

  if (motorRSpeed > 0) { // right motor forward (using PWM)
    analogWrite(right_enable, motorRSpeed);
    digitalWrite(right_dir1, HIGH);
    digitalWrite(right_dir2, LOW);
  }

  else if (motorRSpeed < 0) {// right motor reverse (using PWM)
    analogWrite(right_enable, abs(motorRSpeed));
    digitalWrite(right_dir1, LOW);
    digitalWrite(right_dir2, HIGH);
  }

  else if (motorRSpeed == 0) { // right motor fast stop
    analogWrite(right_enable, 0);
    digitalWrite(right_dir1, LOW);
    digitalWrite(right_dir2, LOW);
  }

  if (motorLSpeed > 0) { // right motor forward (using PWM)
    analogWrite(left_enable, motorLSpeed);
    digitalWrite(left_dir1, HIGH);
    digitalWrite(left_dir2, LOW);
  }

  else if (motorLSpeed < 0) { // right motor reverse (using PWM)
    analogWrite(left_enable, abs(motorLSpeed));
    digitalWrite(left_dir1, LOW);
    digitalWrite(left_dir2, HIGH);
  }

  else if (motorLSpeed == 0) { // left motor fast stop
    digitalWrite(left_enable, HIGH);
    digitalWrite(left_dir1, LOW);
    digitalWrite(left_dir2, LOW);
  }
}

void ultrasonic() {
  dist = 0;
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  unsigned long tm = pulseIn(echo, HIGH);

  if (tm == 0) {
    dist -= 1;
  } else {
    dist = (unsigned int)(tm * 0.034 / 2);
  }

}

void rack() {
  if(dist >= obstkl)
    return;
  unsigned long tm = millis();
  digitalWrite(rack1, HIGH);
  do {
    ultrasonic();
  } while(dist < obstkl);
  tm = millis() - tm;
  digitalWrite(rack1, LOW);
  digitalWrite(rack2, HIGH);
  delay(tm);
  digitalWrite(rack2, LOW);
}
