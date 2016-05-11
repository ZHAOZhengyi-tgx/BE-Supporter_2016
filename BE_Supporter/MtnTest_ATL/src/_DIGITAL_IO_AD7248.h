
#ifndef __DIGITAL_IO_AD7248__
#define __DIGITAL_IO_AD7248__

#include "DASK.H"

#define __MAX_CARD_NUMBER_7248   8

extern void DigitalIO_7248_Registration();
extern int DIO7248_sys_is_using_cards();
extern unsigned int DIO7248_sys_get_total_num_cards();

#endif // __DIGITAL_IO_AD7248__