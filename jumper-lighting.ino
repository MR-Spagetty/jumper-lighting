#include <Arduino.h>
#define COCKPIT_L_PIN D5
#define RCOMPARTMENT_L_PIN D6

#define MAX_PARAMS 5
#define DHD_MOD_ID 0b101

const uint8_t modules[] = {COCKPIT_L_PIN, RCOMPARTMENT_L_PIN};
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

void setup()
{
    pinMode(COCKPIT_L_PIN, OUTPUT);
    pinMode(RCOMPARTMENT_L_PIN, OUTPUT);
    digitalWrite(COCKPIT_L_PIN, LOW);
    digitalWrite(RCOMPARTMENT_L_PIN, LOW);
    Serial.begin(115200);
}

void handleDHD(Command command, byte param_values[])
{
    Serial.println("Handling DHD command");
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
    Serial.printf("Raw command: 0x%x\nToggle: %d, State: %d, Module: %d, nParam: %d\n", command.rawCommand, command.toggle, command.state, command.module, command.nParam);
    if (command.module >= sizeof(modules))
    {
        Serial.println("Invalid module");
        return;
    }
    if (command.module == DHD_MOD_ID)
    {
        handleDHD(command, params);
        return;
    }

    char state = command.state ? HIGH : LOW;
    if (command.toggle)
    {
        state = !digitalRead(modules[command.module]);
    }
    digitalWrite(modules[command.module], state);
}