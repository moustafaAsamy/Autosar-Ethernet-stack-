/*
 * EthIf.c
 *
 *  Created on: Nov 21, 2019
 *      Author: lenovo
 */


#include"autosar_includes/EthIf.h"
#include "string.h"



Std_ReturnType EthIf_Transmit( uint8 CtrlIdx, Eth_BufIdxType BufIdx, Eth_FrameType FrameType, boolean TxConfirmation, uint16 LenByte, const uint8* PhysAddrPtr )
{
   return E_OK;
}
BufReq_ReturnType EthIf_ProvideTxBuffer( uint8 CtrlIdx, Eth_FrameType FrameType, uint8 Priority, Eth_BufIdxType* BufIdxPtr, uint8** BufPtr, uint16*LenBytePtr )
{

        uint8_t frame[1514]={0};
        uint8 * frame_1 = *BufPtr;
        uint8_t *frame_ptr = &frame[0];
        //int x = *LenBytePtr;
        memcpy(frame_ptr, frame_1 , *LenBytePtr);
        int c = 0;
        if ( *LenBytePtr <64)
        {c=64;}
        else
        { c=*LenBytePtr;}
        enc_send_packet(frame,c);
        EthIf_Transmit( 0, 0, 0, 0, 0, 0 );
        return BUFREQ_OK;

}

void EthIf_GetPhysAddr( uint8 CtrlIdx, uint8* PhysAddrPtr )
{

}
