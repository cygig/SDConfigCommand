// Example for SDConfigCommand - Stress Read //
/* 
SDConfigCommand streamlines reading settings from a config file on SD card. SDConfigCommand can read standardised text files stored on a SD card, 
parse and tokenise them into commands and values. The library can also write over existing settings but it is currently slow to do so. 
For every line on the config file this library reads, it will callback a user-specified function. The user can access the current command and values, 
then decide the next action, such as verifying commands and storing values in variables. With regards to writing over existing settings, user can choose one command, 
and the library will search for the command in the config file and replace the whole setting line with a new value. The library does not add or remove settings.

The config file must have a 8.3 file name. Each setting takes one line, with the command on the left, followed by an equal sign, then the value on the right.
Comments can be written after two slashes like in the Arduino IDE. Due to the limitation of Arduino, minimising comments and arranging the settings
to be read sequentially will improve performance. Check out the included example files for a better idea.

In this example, connect your Arduino to read SD card using a SD card shield or module. To connect Uno to a SD module with LEVEL SHIFTER:
5V to VCC, GND to GND, 11 to MOSI, 12 to MISO, 13 to SCK, 4 to Chip Select. *Remember to use level shifters for 5V Arduinos.*

You will also need setting.cfg, stress.cfg and stress_r.cfg in the root folder of your SD card. 
Upload the sketch and observe the Serial Monitor and take instructions from there.
*/

#include <SDConfigCommand.h>

// Define chip select pin, file names, array sizes
#define CHIPSELECT 4
#define STRESS_FILE "stress.cfg"
#define STRESS_FILE_RANDOM "stress_r.cfg"
#define ASIZE 16
#define BSIZE 16

// Arrays to hold the settings
int stressSetting[ASIZE][BSIZE];
int aC=0;
int bC=0;
int settingSize = ASIZE*BSIZE;

// Used to record time
unsigned long timeStart;
unsigned long timeEnd;

// Instace of SDConfigCommand
SDConfigCommand stressSdcc;

void setup() {

  Serial.begin(9600);
  while (!Serial) {} // wait for serial port to connect. Needed for native USB port only

  
  // setting.cfg is the file name, 4 is chip enable pin for SPI, processCmd is the callback function
  while( !(stressSdcc.set(STRESS_FILE, CHIPSELECT, processStressCmd)) ){}

  // Set everything to -1, indicating the setting has yet to be assigned
  resetSetting();

  // Read the sequentially written stress.cfg
  Serial.print(F("Reading sequential config file "));
  Serial.print(STRESS_FILE);
  Serial.println(F(", this will take a while..."));

  timeStart = millis();
  stressSdcc.readConfig();
  timeEnd=millis();
  
  Serial.print(F("It takes "));
  Serial.print(timeEnd-timeStart);
  Serial.print(F("ms to store "));
  Serial.print(settingSize);
  Serial.println(F(" integers from a sequential config file."));
  Serial.println();

  Serial.println(F("Sequential Config File Results:"));
  printResults();

  // Reset the file name to stress_r.cfg
  while( !(stressSdcc.set(STRESS_FILE_RANDOM, CHIPSELECT, processStressCmd)) ){}

  // Set everything to -1, indicating the setting has yet to be assigned
  resetSetting();

  // Read the randomly written stress_r.cfg
  Serial.print(F("Reading randomised config file "));
  Serial.print(STRESS_FILE_RANDOM);
  Serial.println(F(", this will take a while..."));

  timeStart = millis();
  stressSdcc.readConfig();
  timeEnd=millis();

  Serial.print(F("It takes "));
  Serial.print(timeEnd-timeStart);
  Serial.print(F("ms to store "));
  Serial.print(settingSize);
  Serial.println(F(" integers from a randomised config file."));
  Serial.println();

  Serial.println(F("Randomised Config File Results:"));
  printResults();
  Serial.println(F("You should get the same results for both config files."));
  Serial.println(F("Recording your settings sequentially in the config file should yield a faster read time."));

}

void loop() {


}



void processStressCmd() {
// This function will run every time there is a command
// You can then check the command and value and dictate the next action

  bool found=false; // Used to check if a settings had been found

  for (int a=0; a<ASIZE; a++){
    if (found){ break; } // Break the outer for loop once the setting has been found
    
    for (int b=0; b<BSIZE; b++){
      // If the current setting already as value (not -1), skip this inner for loop
      if ( !(stressSetting[a][b] == -1) ){ continue; }

      // Form the string to be compared with the command coming in
      char compare[32]="A";
      strcatint(compare, a);
      strcat(compare, "_B");
      strcatint(compare, b);

  
      if ( strcmp( compare, stressSdcc.getCmd() ) == 0 ){
          stressSetting[a][b]=stressSdcc.getValueInt();
          found=true; // if found, mark as true to skip the outer for loop
          break;  }
          
    }
  }
  
}



void strcatint(char* dest, int myInt){
// Function to concat an integer to a string

  char temp[16];
  itoa(myInt, temp, 10);
  strcat(dest, temp);
}



void resetSetting(){
// Set everything in array to -1

  for (int a=0; a<ASIZE; a++){
    for (int b=0; b<BSIZE; b++){
      stressSetting[a][b]=-1;
    }
  }
}



void printResults(){
  Serial.println(F("==================="));

  // Loop through all elements to print results
  for (int a=0; a<ASIZE; a++){
    for (int b=0; b<BSIZE; b++){
      Serial.print(F("A"));
      Serial.print(a);
      Serial.print(F("_B"));
      Serial.print(b);
      Serial.print(F(": "));
      Serial.print(stressSetting[a][b]);
      Serial.print(F("\t"));
    }
    Serial.println();
  }
  Serial.println(F("==================="));
  Serial.println();
}
