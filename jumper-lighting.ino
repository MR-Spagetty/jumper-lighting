#include <Arduino.h>
#include <Wire.h>
#include <IoAbstraction.h>
#include <IoAbstractionWire.h>

#define COCKPIT_L_PIN 14
#define RCOMPARTMENT_L_PIN 12
#define CONSOLE_LEFT_L_PIN 16
#define CONSOLE_RIGHT_L_PIN 17
#define ENGINE_POD_L_PIN 13

MCP23017IoAbstraction DHD_EXP_1(0x20, &Wire);
MCP23017IoAbstraction DHD_EXP_2(0x21, &Wire);

const uint8_t NON_EXPANSION_DHD_PINS[] = {
    0, 2, 3, 15, 1};
typedef struct DHDPinMapping
{
    byte loc : 2;
    /* 0 = MCU pin
    1 = expander 1
    2 = expander 2
    */
    uint8_t pin;
};
const DHDPinMapping DHD_PINS[] = {
    /*
           00 01 02 __ 03 04 05
        06 07 08 09 10 11 12 13 14

        15 16 17 18 19 20 21 22 23
           24 25 26 27 28 29 30
              31 32 33 34 35
                    36
    */
    {.loc = 1, .pin = 1},
    {.loc = 1, .pin = 2},
    {.loc = 1, .pin = 3},
    {.loc = 1, .pin = 4},
    {.loc = 1, .pin = 5},
    {.loc = 1, .pin = 6},
    {.loc = 1, .pin = 7},
    {.loc = 1, .pin = 8},
    {.loc = 1, .pin = 9},
    {.loc = 1, .pin = 10},
    {.loc = 1, .pin = 11},
    {.loc = 1, .pin = 12},
    {.loc = 1, .pin = 13},
    {.loc = 1, .pin = 14},
    {.loc = 1, .pin = 15},
    {.loc = 2, .pin = 0},
    {.loc = 2, .pin = 1},
    {.loc = 2, .pin = 2},
    {.loc = 2, .pin = 3},
    {.loc = 2, .pin = 4},
    {.loc = 2, .pin = 5},
    {.loc = 2, .pin = 6},
    {.loc = 2, .pin = 7},
    {.loc = 2, .pin = 8},
    {.loc = 2, .pin = 9},
    {.loc = 2, .pin = 10},
    {.loc = 2, .pin = 11},
    {.loc = 2, .pin = 12},
    {.loc = 2, .pin = 13},
    {.loc = 2, .pin = 14},
    {.loc = 2, .pin = 15},
    {.loc = 0, .pin = NON_EXPANSION_DHD_PINS[0]},
    {.loc = 0, .pin = NON_EXPANSION_DHD_PINS[1]},
    {.loc = 0, .pin = NON_EXPANSION_DHD_PINS[2]},
    {.loc = 0, .pin = NON_EXPANSION_DHD_PINS[3]},
    {.loc = 0, .pin = NON_EXPANSION_DHD_PINS[4]},
    {.loc = 1, .pin = 0}, // Engage
};

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