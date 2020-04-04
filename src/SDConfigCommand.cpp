#include "SDConfigCommand.h"


SDConfigCommand::SDConfigCommand(){
  // Empty constructor
}


bool SDConfigCommand::set(String myFilename, int myCs, void (*myFunction)()){
// This is the same as the other one, just that this one takes String for myFilename

  char myFilenameCString[13];
  // Use strncopy to limit how much you can copy to avoid overflow
  strncpy(myFilenameCString, myFilename.c_str(), sizeof(myFilenameCString)-1);
  
  if ( set(myFilenameCString, myCs, myFunction ) ){
    return 1; }

  else{
    return 0; }
    
}


bool SDConfigCommand::set(char myFilename[], int myCs, void (*myFunction)()){
  
  //Put all settings into private values
  strncpy(filename, myFilename, sizeof(filename)-1);
  cs = myCs;
  callback_Function = myFunction;

  // Begin reading SD card
  // Try for TRY amount of times
  for(byte i=0; i<TRY; i++){
    if( SD.begin(cs) ){ return 1; }

    else if (i==TRY-1){
      Serial.println(F("SD read failed!"));
      return 0; }
      
    else{ 
      Serial.print(F("Reading SD...")); 
      Serial.println(i+1);  }

  }
}



bool SDConfigCommand::readConfig(){
  // If open file and read successfully, close file
  if (openFile(cFile, filename, FILE_READ) && parseFile()){
    cFile.close();
    return 1;
  }

  else {
    return 0;
  }
}



bool SDConfigCommand::openFile(File &myFile, char* myFilename, byte rw){
//rw should be FILE_READ or FILE_WRITE
// myFile should be passed by reference
   
  // Try to open the file for TRY number of times
  for (byte i=0; i<TRY; i++){
    if (myFile=SD.open(myFilename, rw)){ return 1; }
    
    else if (i==TRY-1){
      Serial.print(myFilename);
      Serial.println(F(" open failed!"));
      return 0; }
      
    else{
      Serial.print(F("Opening "));
      Serial.print(myFilename);
      Serial.print(F("..."));
      Serial.println(i+1);
    }  
  }


}



bool SDConfigCommand::parseFile(){
// Default function to read in config by Character Analysis
// This section follows this flow chart: 
// https://drive.google.com/file/d/1bJ0vlCzHEXuG8vAbBYMGmATn-_yRA_Rs/view?usp=sharing
// You can open the file with draw.io or app.diagrams.net or view the included image files
// It is easier to read the flow chart to understand the logic
 
  while (cFile.available() > 0){
    
    if (commentMode){ // 1 if
      
        char myChar = cFile.read();
        
        if (myChar=='\n' || myChar=='\r'){
          commentMode=false;
          continue;   }
        else { continue; }
        
    } // 1 if


    else{ // 1 else
      
      
      if (cFile.available()>1){ // 2 if
        char myChar = cFile.read();
        if (myChar=='/'){ //3 if
        
          if (cFile.peek()=='/'){ 
            commentMode=true; 
            continue;   }
            
          else { 
            if(!writeCmdValue(myChar)){ return 0; }    
          }  // 4 else        
        } // 3 if
  
        else if (myChar=='='){ 
          readMode=READ_VALUE; 
          continue;   }
          
        else if (myChar=='\n' || myChar=='\r'){ //3 else if
          if (strcmp(cmd, " ")==0 || strcmp(cmd, "")==0 ) { continue; }
          else { 
            callbackSequence(); 
            continue;   }
        } // 3 else if
  
        else { //3 else
          if (!writeCmdValue(myChar)){ return 0; } 
        } // 3 else
      }// 2 if

      else{ // 2 else
        char myChar = cFile.read();
        if (myChar=='\n' || myChar=='\r'){  callbackSequence(); }

        else{
          writeCmdValue(myChar);
          callbackSequence(); }
          
      } // 2 else
      
   } // 1 else
    
  }//while  
  
  //Serial.println("End of File");
  return 1;
  
}//function




void SDConfigCommand::callbackSequence(){
// Call the user defined callback function after each command and value is read
// and let user decide what to do with them

  callback_Function();

  // Reset all relevant variable for next read
  // cmd and value will be empty string
  cmd[0]='\0'; 
  value[0]='\0';
  readMode=READ_CMD;
  commentMode=false; 
}




bool SDConfigCommand::writeCmdValue(char myChar){
// Concat the incoming char into to form the command or value

   if (readMode==READ_CMD){ 
    byte L = strlen(cmd);
    if ( L < (CMD_SIZE-2) ){ // Make sure there is enough memory, account for NULL and new char
      cmd[L]=myChar; // Append to last space
      cmd[L+1]='\0'; // Manually write in NULL
      return 1;   }
    else {  return 0; }
   }

   // Switch over the value is readMode is set to READ_VALUE
   else{
      byte L = strlen(value);
      if ( L < (VALUE_SIZE-2) ){
        value[L]=myChar;
        value[L+1]='\0';
        return 1;  }
      else {  return 0; }
   }
}



bool SDConfigCommand::openInSerial(){
// Dumps the text in one file all into the Serial monitor

  if (openFile(cFile, filename, FILE_READ)){
    Serial.print(F("--- "));
    Serial.print(filename);
    Serial.println(F(" ---"));

    while (cFile.available()>0){  Serial.write(cFile.read()); }
      
    Serial.println(F("\n---------"));
    cFile.close();
    
    return 1;
  }

  else {
    Serial.println(F("Cannot open in serial"));
    return 0; }
}
