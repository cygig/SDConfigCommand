# Introduction
SDConfigCommand streamlines reading settings from a config file on SD card. 

SDConfigCommand can read standardised text files stored on a SD card, parse and tokenise them into commands and values. The library can also write over existing settings but it is currently slow to do so. 

For every line on the config file this library reads, it will callback a user-specified function. The user can access the current command and values, then decide the next action, such as verifying commands and storing values in variables. With regards to writing over existing settings, user can choose one command, and the library will search for the command in the config file and replace the whole setting line with a new value. The library does not add or remove settings.

Arduino String return and parameter type is supported for beginners, C-string is also available for advanced users.

---

# Updates
- v0.24
  - Change the default string length for command to 32 btyes (31 characters) and value to 16 bytes (15 characters).
  - File name is now exactly 13 bytes (8.3 file name)
  - Byte instead of integer to count TRY
  - Bug fix on all `strncpy` to deduct one from string size (to account for NULL)
  - Corrected keywords.txt to include alternation read and write functions
  - Number of tries changed to 10
  - Shortened certain error messages to save memory

---
# Configuration File

The configuration file (config file) must have a 8.3 file name. That means a maximum of eight characters for file name and three for the file extension, for example "hello.txt" is fine while "123456789.text" is not.

Each setting takes one line, with the command on the left, followed by an equal sign, then the value on the right like:

```
comments=true
speed=100
name=Araragi`
```

Note that it is recommended to use the Windows style \n\r **Carriage Return+Line Feed** for a new line, just Line Feed or Carriage Return works as well, but note that the serial monitor will not display Carriage Return in a new line. Also you are required to stick to ANSI characters (a-z, A-Z, 0-9, common symbols on a QWERTY keyboard), no multi-byte characters.

The following are not recommended, they might not work or might lead to confusion:

```
comments = true
     =100
name="Araragi"
```

The following will not work and likely cause undesirable side effects:
```
comments
 =100
name: Araragi
```

Comments can be written after two slashes like in the Arduino IDE. Due to the limitation of Arduino, minimising comments and arranging the settings to be read sequentially will improve performance. 

Here is an example of a valid config file `setting.cfg`:

```
// CONFIG FILE //
// Comments are behind double strokes

length=128
width=256
height=10
welcomeMsg=Hello world!
```

---

# Connection to SD Card
There are plenty of examples online on how to use a SD card with Arduino. To connect Uno to a SD module with LEVEL SHIFTER: 5V to VCC, GND to GND, 11 to MOSI, 12 to MISO, 13 to SCK, 4 to Chip Select. 

**Remember to use level shifters for 5V Arduinos.**

---

# Default Values
The default values for some of the variables are as follow:
- Command Length 32 bytes
- Value length 16 bytes
- 10 tries to read SD card and open files
- Temporary file name is _temp.tem_

For the command and value of a setting in C-string, one character takes one byte, with a NULL character at the end. Therefore 32 bytes can only store 31 characters, though that should be enough for most cases.

To save memory, a byte is used for counting command, value and the number of tries, thus `cmd`, `value` and `TRY` should be strictly more than 0 and less than 256 (1-255);

**You need to work within the limit of these values.** You can change their definition in the header file.

---
# Public Functions

## SDConfigCommand()
Constructor, not used.

## _bool_ set(char* myFilename, int myCs, void (*myFunction)())
Setup the library. `myFilename` is the file name of the config file in C-string (character array). `myCs` is the chip selection pin. and `myFunction` is the callback function that will activate after each setting is read. Used in `setup()`.

This function will also try to access the SD card. 

Returns 1 if successful, else 0.

## _bool_ set(String myFilename, int myCs, void (*myFunction)())
Same as the one above, but takes Arduino String for filename instead of C-string.

## _bool_ readConfig();
Reads the config file and calls the callback function after each setting (example `height=10`) from the file is read. Users can then use `getCmd`, `getCmdS`, `getValue`, `getValueS`, `getValueInt` or `getValueFloat` to access the current command (`height`) and value (`20`) for further action.

Returns 1 if successful, else 0.

## _bool_ writeConfig(char* thisCmd, char* thisValue)
Finds `thisCmd` in the config file and replace the value with `thisValue`, both in C-string. For example, if it was previously `height=10` in `setting.cfg`, running `writeConfig("height", "999")` will result in `height=10` replaced with `height=999` in the same file.

Note that this function is based on the Arduino SD functions, which does not have any rename feature nor can it write to the middle of the file. Thus, to achieve this feature:
- Create a temporary file.
- Copy the top half of the original file over.
- Write the updated setting line.
- Copy the bottom half of the original file over.
- Delete original file.
- Create original file with the same filename.
- Copy everything over from temporary file.
- Delete temporary file.

In short, this function is slow and it takes a few seconds to change just one line of setting, this please use sparingly.

Returns 1 if successful, else 0.

## _bool_ writeConfig(String thisCmd, String thisValue)
The same fuction as the one above, but accepts Arduino Strings instead of C-string.

## _bool_ openInSerial()
Opens the file and dump its contents into the Serial Monitor. Great for debugging.

Returns 1 if successful, else 0.

## _char*_ getCmd()
Return the current command (`height` in `height=10`) in C-string.

## _String_ getCmdS()
Return the current command (`height` in `height=10`) in Arduino String.

## _char*_ getValue()
Return the current value (`10` in `height=10`) in C-string.

## _String_ getValueS()
Return the current value (`10` in `height=10`) in Arduino String.

## _int_ getValueInt()
Return the current value (`10` in `height=10`) as an integer.
 
## _float_ getValueFloat()
Return the current value (`10` in `height=10`) as a floating point number.

---

# Alternate Read and Write
Included in the source code is one alternate function to read in the setting and another to write setting. They are `ALTreadConfig()` and `ALTwriteConfig()`. They should yield the same results as the default functions and they seemed to perform worst. Feel free to try them out. Those functions and function declarations might be commented away in the source code and header file, so remember to uncomment them before using.

The main difference is that the default function parses the characters as they are streaming in from the config file, while the alternate one stores one whole line of setting before parsing the line.

---

# Read/Write Flowcharts
The flowchart below depicts how the configuration file is read and write to in lieu of comments in the source code. Please note that the flowchart is only a guide, not all variables and functions are exactly the same in working and in name as the code. Certain simple logics are also glossed over in the flowchart.

## Read Config File Flowchart
![Image](flowcharts/SDConfigCommand%20Read%20Config%20(Char%20Analysis)_Page_1.jpg)
![Image](flowcharts/SDConfigCommand%20Read%20Config%20(Char%20Analysis)_Page_2.jpg)

## Write Config File Flowchart
![Image](flowcharts/SDConfigCommand%20Write%20Config%20(Char%20Analysis)_Page_1.jpg)
![Image](flowcharts/SDConfigCommand%20Write%20Config%20(Char%20Analysis)_Page_2.jpg)

---

