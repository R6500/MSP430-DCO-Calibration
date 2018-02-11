
/*********************************************

Masks for register bit fields

(c) VJS

Version 28-06-2011

Modificaciones:
     28-06-2011 : Version inicial
     17-04-2012 : Cambios en la definicion de algunas mascaras

**********************************************/

/********************************************* 

 Example for a 8 bit register REG with   
	1 bit field BIT
        2 bit field F0,F1

 REG|=BIT     To set BIT
 REG&=~BIT    To clear BIT

 For F0,F1 a mask is defined:
 F01_MASK = (0xFF-3*F0)

 REG&=F01_MASK    	       To clear field
 REG=(REG&F01_MASK)+F0         To set F0 and not F1
 REG=(REG&F01_MASK)+F1         To set F1 and not F0
 REG=(REG&F01_MASK)+F0+F1      To set F0 and F1
 REF=(REG&F01_MASK)+(F0*value) To set value 0..3

***********************************************/

/*************** BASIC DEFINITIONS *****************

This definitions define the following macros:

 SET_FLAG(REGISTER,FLAG)	 Set a 1 bit flag
 RESET_FLAG(REGISTER,FLAG)       Reset a 1 bit flag
 RESET_FIELD(REGISTER,MASK)      Clear a multibit field
 
 SET_FIELD(REGISTER,MASK,VALUE)  
 Set a multibit field (value must be consistent with mask)
 
 SET_FIELD_WOFFSET(REGISTER,MASK,VALUE,OFFSET)
 Set a multibit field adding an offset 
 El nombre del campo referencia al ultimo bit del campo, no el primero

 ** Posibilidad de modificar las macros para usar BIS y BIC

***************************************************/

#define SET_FLAG(REGISTER,FLAG)        REGISTER|=(FLAG)
#define RESET_FLAG(REGISTER,FLAG)       REGISTER&=~(FLAG)

#define CLEAR_FIELD(REGISTER,MASK)      REGISTER&=(MASK)
#define SET_FIELD(REGISTER,MASK,VALUE)  REGISTER=(REGISTER&(MASK))+VALUE
#define SET_FIELD_WOFFSET(REGISTER,MASK,VALUE,OFFSET) REGISTER=(REGISTER&(MASK))+((VALUE)*(OFFSET))

#define GET_FIELD(REGISTER,MASK) (REGISTER&(MASK))

/**************** ADC 10 **********************/

#ifdef __MSP430_HAS_ADC10__ 

/* ADC10CTL0 @ 0x01B0 ADC10 Control 0 */

#define ADC10SHT_OFFS  (0x800u)
#define ADC10SHT_MASK  (0xFFFF-3*ADC10SHT_OFFS)     /* 2 bit mask */

#define ADC10SREF_OFFS (0x2000u)
#define ADC10SREF_MASK (0xFFFF-7*ADC10SREF_OFFS)    /* 3 bit mask */


/* ADC10CTL1 @ 0x01B2 ADC10 Control 1 */

#define ADC10CONSEQ_OFFS (2u)
#define ADC10CONSEQ_MASK (0xFFFF-3*ADC10CONSEQ_OFFS)  /* 2 bit mask */

#define ADC10SSEL_OFFS   (8u)
#define ADC10SSEL_MASL   (0xFFFF-3*ADC10SSEL_OFFS)    /* 2 bit mask */

#define ADC10DIV_OFFS    (0x20u)
#define ADC10DIV_MASK    (0xFFFF-7*ADC10DIV_OFFS)    /* 3 bit mask */

#define SHS_OFFS	 (0x400u)
#define SHS_MASK	 (0xFFFF-3*SHS_OFFS)         /* 2 bit mask */

#define INCH_OFFS	 (0x1000u)
#define INCH_MASK    (0xFFFFu-0xF*INCH_OFFS)	     /* 4 bit mask */


#endif

/********* BASIC CLOCK MODULE ****************/

#ifdef __MSP430_HAS_BC2__

/* DCOCTL @ 0x0056 DCO Clock Frequency Control */

#define MOD_OFFS  (MOD0)
#define MOD_MASK  (0xFF-0x1F*MOD0)  /* 5 bit mask */

#define DCO_OFFS  (DCO0)
#define DCO_MASK  (0xFF-7*DCO0)     /* 3 bit mask */

/* BCSCTL1 @ 0x0057 Basic Clock System Control 1 */

#define RSEL_OFFS (RSEL0)
#define RSEL_MASK (0xFF-0xF*RSEL0)  /* 4 bit mask */

#define DIVA_OFFS (DIVA0)
#define DIVA_MASK (0xFF-0x3*DIVA0)  /* 2 bit mask */

/* BCSCTL2 @ 0x0058 Basic Clock System Control 2 */

#define DIVS_OFFS (DIVS0)
#define DIVS_MASK (0xFF-0x3*DIVS0)  /* 2 bit mask */

#define DIVM_OFFS (DIVM0)
#define DIVM_MASK (0xFF-0x3*DIVM0)  /* 2 bit mask */

#define SELM_OFFS (SELM0)
#define SELM_MASK (0xFF-0x3*SELM0)  /* 2 bit mask */

/* BCSCTL3 @ 0x0053 Basic Clock System Control 3 */

#define XCAP_OFFS (XCAP0)
#define XCAP_MASK (~(XCAP0|XCAP1))   /* 2 bit mask */

#define LFXT1S_OFFS (LFXT1S0)
#define LFXT1S_MASK (~(LFXT1S0|LFXT1S1))  /* 2 bit mask */

#define XT2S_OFFS  (XT2S0)
#define XT2S_MASK  (0xFF-0x3*XT2S0)    /* 2 bit mask */

#endif

/******** FLASH MEMORY *************************/

#ifdef __MSP430_HAS_FLASH2__

/* FCTL2 @ 0x012A FLASH Control 2 */

#define FN_OFFS    (FN0)
#define FN_MASK    (0xFF-0x3F*FN0)    /* 6 bit mask */

#define FSSEL_OFFS (FSSEL0)
#define FSSEL_MASK (0xFF-0x3*FSSEL0)  /* 2 bit mask */

#endif

/******* DIGITAL I/O PORTS 1/2 *****************/

/* No definitions are needed here */

/********** TIMER A2 ***************************/

#ifdef __MSP430_HAS_TA2__

/* TAIV @ 0x012E Timer A Interrupt Vector Word (Read Only) */

#define IRQ_OFFS (0x0x2)
#define IRQ_MASK (0xFFFF-7*IRQ_OFFS)  /* 3 bit mask */
/* Mask not normally needed as it is read only !!! */

/* TACTL @ 0x0160 Timer A Control */

#define TAMC_OFFS   (0x10u)
#define TAMC_MASK   (0xFFFF-3*TAMC_OFFS)   /* 2 bit mask */

#define TAID_OFFS   (0x40u)
#define TAID_MASK   (0xFFFF-3*TAID_OFFS)   /* 2 bit mask */

#define TASSEL_OFFS (0x100u)
#define TASSEL_MASK (0xFFFF-3*TASSEL_OFFS) /* 2 bit mask */

/* TACCTL0 @ 0x0162 Timer A Capture/Compare Control 0 */

#define OUTMOD_OFFS  (0x20u)
#define OUTMOD_MASK  (0xFFFF-0x7*OUTMOD_OFFS)  /* 3 bit mask */

#define CCIS_OFFS    (0x1000u)
#define CCIS_MASK    (0xFFFF-0x3*CCIS_OFFS)    /* 2 bit mask */

#define CM_OFFS      (0x4000u)
#define CM_MASK      (0xFFFF-0x3*CM_OFFS)      /* 2 bit mask */

/* TACCTL1 @ 0x0164 Timer A Capture/Compare Control 1 */
/* Las máscaras son iguales a las de TACCTL0 */

/* Aditional Definitions */
#define TAIV_NONE     0x00
#define TAIV_TACCR1   0x02
#define TAIV_TACCR2   0x04
#define TAIV_TAIFG    0x0A

#endif

/************** USI ****************************/

#ifdef __MSP430_HAS_USI__

/* USICKCTL @ 0x007A USI  Clock Control Register */

#define USISSEL_OFFS  (USISSEL0)
#define USISSEL_MASK  (0xFF-7*USISSEL0)  /* 3 bit mask */

#define USIDIV_OFFS   (USIDIV0)
#define USIDIV_MASK   (0xFF-7*USIDIV0)   /* 3 bit mask */

/* USICNT @ 0x007B USI  Bit Counter Register */

#define USICNT_OFFS  (USICNT0)
#define USICNT_MASK  (0xFF-0x1F*USICNT0) /* 5 bit mask */

#endif

/************** WATCHDOG **********************/

/* Mask cannot be used as it is protected by a password */

/************** COMPARATOR A *****************/

#ifdef __MSP430_HAS_COMPA__

/* CACTL1 @ 0x0059 Comparator A Control 1 */

#define CAREF_OFFS  (CAREF0)
#define CAREF_MASK  (0xFF-0x3*CAREF0)  /* 2 bit mask */

/* CACTL2 @ 0x005A Comparator A Control 2 */

#define P2CA_OFFS (P2CA0)
#define P2CA_MASK (0xFF-0x3*P2CA0)   /* 2 bit mask */

/* Definitions for missing signals in .h */

#define P2CA2   BIT4
#define P2CA3   BIT5
#define P2CA4   BIT6
#define CASHORT BIT7

#define SELP_MASK (0xFF-P2CA0-P2CA4)
#define SELP_NONE (0x00)
#define SELP_CA0  (P2CA0)
#define SELP_CA1  (P2CA4)
#define SELP_CA2  (P2CA0+P2CA4)

#define SELN_MASK (0xFF-7*P2CA1)
#define SELN_NONE   (0x00)
#define SELN_CA1    (1*P2Ca1)
#define SELN_CA2    (2*P2Ca1)
#define SELN_CA3    (3*P2Ca1)
#define SELN_CA4    (4*P2Ca1)
#define SELN_CA5    (5*P2Ca1)
#define SELN_CA6    (6*P2Ca1)
#define SELN_CA7    (7*P2Ca1)

#endif






