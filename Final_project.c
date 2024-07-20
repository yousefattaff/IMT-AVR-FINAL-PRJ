#include "../HAL/LCD/LCD_int.h"
#include "../HAL/LED/LED_int.h"
#include "../HAL/ultrasonic/ultrasonic.h"
#include <stdlib.h>

// Define pin and port configurations
#define METAL_PIN 0
#define METAL_PORT 1
#define RED_LED_PIN 1
#define GREEN_LED_PIN 2
#define YELLOW_LED_PIN 3
#define BUZZER_PIN 6
#define LED_PORT 1
#define BUZZER_PORT 3
#define MOTOR_FORWARD 7
#define MOTOR_BACKWARD 6
#define MOTOR_PORT 0

// Function to move the motor forward
void motor_FORWARD()
{
    DIO_enuSetPinValue(MOTOR_PORT, MOTOR_FORWARD, DIO_u8_HIGH);
    DIO_enuSetPinValue(MOTOR_PORT, MOTOR_BACKWARD, DIO_u8_LOW);
}

// Function to stop the motor
void motor_STOP()
{
    DIO_enuSetPinValue(MOTOR_PORT, MOTOR_FORWARD, DIO_u8_LOW);
    DIO_enuSetPinValue(MOTOR_PORT, MOTOR_BACKWARD, DIO_u8_LOW);
}

int main(void)
{
    int16_t ULTRA_DIS = 0; // Variable to store ultrasonic sensor distance
    char DIS_SHOW[16]; // Array to store distance as string for display
    u8 pin_value;
    u8 obstacle_detected = 0; // Flag to track obstacle detection state
    LCD_enuInitialize(); // Initialize LCD
    initializeDistanceMeasurement(); // Initialize ultrasonic sensor

    // Set pin directions for various components
    DIO_enuSetPinDirection(METAL_PORT, METAL_PIN, DIO_u8_INPUT);
    DIO_enuSetPinDirection(LED_PORT, RED_LED_PIN, DIO_u8_OUTPUT);
    DIO_enuSetPinDirection(LED_PORT, GREEN_LED_PIN, DIO_u8_OUTPUT);
    DIO_enuSetPinDirection(LED_PORT, YELLOW_LED_PIN, DIO_u8_OUTPUT);
    DIO_enuSetPinDirection(BUZZER_PORT, BUZZER_PIN, DIO_u8_OUTPUT);
    DIO_enuSetPinDirection(MOTOR_PORT, MOTOR_FORWARD, DIO_u8_OUTPUT);
    DIO_enuSetPinDirection(MOTOR_PORT, MOTOR_BACKWARD, DIO_u8_OUTPUT);

    // Ensure motor is initially stopped
    DIO_enuSetPinValue(MOTOR_PORT, MOTOR_FORWARD, DIO_u8_LOW);
    DIO_enuSetPinValue(MOTOR_PORT, MOTOR_BACKWARD, DIO_u8_LOW);

    while (1)
    {
        // Perform actions for distance measurement
        ULTRA_DIS = measureDistance(); // Get distance from ultrasonic sensor
        LCD_voidGotoXY(1, 0); // Move LCD cursor to first row
        LCD_vidSendStr("Distance="); // Display distance label
        itoa(ULTRA_DIS, DIS_SHOW, 10); // Convert distance to string
        LCD_vidSendStr(DIS_SHOW); // Display distance value
        LCD_vidSendStr(" cm"); // Display unit (centimeters)
        _delay_ms(500); // Delay for stability

        // Check if there is an obstacle nearby
        if (ULTRA_DIS >= 3 && ULTRA_DIS <= 6)
        {
            // Perform actions only if an obstacle is detected
            motor_STOP(); // Stop the motor
            LED_enuturnOffLED(LED_PORT, GREEN_LED_PIN); // Turn off green LED
            LED_enuturnOffLED(LED_PORT, RED_LED_PIN); // Turn off red LED
            LED_enuturnOnLED(LED_PORT, YELLOW_LED_PIN); // Turn on yellow LED
            DIO_enuSetPinValue(BUZZER_PORT, BUZZER_PIN, DIO_u8_LOW); // Turn off buzzer
            LCD_voidGotoXY(2, 0); // Move LCD cursor to second row
            LCD_vidSendStr("Obstacle nearby"); // Display obstacle message
            obstacle_detected = 1; // Set obstacle detected flag
        }
        else if (!obstacle_detected) // If no obstacle nearby, proceed with other checks
        {
            // Perform actions if no obstacle is detected
            // Check if there is no metal detected
            DIO_enuGetPinValue(METAL_PORT, METAL_PIN, &pin_value);
            if (pin_value == DIO_u8_LOW)
            {
                // Perform actions only if metal is not detected
                motor_FORWARD(); // Move the motor forward
                LED_enuturnOnLED(LED_PORT, GREEN_LED_PIN); // Turn on green LED
                LED_enuturnOffLED(LED_PORT, RED_LED_PIN); // Turn off red LED
                LED_enuturnOffLED(LED_PORT, YELLOW_LED_PIN); // Turn off yellow LED
                DIO_enuSetPinValue(BUZZER_PORT, BUZZER_PIN, DIO_u8_LOW); // Turn off buzzer
                LCD_voidGotoXY(2, 0); // Move LCD cursor to second row
                LCD_vidSendStr("No metal detected"); // Display no metal message
            }
            else
            {
                // Stop the motor and turn on the red LED and buzzer if metal is detected
                motor_STOP(); // Stop the motor
                LED_enuturnOffLED(LED_PORT, GREEN_LED_PIN); // Turn off green LED
                LED_enuturnOffLED(LED_PORT, YELLOW_LED_PIN); // Turn off yellow LED
                LED_enuturnOnLED(LED_PORT, RED_LED_PIN); // Turn on red LED
                DIO_enuSetPinValue(BUZZER_PORT, BUZZER_PIN, DIO_u8_HIGH); // Turn on buzzer
                LCD_enuSendInstruction(LCD_u8_CLEAR_DISPLAY); // Clear the LCD display
                LCD_voidGotoXY(2, 0); // Move LCD cursor to second row
                LCD_vidSendStr("Metal detected"); // Display metal detected message
            }
        }

        // Reset the obstacle detection flag if no obstacle is detected
        if (ULTRA_DIS < 3 || ULTRA_DIS > 6)
        {
            obstacle_detected = 0; // Reset obstacle detection flag
        }
    }

    return 0;
}
