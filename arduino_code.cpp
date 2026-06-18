#include <TimerOne.h>                     // Library to control Timer1
#include <MultiFuncShield.h>              // Library specific to the Multifunctional Shield

byte btn; // Defines the button variable
byte buttonNumber; // Defines the button number variable
int contador; // Defines the counter variable
int senha[8]; // Defines the password array
int correct = 0; // Initializes the correct-guesses variable

unsigned long tempoAnterior = 0; // Sets the previous time to 0
const long intervalo = 1000; // Sets the interval to 1 second

int timer (){ // Function to define the time
  do{
  btn = MFS.getButton(); // Reads the pressed button (if any)
  buttonNumber = btn & B00111111;  // Isolates the bits that represent the button number (0 to 3)
  contador = map(analogRead(POT_PIN), 0, 1023, 45, 90); // Reads the potentiometer value and maps it to a time interval between 45 and 90 seconds
  Serial.println(contador); // Prints the interval time to the serial monitor
  }while(buttonNumber != 1); // Keeps the time definition in a loop until button 1 is pressed
  return contador; // Returns the defined time value
}

int gerarSenha(){ // Function to generate a password
  randomSeed(analogRead(A4)); // Sets a random seed from an unused input
  for (int i = 0; i < 8; i++){ // Loop to add each integer of the password
    senha[i] = random(1, 4); // Generates each integer of the password
  }
  return senha; // Returns the pointer to the password
}

void ledAcertos(int a){ // LED function displaying the correct-guess count in binary (lights up the LEDs according to the value)
  if (a == 0){
    MFS.writeLeds(LED_1 | LED_2 | LED_3 | LED_4, OFF);
  } else if (a == 1){
    MFS.writeLeds(LED_1, ON);
    MFS.writeLeds(LED_2 | LED_3 | LED_4, OFF);
  } else if (a == 2){
    MFS.writeLeds(LED_2, ON);
    MFS.writeLeds(LED_1 | LED_3 | LED_4, OFF);
  } else if (a == 3){
    MFS.writeLeds(LED_1 | LED_2, ON);
    MFS.writeLeds(LED_3 | LED_4, OFF);
  } else if (a == 4){
    MFS.writeLeds(LED_3, ON);
    MFS.writeLeds(LED_1 | LED_2 | LED_4, OFF);
  } else if (a == 5){
    MFS.writeLeds(LED_1 | LED_3, ON);
    MFS.writeLeds(LED_2 | LED_4, OFF);
  } else if (a == 6){
    MFS.writeLeds(LED_2 | LED_3, ON);
    MFS.writeLeds(LED_1 | LED_4, OFF);
  } else if (a == 7){
    MFS.writeLeds(LED_1 | LED_2 | LED_3, ON);
    MFS.writeLeds(LED_4, OFF);
  } else if (a == 8){
    MFS.writeLeds(LED_4, ON);
    MFS.writeLeds(LED_1 | LED_2 | LED_3, OFF);
  } else {
    MFS.writeLeds(LED_1 | LED_2 | LED_3 | LED_4, ON);
  }
}

void setup(){
  Serial.begin(9600); // Starts serial communication (9600 bps)
  Timer1.initialize(); // Initializes Timer1
  MFS.initialize(&Timer1); // Initializes the Multifunctional Shield with Timer1
  Serial.println(timer()); // Prints the selected time at startup
  gerarSenha(); // Generates a password
  MFS.write("COOL"); // Writes COOL on the display
  delay(3000); // Delays before clearing the display and starting the countdown
  MFS.write(""); // Clears the display
}

void loop(){
  btn = MFS.getButton(); // Reads the pressed button (if any)
  byte buttonNumber = btn & B00111111; // Isolates the bits that represent the button number (0 to 3)
  byte buttonAction = btn & B11000000; // Isolates the bits that represent how the button was pressed (long or short click)

  if (contador > 0){ // Starts the system
    MFS.write(contador);  // Writes the remaining time on the display
    unsigned long tempoAtual = millis();  // Defines the current time
    if (tempoAtual - tempoAnterior >= intervalo){ // Checks if one second has passed
        tempoAnterior = tempoAtual; // Updates the previous time so it doesn't stay at 0
        contador--; // Decrements the counter by one, simulating a seconds countdown
    }
    if (contador < 10){ // If the remaining time is less than 10
      MFS.beep(20); // Beeps for 200ms
      MFS.blinkDisplay(DIGIT_ALL, ON);  // Activates a function that blinks the display until disabled
    }
    if (buttonAction == BUTTON_PRESSED_INT){  // Checks for a short click to avoid counting two clicks in a single press
      if (buttonNumber == senha[correct]){  // Checks if the pressed button matches the expected password digit
          correct++;  // Increments the correct-guesses variable
          MFS.beep(8, 0, 1);  // Beeps for 80ms with no pause, one cycle
          ledAcertos(correct);  // Calls the function that lights up the LED based on correct guesses
      }
      else{ // If the pressed button does not match the password
        correct = 0;  // Resets the correct-guesses variable
        contador -= 2;  // Deducts two seconds from the remaining time
      }
    }
    if (correct == 8){  // If the password is correct
      MFS.write("SAFE");  // Writes SAFE on the display
      MFS.beep(50, 50, 3) // Beeps for 500ms with 500ms pause, 3 times
    }
  }
  else{ // If the remaining time reaches 0
    MFS.blinkDisplay(DIGIT_ALL, OFF); // Stops blinking the display (it starts blinking when time is below 10)
    MFS.write("ICE"); // Writes ICE on the display
    MFS.beep(30000);  // Continuous beep for 5 minutes (basically indefinite — nobody deserves 5 minutes of that)
  }
}
