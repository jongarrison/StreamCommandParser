#include "sCommand.h"
#include <limits>

/**
 * Constructor
 */
sCommand::sCommand(bool caseSensitive)
  :

	commandList(NULL),
    commandCount(0),
    defaultHandler(NULL),
    term('\n'),           // default terminator for commands, newline character
    caseSensitive(caseSensitive),
	cur(NULL),
	last(NULL)
{
  strcpy(delim, " "); // strtok_r needs a null-terminated string
  clearBuffer();
}

/**
 * Adds a "command" and a handler function to the list of available commands.
 * This is used for matching a found token in the buffer, and gives the pointer
 * to the handler function to deal with it.
 */
void sCommand::addCommand(const char *command, void (*function)(sCommand& streamCommandParser, Stream& stream)) {
  #ifdef SERIALCOMMAND_DEBUG
    Serial.print(" - Adding Command (");
    Serial.print(commandCount);
    Serial.print("): ");
    Serial.println(command);
  #endif

  commandList = (StreamCommandParserCallback *) realloc(commandList, (commandCount + 1) * sizeof(StreamCommandParserCallback));
  strncpy(commandList[commandCount].command, command, STREAMCOMMAND_MAXCOMMANDLENGTH);
  commandList[commandCount].function.f1 = function;
  commandList[commandCount].ptr = NULL;

  if (!caseSensitive) {
    strlwr(commandList[commandCount].command);
  }

  commandCount++;
}
template <typename DataType>
void sCommand::addCommandInternal(const char *command, void (sCommand::*function)(sCommand& streamCommandParser, Stream& stream, DataType* variable, const char* command), DataType* var) {
  #ifdef SERIALCOMMAND_DEBUG
    Serial.print(" - Adding Assign Variable Command (");
    Serial.print(commandCount);
    Serial.print("): ");
    Serial.println(command);
  #endif
    commandList = (StreamCommandParserCallback *) realloc(commandList, (commandCount + 1) * sizeof(StreamCommandParserCallback));
	strncpy(commandList[commandCount].command, command, STREAMCOMMAND_MAXCOMMANDLENGTH);

	if ( var == NULL) {
		//catch NULL pointer and trap with function that can handle it
		commandList[commandCount].function.f2 =  &sCommand::invalidAddress;
		commandList[commandCount].ptr = (void*) 1;
	} else {
		commandList[commandCount].function.f2 = (void(sCommand::*)(sCommand& streamCommandParser, Stream& stream, void* ptr, const char* command)) function;
		commandList[commandCount].ptr = (void*) var;
	}

	if (!caseSensitive) {
	   strlwr(commandList[commandCount].command);
	}
	commandCount++;
}

void sCommand::assignVariable(const char* command, bool* variable) {
	addCommandInternal(command,&sCommand::reportBool, variable);
}

void sCommand::assignVariable(const char* command, int8_t* variable) {
	addCommandInternal(command,&sCommand::reportInt, variable);
}

void sCommand::assignVariable(const char* command, int16_t* variable) {
	addCommandInternal(command,&sCommand::reportInt, variable);
}

void sCommand::assignVariable(const char* command, int* variable) {
	addCommandInternal(command,&sCommand::reportInt, variable);
}

void sCommand::assignVariable(const char* command, long* variable) {
	addCommandInternal(command,&sCommand::reportInt, variable);
}

void sCommand::assignVariable(const char* command, uint8_t* variable) {
	addCommandInternal(command,&sCommand::reportUInt, variable);
}

void sCommand::assignVariable(const char* command, uint16_t* variable) {
	addCommandInternal(command,&sCommand::reportUInt, variable);
}

void sCommand::assignVariable(const char* command, unsigned int* variable) {
	addCommandInternal(command,&sCommand::reportUInt, variable);
}

void sCommand::assignVariable(const char* command, unsigned long* variable){
	addCommandInternal(command,&sCommand::reportUInt, variable);
}

void sCommand::assignVariable(const char* command, double* variable) {
	addCommandInternal(command,&sCommand::reportFloat,variable);
}


void sCommand::assignVariable(const char* command, float* variable) {
	addCommandInternal(command,&sCommand::reportFloat,variable);
}

void sCommand::invalidAddress(sCommand& sC, Stream& S, void* ptr, const char* command) {
	S.printf("Invalid memory address assigned to command %s\n",command);
}

bool sCommand::str2Bool(const char* string) {
	bool result = false;
	const uint8_t stringLen = 10;
	char tempString[stringLen+1];
	strncpy(tempString,string,stringLen); //make copy of argument to convert to lower case
	tempString[stringLen] = '\0'; //null terminate in case arg is longer than size of tempString
	strlwr(tempString);

	if (strcmp(tempString,"on") == 0) result = true;
	else if (strcmp(tempString,"true") == 0) result = true;
	else if (strcmp(tempString,"1") == 0) result = true;
	else if (strcmp(tempString,"off") == 0) result = false;
	else if (strcmp(tempString,"false") == 0) result = false;
	else if (strcmp(tempString,"0") == 0) result = false;
	return result;

}


void sCommand::reportBool(sCommand& sC, Stream& S, bool* ptr, const char* command) {
	if ( sC.next() != NULL) {
		*ptr = sC.str2Bool(command);
	}

	S.printf("%s is %s\n",command, *ptr ? "true":"false");
}

template <class argInt>
void sCommand::reportInt(sCommand& sC, Stream& S, argInt* ptr, const char* command) {
	if ( sC.next() != NULL) {
		int value = atoi(sC.current());
		if ( value < std::numeric_limits<argInt>::min()) {
			*ptr = std::numeric_limits<argInt>::min();
		} else if ( value > std::numeric_limits<argInt>::max()) {
			*ptr = std::numeric_limits<argInt>::max();
		} else {
			*ptr = value;
		}
	}
	S.printf("%s is %d\n",command,*ptr);
}


template <class argUInt>
void sCommand::reportUInt(sCommand& sC, Stream& S, argUInt* ptr, const char* command) {
	if ( sC.next() != NULL) {
		int temp = atoi(sC.current());
		if (temp < 0) {
			*ptr = 0;
		} else {
			unsigned long value = strtoul(sC.current(),NULL,10);
			if ( value > std::numeric_limits<argUInt>::max()) {
				*ptr = std::numeric_limits<argUInt>::max();
			} else {
				*ptr = value;
			}
		}
	}
	S.printf("%s is %u\n",command,*ptr);
}

template <class argFloating>
void sCommand::reportFloat(sCommand& sC, Stream& S, argFloating* ptr, const char* command) {
	if ( sC.next() != NULL) {
		argFloating newValue = atof(sC.current());

		if ( sizeof(argFloating) >  4 ) { //make setting variable atomic for doubles or anything greater than 32bits.
			//__disable_irq(); //JG - This should be revisited in a platform independent way (conditionally using portENTER_CRITICAL() on some platforms)
			*ptr = newValue;
			//__enable_irq(); //JG - This should be revisited in a platform independent way (conditionally using portEXIT_CRITICAL() on some platforms)
		} else {
			*ptr = newValue;
		}
	}
	if ( ( abs(*ptr) > 10 ) || (abs(*ptr) < .1) ) {
		S.printf("%s is %e\n",command,*ptr); //print gain in scientific notation
	} else {
		S.printf("%s is %f\n",command,*ptr);
	}
}


/**
 * This sets up a handler to be called in the event that the receveived command string
 * isn't in the list of commands.
 */
void sCommand::setDefaultHandler(void (*function)(const char *, sCommand& streamCommandParser, Stream& stream)) {
  defaultHandler = function;
}

/**
 * This checks the Serial stream for characters, and assembles them into a buffer.
 * When the terminator character (default '\n') is seen, it starts parsing the
 * buffer for a prefix command, and calls handlers setup by addCommand() member
 */
void sCommand::readSerial(Stream& inputStream) {
    while (inputStream.available() > 0) {
    char inChar = inputStream.read();   // Read single available character, there may be more waiting
    #ifdef SERIALCOMMAND_DEBUG
      Serial.print(inChar);   // Echo back to serial stream
    #endif

    if (inChar == term) {     // Check for the terminator (default '\n') meaning end of command
      #ifdef SERIALCOMMAND_DEBUG
        Serial.print("Received: ");
        Serial.println(buffer);
      #endif

        if (!caseSensitive) {
		  strlwr(buffer);
        }
      char *command = strtok_r(buffer, delim, &last);   // Search for command at start of buffer
      if (command != NULL) {
        boolean matched = false;
        for (int i = 0; i < commandCount; i++) {
          #ifdef SERIALCOMMAND_DEBUG
            Serial.print(" - Comparing [");
            Serial.print(command);
            Serial.print("] to [");
            Serial.print(commandList[i].command);
            Serial.println("]");
          #endif

          // Compare the found command against the list of known commands for a match
          if (strncmp(command, commandList[i].command, STREAMCOMMAND_MAXCOMMANDLENGTH) == 0) {
            #ifdef SERIALCOMMAND_DEBUG
              Serial.print("Matched Command: ");
              Serial.println(command);
            #endif

            // Execute the stored handler function for the command
              if (commandList[i].ptr == NULL) {
            	  (*commandList[i].function.f1)(*this,inputStream);
              } else {
            	  (this->*commandList[i].function.f2)(*this,inputStream,commandList[i].ptr,commandList[i].command);
              }

            matched = true;
            break;
          }
        }
        if (!matched) {
        	if (defaultHandler != NULL) {
                (*defaultHandler)(command, *this, inputStream);
        	} else {
        		inputStream.print("Unknown command: ");
        		inputStream.println(command);
        	}
        }
      }
      bufPos = 0; //do not clear buffer to enter after command repeats it.
    } else if (inChar == '\b') { //backspace detected
      if (bufPos > 0) {
          bufPos--; //move back bufPos to overright previous character
          buffer[bufPos] = '\0';// Null terminate
      }
    } else if (isprint(inChar)) {     // Only printable characters into the buffer
      if (bufPos < STREAMCOMMAND_BUFFER) {
        buffer[bufPos++] = inChar;  // Put character into buffer
        buffer[bufPos] = '\0';      // Null terminate
      } else {
        #ifdef SERIALCOMMAND_DEBUG
          Serial.println("Line buffer is full - increase STREAMCOMMAND_BUFFER");
        #endif
      }
    }
  }
  // Serial.print("Time OUT: ");
  // Serial.println(millis());
}


/*
 * Print list of all available commands
 */
void sCommand::printAvailableCommands(Stream& outputStream) {
  for (int i = 0; i < commandCount; i++) {
    outputStream.println(commandList[i].command);
  }
}

/*
 * Clear the input buffer.
 */
void sCommand::clearBuffer() {
  buffer[0] = '\0';
  bufPos = 0;
}

/**
 * Retrieve the next token ("word" or "argument") from the command buffer.
 * Returns NULL if no more tokens exist.
 */
char *sCommand::next() {
  cur = strtok_r(NULL, delim, &last);
  return cur;
}

/**
 * Retrieve the current token ("word" or "argument") from the command buffer.
 * Returns NULL if no more tokens exist.
 */
char *sCommand::current() {
  return cur;
}
