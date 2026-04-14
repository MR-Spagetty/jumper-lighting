#include <pwm.h>
#define COCKPIT_L_PIN D5
#define RCOMPARTMENT_L_PIN D6

void setup()
{
    pinMode(COCKPIT_L_PIN, OUTPUT);
    pinMode(RCOMPARTMENT_L_PIN, OUTPUT);
    digitalWrite(COCKPIT_L_PIN, LOW);
    digitalWrite(RCOMPARTMENT_L_PIN, LOW);
    Serial.begin(115200);
}

void loop()
{
    while (Serial.available() == 0)
    {
        /* code */
    }
    switch ((char)Serial.read())
    {
        case 'a':
            digitalWrite(COCKPIT_L_PIN, !digitalRead(COCKPIT_L_PIN));
            break;
        case 'b':
            digitalWrite(RCOMPARTMENT_L_PIN, !digitalRead(RCOMPARTMENT_L_PIN));
            break;
    }
}