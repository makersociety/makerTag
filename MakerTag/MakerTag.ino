/*
 * Laser Tag gun
 * An IR LED must be connecte    d to Arduino PWM pin 3.
 * Pew Pew
 */

#include <IRremote.h>

#ifndef INTERRUPT_H
  #include <avr/interrupt.h>
#endif

// Declare some IR shit
int RECV_PIN = 6;     // IR Recieve (PD6)
int IR_FIRE = 3;      // IR LED for transmit(PD3)
IRrecv irrecv(RECV_PIN);
decode_results results;
IRsend irsend;

// Declare pin numbers
int FIRE_GUN_PIN = 2;        // Trigger (PD2)
int RESET_GUN_PIN = 5;   // When Dead Reset for more lives (PD5)
int RELOAD_GUN_PIN = 4;  // PD4
int HIT_LED0 = 9;        // Led0 - led4 indicates how many lives are left (PB1)
int HIT_LED1 = 10;       // (PB2)
int HIT_LED2 = 11;       // (PB3)
int HIT_LED3 = 12;       // (PB4)
int HIT_LED4 = 13;       // (PB5)
int TEAM_A_PIN    = 0;       // Select Team A (PC0)
int TEAM_B_PIN    = 1;       // Select Team B (PC1)
int TEAM_FREE_PIN = 2;       // Selct Free-For-All(PC2)


// Sound Effect Pins (when high, the sound plays in the gun)
int SHOOT_SFX = 7;      // (PD7)
int EMPT_REL_SFX = 8;   // (PB0) 


// Team Identifiers and config values
int freeForAll = 0xb25;  // Default value for free for all
int teamA = 0xb4A;       // Value for team A
int teamB = 0xDB;        // Value for B
int triggerDelay = 250;  // ms that the gun will be inactve after trigger press

// Variables for gun functionality
volatile int hit;
volatile int ammo;
int currentTeam;      // The current gun ID
int currentEnemy;     // People trying to kill you


/********************
  Set up the gun. This routine runs only once
*********************/
void setup()
{ 
  // Initialize variables
  hit = 0;    // No Hits yet...
  ammo = 10;  // Start off with ten rounds
  
  // Set up the pins
  // Inputs
  pinMode(FIRE_GUN_PIN,INPUT_PULLUP);
  pinMode(RESET_GUN_PIN,INPUT_PULLUP);
  pinMode(RELOAD_GUN_PIN, INPUT_PULLUP);
  pinMode(TEAM_A_PIN, INPUT_PULLUP);
  pinMode(TEAM_B_PIN, INPUT_PULLUP);
  pinMode(TEAM_FREE_PIN, INPUT_PULLUP);

  // Output
  pinMode(HIT_LED0, OUTPUT);
  pinMode(HIT_LED1, OUTPUT);
  pinMode(HIT_LED2, OUTPUT);
  pinMode(HIT_LED3, OUTPUT);
  pinMode(HIT_LED4, OUTPUT);
  pinMode(SHOOT_SFX, OUTPUT);
  pinMode(EMPT_REL_SFX, OUTPUT);
  
  delay(10);  // Not completely required but gives the
              // chances to propegate
              
  // Team selection
  // Check the 3 position switch to check what team mode and or team.
  if(digitalRead(TEAM_A_PIN) == LOW){ // You're on team A
    currentTeam = teamA;
    currentEnemy = teamB;
  }
  else if(digitalRead(TEAM_B_PIN == LOW)){
    currentTeam = teamB;
    currentEnemy = teamA;
  }
  else if(digitalRead(TEAM_FREE_PIN) == LOW){
    currentTeam = freeForAll;
    currentEnemy = freeForAll;
  }
  
  // Attach some interrupts
  attachInterrupt(0, shoot, LOW);    
 
  irrecv.enableIRIn(); // Start the receiver
}

void loop() {
  if (irrecv.decode(&results)) {// If there is some decoded values available 
      if(results.value == currentEnemy){ // If it matches our val, increment
        hit++;
        if (hit == 1)
          digitalWrite(HIT_LED0, HIGH);
        else if(hit==2)
          digitalWrite(HIT_LED1, HIGH);
        else if(hit==3) 
          digitalWrite(HIT_LED2, HIGH);
        else if(hit == 4)
          digitalWrite(HIT_LED3, HIGH);
        else if(hit == 5){  // You are dead.
          digitalWrite(HIT_LED4, HIGH);
          delay(200);
          while(hit==5 && digitalRead(RESET_GUN_PIN) == HIGH){
            // Blink the lights
            digitalWrite(HIT_LED0, LOW);
            digitalWrite(HIT_LED1, LOW);
            digitalWrite(HIT_LED2, LOW);
            digitalWrite(HIT_LED3, LOW);
            digitalWrite(HIT_LED4, LOW);
            delay(80);
            digitalWrite(HIT_LED0, HIGH);
            digitalWrite(HIT_LED1, HIGH);
            digitalWrite(HIT_LED2, HIGH);
            digitalWrite(HIT_LED3, HIGH);
            digitalWrite(HIT_LED4, HIGH);
            delay(80);
          }
          hit=0; // Reset the hit count, because you died
        }  
      }     
      irrecv.resume(); // Go back to waiting for a value
    } 
  // Else if you are trying to reload the gun
  else if(digitalRead(RELOAD_GUN_PIN) == LOW){
    delay(30); // Wait for the switch to stop bounching
    if(digitalRead(RELOAD_GUN_PIN) == LOW){ // If the switch is steady
      while(digitalRead(RELOAD_GUN_PIN) == LOW){;}  // Do nothing while the hammer is back
      digitalWrite(EMPT_REL_SFX, HIGH);  // Make the reload noise
      delay(triggerDelay);
      digitalWrite(EMPT_REL_SFX, LOW);
      ammo = 10;  // Reset ammo value
    }
  }
}  // End of main loop




/*
  This subroutine will be called while the interupt is called
  in this subroutine a hex value will be sent via IR 
*/
void shoot() {
  cli();  // Disable interrupts to be a little atomic
  
  if ( ammo > 0 ){// If there is still ammo
    // Decrement ammo
    ammo--;
    irsend.sendRC5(currentTeam, 12); // Send Hex value via IR
    digitalWrite(SHOOT_SFX, HIGH);    // Make pew noise
    
    // While the trigger is pressed do nothing
    while (digitalRead(FIRE_GUN_PIN) == LOW){;}
    
    // Wait for debounce and to limit shoots
    delay(triggerDelay);
    digitalWrite(SHOOT_SFX,LOW);
  }
  else{// Else there is no ammo left. Do not shoot and make empty sound
    // While trigger is pressed do nothing
    digitalWrite(EMPT_REL_SFX, HIGH);
    while(digitalRead(FIRE_GUN_PIN) == LOW){;} 
    delay(triggerDelay);  // Delay for a bit. No Rapid fire!!
    digitalWrite(EMPT_REL_SFX, LOW);
  }
  // On the way out... 
  irrecv.enableIRIn(); // Enable the Receive in... I don't know why
  sei();               // Enable global interrupts 
}

 
