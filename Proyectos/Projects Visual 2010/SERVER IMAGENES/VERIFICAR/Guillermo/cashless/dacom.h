// dacom.h

#define DACOM_SUCCESS                     EFT_SUCCESS
#define DACOM_NO_RESPONSE                 EFT_NO_RESPONSE
#define DACOM_BAD_RESPONSE                EFT_BAD_RESPONSE
#define DACOM_TRANSFER_CANCELLED_BY_HOST  AFT_TRANSFER_CANCELLED_BY_HOST

// archivo de encabezado de dacom.c

unsigned char dacomProcessing(void);
unsigned char dacom_cashin(unsigned long);
unsigned char dacom_cashout(unsigned long *);
void initDacom(void);

