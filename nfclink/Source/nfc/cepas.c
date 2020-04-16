/*
 * cepas.c
 *
 *  Created on: 28 Dec 2019
 *      Author: natsa
 */

#include "cepas.h"
#include "string.h"
#include "stdlib.h"

CEPAS* initCEPAS(void)
{

    CEPAS* card = malloc(sizeof(CEPAS));
    card->purse.version = 0x02;
    card->purse.purseStatus = 0x01;
    memcpy(card->purse.purseBalance, "\x7f\xff\xff", 3);
    memcpy(card->purse.autoloadAmt, "\x00\x00\x00", 3);
    memcpy(card->purse.can, "\x01\x02\x03\x04\x05\x06\x07\x08", 8);
    memcpy(card->purse.csn, "\x00\x00\x00\x00\x00\x00\x00\x00", 8);
    memcpy(card->purse.purseExp, "\xee\x2f", 2);
    memcpy(card->purse.purseCreate, "\x00\x00", 2);
    memcpy(card->purse.lastCreditTrp, "\x00\x00\x00\x00", 4);
    memcpy(card->purse.lastCreditHeader, "\x00\x00\x00\x00\x00\x00\x00\x00", 8);
    card->purse.numTrans = 0x0D;
    card->purse.IssuerDataLen = 0x20;
    memcpy(card->purse.lastTransTRP, "\x00\x00\x00\x00", 4);
    memcpy(card->purse.lastTransRec,
           "\x00\x00\x00\x00\x00\x00\x00\x00\x01\x02\x03\x04\x05\x06\x07\x20",
           16);

    if (card->purse.IssuerDataLen != 0x00)
    {
        card->purse.issuerDataPtr = calloc(card->purse.IssuerDataLen,
                                           sizeof(uint8_t));
        memcpy(card->purse.issuerDataPtr,
               "\x00\x00\x00\x00\x00\x00\x00\x00\x00", 9);

    }

    uint8_t i;
    TRANS* prevTrans = NULL;
    for (i = 0; i < card->purse.numTrans; i++)
    {
        TRANS* trans = calloc(1, sizeof(TRANS));
        memcpy(&trans->type, "\x76", 1);
        memcpy(&trans->amount, "\x00\x00\x10", 3);
        memcpy(&trans->dateTime, "\x2C\x8E\x34\xD8", 4);
        strcpy(&trans->userData, "BUS 123");

        if (prevTrans == NULL)
        {
            card->purse.transPtr = trans;
            prevTrans = trans;
        }
        else
        {
            prevTrans->nextTrans = trans;
            prevTrans = trans;
        }
    }

    return card;
}

void processCommand(CEPAS* card, uint8_t * pui8RxPayload, uint8_t ui8CmdLength)
{
    memset(response, 0, sizeof response);
    resLength = 0;
    if (ui8CmdLength < 5)
    {
        responseCode = WRONG_LENGTH;
        resLength = 0;
        return;
    }

    int dataLen = ui8CmdLength - 6;

    uint8_t cmdCLA;
    uint8_t cmdINS;
    uint8_t cmdP1;
    uint8_t cmdP2;
    uint8_t cmdLC;
    uint8_t cmdLE;

    cmdCLA = pui8RxPayload[0];
    cmdINS = pui8RxPayload[1];
    cmdP1 = pui8RxPayload[2];
    cmdP2 = pui8RxPayload[3];
    cmdLC = pui8RxPayload[4];
    cmdLE = pui8RxPayload[ui8CmdLength - 1];
    uint8_t respMaxLen = 95;

    if (cmdP1 != 0x03)
    {
        responseCode = APP_NOT_FOUND;
        resLength = 0;
        return;
    }

    if (cmdLE > 253)
    {
        responseCode = WRONG_LENGTH;
        resLength = 0;
        return;
    }
    else if (cmdLE != 0)
    {
        respMaxLen = cmdLE;
    }

    if (cmdCLA == 0x90 && cmdINS == 0x32)
    {

        switch (cmdLC)
        {
        case 0x00:
        {
            if (respMaxLen < 63)
            {
                memcpy(response, card, respMaxLen);
                responseCode = COMMAND_SUCCESS;
                resLength = respMaxLen;
                return;
            }
            else
            {
                memcpy(response, card, 62);
                uint8_t remainderData = respMaxLen - 62;
                memcpy(&response[61], card->purse.issuerDataPtr, remainderData);
                responseCode = COMMAND_SUCCESS;
                resLength = respMaxLen;
                return;
            }

            break;
        }
        case 0x01:
        {
            if (dataLen > 1)
            {
                responseCode = INCORRECT_DATA;
                resLength = 0;
                return;
            }
            uint8_t entries = pui8RxPayload[5];
            if (entries == 0)
            {
                entries = card->purse.numTrans;
            }
            else if(entries > card->purse.numTrans){
                responseCode = INCORRECT_DATA;
                resLength = 0;
                return;
            }
            uint8_t maxNumOfTrans = respMaxLen / 16;
            uint8_t i;
            TRANS * currentTrans = card->purse.transPtr;
            for (i = 0; i < maxNumOfTrans; i++)
            {
                if (currentTrans == NULL)
                {
                    responseCode = COMMAND_SUCCESS;
                    return;
                }
                else
                {
                    memcpy(&response[i * 16], currentTrans, 16);
                    resLength = resLength + 16;
                    currentTrans = currentTrans->nextTrans;
                }
            }
            responseCode = COMMAND_SUCCESS;
            return;
            break;
        }
        default:
            responseCode = COND_NOT_SASTFD;
            resLength = 0;
            return;
        }

    }
}

uint8_t getResponseLength()
{
    return resLength + 2;
}

void getResponse(uint8_t* pui8PduBufferPtr)
{
    memcpy(pui8PduBufferPtr, response, resLength);
    pui8PduBufferPtr[resLength] = (uint8_t) (responseCode >> 8);
    pui8PduBufferPtr[resLength + 1] = (uint8_t) (0xFF & responseCode);
    return;
}
