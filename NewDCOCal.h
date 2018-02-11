/*
 NewDCOCal.h

 New calibration data for the DCO

 */

#include <iomacros.h>

// Test ton include the file only one time
#ifndef _NEW_DCO_CAL_
#define _NEW_DCO_CAL_

#define NCALDCO_500kHZ_        0x10AE    /* DCOCTL  Calibration Data for 500kHz */
const_sfrb(NCALDCO_500kHZ, NCALDCO_500kHZ_);
#define NCALBC1_500kHZ_        0x10AF    /* BCSCTL1 Calibration Data for 500kHz */
const_sfrb(NCALBC1_500kHZ, NCALBC1_500kHZ_);

#define NCALDCO_16MHZ_         0x10B0    /* DCOCTL  Calibration Data for 16MHz */
const_sfrb(NCALDCO_16MHZ, NCALDCO_16MHZ_);
#define NCALBC1_16MHZ_         0x10B1    /* BCSCTL1 Calibration Data for 16MHz */
const_sfrb(NCALBC1_16MHZ, NCALBC1_16MHZ_);

#define NCALDCO_12MHZ_         0x10B2    /* DCOCTL  Calibration Data for 12MHz */
const_sfrb(NCALDCO_12MHZ, NCALDCO_12MHZ_);
#define NCALBC1_12MHZ_         0x10B3    /* BCSCTL1 Calibration Data for 12MHz */
const_sfrb(NCALBC1_12MHZ, NCALBC1_12MHZ_);

#define NCALDCO_10MHZ_         0x10B4    /* DCOCTL  Calibration Data for 10MHz */
const_sfrb(NCALDCO_10MHZ, NCALDCO_10MHZ_);
#define NCALBC1_10MHZ_         0x10B5    /* BCSCTL1 Calibration Data for 10MHz */
const_sfrb(NCALBC1_10MHZ, NCALBC1_10MHZ_);

#define NCALDCO_8MHZ_         0x10B6    /* DCOCTL  Calibration Data for 8MHz */
const_sfrb(NCALDCO_8MHZ, NCALDCO_8MHZ_);
#define NCALBC1_8MHZ_         0x10B7    /* BCSCTL1 Calibration Data for 8MHz */
const_sfrb(NCALBC1_8MHZ, NCALBC1_8MHZ_);

#define NCALDCO_6MHZ_         0x10B8    /* DCOCTL  Calibration Data for 6MHz */
const_sfrb(NCALDCO_6MHZ, NCALDCO_6MHZ_);
#define NCALBC1_6MHZ_         0x10B9    /* BCSCTL1 Calibration Data for 6MHz */
const_sfrb(NCALBC1_6MHZ, NCALBC1_6MHZ_);

#define NCALDCO_4MHZ_         0x10BA    /* DCOCTL  Calibration Data for 4MHz */
const_sfrb(NCALDCO_4MHZ, NCALDCO_4MHZ_);
#define NCALBC1_4MHZ_         0x10BB    /* BCSCTL1 Calibration Data for 4MHz */
const_sfrb(NCALBC1_4MHZ, NCALBC1_4MHZ_);

#define NCALDCO_2MHZ_         0x10BC    /* DCOCTL  Calibration Data for 2MHz */
const_sfrb(NCALDCO_2MHZ, NCALDCO_2MHZ_);
#define NCALBC1_2MHZ_         0x10BD    /* BCSCTL1 Calibration Data for 2MHz */
const_sfrb(NCALBC1_2MHZ, NCALBC1_2MHZ_);

#define NCALDCO_1MHZ_         0x10BE    /* DCOCTL  Calibration Data for 1MHz */
const_sfrb(NCALDCO_1MHZ, NCALDCO_1MHZ_);
#define NCALBC1_1MHZ_         0x10BF    /* BCSCTL1 Calibration Data for 1MHz */
const_sfrb(NCALBC1_1MHZ, NCALBC1_1MHZ_);







#endif // NewDCOCal
