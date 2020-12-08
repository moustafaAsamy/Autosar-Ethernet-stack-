/*
 * EthIf_config.c
 *
 *  Created on: Dec 25, 2019
 *      Author: lenovo
 */



#include "autosar_includes/TcpIp.h"
#include "autosar_includes/ComStack_Types.h"
#include "Eth.h"

struct  EthIfController   EthIfController_list[2] =
  {
   {
     .EthIfCtrlIdx =0,
     .EthIfCtrlMtu =1500,
     .EthIfMaxTxBufsTotal =1,
   },
  };
