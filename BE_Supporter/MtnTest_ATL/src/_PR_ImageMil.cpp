
#include <mil.h> 

static   MIL_ID MilApplication,  /* Application identifier.  */
          MilSystem,       /* System identifier.       */
          MilDisplay,      /* Display identifier.      */
          MilDigitizer,    /* Digitizer identifier.    */ 
          MilImage;        /* Image buffer identifier. */

void mil_init_alloc_default()
{
   /* Allocate defaults. */
   MappAllocDefault(M_SETUP, &MilApplication, &MilSystem,
                             &MilDisplay, &MilDigitizer, &MilImage);
}

//MFTYPE32 void MFTYPE    MimHistogram         (MIL_ID SrcImageId,
//                                              MIL_ID HistogramListId);
//
//MFTYPE32 void MFTYPE    MimHistogramEqualize (MIL_ID src_id,
//                                              MIL_ID dst_id,
//                                              long  EqualizationType,
//                                              double Alpha,
//                                              double Min,
//                                              double Max);
//
//MFTYPE32 void MFTYPE    MimProject           (MIL_ID SrcImageId,
//                                              MIL_ID DestArrayId,
//                                              double ProjectionAngle);
//
//MFTYPE32 void MFTYPE    MimFindExtreme       (MIL_ID SrcImageId,
//                                              MIL_ID ResultListId,
//                                              long ExtremeType);
//
//MFTYPE32 void MFTYPE    MimStat              (MIL_ID SrcImageId,
//                                              MIL_ID StatResultId,
//                                              long StatType,
//                                              long Condition,
//                                              double CondLow,
//                                              double CondHigh);
//
//MFTYPE32 void MFTYPE    MimLocateEvent       (MIL_ID SrcImageId,
//                                              MIL_ID EventResultId,
//                                              long Condition,
//                                              double CondLow,
//                                              double CondHigh);
//
//MFTYPE32 void MFTYPE    MimCountDifference   (MIL_ID Src1ImageId,
//                                              MIL_ID Src2ImageId,
//                                              MIL_ID ResId);
//
//MFTYPE32 MIL_ID MFTYPE  MimAllocResult       (MIL_ID SystemId,
//                                              long NumberOfResultElement,
//                                              long ResultType,
//                                              MIL_ID MPTYPE *IdVarPtr);
//
//MFTYPE32 void MFTYPE    MimFree              (MIL_ID ImResultId);
//
//MFTYPE32 long MFTYPE    MimInquire           (MIL_ID BufId,
//                                              long InquireType,
//                                              void MPTYPE *TargetVarPtr);
//
//MFTYPE32 void MFTYPE    MimGetResult1d       (MIL_ID ImResultId,
//                                              long Offresult,
//                                              long Sizeresult,
//                                              long ResultType,
//                                              void MPTYPE *UserTargetArrayPtr);
//
//MFTYPE32 void MFTYPE    MimGetResult         (MIL_ID ImResultId,
//                                             long ResultType,
//                                             void MPTYPE *UserTargetArrayPtr);

void mil_get_contrast_current_image()
{
}

void mil_start_grab_continuous()
{ 

   /* Grab continuously. */
   MdigGrabContinuous(MilDigitizer, MilImage);
}
   ///* When a key is pressed, halt. */
   //printf("\nDIGITIZER ACQUISITION:\n");
   //printf("----------------------\n\n");
   //printf("Continuous image grab in progress.\n");
   //printf("Press <Enter> to stop.\n\n");
   //getch();


   ///* Pause to show the result. */
   //printf("Continuous grab stopped.\n\n");
   //printf("Press <Enter> to do a single image grab.\n\n");
   //getch();

   ///* Monoshot grab. */
   //MdigGrab(MilDigitizer, MilImage);

   ///* Pause to show the result. */
   //printf("Displaying the grabbed image.\n");
   //printf("Press <Enter> to end.\n\n");
   //getch();

void mil_stop_grab_image()
{
   /* Stop continuous grab. */
   MdigHalt(MilDigitizer);
}

void mil_release_resource()
{
   /* Free defaults. */
   MappFreeDefault(MilApplication, MilSystem, MilDisplay, MilDigitizer, MilImage);
}

