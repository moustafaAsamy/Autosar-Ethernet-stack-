//
//
//
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "lwip_int.h"
#include "config.h"
#include "autosar_includes/TcpIp.h"
#include "led.h"
extern void lwIPServiceTimers(void);
extern void TCP_sent_buffer(void);
extern void lwIPTimer(uint32_t ui32TimeMS) ;

extern void  timer_start(void)
{
    SysTickPeriodSet(systic_rate);
    IntMasterEnable();
    SysTickIntEnable();
    SysTickEnable();
    int x =SysCtlClockGet();
}

volatile uint32_t leds_timer = 0;   //Required every 1000 ms
volatile uint32_t TCp_counter = 0;   //Required every 500 ms 50 *10
volatile uint32_t UDp_counter = 0;   //Required every 600 ms
volatile uint32_t TCP_timer =   0;   //Required every 500 ms
volatile uint32_t main_function = 0;   //Required every 100 ms
uint32_t g_ui32_APP_Timer = 0;
#define ui32TickMS 10

void SysTickIntHandler(void)
{
#if NO_SYS
    lwIPTimer(ui32TickMS);
    lwIPServiceTimers();
    g_ui32_APP_Timer =  g_ui32_APP_Timer + ui32TickMS;
    UDp_counter =  UDp_counter + ui32TickMS;
    main_function ++;  //main function trigger
    leds_timer++;
#if ECU1
    if(g_ui32_APP_Timer >= 1500)
    {
        app_task_tx_tcp();
        g_ui32_APP_Timer=0;
    }
        if  (UDp_counter == 3000 )
           {
               UDp_counter =0;
               app_task_tx_udp();
           }
#endif
        if (main_function >= 10 )
            {
                main_function =0;
                TcpIp_MainFunction();
            }
            if ((leds_timer == 200  ) || (leds_timer > 200  ))
                {
                leds_timer=0;
                led_off();
                }

#endif
}
//extern void SysTickIntHandler(void)
//{
//
//    if ((main_function == 10 ) )
//        {
//            main_function =0;
//            TcpIp_MainFunction();
//        }
//    if ((main_function > 10 ))
//            {
//                main_function =0;
//                TcpIp_MainFunction();
//            }
//#if ECU1
//    if ((TCp_counter == 300 ) )
//    {
//        TCp_counter =0;
//        app_task_tx_tcp();
//    }
//    if (  (TCp_counter > 300 ))
//        {
//            TCp_counter =0;
//            app_task_tx_tcp();
//        }
//
////    if ((UDp_counter == 400 ) || (UDp_counter > 400 ))
////       {
////           UDp_counter =0;
////           app_task_tx_udp();
// //      }
//#endif
//
//    if ((TCP_timer == 50 ) )
//        {
//        TCP_timer=0;
//        timer();
//        }
//    if ((TCP_timer > 50 ))
//            {
//            TCP_timer=0;
//            timer();
//            }
//    if ((leds_timer == 150 ))
//        {
//        leds_timer=0;
//        led_off();
//        }
//    if ( (leds_timer > 150 ))
//            {
//            leds_timer=0;
//            led_off();
//            }
//      leds_timer ++;     //Required every 1000 ms
//      TCp_counter ++;    //Required every 500 ms 50 *10
//      UDp_counter ++;    //Required every 600 ms
//      TCP_timer   ++;    //Required every 500 ms
//      main_function ++;  //main function trigger
//
//}
