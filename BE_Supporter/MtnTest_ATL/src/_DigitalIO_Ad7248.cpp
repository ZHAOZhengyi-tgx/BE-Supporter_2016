
#include "stdafx.h"

#include "_Digital_IO_Ad7248.h"

U32 uiTotalNumCard7248 = 0;
I16 sCardId_7248[__MAX_CARD_NUMBER_7248] = {-1, -1, -1, -1, -1, -1, -1, -1};
U16 uiCard_number_7248;
U32 inputA[2], inputB[2], inputC[2] , inputD[2];
U32 outputA[2],outputB[2],outputC[2] ,outputD[2];

U16 uiComboSelectionInputCard7248 = -1, uiComboSelectionOutputCard7248= -1;
U32 ulDigitalIO7248_ReadByteInPortValue, ulDigitalIO7248_ReadByteOutPortValue;

int iFlagUsingIO7248 = 0;

void test_adlink_7248_function_by_link()
{
	unsigned int ii;
	for(ii = 0; ii< __MAX_CARD_NUMBER_7248; ii++)
	{
		uiCard_number_7248 = ii;
		sCardId_7248[ii] = Register_Card(PCI_7248, uiCard_number_7248);
		if(sCardId_7248[ii] >= 0)
		{
			uiTotalNumCard7248 ++;
			// Config
			DIO_PortConfig(sCardId_7248[ii], Channel_P1A, OUTPUT_PORT);
			DIO_PortConfig(sCardId_7248[ii], Channel_P1B, INPUT_PORT);
			DIO_PortConfig(sCardId_7248[ii], Channel_P1C, INPUT_PORT);

			DIO_PortConfig(sCardId_7248[ii], Channel_P2A, OUTPUT_PORT);
			DIO_PortConfig(sCardId_7248[ii], Channel_P2B, INPUT_PORT);
			DIO_PortConfig(sCardId_7248[ii], Channel_P2C, INPUT_PORT);

			DO_WritePort(sCardId_7248[ii], Channel_P1A, outputA[0]);
			DO_WritePort(sCardId_7248[ii], Channel_P2A, outputA[1]);

			DO_ReadPort(sCardId_7248[ii], Channel_P1C, &inputC[0]);
			DO_ReadPort(sCardId_7248[ii], Channel_P2C, &inputC[1]);

		}
	}
}

int DIO7248_sys_is_using_cards()
{
	return iFlagUsingIO7248;
}

unsigned int DIO7248_sys_get_total_num_cards()
{
	return uiTotalNumCard7248;
}

void DigitalIO_7248_Registration()
{
	unsigned int ii;
	for(ii = 0; ii< __MAX_CARD_NUMBER_7248; ii++)
	{
		uiCard_number_7248 = ii;
		sCardId_7248[ii] = Register_Card(PCI_7248, uiCard_number_7248);
		if(sCardId_7248[ii] >= 0)
		{
			uiTotalNumCard7248 ++;

			DIO_PortConfig(sCardId_7248[ii], Channel_P1A, OUTPUT_PORT);
			DIO_PortConfig(sCardId_7248[ii], Channel_P1B, INPUT_PORT);
			DIO_PortConfig(sCardId_7248[ii], Channel_P1C, INPUT_PORT);

			DIO_PortConfig(sCardId_7248[ii], Channel_P2A, OUTPUT_PORT);
			DIO_PortConfig(sCardId_7248[ii], Channel_P2B, INPUT_PORT);
			DIO_PortConfig(sCardId_7248[ii], Channel_P2C, INPUT_PORT);

			DO_WritePort(sCardId_7248[ii], Channel_P1A, outputA[0]);
			DO_WritePort(sCardId_7248[ii], Channel_P2A, outputA[1]);

		}
	}
	if(uiTotalNumCard7248 >= 1)
	{
		iFlagUsingIO7248 = 1;
	}
}
