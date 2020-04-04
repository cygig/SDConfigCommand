#include "SDConfigCommand.h"

bool SDConfigCommand::writeConfig(String myFindCmd, String myNewValue){
// This is the same as the other one, just that this one takes String for parameters

  char myFindCmdCString[CMD_SIZE];
  char myNewValueCstring[VALUE_SIZE];

  strncpy(myFindCmdCString, myFindCmd.c_str(), sizeof(myFindCmdCString)-1);
  strncpy(myNewValueCstring, myNewValue.c_str(), sizeof(myNewValueCstring)-1);
  if (writeConfig(myFindCmdCString, myNewValueCstring)){ return 1; }
  else{ return 0; }
}

bool SDConfigCommand::writeConfig(char* myFindCmd, char* myNewValue){
// Default function to write config by Character Analysis
// This section follows this flow chart: 
// https://drive.google.com/file/d/1KdSDMLXn8sEJRiuMLo2FEu2HIedoj9jH/view?usp=sharing
// You can open the file with draw.io or app.diagrams.net or view the included image files
// It is easier to read the flow chart to understand the logic

  File tempFile;
  char findCmd[CMD_SIZE];
  char newValue[VALUE_SIZE];
  bool lastEntry=false;
  strcpy(findCmd, myFindCmd);
  strcpy(newValue, myNewValue);
  
  bool cmdFound=false;
  unsigned long cmdStart=0;
  unsigned long cmdEnd=0;

  cmd[0]='\0';
  value[0]='\0';
  readMode=READ_CMD;
  lastEntry=false;


  // Check if temp file exists, if yes, delete it
  if(SD.exists(TEMP_FILENAME)){
    
    Serial.print(TEMP_FILENAME);
    Serial.println(F(" exists!"));
    
    if(!SD.remove(TEMP_FILENAME)){ 
      Serial.print(TEMP_FILENAME);
      Serial.println(F(" can't be removed!"));
      return 0; }
      
    else { 
      Serial.print(TEMP_FILENAME);
      Serial.println(F(" deleted.")); }
      
  }
  
  // Recreate a new empty temp file
  if ( !(openFile(tempFile, TEMP_FILENAME, FILE_WRITE)) ){ return 0; }
    
  if ( !(openFile(cFile, filename, FILE_READ)) ){ return 0; }

  while (cFile.available()>0){ // 1 while
    char myChar = cFile.read();

    if (myChar == '\n' || myChar =='\r' || cFile.available()==0){ // 2 if
      if (cmdFound){ // 3 if

        if (cFile.available()==0){  lastEntry=true; }
        
        else { // 4 else
          if (cFile.peek()=='\n' || cFile.peek()=='\r'){  cmdEnd = cFile.position()+1; }
          else{ cmdEnd = cFile.position(); } 
        } // 4 else
          
          updateFile(tempFile, cmdStart, cmdEnd, lastEntry, findCmd, newValue);
          Serial.print(F("Updated "));
          Serial.print(findCmd);
          Serial.print(F("="));
          Serial.println(newValue);
          return 1; 
          
          
       }// 3 if
          
      else{ // 3 else
        readMode=READ_CMD;
        cmd[0]='\0'; 
        continue; } // 3 else    
    }// 2 if

    else if(myChar=='='){ // 2 else if
      readMode = READ_VALUE;

      if(strcmp(cmd,"")==0 || strcmp(cmd, " ")==0 || (cmd[0]=='/' && cmd[1]=='/')){ // 3 if
        cmd[0]='\0';
        continue; }

      else{ // 3 else
        if(strcmp(cmd, findCmd)==0){ //4 if
          cmdStart = cFile.position() - strlen(cmd) - 1;
          cmdFound = true;
          cmd[0]='\0';
          continue; }
          
        else{ // 4 else
          cmd[0]='\0';
          continue; }

      } // 3 else

      
    }// 2 else if

    else{ // 2 else
      
      if (readMode==READ_CMD){ // 3 if
        writeCmdValue(myChar); 
        continue; }
        
      else { continue; } // 3 else
      
    }// 2 else

  }// 1 while

  if (cFile.available()<=0){ // 1 if
    cFile.close();
    tempFile.close();
    Serial.println (F("No match found."));
    return 1;
  } // 1 if
}



bool SDConfigCommand::updateFile(File &tempFile, unsigned long cmdStart, unsigned long cmdEnd, bool lastEntry, char* findCmd, char* newValue){
// This function does the actual updating of the config file once we know the location
// of where the command to be change starts and ends (position in file).
// Assumes both cFile and tempFile is already opened.

  SDWarning();
  
  cFile.seek(0); // Rewind our source file

  // Copy till where the command starts
  for(unsigned long i=0; i<cmdStart; i++){
    tempFile.write(cFile.read());
  }
  

  // Print updated values
  tempFile.print(findCmd);
  tempFile.print('=');
  tempFile.print(newValue);

  if(!lastEntry){

    // If this setting is in the middle, print line
    tempFile.println();

    // Fast forward to where the command and value ends
    cFile.seek(cmdEnd);
  
    // Copy the rest of cFile to tempFile
    while(cFile.available() > 0){
      tempFile.write(cFile.read()); }
  }

  // Close cFile so we can delete later
  // Flush temp File to commit change but don't close it
  // as we still need to read it later
  tempFile.flush();
  cFile.close();
  

  // Complicated file rename process starts here //
  
  // Delete cFile
  for (byte i=0; i<TRY; i++){
    if ( SD.remove(filename) ){  break;  }
      
    else if (i==TRY-1){
      Serial.println(F("File rename failed."));
      return 0; }

  }

  // Recreate blank cFile
  if( !(openFile(cFile, filename, FILE_WRITE)) ){ return 0; }

  // Rewind tempFile
  tempFile.seek(0);

  // Copy everything from tempFile to cFile
  while (tempFile.available() > 0){
    cFile.write(tempFile.read());
    
  }
  // Close all files and remove tempFile
  // No need to check if tempFile can delete since there is a check 
  // in the begining of writeConfig()
  cFile.close();
  tempFile.close();
  
  if (!SD.remove(TEMP_FILENAME)){
    Serial.println(F("Temp file can't be deleted"));
  }
  return 1;

}

void SDConfigCommand::SDWarning(){
// Prints warning message while writing files
Serial.println(F("Saving, please wait..."));
Serial.println(F("**DON'T reset/power off Arduino, close Serial Mon. or remove SD!**"));
}
