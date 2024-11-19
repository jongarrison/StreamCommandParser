#include "sCommand.h"
sCommand sC;
//sCommand sC = sCommand(true); //Use this line instead for case-sensitive commands

double loopGain = 1.021;
int anInteger;

void setup() {
  sC.setDefaultHandler(UnknownCommand);
  sC.addCommand("Hello", hello);
  sC.addCommand("Hi", hello);
  sC.addCommand("Gain",gain);
  sC.assignVariable("Int",&anInteger);
  sC.addCommand("Mult",multiply);
  sC.addCommand("Help", help);
}

void loop() {
  sC.readSerial(Serial);
  sC.readSerial(Serial2);
}

void hello(sCommand& sC, Stream& S) {
  if ( sC.next() == NULL) {
    S.println("Hello.");
  } else {
    S.printf("Hello %s, it is nice to meet you.\n",sC.current());
  }
}

void gain(sCommand& sC, Stream& S) {
  if ( sC.next() != NULL) {
    loopGain = atof(sC.current());
    if (loopGain < 0) {
      loopGain = 0;
    } else if (loopGain > 10) {
      loopGain = 10;
    }
 }
 S.printf("The gain is %f\n",loopGain);
}

void multiply(sCommand& sC, Stream& S) {
  double a;
  int b;
  if ( sC.next() == NULL) {
    S.println("The multiply command needs two arguments, none given.");
    return;
  } else {
    a = atof(sC.current());
  }
  if ( sC.next() == NULL) {
    S.println("The multiply command needs two arguments, only one given.");
    return;
  } else {
    b = atoi(sC.current());
  }
  S.printf("%e times %d is %e\n",a,b,a*b);
}

void help(sCommand& sC, Stream& S) {
  S.println("Available commands are:");
  sC.printAvailableCommands(S);
}

void UnknownCommand(const char* command, sCommand& sC, Stream& S) {
  S.printf("I'm sorry, I didn't understand that. (You said '%s'?)\n",command);
  S.println("You can type 'help' for a list of commands");
}
