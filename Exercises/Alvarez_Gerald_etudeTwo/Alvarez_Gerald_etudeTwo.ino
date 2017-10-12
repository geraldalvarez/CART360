/**********************ETUDE 2 CART 360 2017*******************************
 * WELCOME!
 * THE PURPOSE OF THIS EXERCISE IS TO DESIGN A VERY SIMPLE KEYBOARD (5 KEYS)
 * WHICH ALLOWS YOU TO PLAY LIVE, RECORD, AND PLAYBACK.
 * THERE WILL BE A BUTTON WHICH WHEN PRESSED WILL TAKE THE USER TO THE NEXT MODE:
 * THERE ARE 5 POSSIBLE MODES
 * 0 ==  reset    ==   led off == also resets
 * 1 ==  live     ==   led BLUE
 * 2 ==  record   ==   led RED
 * 3 ==  play     ==   led GREEN
 * 4 ==  loopMODE ==   led PURPLE
 *
 * ADDITIONALLY - WHEN THE USER CHANGES MODE,
 * THE RGB LED WILL CHANGE COLOR (ACCORDING TO THE SPECIFICATIONS)

 * PLEASE FOLLOW THE INSTRUCTIONS IN THE COMMENTS:
 * DO NOT ADD ANY MORE FUNCTION DEFINITIONS
 * ONLY IMPLEMENT THE FUNCTION DEFINITIONS SUPPLIED
 * THERE IS NO NEED TO ADD ANY NEW VARIABLES OR CONSTANTS
 * PROVIDE COMMENTS FOR ANY OF THE CODE IMPLEMENTED
 * GOOD LUCK!
 */
/**** CONSTANTS ********************************************************/

#define BUTTON_MODE_PIN 2 // Button to change the mode

// constants for RGB LED
#define LED_PIN_R 5 // R PIN
#define LED_PIN_G 9 // G PIN
#define LED_PIN_B 6 // B PIN

// constant for note in (button-resistor ladder on breadboard)
# define NOTE_IN_PIN A0

//constant for max number of notes in array
#define MAX_NOTES 16

// a constant for duration
const int duration = 200;

// constant for pin to output for buzzer
#define BUZZER_PIN 3 // PWM

/******** VARIABLES *****************************************************/
// counter for how many notes we have
int countNotes = 0;
// array to hold the notes played (for record/play mode)
int notes [MAX_NOTES];
int mode = 0; // start at off

/*************************************************************************/


/**********************SETUP() DO NOT CHANGE*******************************/
// Declare pin mode for the single digital input
void setup()
{
  pinMode(BUTTON_MODE_PIN, INPUT);
  //debugging purpose
  Serial.begin(9600);
}

/**********************LOOP() DO NOT CHANGE *******************************
 * INSTRUCTIONS:
 * There is NO NEED to change the loop - it establishes the flow of the program
 * We call here 3 functions repeatedly:
 * 1: chooseMode(): this function will determine the mode that your program is in
 * based on if the button (linked to the BUTTON_MODE_PIN) was pressed
 * 2: setRGB(): will set the color of the RGB LED based on the value of the mode variable
 * 3: selectMode(): will determine which function to call based on the value of the mode variable

**************************************************************************/
void loop()
{
  chooseMode();
  setRGB();
  selectMode();
}
/******************CHOOSEMODE(): IMPLEMENT *********************************
 * INSTRUCTIONS:
 * Read the value from the Button (linked to the BUTTON_MODE_PIN) and
 * if is being pressed then change the mode variable.
 * REMEMBER:
 * mode == 0 is reset
 * mode == 1 is live
 * mode == 2 is record
 * mode == 3 is play
 * mode == 4 is loopMode
 * Every time the user presses the button, the program will go to the next mode,
 * once it reaches 4, it should go back to mode == 0.
 * (i.e. if mode ==2 and we press, then mode ==3) ...
**************************************************************************/
void chooseMode() {

  //flag for intial button state
  boolean buttonState = false;
  
  //set a previous button state
  boolean prevState = true;

  //check if the button pin is on/high
  if (digitalRead(BUTTON_MODE_PIN) == HIGH)
  {
    //delay the processing
    delay(200);
    
    //check if the button is not equal to prev state to avoid duplicate input
    if (buttonState != prevState) {
      
      //flips the currunt state of the buttonState
      buttonState = !buttonState;
      
      //assign the value of buttonState into prevState
      prevState = buttonState;
    }
  } else {
    
    //assign the value of the opposite
    prevState = !buttonState;
  }

  if (buttonState) {
    
    if (mode < 4) {
      
      //increment mode
      mode++;
    } else {
      
      //reset the mode
      mode = 0;
    }
  }
}

/******************SETRGB(): IMPLEMENT *********************************
 * INSTRUCTIONS:
 * Depending on the value of the mode variable:
 * if the mode is 0 - RGB LED IS OFF
 * if the mode is 1 - RGB LED IS BLUE
 * if mode is 2 - RGB LED IS RED
 * if mode is 3 - RGB LED IS GREEN
 * if mode is 4 - RGB LED iS PURPLE
 * YOU MUST USE A SWITCH CASE CONSTRUCT (NOT A SERIES OF IF / ELSE STATEMENTS
**************************************************************************/
void setRGB()
{

  //switch statement that selects which colour to display according to mode
  switch (mode) {
    case 0:
      //light off
      analogWrite(LED_PIN_R, LOW);
      analogWrite(LED_PIN_G, LOW);
      analogWrite(LED_PIN_B, LOW);
      break;
    case 1:
      //blue light
      analogWrite(LED_PIN_R, LOW);
      analogWrite(LED_PIN_G, LOW);
      analogWrite(LED_PIN_B, HIGH);
      break;

    case 2:
      //red light
      analogWrite(LED_PIN_R, HIGH);
      analogWrite(LED_PIN_G, LOW);
      analogWrite(LED_PIN_B, LOW);
      break;
    case 3:
      //green light
      analogWrite(LED_PIN_R, LOW);
      analogWrite(LED_PIN_G, HIGH);
      analogWrite(LED_PIN_B, LOW);
      break;
    case 4:
      //Cyan light *Purple light does not light well
      analogWrite(LED_PIN_R, LOW);
      analogWrite(LED_PIN_G, HIGH);
      analogWrite(LED_PIN_B, HIGH);
      break;
  }

}
/**********************SELECTMODE() DO NOT CHANGE *******************************
 * INSTRUCTIONS:
 * There is NO NEED to change this function - it selects WHICH function should run based on the mode variable
 * There are 4 possibilities
 * 1: reset(): this function will reset any arrays etc, and will do nothing else
 * 2: live(): this function will play the corresponding notes
 * to the user pressing the respective buttons.
 * NOTE:: the notes played are NOT stored
 * 3: record(): this function will play the corresponding notes
 * to the user pressing the respective buttons
 * AND will STORE up to 16 consecutive notes in an array.
 * 4: play(): this function will playback any notes stored in the array that were recorded
 * in the previous mode
 * 5: loopMode(): this function will playback any notes stored in the array that were recorded,
 * BUT unlike the previous mode, you can choose in which sequence the notes are played.
 * REQUIRED: only play notes from the array (no live stuff)

******************************************************************************/
void selectMode()
{
  if (mode == 0) {
    reset();
  }
  else if (mode == 1) {
    live();
  }
  else if (mode == 2) {
    record();
  }

  else if (mode == 3) {
    play();
  }

  else if (mode == 4) {
    loopMode();
  }
}
/******************RESET(): IMPLEMENT **************************************
 * INSTRUCTIONS:
 * this function should ensure that any notes recorded are no longer available
**************************************************************************/
void reset()
{
  //turn off any tone
  noTone(BUZZER_PIN);

  //loops through notes array and assigns 0 value in every element
  for (int i = 0; i < MAX_NOTES; i++) {
    
    //assign 0 for each iteration
    notes[i] = 0;
  }

  //reset countNotes
  countNotes = 0;

}
/******************LIVE(): IMPLEMENT **************************************
 * INSTRUCTIONS:
 * this function will play the corresponding notes
 * to the user pressing the respective buttons
 * NOTE:: the notes played are NOT stored
 * SO: you need read in the input from the analog input (linked to the button-resistor ladder combo)
 * THEN - output the note to the buzzer using the tone() function
**************************************************************************/
void live()
{
  //invoke tone to play the freq tone
  tone(BUZZER_PIN, analogRead(NOTE_IN_PIN), duration);

}
/******************RECORD(): IMPLEMENT **********************************
 * INSTRUCTIONS:
 * this function will play the corresponding notes
 * to the user pressing the respective buttons
 * AND will STORE up to 16 consecutive notes in an array.
 * SO:you need read in the input from the analog input (linked to the button-resistor ladder combo)
 * AND - output the note to the buzzer using the tone() function
 * THEN store that note in the array  - BE CAREFUL - you can only allow for up to MAX_NOTES to be stored
**************************************************************************/
void record()
{
  //flag for intial button state
  boolean buttonState = false;
  
  //set a previous button state
  boolean prevState = true;
  
  //store the analog value as frequency
  int frequency = analogRead(NOTE_IN_PIN);

  //play the tone which the user is pressing
  tone(BUZZER_PIN, frequency, duration);

  //evaluate if the frequency is greater than 0
  if (frequency > 0) {
    
    //delaying the looping process to prevent duplicated input
    delay(duration);
    
    //check if the button is not equal to prev state to avoid duplicate input
    if (buttonState != prevState) {
      
      //flip the value of buttonState
      buttonState = !buttonState;
      
      //assign the value of buttonState to prevState
      prevState = buttonState;
    }
  } else {
    
    //assign the opposite value of buttonState to prevState
    prevState = !buttonState;
  }

  //check if buttonState is true
  if (buttonState) {
    
    //check if the countNotes is less than MAX Notes
    if (countNotes < MAX_NOTES) {
      
      //assign the value of the freq into array
      notes[countNotes] = frequency;
      
      //increment countNotes
      countNotes++;
    }
  }
}
/******************PLAY(): IMPLEMENT ************************************
 * INSTRUCTIONS:
 * this function will playback any notes stored in the array that were recorded
 * in the previous mode
 * SO: you need to go through the array of values (be careful - the user may not have put in MAX_NOTES)
 * READ each value IN ORDER
 * AND output each note to the buzzer using the tone() function
 * ALSO: as long as we are in this mode, the notes are played over and over again
 * BE CAREFUL: make sure you allow for the user to get to another mode from the mode button...
**************************************************************************/
void play()
{
  //check if countNotes is less than MAX_NOTES to iterate the notes array
  if (countNotes < MAX_NOTES) {
    
    //delay the processing
    delay(duration);
    
    //play the tones according the freq in the array element index position countNotes
    tone(BUZZER_PIN, notes[countNotes]);
    
    //increment countNotes
    countNotes++;
  } else {
    
    //reset the counter
    countNotes = 0;
  }

}
/******************LOOPMODE(): IMPLEMENT *********************************
 * INSTRUCTIONS:
 * this function will playback any notes stored in the array that were recorded
 * in the previous mode
 * SO: you need to go through the array of values (be careful - the user may not have put in MAX_NOTES)
 * READ values IN ANY ORDERING (You MUST use the array - but you can determine your own sequence)
 * AND output each note to the buzzer using the tone() function
 * ALSO: as long as we are in this mode, the notes are played over and over again
 * BE CAREFUL: make sure you allow for the user to get to another mode from the mode button...
**************************************************************************/
void loopMode()
{
  //delay the processing
  delay(duration);

  //generate a random integer number in the range of 0 to 15 inclusive
  int randomNum = random(0, MAX_NOTES);

  //play the tone randomly
  tone(BUZZER_PIN, notes[randomNum]);

}

/**************************************************************************/

/*
 * Analysis Behavior
 * 
 * The tone's output is determined by resistor. The higher ohm produces low frequency and lower ohm produces high frequency. 
 * In addition, the series of resistor further lower the voltage and current thus affecting the tone. For instance, 
 * whenever a button is pressed, the voltage and current will be reduced by the accumulative amount of the series of resistor starting 
 * from the pressed button. This reduction of voltage and current only occures in mode 1 and 2 due to analogRead() function that takes value for tone() function. 
 * Whereas in the mode 3 and 4, it does not input any analog value but output it. It outputs the frequency(the input values from A0) 
 * as tone using tone() function. 
 * 
 * 
 */


