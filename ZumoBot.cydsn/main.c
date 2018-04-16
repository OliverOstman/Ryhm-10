/**
* @mainpage ZumoBot Project
* @brief    You can make your own ZumoBot with various sensors.
* @details  <br><br>
    <p>
    <B>General</B><br>
    You will use Pololu Zumo Shields for your robot project with CY8CKIT-059(PSoC 5LP) from Cypress semiconductor.This 
    library has basic methods of various sensors and communications so that you can make what you want with them. <br> 
    <br><br>
    </p>
    
    <p>
    <B>Sensors</B><br>
    &nbsp;Included: <br>
        &nbsp;&nbsp;&nbsp;&nbsp;LSM303D: Accelerometer & Magnetometer<br>
        &nbsp;&nbsp;&nbsp;&nbsp;L3GD20H: Gyroscope<br>
        &nbsp;&nbsp;&nbsp;&nbsp;Reflectance sensor<br>
        &nbsp;&nbsp;&nbsp;&nbsp;Motors
    &nbsp;Wii nunchuck<br>
    &nbsp;TSOP-2236: IR Receiver<br>
    &nbsp;HC-SR04: Ultrasonic sensor<br>
    &nbsp;APDS-9301: Ambient light sensor<br>
    &nbsp;IR LED <br><br><br>
    </p>
    
    <p>
    <B>Communication</B><br>
    I2C, UART, Serial<br>
    </p>
*/

#include <project.h>
#include <stdio.h>
#include "Systick.h"
#include "Motor.h"
#include "Ultra.h"
#include "Nunchuk.h"
#include "Reflectance.h"
#include "I2C_made.h"
#include "Gyro.h"
#include "Accel_magnet.h"
#include "IR.h"
#include "Ambient.h"
#include "Beep.h"
#include <time.h>
#include <sys/time.h>
int rread(void);

/**
 * @file    main.c
 * @brief   
 * @details  ** Enable global interrupt since Zumo library uses interrupts. **<br>&nbsp;&nbsp;&nbsp;CyGlobalIntEnable;<br>
*/

#if 0
//battery level//
int main()
{
    CyGlobalIntEnable; 
    UART_1_Start();
    Systick_Start();
    
    ADC_Battery_Start();        

    int16 adcresult = 0;
    float volts = 0.0;

    printf("\nBoot\n");

    //BatteryLed_Write(1); // Switch led on 
    BatteryLed_Write(0); // Switch led off 
    //uint8 button;
    //button = SW1_Read(); // read SW1 on pSoC board
    // SW1_Read() returns zero when button is pressed
    // SW1_Read() returns one when button is not pressed

    for(;;)
    { 
        ADC_Battery_StartConvert();
        if(ADC_Battery_IsEndConversion(ADC_Battery_WAIT_FOR_RESULT)) {   // wait for get ADC converted value
            adcresult = ADC_Battery_GetResult16(); // get the ADC value (0 - 4095)
            volts = (adcresult / 4096.0) * 5; // convert value to Volts
            volts = volts * 1.5;
            if (volts < 4.0) {
                BatteryLed_Write(1);
                CyDelay(500);
                BatteryLed_Write(0);
            }
            printf("%d %.3f\r\n", adcresult, volts); // Print both ADC results and converted value
        }
        CyDelay(500);
        
    }
 }   
#endif

#if 0
// button
int main()
{
    CyGlobalIntEnable; 
    UART_1_Start();
    Systick_Start();
    
    printf("\nBoot\n");

    //BatteryLed_Write(1); // Switch led on 
    BatteryLed_Write(0); // Switch led off 
    
    //uint8 button;
    //button = SW1_Read(); // read SW1 on pSoC board
    // SW1_Read() returns zero when button is pressed
    // SW1_Read() returns one when button is not pressed
    
    bool led = false;
    
    for(;;)
    {
        // toggle led state when button is pressed
        if(SW1_Read() == 0) {
            led = !led;
            BatteryLed_Write(led);
            ShieldLed_Write(led);
            if(led) printf("Led is ON\n");
            else printf("Led is OFF\n");
            Beep(1000, 150);
            while(SW1_Read() == 0) CyDelay(10); // wait while button is being pressed
        }        
    }
 }   
#endif


#if 0
//ultrasonic sensor//
int main()
{
    CyGlobalIntEnable; 
    UART_1_Start();
    Systick_Start();
    Ultra_Start();                          // Ultra Sonic Start function
    while(1) {
        int d = Ultra_GetDistance();
        //If you want to print out the value  
        printf("distance = %d\r\n", d);
        CyDelay(200);
    }
}   
#endif


#if 0
//IR receiver//
int main()
{
    CyGlobalIntEnable; 
    UART_1_Start();
    IR_Start();
    
    uint32_t IR_val; 
    
    printf("\n\nIR test\n");
    
    IR_flush(); // clear IR receive buffer
    printf("Buffer cleared\n");
    
    IR_wait(); // wait for IR command
    printf("IR command received\n");
    
    // print received IR pulses and their lengths
    for(;;)
    {
        if(IR_get(&IR_val)) {
            int l = IR_val & IR_SIGNAL_MASK; // get pulse length
            int b = 0;
            if((IR_val & IR_SIGNAL_HIGH) != 0) b = 1; // get pulse state (0/1)
            printf("%d %d\r\n",b, l);
            //printf("%d %lu\r\n",IR_val & IR_SIGNAL_HIGH ? 1 : 0, (unsigned long) (IR_val & IR_SIGNAL_MASK));
        }
    }    
 }   
#endif


#if 1
//reflectance//
    void motor_turnRight(uint8 l_speed, uint8 r_speed, uint32 delay);
    void motor_turnLeft(uint8 l_speed, uint8 r_speed, uint32 delay);
    void motor_basicTurn (uint8 l_speed, uint8 r_speed, uint32 delay);
    void motor_veryStraight(uint8 l_speed, uint8 r_speed, uint32 delay);
    
int main()
{
    struct sensors_ ref;
    struct sensors_ dig;
    
    Systick_Start();

    CyGlobalIntEnable; 
    UART_1_Start();
    
    reflectance_start();
    reflectance_set_threshold(9000, 9000, 11000, 11000, 9000, 9000); // set center sensor threshold to 11000 and others to 9000
    
    CyDelay(5000);
    
    motor_start();

    for(;;)
    {
        // read raw sensor values
        reflectance_read(&ref);
        printf("%5d %5d %5d %5d %5d %5d\r\n", ref.l3, ref.l2, ref.l1, ref.r1, ref.r2, ref.r3);   // print out each period of reflectance sensors
        
        // read digital values that are based on threshold. 0 = white, 1 = black
        // when blackness value is over threshold the sensors reads 1, otherwise 0
        reflectance_digital(&dig);      //print out 0 or 1 according to results of reflectance period
        printf("%5d %5d %5d %5d %5d %5d \r\n", dig.l3, dig.l2, dig.l1, dig.r1, dig.r2, dig.r3);  //print out 0 or 1 according to results of reflectance period
        
        
        //Versio 4
        if (dig.l1 == 1 && dig.l2 == 1 && dig.l3 == 1 && dig.r1 == 1 && dig.r2 == 1 && dig.r3 == 1) { // Stop on black line
            motor_stop();   
        }
        else if (dig.l1 == 1 && dig.r1 == 1) { // Forward
            motor_start();
            motor_veryStraight(240,250,1);
        } 
        else if (dig.r3 == 1) { // Big turn to Right
            motor_start();
            motor_turnRight(250,80,1);
        }
        else if (dig.l3 == 1) { // Big turn to Left
            motor_start();
            motor_turnLeft(80,250,1);
        }
        else if (dig.r2 == 1) { // Turn to Right
            motor_start();
            motor_basicTurn(250,125,1);
        }
        else if (dig.l2 == 1) { // Turn to Left
            motor_start();
            motor_basicTurn(125,250,1);
        }
        else if (dig.l1 == 1) { // Small turn to Right
            motor_start();
            motor_basicTurn(250,160,1);
        }
        else if (dig.r1 == 1) { // Small turn to Left
            motor_start();
            motor_basicTurn(160,250,1);
        }
        
        /*Versio 3
        if (dig.l1 == 1 && dig.l2 == 1 && dig.l3 == 1 && dig.r1 == 1 && dig.r2 == 1 && dig.r3 == 1) {
            //Kaikki näkee mustaa
            motor_stop();
        }
        else if (dig.r3 == 1) {
            //Oikealle
            motor_start();
            motor_turnRight(200,1);
        }
        else if (dig.l3 == 1) {
            //Vasemmalle
            motor_start();
            motor_turnLeft(200,1);
        }
        else if (dig.r2 == 1) {
            //Oikealle
            motor_start();
            motor_turnRight(200,1);
        }
        else if (dig.l2 == 1) {
            //Vasemmalle
            motor_start();
            motor_turnLeft(200,1);
        }
        else if (dig.l1 == 1 && dig.r1 == 1) {
            // Suoraan
            motor_start();
            motor_forward(200,1);
        }
        */
        /* Versio 2
        if (dig.l1 == 1 && dig.r3 == 1 && dig.r2 == 1 && dig.r1 == 1 && dig.l3 == 1 && dig.l2 == 1) {
            //Kaikki näkee mustaa
            motor_stop();
        }
        else if (dig.l1 == 0 && dig.r3 == 0 && dig.r2 == 0 && dig.r1 == 0 && dig.l3 == 0 && dig.l2 == 0)  {
            //Kaikki näkee valkoista
            motor_stop();
        }
        else if (dig.l1 == 1 && dig.r1 == 1 && dig.r3 == 0 && dig.r2 == 0 && dig.l3 == 0 && dig.l2 == 0) {
            //Vain keskimmäiset näkee mustaa --> Suoraan
            motor_start();
            motor_turn(95,100,1);
        }
        else if (dig.l1 == 1 && dig.r1 == 0 && dig.r3 == 0 && dig.r2 == 0 && dig.l3 == 0 && dig.l2 == 0) {
            //Vain keskimmäinen vasen näkee mustaa --> Korjaus vasemmalle
            motor_start();
            motor_turn(75,100,1);
        }
        else if (dig.r1 == 1 && dig.l1 == 0 && dig.r3 == 0 && dig.r2 == 0 && dig.l3 == 0 && dig.l2 == 0) {
            //Vain keskimmäinen oikea näkee mustaa --> Korjaus oikealle
            motor_start();
            motor_turn(100,75,1);
        }
        else if (dig.l1 == 1 && dig.l2 == 1 && dig.l3 == 1) { 
            //Kaikki vasemmaiset näkee mustaa --> Kääntyy vasemmalle
            motor_start();
            motor_turn(35,100,1);
        }
        else if (dig.l1 == 1 && dig.l2 == 1 && dig.l3 == 0) {
            //Vain kaksi ekaa vasemmaista näkee mustaa --> Pienempi käännös vasemmalle
            motor_start();
            motor_turn(55,100,1);
        }
        else if (dig.r1 == 1 && dig.r2 == 1 && dig.r3 == 1) { 
            //Kaikki oikeammaiset näkee mustaa --> Kääntyy oikealle
            motor_start();
            motor_turn(100,35,1);
        }
        else if (dig.r1 == 1 && dig.r2 == 1 && dig.r3 == 0) {
            //Vain kaksi ekaa oikeammaista näkee mustaa --> Pienempi käännös oikealle
            motor_start();
            motor_turn(100,55,1);
        }
        */
        /* Versio 1
        if (dig.l1 == 1 && dig.r3 == 1 && dig.r2 == 1 && dig.r1 == 1 && dig.l3 == 1 && dig.l2 == 1) {
            motor_stop();
        }
        else if (dig.l1 == 0 && dig.r3 == 0 && dig.r2 == 0 && dig.r1 == 0 && dig.l3 == 0 && dig.l2 == 0)  {
            motor_stop();
        }
        else if (dig.r1 == 1 && dig.r2 == 1 && dig.r3 == 1 && dig.l3 == 0) { //90 asteen kääntö oikealle
            motor_start();
            CyDelay(50);
            motor_turnRight(100,1);
        }
        else if (dig.l1 == 1 && dig.l2 == 1 && dig.l3 == 1) { //3 sensorin kääntö vasemmalle
            motor_start();
            motor_turn(100,50,1);
        }
        else if (dig.r1 == 1 && dig.r2 == 1 && dig.r3 == 1) { //3 sensorin kääntö oikealle
            motor_start();
            motor_turn(50,100,1);
        }
        else if (dig.r1 == 1 && dig.r2 == 1) { //Oikealle pieni
            motor_start();
            motor_turn(90,65,1);
        }
        else if (dig.l1 == 1 && dig.l2 == 1) { //Vasemmalle pieni
            motor_start();
            motor_turn(65,90,1);
        }
        else if (dig.l2 == 1 && dig.l3 == 0) {//Vasemmalle kun vain vasen keskimmäinen näkee
            motor_start();
            motor_turn(65,90,1);
        }
        else if (dig.l3 == 1) {//Vasemmalle vikan sensorin käännös
            motor_start();
            motor_turn(85,100,1);
        }
        else if (dig.r3 == 1) {//Oikealle vikan sensorin käännös
            motor_start();
            motor_turn(100,85,1);
        }
        else if (dig.l1 == 1 && dig.r1 == 1) { //Suoraan
            motor_start();
            //motor_turn(190,200,1);
            motor_forward(100,1);
        }
        else if (dig.l1 == 1 && dig.r1 == 0) { //Korjaus oikealle
            motor_start();
            motor_turn(75,100,1);
        }
        else if (dig.r1 == 1 && dig.l1 == 0) { //Korjaus vasemmalle
            motor_start();
            motor_turn(100,75,1);
        }
        */
    }  
}

void motor_veryStraight(uint8 l_speed, uint8 r_speed, uint32 delay) {
    MotorDirLeft_Write(0);
    MotorDirRight_Write(0);
    PWM_WriteCompare1(l_speed); 
    PWM_WriteCompare2(r_speed); 
    CyDelay(delay);
}
void motor_turnRight(uint8 l_speed, uint8 r_speed, uint32 delay) {
    MotorDirLeft_Write(0);
    MotorDirRight_Write(1);
    PWM_WriteCompare1(l_speed); 
    PWM_WriteCompare2(r_speed); 
    CyDelay(delay);
}
void motor_turnLeft(uint8 l_speed, uint8 r_speed, uint32 delay) {
    MotorDirLeft_Write(1);
    MotorDirRight_Write(0);
    PWM_WriteCompare1(l_speed); 
    PWM_WriteCompare2(r_speed); 
    CyDelay(delay);
}
void motor_basicTurn (uint8 l_speed, uint8 r_speed, uint32 delay) {
    MotorDirLeft_Write(0);
    MotorDirRight_Write(0);
    PWM_WriteCompare1(l_speed); 
    PWM_WriteCompare2(r_speed); 
    CyDelay(delay);
}
        
#endif


#if 0
//motor//
    void motor_turnRight(uint8 l_speed,uint8 r_speed,uint32 delay);
    
int main()
{
    CyGlobalIntEnable; 
    UART_1_Start();
    
    CyDelay(1000);

    motor_start();
    
//    motor_forward(200,2000);
    motor_turn(195,200,1800);
   // motor_turn(200,0,400);
    motor_turnRight(190,200,250);
    motor_forward(0,1);
    motor_turn(195,200,1600);
   // motor_turn(200,0,400);
    motor_turnRight(190,200,250);
    motor_forward(0,1);
    motor_turn(195,200,2000);
   // motor_turn(200,0,400);
    motor_turnRight(190,200,250);
    motor_forward(0,1);
    motor_turn(200,130,2000);
    
   // motor_forward(100,2000);     // moving forward
   // motor_turn(0,200,2000);     // turn
   // motor_backward(100,2000);    // moving backward
       
    motor_stop();               // motor stop
    
    for(;;)
    {

    }
}

void motor_turnRight(uint8 l_speed,uint8 r_speed,uint32 delay) {
    MotorDirLeft_Write(0);
    MotorDirRight_Write(1);
    PWM_WriteCompare1(l_speed); 
    PWM_WriteCompare2(r_speed); 
    CyDelay(delay);
}
#endif

#if 0 
 //omat jutut//
 int main()
{
    ADC_Battery_Start();        

    int16 adcresult = 0;
    float volts = 0.0;
    
    ADC_Battery_StartConvert();
        if(ADC_Battery_IsEndConversion(ADC_Battery_WAIT_FOR_RESULT)) {   // wait for get ADC converted value
            adcresult = ADC_Battery_GetResult16(); // get the ADC value (0 - 4095)
            volts = (adcresult / 4096.0) * 5; // convert value to Volts
            volts = volts * 1.5;
            if (volts < 4.0) {
                BatteryLed_Write(1);
                CyDelay(500);
                BatteryLed_Write(0);
            }
            printf("%d %.3f\r\n", adcresult, volts); // Print both ADC results and converted value
        }
        CyDelay(500);
    
    void motor_turnLeft(uint8 speed,uint32 delay) {
    MotorDirLeft_Write(0);
    MotorDirRight_Write(1);
    PWM_WriteCompare1(speed); 
    PWM_WriteCompare2(speed); 
    CyDelay(delay);
}
    
    void motor_turnRight(uint8 speed,uint32 delay) {
    MotorDirLeft_Write(1);
    MotorDirRight_Write(0);
    PWM_WriteCompare1(speed); 
    PWM_WriteCompare2(speed); 
    CyDelay(delay);
}
}
#endif
/* [] END OF FILE */
