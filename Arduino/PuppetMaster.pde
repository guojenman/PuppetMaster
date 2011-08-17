
 #include <Servo.h>      // include the servo library

 Servo servos[6];
 
 int rightArm = 0;
 int leftArm = 1;
 int rightLeg = 2;
 int leftLeg = 3;
 
 int rightArmPin = 5;       // Control pin for servo motor, may only be pin 9 or 10
 int leftArmPin = 6;
 int rightLegPin = 10;
 int leftLegPin = 11;
 int limbPins[6];
 
 int leftEarLed = 1;
 int rightEarLed = 2;
 
 int maxLimbPosition = 1100;
 int limbPositions[6];
 
 int currentTime;
 int lastTime;
 int stoppedPosition = 88;
 
 int inByte= -1;                   // incoming byte from serial RX
 const int inStringLength = 20;    // max length is 19 and then 1 for the termination 0?
 char inString[inStringLength];    // string for incoming serial data
 int stringPos = 0;                // string index counter
 int sensorValues[6];
 
 int ra = 0;
 int la = 0;
 int rl = 0;
 int ll = 0;
 int re = 0;
 int le = 0;

 void setup() { 
   
   limbPins[rightArm] = rightArmPin;
   limbPins[leftArm] = leftArmPin;
   limbPins[rightLeg] = rightLegPin;
   limbPins[leftLeg] = leftLegPin;
   
   servos[rightArm].attach(limbPins[rightArm]);
   servos[leftArm].attach(limbPins[leftArm]);
   servos[rightLeg].attach(limbPins[rightLeg]);
   servos[leftLeg].attach(limbPins[leftLeg]);
   
   moveLimb(rightArm, ra);
   moveLimb(leftArm, la);
   moveLimb(rightLeg, rl);
   moveLimb(leftLeg, ll);
   
   pinMode(rightEarLed, OUTPUT);  
   pinMode(leftEarLed, OUTPUT);  
   digitalWrite(rightEarLed, HIGH);
   digitalWrite(rightEarLed, HIGH);
   
   //servoMotor.attach(rightArm);  // attaches the servo on pin 2 to the servo object
   Serial.begin(19200); 
   
   lastTime = millis();
 } 

 void loop() 
 {    
   if (Serial.available() > 0) {
     handleSerial();
     //Serial.flush();
   } else {
     //ra = 0;
     //la = 0;
   }
   
   currentTime = millis();
   
   moveLimb(rightArm, ra);
   moveLimb(leftArm, la);
   moveLimb(rightLeg, rl);
   moveLimb(leftLeg, ll);
   
   //Serial.print("rightArm:"); Serial.print(ra); Serial.print(",leftArm:"); Serial.print(la);
   //Serial.print("rightPos:"); Serial.print(limbPositions[rightArm]); Serial.print(",leftPos:"); Serial.println(limbPositions[leftArm]);
   
   lastTime = currentTime;
   
   //delay(10);
 }
 
 void moveLimb(int limb, int dir)
 {
   // do nothing if it's at its min or max
   if (limbPositions[limb] < 0) {
     limbPositions[limb] = 0;
     servos[limb].detach();
     return;
   } else if (limbPositions[limb] > maxLimbPosition){
     limbPositions[limb] = maxLimbPosition;
     servos[limb].detach();
     return;
   } else if (limbPositions[limb] == 0) {
     if (dir == -1){ 
       limbPositions[limb] = 0;
       servos[limb].detach();
       return;
     }
   } else if (limbPositions[limb] == maxLimbPosition){
     if (dir == 1){
       limbPositions[limb] = maxLimbPosition;
       servos[limb].detach();
       return;
     }
   }
   
     int elapsedTime = currentTime - lastTime;
     limbPositions[limb] += elapsedTime * dir;
   
     if (dir < 0){
       servos[limb].attach(limbPins[limb]);
       servos[limb].write(0);
     } else if (dir > 0) {
       servos[limb].attach(limbPins[limb]);
       servos[limb].write(180);
     } else {
       servos[limb].detach();
     }
 }
 
  void handleSerial() {
    inByte = Serial.read();
  
    if (inByte == '1'){
      la = 1;
    } else if (inByte == '2'){
      la = -1;
    } else if (inByte == '3'){
      la = 0;
    } else if (inByte == '4'){
      ra = 1;
    } else if (inByte == '5'){
      ra = -1;
    } else if (inByte == '6'){
      ra = 0;
    } else if (inByte == 'q'){
      ll = 1;
    } else if (inByte == 'w'){
      ll = -1;
    } else if (inByte == 'e'){
      ll = 0;
    } else if (inByte == 'r'){
      rl = 1;
    } else if (inByte == 't'){
      rl = -1;
    } else if (inByte == 'y'){
      rl = 0;
    }
  }
   
 /*
 void handleSerial() {
  inByte = Serial.read();
  
  // save only ASCII numeric characters (ASCII 0 - 9) and commas:
  if (((inByte >= '0') && (inByte <= '9')) || inByte == ','){
    inString[stringPos] = inByte;
    Serial.println(inByte);
    stringPos++;
  }
  // if you get an ASCII carriage return:

  if (inByte == '\r' || inByte == '|') {

    inString[stringPos] = 0;
    
    Serial.print("inString: ");
    Serial.println(inString);
    int num = 0;
    int d = 0;
    for (int c = 0; c < inStringLength; c++) {
      if (inString[c] == ',' || inString[c] == 0){
        sensorValues[d] = num;
        num = 0;
        d++;
      } else {
        if (num != 0){
          //skip until next comma or end
        } else {
          //turns out this assigns multi digit number until the next comma
          num = atoi(&inString[c]);
        }
      }
    } 
    
    ra = sensorValues[0];
    la = sensorValues[1];
    rl = sensorValues[2];
    ll = sensorValues[3];
    re = sensorValues[4];
    le = sensorValues[5];
    
    // clear the string with spaces
    for (int c = 0; c < inStringLength; c++) {
      inString[c] = ' ';
    } 
    // reset the string pointer:
    stringPos = 0;
  }
   */
   
   
   

