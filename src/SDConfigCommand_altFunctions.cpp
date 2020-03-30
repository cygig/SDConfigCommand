/*
#include "SDConfigCommand.h"

bool SDConfigCommand::ALTreadConfig(){
// Alternate function to read in config by String Analysis
// Study this flowchart for a better idea: 
// https://drive.google.com/file/d/1pWj8-veBC2ZhYhKK7kK0X06C1udDUMAr/view?usp=sharing
// You can open the file with draw.io or app.diagrams.net or view the included image files
// Uncomment the fucntion and the function name in header file to use
  if( !openFile(cFile, filename, FILE_READ) ){ return 0; }

  char setting[128];
  cmd[0]='\0';
  value[0]='\0';

  while(cFile.available() >0){ // 1 While
    char myChar = cFile.read();
    
    if (myChar=='\n' || myChar=='\r' || cFile.available()==0){ // 2 If
      if (cFile.peek()=='\n' || cFile.peek()=='\r'){  cFile.read(); }

      if(setting[0]=='/' && setting[1]=='/'){ setting[0]='\0'; continue; }
      else{ // 3 Else
        int E=-1;
        for (int i=0; i<strlen(setting); i++){
          if(setting[i] == '='){  E=i; break; }
        }

        if(E == -1){ setting[0]='\0'; continue; }
        else{ //4 Else
          for (int i=0; i<E; i++){
            cmd[i] = setting[i];  }
          cmd[E] = '\0';
            
          int C=0;
          for (int i=E+1; i<strlen(setting); i++){
            value[C] = setting[i];
            C++;  }
          value[C] = '\0';

          callback_Function();
          cmd[0]='\0';
          value[0]='\0';
          setting[0]='\0';
          continue; } // 4 Else
          
      }// 3 Else
      
    } // 2 If

    else{ // 2 Else
      int L = strlen(setting);
      if (L < sizeof(setting)-2 ){ // 3 If
        setting[L] = myChar;
        setting[L+1] = '\0';
      } // 3 If
      else{ return 0; }
    } //2 Else
  } // 1 While 
  cFile.close();
  return 1;
}



bool SDConfigCommand::ALTwriteConfig(char* myFindCmd, char* myNewValue){
// Alternate function to write config by String Analysis
// Study this flowchart for a better idea: 
// https://drive.google.com/file/d/189qikWLgSomj6859Mn5RHo4NOv_-oCnQ/view?usp=sharing
// You can open the file with draw.io or app.diagrams.net or view the included image files
// Uncomment the fucntion and the function name in header file to use

  unsigned long cmdStart;
  unsigned long cmdEnd;
  bool lastEntry=false;
  
  if( !openFile(cFile, filename, FILE_READ) ){ return 0; }

  cmd[0]='\0';
  char setting[128];

  while (cFile.available() > 0){ // 1 While
    char myChar = cFile.read();

    if (myChar=='\n' || myChar=='\r' || cFile.available()==0){ // 2 If
      
      if(setting[0]=='/' && setting[1]=='/'){ setting[0]='\0'; continue; }
      
      else{ // 3 Else
        
        int E = -1;
        
        for (int i=0; i<strlen(setting); i++) { // 4 For
          if (setting[i] == '='){ E=i; break; }          
        } // 4 For

        if (E==-1){ setting[0]='\0'; continue;}

        else{ // 4 Else
          for (int i=0; i<E; i++){ // 5 For
            cmd[i] = setting[i];  } // 5 For
          cmd[E]='\0';

          if (strcmp(myFindCmd, cmd)==0){ // 5 If
            cmdStart = cFile.position() - strlen(setting) - 1;

            if (cFile.available()==0){ // 6 If
              lastEntry=true; } // 6 If

            else { // 6 else
              if (cFile.peek() == '\n' || cFile.peek() == '\r'){ cmdEnd = cFile.position() + 1; }
              else{ cmdEnd = cFile.position(); }
            } //6 Else
            
            cFile.seek(0);

            if( SD.exists(TEMP_FILENAME) ){ // 6 If
              for (int i = 0; i<TRY; i++){ // 7 For
                if ( SD.remove(TEMP_FILENAME) ){ break; }
                else if (i==TRY-1){ Serial.println(F("Cannot delete temp file.")); return 0; }
              } // 7 For
            } // 6 If

            File tempFile;
            if ( !openFile(tempFile, TEMP_FILENAME, FILE_WRITE) ){ return 0; }

            for (unsigned long i=0; i<cmdStart; i++){
              tempFile.write(cFile.read()); }
              
            tempFile.print(myFindCmd);
            tempFile.print('=');
            tempFile.print(myNewValue);

            if (!lastEntry){
              tempFile.println();
              cFile.seek(cmdEnd);
              while(cFile.available()>0){
               tempFile.write(cFile.read());  }
            }
            
            tempFile.flush();
            cFile.close();

            for (int i=0; i<TRY; i++){ // 6 For
              if( SD.remove(filename) ){ break; }
              else if (i==TRY-1){ Serial.println(F("Cannot delete config file.")); return 0; }
            } // 6 For

            if ( !openFile(cFile, filename, FILE_WRITE) ){ return 0; }

            tempFile.seek(0);
            while(tempFile.available()>0){ // 6 While
              cFile.write(tempFile.read());
            } // 6 While

            cFile.close();

            for (int i=0; i<TRY; i++){  //6 For
              if ( SD.remove(TEMP_FILENAME) ){ break; }
              else if (i==TRY-1){ Serial.println(F("Cannot delete temp file.")); }
            } // 6 For

            return 1;
            
          } // 5 If

          else{ // 5 Else
            setting[0]='\0';
            cmd[0]='\0';
            continue;
          } // 5 Else
          
        } // 4 Else
        
      } // 3 Else
    } // 2 If

    else{ // 2 Else
      int L = strlen(setting);
      if (L < sizeof(setting) - 2){ // 3 If
        setting[L] = myChar;
        setting[L+1] = '\0';
      }// 3 If

      else{ return 0; }
      
    } // 2 Else

  } // 1 While

  return 1;

} // Function
*/
