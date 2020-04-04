// Example for SDConfigCommand - Read And Write Config File //
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


// Define chip select pin and file names
#define CHIPSELECT 4
#define SETTING_FILE "setting.cfg"

// Some variables to store main settings
int len, wid, hei;
char welcomeMsg[64]; 

// Create an instance of SDConfigCommand
SDConfigCommand sdcc;

void setup() {
  Serial.begin(9600);
  
  while (!Serial) {} // wait for serial port to connect. Needed for native USB port only

  // setting.cfg is the file name, 4 is chip enable pin for SPI, processCmd is the callback function
  while( !(sdcc.set(SETTING_FILE, CHIPSELECT, processCmd)) ){}

  // Display the setting.cfg in Serial Monitor
  sdcc.openInSerial();
  Serial.println();

  // Read in and store the commands in config file
  sdcc.readConfig();
  
  // Check local variables for stored values
  Serial.println(F("Values stored:"));
  Serial.print(F("length is "));
  Serial.println(len);
  Serial.print(F("width is "));
  Serial.println(wid);
  Serial.print(F("height is "));
  Serial.println(hei);
  Serial.print(F("welcomeMsg is "));
  Serial.println(welcomeMsg);
  Serial.println();


  // Attempt to change "width" to "999"
  if (sdcc.writeConfig("width","999")){ Serial.println(F("Write config done!")); }
  else{ Serial.println(F("Some issues occured"));  }  
  Serial.println();

  // Display the setting.cfg in Serial Monitor again
  Serial.println(F("This is how the edited file looks like:"));
  sdcc.openInSerial();
  Serial.println();
  
  // Revert "width" back to "256"
  if (sdcc.writeConfig("width","256")){ Serial.println(F("Write config done!")); }
  else{ Serial.println(F("Some issues occured"));  }
  Serial.println();
  
  Serial.println(F("setting.cfg reverted back to original."));

}

void loop() {

}



void processCmd() {
// This function will run every time there is a command
// You can then check the command and value and dictate the next action
  
  if (strcmp(sdcc.getCmd(), "length")==0){ // getCmd() returns c-string, thus use strcmp
    len=sdcc.getValueInt(); // getValueInt converts the string value into int
  }

  else if (sdcc.getCmdS() == "width"){ // getCmdS() returns Arduino String(), you can use == to compare
    wid=sdcc.getValueS().toInt(); // getValueS() returns Arduino String(), you can use String.toInt() function
  }

  else if (strcmp(sdcc.getCmd(), "height")==0){
    hei=atoi(sdcc.getValue()); // You can manually convert the returned c-string as well
  }

  else if (strcmp(sdcc.getCmd(), "welcomeMsg")==0){
    strcpy(welcomeMsg, sdcc.getValue()); // Use strcpy instead of = for c-string
  }

  else{
    Serial.print(sdcc.getCmd());
    Serial.println(F(" is an unrecognised command."));
  }

}



bool restore(){
// This function will recreate setting.cfg
// This is not used for this example but useful for experimentation purposes

  // Access SD card
  if ( !SD.begin(CHIPSELECT) ){
    return 0;
  }

  // Remove the original config file
  SD.remove(SETTING_FILE);

  // Re-create a blank config file
  File restore;
  if ( !(restore=SD.open(SETTING_FILE, FILE_WRITE)) ){
    return 0;
  }

  // Write everything back into the file
  restore.println("// CONFIG FILE //");
  restore.println("// Comments are behind double strokes");
  restore.println();
  restore.println("length=128");
  restore.println("width=256");
  restore.println("height=10");
  restore.print("welcomeMsg=Hello world!");

  // Close the file
  restore.close();

  Serial.println("setting.cfg restored.");
  return 1;
}
