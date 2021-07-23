#include <Adafruit_NeoPixel.h>
#include <math.h>
#include <EEPROM.h>
#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

//LED-PIN
#define PIN        A0 // On Trinket or Gemma, suggest changing this to 1

//Number of LEDs
#define NUMPIXELS 30 // Popular NeoPixel ring size

// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

#define DELAYVAL 500 // Time (in milliseconds) to pause between pixels
#define w 1
#define period 12

byte map1[NUMPIXELS];
byte map2[NUMPIXELS];

int ledPin = 13;
int input0 = 4;
int input1 = 3;
int input2 = 4;

//temporary values
int temp0 = 0;
int temp1 = 0;

//scene value
int scene = 0;
int dynstc = 0;

//persistent color values
int r = 0, g = 0, b = 0;

// 0 = static, 1 = dynamic
void setup() {
  for (int i = 0; i < NUMPIXELS; i++) {
    map1[i] = 0;
  }
  for (int i = 0; i < NUMPIXELS; i++) {
    map2[i] = random(15) * random(10);
  }
  // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
  // Any other board, you can remove this part (but no harm leaving it):
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  // END of Trinket-specific code.

  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)

  Serial.begin(9600);

  r = EEPROM.read(0);
  g = EEPROM.read(1);
  b = EEPROM.read(2);
  dynstc = EEPROM.read(3);
  scene = EEPROM.read(4);

  if(dynstc == 1){
    fadeIn();
  }

  pinMode(input0, INPUT_PULLUP);
  pinMode(input1, INPUT_PULLUP);
  pinMode(input2, INPUT_PULLUP);
  pinMode(13, OUTPUT);
  //fadeIn();
}

void loop() {
  //Button wenn gedrückt: LOW!
  interrupt();
  if(dynstc == 1){
    digitalWrite(ledPin, HIGH);
    sScene(scene);
    EEPROM.write(4, scene);
  }
  /*if(digitalRead(input2) == HIGH){
    digitalWrite(ledPin, LOW);
    pixels.clear();
    pixels.show();
  }*/
}

void sScene(int scene){
  switch(scene){
    case 0: 
      setColor(0, 0, 255);
    break;
    case 1: 
      setColor(255, 0, 0);
    break;
    case 2: 
      setColor(255, 0, 160);
    break;
    case 3: 
      setColor(0, 150, 0);
    break;
    case 4: 
      setColor(255, 50, 0);
    break;
  }
}

void fadeIn(){
  for(float i = 0; i < 1; i = i + 0.01){
    for(int j = 0; j < NUMPIXELS; j++){
      pixels.setPixelColor(j, r*i, g*i, b*i);  
    }
    pixels.show();
    Serial.println(137);
    delay(3);
  }
  pixels.show();
}

void fadeOut(){
  for(float i = 1; i > 0; i = i - 0.01){
    for(int j = 0; j < NUMPIXELS; j++){
      pixels.setPixelColor(j, r*i, g*i, b*i);  
    }
    pixels.show();
    delay(3);
  }
  Serial.println(220);
  pixels.clear();
  pixels.show();
}

void setColor(int pR, int pG, int pB){
  r = pR;
  g = pG;
  b = pB;
  EEPROM.write(0, r);
  EEPROM.write(1, g);
  EEPROM.write(2, b);
  for(int i = 0; i < NUMPIXELS; i++){
    pixels.setPixelColor(i, r, g, b);
  }
  pixels.show();
}

int ringNumber(int number, int ringSize) {
  if (number < ringSize) {
    return number;
  }
  else if (number < 0) {
    return (ringNumber(number + ringSize, ringSize));
  }
  else return (ringNumber(number-ringSize, ringSize));
}

void interrupt(){
  if(temp0 == 0 && digitalRead(input0) == LOW){
    scene = ringNumber(scene + 1, 5);
    Serial.println(digitalRead(input0));
    temp0 = 1;
  }
  if(temp1 == 0 && digitalRead(input1) == LOW){
    temp1 = 1;
    dynstc = ringNumber(dynstc + 1, 2);
    Serial.println(digitalRead(input1));
    if(dynstc == 1){
      fadeIn();
    }
    else{
      fadeOut();
    }
    EEPROM.write(3, dynstc);
  }
  if(temp0 == 1 && digitalRead(input0) == HIGH){
    temp0 = 0;
  }
  if(temp1 == 1 && digitalRead(input1) == HIGH){    
    temp1 = 0;
  }
}
