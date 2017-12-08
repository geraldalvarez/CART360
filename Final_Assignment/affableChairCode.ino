//pressure sensors pins
#define PRESSURE_SENSOR_BACK A0
#define PRESSURE_SENSOR_SEAT_TOP A1
#define PRESSURE_SENSOR_SEAT_MID A2
#define PRESSURE_SENSOR_SEAT_BOT A3


//vibration motors
#define MOTOR_RIGHT 13
#define MOTOR_LEFT 12

//buzzer sensor
#define BUZZER 3

//heat pad pins
#define HEATPAD_TOP 6
#define HEATPAD_BOT 7

// constants for RGB LED
#define LED_PIN_R 11 // R PIN
#define LED_PIN_G 10 // G PIN
#define LED_PIN_B 9  // B PIN


//timer variables
unsigned long startTimer;
int timeMultipler = 1;
long sitDuration = 30000 * timeMultipler;
long warnDuration = 15000 * timeMultipler;
long cooldownDuration = 5000 * timeMultipler;

//stages of warning
int stageWarning = 0;

//the running sample count
const int RUNNING_SAMPLE = 20;

//arrays for running the average
int backSensorArray[RUNNING_SAMPLE];
int topSensorArray[RUNNING_SAMPLE];
int midSensorArray[RUNNING_SAMPLE];
int botSensorArray[RUNNING_SAMPLE];

//array index counter
int arrayCtr = 0;


//buzzer duration
int toneDuration = 200;

//the lowest values of the sensors
int backLowestVal = 0;
int topLowestVal = 0;
int midLowestVal = 0;
int botLowestVal = 0;

//code mode
int mode = 0;


//add to lowest to mark a threshold
int thresh = 50;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  //set pin motors pin
  pinMode(MOTOR_RIGHT, OUTPUT);
  pinMode(MOTOR_LEFT, OUTPUT);


  //set pin mode for the heatpads
  pinMode(HEATPAD_TOP, OUTPUT);
  pinMode(HEATPAD_BOT, OUTPUT);


  // prefill the arrays
  for (int i = 0; i < RUNNING_SAMPLE; i++)
    updateArrays();

  //set the lowest value of the sensors
  backLowestVal = getPressureBack();
  topLowestVal = getPressureTop();
  midLowestVal = getPressureMid();
  botLowestVal = getPressureBot();


  Serial.print("top : ");
  Serial.println(topLowestVal);
  Serial.print("mid : ");
  Serial.println(midLowestVal);
  Serial.print("bot : ");
  Serial.println(botLowestVal);

  //start the timer
  startTimer = millis();
}

void loop() {

  updateArrays();
  setRGB();
  selectMode();
  delay(100);
}

//function that manages the code cycle mode
void selectMode() {
  if (mode == 0) idleMode();
  else if (mode == 1) useMode();
  else if (mode == 2) warnMode();
  else if (mode == 3) cooldownMode();
}

//function that waits for user to trigger the next mode
void idleMode() {

  //update lowest values
  updateLowestValues();

  //check if one of the sensor passed the lowest val plus the added
  //threshold | someone sat on the chair
  if (  topLowestVal + thresh < getAveragePressureTop()
        || midLowestVal + thresh < getAveragePressureMid()
        || botLowestVal + thresh < getAveragePressureBot()) {

    //if time passed 3 seconds to make sure that the user sat
    if (getTimePassed() >= 3000) {

      //user's feedback to indicate mode changes
      playTone(440, 5);
      //turn on the vibration motors
      turnOnMotors(true);
      //delay for one seconds
      delay(1000);
      //turn off the buzzer
      noTone(BUZZER);
      //turn off the vibration motors
      turnOnMotors(false);

      //next mode | increment
      mode++;
      //reset timer
      resetTimer();
    }
  }

  Serial.println("IDLE MODE.");
  Serial.println();
}

//function that manages the behaviors of the chair on use mode
void useMode() {

  //check if any of the pressure sensor has been sat on
  if (getTimePassed() >=  sitDuration) {
    //next mode
    mode++;
    //reset the timer
    resetTimer();
    //turn of the vibration motors
    turnOnMotors(false);
  } else {
    //check is the user is still sitting
    if (topLowestVal + thresh < getAveragePressureTop()
        || midLowestVal + thresh < getAveragePressureMid()
        || botLowestVal + thresh < getAveragePressureBot()) {
      //check for bad posture
      if (isPostureBad()) {
        Serial.println("Bad posture");
        //notify  the user by vibration
        turnOnMotors(true);
      } else {
        Serial.println("Good posture");
        //turn off the vibration
        turnOnMotors(false);
      }
    } else {
      Serial.println("stood up");
      //turn off the vibration
      turnOnMotors(false);
    }
  }

  Serial.println("USE MODE");
  Serial.println();
}

//function that warns the user to get up
void warnMode() {

  //check if time over
  if (getTimePassed() <=  warnDuration) {

    //check if the user is still sitting
    if (  topLowestVal + thresh < getAveragePressureTop()
          || midLowestVal + thresh < getAveragePressureMid()
          || botLowestVal + thresh < getAveragePressureBot()) {

      //first stage of warning
      if (stageWarning == 0) {
        //heat the pads
        turnOnHeatpad(true);
      }

      //second stage of warning
      if (stageWarning == 1) {
        //turn on the heat and vibration
        turnOnHeatpad(true);
        turnOnMotors(true);
      }

      //third stage of warning
      if (stageWarning >= 2) {
        //turn on the heat, vibration, and buzzer
        turnOnHeatpad(true);
        turnOnMotors(true);
        playTone(110 * stageWarning, 1);
      }
    } else {
      //turn off the pads
      turnOnHeatpad(false);
      //reset the stages
      stageWarning = 0;
      //set no tone
      noTone(BUZZER);
      //turn off the motors
      turnOnMotors(false);
      //next mode | increment the mode
      mode++;
      //reset the timer
      resetTimer();
    }
  } else {
    //increment the warning stage
    if (stageWarning < 3)
      stageWarning++;

    //reset the timer
    resetTimer();
  }

  Serial.println("WARN MODE");
  Serial.println();
}

//function that manages the cool down mode
void cooldownMode() {

  //check if time is over
  if (getTimePassed() >= cooldownDuration) {

    //next mode
    mode = 0;
    //reset the timer
    resetTimer();
  } else {
    //check if the user sat down
    if (  topLowestVal + thresh < getAveragePressureTop()
          || midLowestVal + thresh < getAveragePressureMid()
          || botLowestVal + thresh < getAveragePressureBot()) {

      //reset the timer due to the user sat down during cooldown mode
      resetTimer();

      //send audible feedback
      playTone(440, 10);
      //SEND HAPTIC FEEDBACK
      turnOnMotors(true);
      //delay to play the tone and the vibration
      delay(2000);
      // noTone();
      turnOnMotors(false);

      Serial.println("COOLDOWN MODE: RESET THE TIMER BECAUSE YOU SAT DOWN");
      Serial.println();
    }
  }

  //TO DO: make sure that the user does not cheat by checking if the user sat on the chair.
  Serial.println("COOLDOWN MODE");
  Serial.println();
}

////---------------------------TIMER------------------------------------------//
//function that resets the timer
void resetTimer() {
  //set the starttimer to the current time
  startTimer = millis();
}

//function that returns the time passed
long getTimePassed() {
  //return how long the time passed
  return millis() - startTimer;
}
////---------------------------------------------------------------------------//

//function that gets the sensor value
int getPressureBack() {

  //return the input value of the pressure sensor
  return analogRead(PRESSURE_SENSOR_BACK);
}

//function that calculates the average of the back sensor inputs
int getAveragePressureBack() {
  //place holder for the average
  int avg = 0;
  //loop through the array to calculate the sum of the pressure sensor's value
  for (int i = 0; i < RUNNING_SAMPLE; i++ ) {
    //adds the sum
    avg += backSensorArray[i];
  }
  //return the average
  return avg / RUNNING_SAMPLE;
}


//function that gets the sensor value
int getPressureTop() {

  //return the input value of the pressure sensor
  return analogRead(PRESSURE_SENSOR_SEAT_TOP);
}

//function that calculates the average of the top sensor inputs
int getAveragePressureTop() {
  //place holder for the average
  int avg = 0;
  //loop the array to calculate the sum of the pressure sensor's value
  for (int i = 0; i < RUNNING_SAMPLE; i++) {
    //add the sum
    avg += topSensorArray[i];
  }
  //return the average
  return avg / RUNNING_SAMPLE;
}

//function that gets the sensor value
int getPressureMid() {
  //return the input value of the pressure sensor
  return analogRead(PRESSURE_SENSOR_SEAT_MID);
}

//function that calculates the average of the mid sensor inputs
int getAveragePressureMid() {
  //place holder for the average
  int avg = 0;
  //loop the array to calculate the sum
  for (int i = 0; i < RUNNING_SAMPLE; i++) {
    //add the sum to the avg place holder
    avg += midSensorArray[i];
  }
  //return the average
  return avg / RUNNING_SAMPLE;
}

//function that gets the sensor value
int getPressureBot() {
  //return the input value of the pressure sensor
  return analogRead(PRESSURE_SENSOR_SEAT_BOT);
}

//function that calculates the average of the bot sensor inputs
int getAveragePressureBot() {
  //place holder for the average
  int avg = 0;
  //loop the array to calculate the sum of pressure sensor's value
  for (int i = 0; i < RUNNING_SAMPLE; i++) {
    //add the sum
    avg += botSensorArray[i];
  }
  //return the average
  return avg / RUNNING_SAMPLE;
}

//function that updates the running average of the pressure sensors array
void updateArrays() {

  //check if the current index is less than the max length of the running sample
  if (arrayCtr < RUNNING_SAMPLE) {
    //set the current value of the pressure sensor into the array
    backSensorArray[arrayCtr] = getPressureBack();
    topSensorArray[arrayCtr] = getPressureTop();
    midSensorArray[arrayCtr] = getPressureMid();
    botSensorArray[arrayCtr] = getPressureBot();

    //increment the index position of the array
    arrayCtr++;
  } else {
    //reset the array index position
    arrayCtr = 0;
  }
}

//update the lowest pressure sensors' value
void updateLowestValues() {
  //check if the saved lowest value is greater than the current value
  if (botLowestVal > getAveragePressureBot())
    //assign current value to the lowest value place holder
    botLowestVal = getAveragePressureBot();

  //check if the saved lowest value is greater than the current value
  if (midLowestVal > getAveragePressureMid())
    //assign current value to the lowest value place holder
    midLowestVal = getAveragePressureMid();

  //check if the saved lowest value is greater than the current value
  if (topLowestVal > getAveragePressureTop())
    //assign current value to the lowest value place holder
    topLowestVal = getAveragePressureTop();

  //check if the saved lowest value is greater than the current value
  if (backLowestVal > getAveragePressureBack())
    //assign current value to the lowest value place holder
    backLowestVal = getAveragePressureBack();
}

//function that verifies the posture of the user and returns true if the posture and false is good
boolean isPostureBad() {

  //bad posture 1: sitting on the tip of the seat
  if ( botLowestVal + thresh < getAveragePressureBot()
       && topLowestVal + thresh > getAveragePressureTop()) {

    return true;
  }

  //bad posture 2: sitting on the tip + leaning on the backrest
  if (botLowestVal + thresh < getAveragePressureBot()
      && backLowestVal + (thresh * 0.8) < getAveragePressureBack()
      && topLowestVal + thresh > getAveragePressureTop()) {

    return true;
  }

  return false;
}

//function that turns on or off the heat pads
void turnOnHeatpad(boolean on) {
  if (on) {
    digitalWrite(HEATPAD_TOP, HIGH);
    digitalWrite(HEATPAD_BOT, HIGH);
  } else {
    digitalWrite(HEATPAD_TOP, LOW);
    digitalWrite(HEATPAD_BOT, LOW);
  }
}

//function that turns on or off the motors
void turnOnMotors(boolean on) {
  if (on) {
    digitalWrite(MOTOR_RIGHT, HIGH);
    digitalWrite(MOTOR_LEFT, HIGH);
  } else {
    digitalWrite(MOTOR_RIGHT, LOW);
    digitalWrite(MOTOR_LEFT, LOW);
  }
}

//function that plays a tone 
void playTone(int freq, int multiply) {

  //play a tone at the freq. and for the duration of 200ms x multiply
  tone(BUZZER, freq, toneDuration * multiply);

}

//function that assigns which color to display
void setRGB()
{
  //select the color according the mode 
  switch (mode)
  {
    case 0:
      {
        analogWrite(LED_PIN_R, 125);   // Turn on the LED -R
        analogWrite(LED_PIN_G, 0);   // Turn on the LED -G
        analogWrite(LED_PIN_B, 255);   // Turn on the LED -B
        break;
      }
    case 1:
      {
        analogWrite(LED_PIN_R, 0);   // Turn on the LED -R
        analogWrite(LED_PIN_G, 0);   // Turn on the LED -G
        analogWrite(LED_PIN_B, 255);   // Turn on the LED -B
        break;
      }
    case 2:
      {
        analogWrite(LED_PIN_R, 255);   // Turn on the LED -R
        analogWrite(LED_PIN_G, 0);   // Turn on the LED -G
        analogWrite(LED_PIN_B, 0);   // Turn on the LED -B
        break;
      }
    case 3:
      {
        analogWrite(LED_PIN_R, 0);   // Turn on the LED -R
        analogWrite(LED_PIN_G, 255);   // Turn on the LED -G
        analogWrite(LED_PIN_B, 0);   // Turn on the LED -B
        break;
      }
  }
}


