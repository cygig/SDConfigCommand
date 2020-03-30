#include "SDConfigCommand.h"

char* SDConfigCommand::getCmd(){
  return cmd;
}



String SDConfigCommand::getCmdS(){
// Somehow String can take char[] as a value
// So no need to convert

  return cmd;
}



char* SDConfigCommand::getValue(){
  return value;
}



String SDConfigCommand::getValueS(){
  return value;
}



int SDConfigCommand::getValueInt(){
  return atoi(value);
}



float SDConfigCommand::getValueFloat(){
  return atof(value); 
}
