/*
 Clock Calibration Project (main.c)

 Calibrates the DCO using the CLK32 XTal

 This program provides serveral calibration frequencies for the DCO
 Uses an external 32768 Hz used as a reference

 The program calibrates the DCO clock for the following frequences:
 500kHz,1MHz,2MHz,4MHz,6MHz,8MHz,10MHz,12MHZ,16MHz

 But it's easy to change this values

 The calibrated DCOCTL and BCSCTL1 values are stored in the
 flash information memory Section B.
 The file NewDCOCal.h provides the positions of each calibrated value

 The complete program can be used in MSP430 value line MCUs with
 2kB of flash memory or more if size optimization is used for compiling.

 External pins used:

     Xin / Xout : External 32768 Hz 12.5pF Quartz Cristal

     P1.0 : Red Led to ground
     P1.3 : Pushbutton to ground
     P1.4 : Outputs MCLK
     P1.5 : Outputs 32kHz clock divided by 128
     P1.6 : Green Led to ground

 Program operation:

    When the program starts, it checks if there is data in the
    calibration area in information area section B.
    If there is data it enters loop frequency mode.
    If there is no data, it calibrates the selected frequencies and
    stores the calibration data in information area section B
    then, it enters in loop frequency mode.
    During calibration the P.14 pin outputs the current DCO frequency.
    During calibration the green led blinks each time it changes the frequency.
    If a frequency scan must be repeated, both red and green leds will blink.

    In loop frequency mode, the system programs the DCO to the first
    calibration data in section B. A push of the button changes to the
    next frequency. After the last frequency it returns to the first one.
    During loop frequency mode the P1.4 pin outputs DCO frequency.
    In loop frequency mode the gren led blinks with a frequency
    equal to DCO frequency divided by 20*2^16  (20 Timer A0 Overflows)

    In case of error the program locks showing an error code using a
    number of blinks on the red led:

       1 32768 Hz oscilator fault
       2 One Frequency cannot be obtained
       4 No factory DCO cal for 1MHz to guarantee flash timing
       5 A calibration has more tan the maximum allowed frequency error

       Continuous red led: 32768 Hz oscilator fault

  Calibration operation:

    Watchdog is configured as timer with 64/fclk32 period
    Timer A is configured with DCO input
    The number of DCO cycles (diff) between Watchdog overflows is computed.
    The DCO frequency is calculated:

              DCO freq = 512 diff

    The counter differences are averaged during NCAP cycles (50 by default) to
    reduce the error.

  Conditional compilation defines:

    If the DEBUG define is activated, the program stores the
    RSEL,DCO and MOD oscillator values in array for each one.
    It also stores the frequency error of each calibration.

    If the FLASH_OVERRIDE define is activated, the program will override
    any value stored in the flash information area section B.

    If the TEST_MODE define is activated, the program will do the calibration
    and the final loop frequency mode, but all the information will be
    stored only on RAM so, the flash information area will not be used at all.

   */

/********** CONDITIONAL COMPILATION DEFINES ************************/

// Flash Override
// If active, calibration data will override non blank flash on Section B
//#define FLASH_OVERRIDE

// Test Mode
// Do not touch or reads the flash
// Only works in RAM to test operations
//#define TEST_MODE

// Debug define
//#define DEBUG   // Eliminate to reduce code and memory usage

/***************** OTHER OPERATION DEFINES *****************************/

// Max allowed error (in %)
#define MAX_ERROR 5

// Max test cycles for each frequency
#define MAX_CYCLES 10

// Number of captures to average
#define NCAP 50

/********************* INCLUDES ***************************************/

#include <msp430.h>
#include <msp430g2553.h>
#include <legacymsp430.h>   // Needed for interrupts
#include "io430masks.h"     // My SET/RESET macros
#include "NewDCOCal.h"      // Positions to store new calibration data

/******************** OTHER DEFINES **********************************/

#define LED_RED     BIT0             // Red LED at P1.0
#define LED_GREEN   BIT6             // Green LED at P1.6
#define F_OUT       BIT5             // Freq out for CLK32/128
#define SMCLK_PIN   BIT4             // SMCLK output at P1.4
#define SWITCH      BIT3             // Switch input

/******** Constants with data for the frequecies to scan *************/

// Number of frequencies to scan
#define NFREQ 9

// Scan frequencies in kHz
#ifdef DEBUG
const unsigned int  FreqK[NFREQ] ={500,1000,2000,4000,6000 ,8000 ,10000,12000,16000};
#endif

// Goal counter values for each frequency (freq(Hz)/512)
const unsigned int  GoalN[NFREQ] ={977,1953,3906,7813,11719,15625,19531,23438,31250};

// Calibration data position for each frequency (from NewDCOCal.h)
const unsigned int  CalPos[NFREQ]={NCALDCO_500kHZ_
                                  ,NCALDCO_1MHZ_
                                  ,NCALDCO_2MHZ_
                                  ,NCALDCO_4MHZ_
                                  ,NCALDCO_6MHZ_
                                  ,NCALDCO_8MHZ_
                                  ,NCALDCO_10MHZ_
                                  ,NCALDCO_12MHZ_
                                  ,NCALDCO_16MHZ_};

/*************** VARIABLES **********************************/

// Calibration data for each frequency
// To be stored in information memory at the end of calibration
unsigned char CalDCO[NFREQ];
unsigned char CalBC1[NFREQ];

// Data found for each frequency (Only in DEBUG mode)
#ifdef DEBUG
unsigned char FoundRsel[NFREQ];  // RSEL value
unsigned char FoundDco[NFREQ];   // DCO value
unsigned char FoundMod[NFREQ];   // Modulation value
         char FoundErr[NFREQ];   // Frequency error in %
#endif

// Last captured value on Timerer A2 module 0
unsigned int LastCapture=0;

// Last difference in captured values
unsigned int LastDifference;

// Averaged differences
unsigned int AveragedDifference;

// Number of captures
volatile int ncap=0;

// Current oscilator values during calibration
int rsel=0;
int dco=0;
int mod=0;

// Long value to average
unsigned long int Mean;

// Blink counter
int bcount=0;

/*************************** FUNCTIONS **********************/


// Simple delay function
void simpleDelay()
 {
 __delay_cycles(1000);
 }

// Long delay function
void longDelay()
 {
 int i;
 for(i=0;i<20;i++)
	 __delay_cycles(10000);
 }

// Locks the system with an error code
// Error code must be 1 or greater
// The error is shown with a number of blinks
// This function never returns
void errorLock(int error)
 {
 int i;

 // Calibrated DCO data for 1MHz
 DCOCTL=CALDCO_1MHZ;    // DCOCTL   Cal. data for 1MHz
 BCSCTL1=CALBC1_1MHZ;   // BCSCTL1  Cal. data for 1MHz

 while (1)
   {
   // As blinks as error code number
   for(i=0;i<error;i++)
       {
	   SET_FLAG(P1OUT,LED_RED);
	   longDelay();
	   RESET_FLAG(P1OUT,LED_RED);
	   longDelay();
       }

   // A final space to separate the blink series
   for(i=0;i<8;i++)
            longDelay();
   }
 }

// One fast blink on green led
void ledBlink(unsigned char bits)
 {
 SET_FLAG(P1OUT,bits);
 longDelay();
 RESET_FLAG(P1OUT,bits);
 }

// Start of the Clk32 clock
// Uses default value of DIVA_0
void startClk32()
 {
 do
   {
	BCSCTL3=XCAP_3; // Set 12.5pF
	simpleDelay();
   }
   while (BCSCTL3&LFXT1OF); // Loop while faulty
 }

// Configure all the peripherals
void configureAll()
 {
 // Calibrated DCO data for 1MHz
 DCOCTL=CALDCO_1MHZ;    // DCOCTL   Cal. data for 1MHz
 BCSCTL1=CALBC1_1MHZ;   // BCSCTL1  Cal. data for 1MHz

 // Set LEDs and F_OUT clk32/128 as output
 SET_FLAG(P1DIR,LED_RED+LED_GREEN+F_OUT+SMCLK_PIN);

 // Turn on red led, clear the rest of P1
 P1OUT=LED_RED;

 // Start the 32k clock
 startClk32();

 // Turn OFF port 1 (Red led included)
 P1OUT=0;

 // Configure WDT as a timer
 WDTCTL=WDT_ADLY_1_9;   // t=64/fACLK=1.9ms
 SET_FLAG(IE1,WDTIE);   // Enable WDT interrupt

 // Configure the Timer A

 // TACTL
 // TASSEL_2   Use SMCLK
 // ID_0       Divide by 1
 // MC_2       Mode Up Continuous
 TACTL=TASSEL_2+ID_0+MC_2;

 // TACCTL0
 // CAP        Capture mode
 // CCIE       Enable capture 0 interrupt
 // CM_3       Capture on both edges
 // CCIS_0     Capture on GND
 TACCTL0=CAP+CCIE+CM_3+CCIS_2;

 // Program P1.4 as SMCLK output
 SET_FLAG(P1SEL,SMCLK_PIN);
 }

// Programs the DCO with
// rsel: Range 0..15
// dco:  Step 0..7
// mod:  Modulation 0..31
// Then lets the Oscilator work for several NCAP captures
// to average the frequency measure.
void setDCO(int rsel,int dco,int mod)
 {
 // Set BSCTL1
 BCSCTL1=XT2OFF+rsel;

 // Set DCOCTL
 DCOCTL=dco*DCO0+mod;


 // Loops for several captures
 dint();
 Mean=0;  // No captures yet
 ncap=-5; // 5 cycles before start averaging
 eint();
 while (ncap<NCAP) {};
 AveragedDifference=(unsigned int)(Mean/NCAP);
 }


// Search for the indicated goal counter value
// Returns 0 if the frequency is obtained
// Returns 1 if cannot be obtained
int searchGoal(unsigned int CurrentNGoal)
 {
 unsigned int prev;
 int m0diff;

 // Search RSEL ranges with central DCO and
 // no modulation
 for(rsel=0;rsel<16;rsel++)
   {
   // Set central DCOx
   setDCO(rsel,3,0);
   if (AveragedDifference>CurrentNGoal) break;
   prev=AveragedDifference;
   }

 if (rsel<16) // If we breaked...
   {
   if (rsel)  // and there is a previous rsel...
	   //                and error was less before...
      if ((CurrentNGoal-prev)<(AveragedDifference-CurrentNGoal))
    	                                 rsel--;
   }
  else
   rsel=15;	 // If we don't breaked, limit rsel to 15

 // Now we have RSEL value so we search for DCO value

 // Search DCO value
 for(dco=0;dco<8;dco++)
    {
	setDCO(rsel,dco,0);
	if (AveragedDifference>CurrentNGoal) break;
    }

 // If we did not break cannot modulate
 if (dco>7) return 1;

 // If dco=0 we cannot modulate also
 if (!dco) return 1;

 // Backup value of last error before MOD modulation
 m0diff=AveragedDifference-CurrentNGoal;

 // Now we have DCO value we then search for MOD value

 // Take base freq less than over goal
 dco--;

 // Search MOD value
 for(mod=0;mod<32;mod++)
    {
 	setDCO(rsel,dco,mod);
 	if (AveragedDifference>CurrentNGoal) break;
 	prev=AveragedDifference;
    }

 if (mod<32)  // If we breaked
  {
  if (mod) // If mod not zero...
	 // If lower mod got less error...
	 if ((AveragedDifference-CurrentNGoal)>(CurrentNGoal-prev))
		       mod--;
  }
 else     // If we didn't breaked...
  {
  mod=31; // Limit for mod
  }

 // Check special case
 if (mod==31)
   if (m0diff<(AveragedDifference-CurrentNGoal))
           {
 	      dco++;
 	      mod=0;
           }


 // Set final DCO configuration
 setDCO(rsel,dco,mod);

 return 0;
 }

// Test if the flash to program in section B is empty
// If the zone has any data, return 1
// If the zone is empty, return 0
int testFlashEmpty()
 {
 int i;
 unsigned char *ptr;

 // Check every frequency cal location
 for(i=0;i<NFREQ;i++)
     {
	 ptr=(unsigned char *) CalPos[i];
	 if ((*(ptr++))!=0xFF) return 1;  // Check DCOCTL
	 if (*ptr!=0xFF) return 1;        // Check BCSCTL1
     }

 return 0;
 }

// Writes the calibration data found to flash
// information area in section B
void flashWrite()
 {
 int i;
 unsigned char *pFlash;

 // Flash Timing

 // Test for 1MHz calibration data
 // as we need known DCO frequency for flash timing.
 if (CALBC1_1MHZ ==0xFF || CALDCO_1MHZ == 0xFF)
	    errorLock(4);

 dint(); // Disable interrupts in flash operation

 // Set calibrated DCO data for 1MHz
 DCOCTL=CALDCO_1MHZ;    // DCOCTL   Cal. data for 1MHz
 BCSCTL1=CALBC1_1MHZ;   // BCSCTL1  Cal. data for 1MHz

 // Set Flash freq. as 333kHz
 // It's supposed that MCLK is 1MHz
 // Divider is 3
 FCTL2 = FWKEY + FSSEL_1 + FN1;

 // Unlock the flash
 FCTL3=FWKEY;

 // We only need to erase if override
 #ifdef FLASH_OVERRIDE
   // Erase individual segment only
   FCTL1 = FWKEY + ERASE;

   // Dummy Write
   pFlash = (unsigned char *) CalPos[0];
  (*pFlash)=0;
 #endif

 // Set write mode
 FCTL1 = FWKEY + WRT;

 // Write calibration data
 for(i=0;i<NFREQ;i++)
      {
	  pFlash = (unsigned char *) CalPos[i];
	  (*pFlash)=CalDCO[i];
	  pFlash++;
	  (*pFlash)=CalBC1[i];
      }

 // End flash operation
 FCTL1 = FWKEY; // Clear WRT bit
 FCTL3 = FWKEY + LOCK; // Set LOCK bit

 eint(); // Enable interrupts
 }


// Loop through all the frequencies
// If flash is true, it loops through flash data
// If not, it loops thorugh RAM data
// Never returns
void loopFrequencies(int flash)
 {
 int i;
 unsigned char *pFlash;

 // Turn On Green Led
 SET_FLAG(P1OUT,LED_GREEN);

 // DCO is set for SMCLK

 // Enable TAR Timer Overflow Interrupt
 // Used to blink the green led
 SET_FLAG(TACTL,TAIE);

 // Program SWITCH as input with pull-up
 RESET_FLAG(P1DIR,SWITCH);    // Input mode
 SET_FLAG(P1OUT,SWITCH);      // Output "1" for Pull-Up
 SET_FLAG(P1REN,SWITCH);      // Resistor Enable

 i=0;
 while (1) // Do forever
       {
	   if (flash)
	       {
		   // Set the frequency from flash
		   pFlash = (unsigned char *) CalPos[i];
		   DCOCTL=*pFlash;
		   pFlash++;
		   BCSCTL1=*pFlash;
		   }
	      else
	      {
	      // Set the frequency form memory table
	      DCOCTL=CalDCO[i];
	      BCSCTL1=CalBC1[i];
	      }

	   // Wait to release switch if was pressed
	   while (!(P1IN&SWITCH)) {};

	   // Turn Off Red Led
	   RESET_FLAG(P1OUT,LED_RED);

	   // Loops for several captures to debounce
	   ncap=0;
	   while (ncap<200) {};

	   // Wait to press switch
	   while (P1IN&SWITCH) {};

	   // Turn On Red Led
	   SET_FLAG(P1OUT,LED_RED);

	   // Increase frequency
	   i++;
	   if (i>=NFREQ) i=0;
	   }
 }

// Main function
int main()
 {
 int i,j,error;

 // Disable the watchdog.
 WDTCTL = WDTPW + WDTHOLD;

 // Configure all the peripherals
 configureAll();

 // Enable interrupts
 eint();

 // Flash is tested to be empty if
 // test mode and flash override are not selected
 #ifndef TEST_MODE
   #ifndef FLASH_OVERRIDE
      // Test if the zone to program is empty
      if (testFlashEmpty())
          {
    	  // Loop from flash if data found
          loopFrequencies(1);
          }
   #endif
 #endif

 // Search for each frequency
 for(i=0;i<NFREQ;i++)
    {
	// If clock fault, generate error 1
	if (BCSCTL3&LFXT1OF) errorLock(1);

    // Blinks green led
	ledBlink(LED_GREEN);

	// Search for each frequency
	for(j=0;j<MAX_CYCLES;j++)
	    {
        // Blinks red led if repeating
	    if (j) ledBlink(LED_GREEN+LED_RED);

	    // Error lock 2 if cannot adquire frequency
        if (searchGoal(GoalN[i])) errorLock(2);

        // Compute frequency error in %
	    error=(char)((100*((long int)AveragedDifference-(long int)GoalN[i]))/GoalN[i]);
	    if ((error<=MAX_ERROR)&&(error>=(-MAX_ERROR))) break;
	    }

	// If we didn't breaked then we are out of tolerance error
	if (j>=MAX_CYCLES) errorLock(5);

	// Store calibration data
	CalDCO[i]=DCOCTL;
	CalBC1[i]=BCSCTL1;

	// Store debug information if enabled
    #ifdef DEBUG
      FoundRsel[i]=rsel;
      FoundDco[i]=dco;
	  FoundMod[i]=mod;
	  FoundErr[i]=error;
	#endif
    }

 // Return to calibrated DCO data for 1MHz
 DCOCTL=CALDCO_1MHZ;    // DCOCTL   Cal. data for 1MHz
 BCSCTL1=CALBC1_1MHZ;   // BCSCTL1  Cal. data for 1MHz


 #ifdef TEST_MODE
  // Loop from ram table
  loopFrequencies(0);
 #endif

 // Write the flash
 flashWrite();

 // Loop from flash
 loopFrequencies(1);

 // We should never arrive to this point
 // as loopFrequencies never returns

 // Locks with green led
 SET_FLAG(P1OUT,LED_GREEN);
 while(1) {};

 // This function never returns
 return 0;
 }

/********************** RSI FUNCTIONS *******************/

// Watchdog timer interrupt
// Called each 64/fCLK32 period (1,9ms)
// Generates a capture in module 0 of timer 0
// each time it is called.
interrupt(WDT_VECTOR) WDT_ISR(void)
 {
 // Don't need to clear any flag

 // Toggle Fout
 P1OUT^=F_OUT;

 // Generate a capture by changing the capture
 // input between Vdd and GND using CCIS0
 TACCTL0^=CCIS0;
 }

// Capture 0 interrupt
// A capture is generated each time the
// Watchdog timer overflows
// The RSI computes the diference between captures
interrupt(TIMER0_A0_VECTOR) CAPTURE0_ISR(void)
 {
 // Don't need to clear any flag

 // Current distance between captures
 LastDifference=TACCR0-LastCapture;
 // Current last capture
 LastCapture=TACCR0;

 if (ncap>=0)
	  if (ncap<NCAP)
		 Mean+=LastDifference;

 // We don't want to roll over
 if (ncap<10000) ncap++;
 }

// General Timera A0 ISR
interrupt(TIMER0_A1_VECTOR) Timer0_ISR(void)
 {
 if (TA0IV==TA0IV_TAIFG)
      {
	  bcount++;
	  if (bcount>=10)
	      {
	      P1OUT^=LED_GREEN;
	      bcount=0;
	      };
      }
 }

