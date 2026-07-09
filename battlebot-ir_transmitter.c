#include <msp430.h>

#define JOY_X_INPUT_PIN    BIT7  // P2.7 for VRx
#define JOY_Y_INPUT_PIN    BIT6  // P2.6 for VRy

#define JOY_X_ADC_CHANNEL  7     // A7
#define JOY_Y_ADC_CHANNEL  6     // A6

void initjoystick(void) {
    // Configure pins as analog input
    P2DIR &= ~(JOY_X_INPUT_PIN | JOY_Y_INPUT_PIN);
    P2SEL0 |= (JOY_X_INPUT_PIN | JOY_Y_INPUT_PIN);
    P2SEL1 |= (JOY_X_INPUT_PIN | JOY_Y_INPUT_PIN);

    ADC12CTL0 &= ~ADC12ENC;
    ADC12CTL0 = ADC12SHT0_2 | ADC12ON;
    ADC12CTL1 = ADC12SHP | ADC12CONSEQ_1; // single sequence of channels
    ADC12CTL2 = ADC12RES_1; // 10-bit resolution
    ADC12MCTL0 = ADC12INCH_7; // MEM0 = A7 (VRx)
    ADC12MCTL1 = ADC12INCH_6 | ADC12EOS; // MEM1 = A6 (VRy)
    ADC12CTL0 |= ADC12ENC;
}

void initUART(void) {
    // USCI_A0 UART on P2.0 (UCA0TXD) and P2.1 (UCA0RXD)
    P2SEL0 |= BIT0 | BIT1;
    P2SEL1 &= ~(BIT0 | BIT1);

    UCA0CTL1 |= UCSWRST; // Put state machine in reset
    UCA0CTL1 |= UCSSEL_2; // SMCLK
    UCA0BR0 = 104;        // 9600 baud (for 1MHz clock)
    UCA0BR1 = 0;
    UCA0MCTLW = 0xD600;   // Modulation
    UCA0CTL1 &= ~UCSWRST; // Initialize USCI state machine
}

void readJoystick(unsigned int *x, unsigned int *y) {
    ADC12CTL0 |= ADC12SC; // Start conversion sequence
    while (ADC12CTL1 & ADC12BUSY);

    *x = ADC12MEM0; // VRx
    *y = ADC12MEM1; // VRy
}

void sendJoystickUART(unsigned int x, unsigned int y) {
    // Send X value (10-bit, send as two bytes)
    while (!(UCA0IFG & UCTXIFG));
    UCA0TXBUF = (x >> 8) & 0x03; // High 2 bits
    while (!(UCA0IFG & UCTXIFG));
    UCA0TXBUF = x & 0xFF;        // Low 8 bits

    // Send Y value (10-bit, send as two bytes)
    while (!(UCA0IFG & UCTXIFG));
    UCA0TXBUF = (y >> 8) & 0x03;
    while (!(UCA0IFG & UCTXIFG));
    UCA0TXBUF = y & 0xFF;
}

int main(void) {
    WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer

    initjoystick();
    initUART();

    while (1) {
        unsigned int xValue;
        unsigned int yValue;

        readJoystick(&xValue, &yValue);

        sendJoystickUART(xValue, yValue); // Send joystick X/Y

        __delay_cycles(10000); // Delay for stability
    }
}
