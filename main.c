#include <msp430.h>

// Motor control pins
#define LEFT_IN1_PIN BIT2 // P2.2
#define LEFT_IN2_PIN BIT4 // P1.4
#define RIGHT_IN3_PIN BIT5 //P1.5
#define RIGHT_IN4_PIN BIT3 //P1.3
#define LEFT_PWM_PIN BIT0  //P2.0(ENA on HBridge)
#define RIGHT_PWM_PIN BIT4 //P2.4(ENB on HBridge)


// Joystick center and threshold values
#define JOY_X_CENTER  512
#define JOY_Y_CENTER  512
#define THRESHOLD     100

volatile unsigned int xValue = JOY_X_CENTER;
volatile unsigned int yValue = JOY_Y_CENTER;
volatile unsigned char byteCount = 0;
volatile unsigned int rxBuffer[2] = {0,0}; // [0]=X, [1]=Y

// UART RX interrupt service routine
#pragma vector=USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void) {
    static unsigned int tempVal = 0;
    unsigned char byte = UCA0RXBUF;

    if (byteCount % 2 == 0) {
        tempVal = (byte & 0x03) << 8; // High 2 bits
    } else {
        tempVal |= byte; // Low 8 bits
        rxBuffer[byteCount / 2] = tempVal;
    }

    byteCount++;

    if (byteCount >= 4) { // 2 bytes each for X and Y
        xValue = rxBuffer[0];
        yValue = rxBuffer[1];
        byteCount = 0;
    }
}

void initUART(void) {
    P2SEL0 |= BIT0 | BIT1;
    P2SEL1 &= ~(BIT0 | BIT1);

    UCA0CTL1 |= UCSWRST;
    UCA0CTL1 |= UCSSEL_2; // SMCLK
    UCA0BR0 = 104;        // 9600 baud @ 1MHz
    UCA0BR1 = 0;
    UCA0MCTLW = 0xD600;
    UCA0CTL1 &= ~UCSWRST;
    UCA0IE |= UCRXIE;     // Enable RX interrupt
}

void initMotors(void) {
    // Set left and right direction pins as output
    P2DIR |= LEFT_IN1_PIN;   // Set P2.2 as output
    P1DIR |= LEFT_IN2_PIN;   // Set P1.4 as output
    P1DIR |= RIGHT_IN3_PIN;  // Set P1.5 as output
    P1DIR |= RIGHT_IN4_PIN;  // Set P1.3 as output

    // Set left and right PWM (enable) pins as output
    P2DIR |= LEFT_PWM_PIN;   // Set P2.0 as output (ENA)
    P2DIR |= RIGHT_PWM_PIN;  // Set P2.4 as output (ENB)
}

void forward(void) {
    P2OUT |= LEFT_IN1_PIN;
    P1OUT &= ~LEFT_IN2_PIN; //left motor forward
    P1OUT |= RIGHT_IN3_PIN;
    P1OUT &= ~RIGHT_IN4_PIN; //right motor forward
    P2OUT |= LEFT_PWM_PIN;
    P2OUT |= RIGHT_PWM_PIN;
}

void backward(void) {
    P2OUT &= ~LEFT_IN1_PIN;
    P1OUT |= LEFT_IN2_PIN; //left motor back
    P1OUT &= ~RIGHT_IN3_PIN;
    P1OUT |= RIGHT_IN4_PIN; //right motor back
    P2OUT |= LEFT_PWM_PIN;
    P2OUT |= RIGHT_PWM_PIN;
}

void right(void) {
    P2OUT |= LEFT_IN1_PIN;
    P1OUT &= ~LEFT_IN2_PIN; //left motor forward
    P1OUT &= ~RIGHT_IN3_PIN;
    P1OUT |= RIGHT_IN4_PIN; //right motor back
    P2OUT |= LEFT_PWM_PIN;
    P2OUT |= RIGHT_PWM_PIN;
}

void left(void) {
    P2OUT &= ~LEFT_IN1_PIN;
    P1OUT |= LEFT_IN2_PIN; //left motor back
    P1OUT |= RIGHT_IN3_PIN;
    P1OUT &= ~RIGHT_IN4_PIN; //right motor forward
    P2OUT |= LEFT_PWM_PIN;
    P2OUT |= RIGHT_PWM_PIN;
}

void stop(void) {
    // Disable all direction pins
    P2OUT &= ~LEFT_IN1_PIN;
    P1OUT &= ~LEFT_IN2_PIN;
    P1OUT &= ~RIGHT_IN3_PIN;
    P1OUT &= ~RIGHT_IN4_PIN;

    // stop both motors
    P2OUT &= ~LEFT_PWM_PIN;
    P2OUT &= ~RIGHT_PWM_PIN;
}

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;
    initMotors();
    initUART();

    __enable_interrupt();

    while (1) {
        // Decide motor action from received joystick values
        if (yValue > JOY_Y_CENTER + THRESHOLD) {
            forward();
        } else if (yValue < JOY_Y_CENTER - THRESHOLD) {
            backward();
        } else if (xValue > JOY_X_CENTER + THRESHOLD) {
            right();
        } else if (xValue < JOY_X_CENTER - THRESHOLD) {
            left();
        } else {
            stop();
        }
    }
}
