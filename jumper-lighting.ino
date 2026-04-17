#include <Arduino.h>
#include <Wire.h>
#include <IoAbstraction.h>
#include <IoAbstractionWire.h>

#define COCKPIT_L_PIN 16
#define RCOMPARTMENT_L_PIN 0
#define CONSOLE_LEFT_L_PIN 2
#define CONSOLE_RIGHT_L_PIN 14
#define ENGINE_POD_L_PIN 17

MCP23017IoAbstraction DHD_EXP_1(0x20, &Wire);
MCP23017IoAbstraction DHD_EXP_2(0x21, &Wire);

const uint8_t NON_EXPANSION_DHD_PINS[] = {
    12, 13, 15, 3, 1};

#define MAX_PARAMS 5
#define DHD_MOD_ID 0b101

const uint8_t MODULES[] = {COCKPIT_L_PIN, RCOMPARTMENT_L_PIN, CONSOLE_LEFT_L_PIN, CONSOLE_RIGHT_L_PIN, ENGINE_POD_L_PIN};
typedef union Command
{
    struct
    {
        byte nParam : 3;
        byte module : 3;
        byte state : 1;
        byte toggle : 1;
    };
    byte rawCommand;
};

void handleDHD(Command command, byte param_values[])
{
    Serial.println("Handling DHD command");
    switch (command.nParam)
    {
    case 0:
        for (uint8_t pin = 0; pin < 16; pin++)
        {
            byte state1 = command.state;
            byte state2 = command.state;
            if (command.toggle)
            {
                state1 = !DHD_EXP_1.digitalRead(pin);
                state2 = !DHD_EXP_2.digitalRead(pin);
            }
            DHD_EXP_1.digitalWrite(pin, state1);
            DHD_EXP_2.digitalWrite(pin, state2);
        }
        Serial.print(DHD_EXP_1.sync());
        Serial.println(DHD_EXP_2.sync());
        break;
    case 1:
        // TODO 1 param byte
        Serial.println("1 byte param behaviour NYI");
        break;
    case 2:
        // TODO 2 param bytes
        Serial.println("2 byte param behavior NYI");
        break;
    case 5:
        // TODO 5 param bytes
        Serial.println("5 byte param behavior NYI");
        break;
    default:
        Serial.println("Invalid number of parameters for DHD command");
    }
    
}

void setup()
{
    Serial.begin(115200);
    Wire.begin(4, 5);
    delay(3000);
    for (uint8_t i : MODULES)
    {
        pinMode(i, OUTPUT);
        digitalWrite(i, LOW);
    }
    for (pinid_t pin = 0; pin < 16; pin++)
    {
        DHD_EXP_1.pinMode(pin, OUTPUT);
        DHD_EXP_2.pinMode(pin, OUTPUT);
    }
    /* all that actually matters here is that the toggle bit
       is 0 and the state bit is 0 and that the nParams is 0
       as the function doesnt check anything else.
       i have used the correct module id as good practice
    */
    handleDHD({.module = DHD_MOD_ID}, nullptr);
}

void loop()
{
    while (Serial.available() == 0)
    {
        // wait for incoming data
    }
    Command command = {.rawCommand = Serial.read()};
    byte params[MAX_PARAMS];
    for (int i = 0; i < command.nParam; i++)
    {
        while (Serial.available() == 0)
        {
            // wait for the next parameter
        }
        byte value = Serial.read();
        if (i < MAX_PARAMS)
        {
            params[i] = value;
        }
    }
    Serial.println("Received command:");
    Serial.printf("Raw command: 0x%02x\nToggle: %d, "
                  "State: %d, Module: %d, nParam: %d\n",
                  command.rawCommand,
                  command.toggle,
                  command.state,
                  command.module,
                  command.nParam);
    if (command.module >= sizeof(MODULES) + 1)
    {
        Serial.println("Invalid module");
        return;
    }
    // DHD module is handled separately as it has more complex functionality
    if (command.module == DHD_MOD_ID)
    {
        handleDHD(command, params);
        return;
    }

    char state = command.state ? HIGH : LOW;
    if (command.toggle)
    {
        state = !digitalRead(MODULES[command.module]);
    }
    digitalWrite(MODULES[command.module], state);
}