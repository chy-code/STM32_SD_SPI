
#include <stdio.h>
#include <string.h>
#include "stm32f10x.h"
#include "Serial.h"
#include "SD.h"
#include "LED.h"
#include "Delay.h"


#define BLOCKS_TO_TEST		1


static void PrintCID(SD_CID *CID)
{
    printf("CID:\r\n");
    printf("ManufacturerID: %02XH\r\n", CID->ManufacturerID);
    printf("OEM_AppliID: 	%04XH\r\n", CID->OEM_AppliID);
    printf("ProdName: 		%s\r\n", CID->ProdName);
    printf("ProdRev: 		%02XH\r\n", CID->ProdRev);
    printf("ProdSN: 		%08XH\r\n", CID->ProdSN);
    printf("ManufactDate: 	%04XH\r\n", CID->ManufactDate);
    printf("CRC: 			%02XH\r\n", CID->CID_CRC);
}


static void PrintCSD(SD_CSD *CSD)
{
    printf("CSD:\r\n");
    printf("CSDStruct: %02XH\r\n", CSD->CSDStruct);
    printf("TAAC: %02XH\r\n", CSD->TAAC);
    printf("NSAC: %02XH\r\n", CSD->NSAC);
    printf("MaxBusClkFrec: %02XH\r\n", CSD->MaxBusClkFrec);
    printf("CardComdClasses: %03XH\r\n", CSD->CardComdClasses);
    printf("RdBlockLen: %02XH\r\n", CSD->RdBlockLen);
    printf("PartBlockRead: %02XH\r\n", CSD->PartBlockRead);
    printf("WrBlockMisalign: %02XH\r\n", CSD->WrBlockMisalign);
    printf("RdBlockMisalign: %02XH\r\n", CSD->RdBlockMisalign);
    printf("DSRImpl: %02XH\r\n", CSD->DSRImpl);

    if (CSD->CSDStruct == SD_CSD_STRUCT_V1) {
        printf("DeviceSize: %04X\r\n", CSD->version.v1.DeviceSize);
        printf("MaxRdCurrentVDDMin: %02XH\r\n", CSD->version.v1.MaxRdCurrentVDDMin);
        printf("MaxRdCurrentVDDMax: %02XH\r\n", CSD->version.v1.MaxRdCurrentVDDMax);
        printf("MaxWrCurrentVDDMin: %02XH\r\n", CSD->version.v1.MaxWrCurrentVDDMin);
        printf("MaxWrCurrentVDDMax: %02XH\r\n", CSD->version.v1.MaxWrCurrentVDDMax);
        printf("DeviceSizeMul: %02XH\r\n", CSD->version.v1.DeviceSizeMul);
    }
    else {
        printf("DeviceSize: %08XH\r\n", CSD->version.v2.DeviceSize);
    }

    printf("EraseSingleBlockEnable: %02XH\r\n", CSD->EraseSingleBlockEnable);
    printf("EraseSectorSize: 	%02XH\r\n", CSD->EraseSectorSize);
    printf("WrProtectGrSize: 	%02XH\r\n", CSD->WrProtectGrSize);
    printf("WrProtectGrEnable: 	%02XH\r\n", CSD->WrProtectGrEnable);
    printf("WrSpeedFact: 		%02XH\r\n", CSD->WrSpeedFact);
    printf("MaxWrBlockLen: 		%02XH\r\n", CSD->MaxWrBlockLen);
    printf("WriteBlockPartial: 	%02XH\r\n", CSD->WriteBlockPartial);

    printf("FileFormatGroup: %02XH\r\n", CSD->FileFormatGroup);
    printf("CopyFlag: %02XH\r\n", CSD->CopyFlag);
    printf("PermWrProtect: %02XH\r\n", CSD->PermWrProtect);
    printf("TempWrProtect: %02XH\r\n", CSD->TempWrProtect);
    printf("FileFormat: %02XH\r\n", CSD->FileFormat);
    printf("CRC: %02XH\r\n", CSD->crc);
}



__NO_RETURN int main()
{
    int i;
    SD_Error ret;
    static SD_CardInfo cardInfo;

    static uint8_t orgDataBuf[SD_BLOCK_SIZE * BLOCKS_TO_TEST];
    static uint8_t dataBuf[SD_BLOCK_SIZE * BLOCKS_TO_TEST];

    SER_Init();
    LED_Init();
    SD_Init();

    while (1) {
        printf("Input any key to start test...\r\n");
        getchar();

        printf("Initialization:\r\n");

        ret = SD_ResetAllCards();
        if (ret != SD_OK) {
            printf("Failed to initialize the cards: %d\r\n", ret);
            continue;
        }

        printf("Turn On CRC:\r\n");
        ret = SD_SetCrcOn(true);
        if (ret != SD_OK) {
            printf("Failed to turn on CRC: %d\r\n", ret);
            continue;
        }

        printf("Get Card Info:\r\n");
        ret = SD_GetCardInfo(&cardInfo);
        if (ret != SD_OK) {
            printf("Failed to retrieve the card info: %d\r\n", ret);
            continue;
        }

        PrintCSD(&cardInfo.CSD);
        PrintCID(&cardInfo.CID);

        printf("Card Capacity(MB): %d\r\n", cardInfo.Capacity);
        printf("Number of Blocks: %d\r\n", cardInfo.BlocksNum);


        printf("1. Read Orginal Data\r\n");
        ret = SD_ReadBlocks(orgDataBuf, 0, BLOCKS_TO_TEST);
        if (ret != SD_OK) {
            printf("Faild to read blocks: %d\r\n", ret);
            continue;
        }

        for (i = 0; i < sizeof(orgDataBuf); i++)
            printf("%02X ", orgDataBuf[i]);
        printf("\r\n");


        printf("2. Erase Orginal Data\r\n");
        ret = SD_Erase(0, BLOCKS_TO_TEST);
        if (ret != SD_OK) {
            printf("Failed to erase blocks: %d\r\n", ret);
            continue;
        }

        printf("3. Read Data\r\n");
        ret = SD_ReadBlocks(dataBuf, 0, BLOCKS_TO_TEST);
        if (ret != SD_OK) {
            printf("Failed to read blocks: %d\r\n", ret);
            continue;
        }

        for (i = 0; i < sizeof(dataBuf); i++)
            printf("%02X ", dataBuf[i]);
        printf("\r\n");


        printf("4. Write Orginal Data\r\n");
        ret = SD_WriteBlocks(orgDataBuf, 0, BLOCKS_TO_TEST);
        if (ret != SD_OK) {
            printf("Failed to write blocks: %d\r\n", ret);
            continue;
        }

        printf("5. Read Data\r\n");
        ret = SD_ReadBlocks(dataBuf, 0, BLOCKS_TO_TEST);
        if (ret != SD_OK) {
            printf("Failed to read blocks: %d\r\n", ret);
            continue;
        }

        for (i = 0; i < sizeof(dataBuf); i++)
            printf("%02X ", dataBuf[i]);
        printf("\r\n");

    }
}
