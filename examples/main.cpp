#include <Arduino.h>
#include "StreamCommandParser.h"

StreamCommandParser streamCommandParser(Serial, "streamCommandParser");

void hi_handler(StreamCommandParser& commandParser) {
    commandParser.preferredResponseStream.println("Whazzup?");
}

void sum_handler(StreamCommandParser& commandParser) {
    int arg1 = atoi(commandParser.next());
    int arg2 = atoi(commandParser.next());

    commandParser.preferredResponseStream.println(arg1 + arg2);
}

void default_handler(const char * command, StreamCommandParser& commandParser) {
    commandParser.preferredResponseStream.println("Unrecognized Command");
}

void setup() {
    Serial.begin(9600);
    streamCommandParser.addCommand("hi", hi_handler);
    streamCommandParser.addCommand("sum", sum_handler);
    streamCommandParser.setDefaultHandler(default_handler);
    Serial.println("ready...");
}

void loop() {
    streamCommandParser.readSerial(Serial);
}

