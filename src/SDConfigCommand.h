// SDConfigCommand 0.15 //
/* 
SDConfigCommand streamlines reading settings from a config file on a SD card. SDConfigCommand can read standardised text files stored on a SD card, 
parse and tokenise them into commands and values. The library can also write settings in a similar manner but it is currently slow to do so.
For every line on the config file this library reads, it will callback a user-specified function. The user can access the current command and values,
then decide the next action, such as verifying commands and storing values in variables.

The config file must have a 8.3 file name. Each setting takes one line, with the command on the left, followed by an equal sign, then the value on the right.
Comments can be written after two slashes like in the Arduino IDE. Due to the limitation of Arduino, minimising comments and arranging the settings
to be read sequentially will improve performance. Check out the included example files for a better idea.
*/

#ifndef SDCONFIGCOMMAND
#define SDCONFIGCOMMAND

// This is the number of tries to begin SD or open file
#define TRY 10 

// Use READ_CMD or READ_VALUE instead of 1 and 0 for readMode
#define READ_CMD 0 
#define READ_VALUE 1

// The number of char allocated for command and value
#define CMD_SIZE 32
#define VALUE_SIZE 16

// The file name for temporary file
#define TEMP_FILENAME "temp.tem"

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif
#include <SPI.h>
#include <SD.h>

class SDConfigCommand {
  
  public:
    SDConfigCommand();
    bool set(char* myFilename, int myCs, void (*myFunction)());
    bool set(String myFilename, int myCs, void (*myFunction)());
    bool readConfig();
    bool writeConfig(char* myFindCmd, char* myNewValue);
    bool writeConfig(String myFindCmd, String myNewValue);
    bool openInSerial();
    
    //bool ALTreadConfig();
    //bool ALTwriteConfig(char* myFindCmd, char* myNewValue);
    
    char* getCmd();
    String getCmdS();
    char* getValue();
    String getValueS();
    int getValueInt();
    float getValueFloat();

  private:
    void (*callback_Function)();
    File cFile;
    char filename[13];
    int cs; // chip select pin
    char cmd[CMD_SIZE]="";
    char value[VALUE_SIZE]="";
    
    bool readMode=READ_CMD;
    bool commentMode=false;
    
    bool openFile(File &myFile, char* myFilename, byte rw);
    bool parseFile();
    void callbackSequence();
    bool writeCmdValue(char myChar);

    // For editing and writing config file
    bool updateFile(File &tempFile, unsigned long cmdStart, unsigned long cmdEnd, bool lastEntry, char* findCmd, char* newValue);
    void SDWarning();
    
};

#endif
