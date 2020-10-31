/*
*********************************************************************************************************
*                                             EXAMPLE CODE
*********************************************************************************************************
* Licensing:
*   The licensor of this EXAMPLE CODE is Silicon Laboratories Inc.
*
*   Silicon Laboratories Inc. grants you a personal, worldwide, royalty-free, fully paid-up license to
*   use, copy, modify and distribute the EXAMPLE CODE software, or portions thereof, in any of your
*   products.
*
*   Your use of this EXAMPLE CODE is at your own risk. This EXAMPLE CODE does not come with any
*   warranties, and the licensor disclaims all implied warranties concerning performance, accuracy,
*   non-infringement, merchantability and fitness for your application.
*
*   The EXAMPLE CODE is provided "AS IS" and does not come with any support.
*
*   You can find user manuals, API references, release notes and more at: https://doc.micrium.com
*
*   You can contact us at: https://www.micrium.com
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                           BSP CAN MODULE
*
*                                            Silicon Labs
*                                        SLSTK3701A_EFM32GG11
*
* File : bsp_can.c
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*********************************************************************************************************
*/

#include  <can/include/can_bus.h>
#include  <can/source/can_bus_priv.h>
#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/include/lib_def.h>


/*
*********************************************************************************************************
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*********************************************************************************************************
*/

CAN_BUS_HANDLE  BSP_CAN0_BusHandle;
CAN_BUS_HANDLE  BSP_CAN1_BusHandle;


/*
*********************************************************************************************************
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*********************************************************************************************************
*/

static  void  BSP_CAN0_Open             (void);

static  void  BSP_CAN1_Open             (void);

static  void  BSP_CAN0_Close            (void);

static  void  BSP_CAN1_Close            (void);

static  void  BSP_CAN0_IntCtrl          (CAN_BUS_HANDLE  bus_handle);

static  void  BSP_CAN1_IntCtrl          (CAN_BUS_HANDLE  bus_handle);

static  void  BSP_CAN_TmrCfg            (CPU_INT32U      tmr_period);

static  void  BSP_CAN_Tmr_ISR_Handler (void);

static  void  BSP_CAN0_ISR_Handler      (void);

static  void  BSP_CAN1_ISR_Handler      (void);


/*
*********************************************************************************************************
*********************************************************************************************************
*                                           LOCAL CONSTANTS
*********************************************************************************************************
*********************************************************************************************************
*/

                                                                /* ---------------- BSP API STRUCTURE ----------------- */
const CAN_CTRLR_BSP_API  CAN0_BSP_API = {
    .Open        = BSP_CAN0_Open,
    .Close       = BSP_CAN0_Close,
    .IntCtrl     = BSP_CAN0_IntCtrl,
    .TmrCfg      = BSP_CAN_TmrCfg
};

const CAN_CTRLR_BSP_API  CAN1_BSP_API = {
    .Open        = BSP_CAN1_Open,
    .Close       = BSP_CAN1_Close,
    .IntCtrl     = BSP_CAN1_IntCtrl,
    .TmrCfg      = DEF_NULL
};


/*
*********************************************************************************************************
*********************************************************************************************************
*                                           GLOBAL CONSTANTS
*********************************************************************************************************
*********************************************************************************************************
*/

const  CAN_CTRLR_DRV_INFO  BSP_CAN0_BSP_DrvInfo = {
    .BSP_API_Ptr        = &CAN0_BSP_API,
    .HW_Info.BaseAddr   = (CPU_ADDR)CAN0,
    .HW_Info.InfoExtPtr = DEF_NULL,
    .HW_Info.IF_Rx      = 1u,
    .HW_Info.IF_Tx      = 0u
};

const  CAN_CTRLR_DRV_INFO  BSP_CAN1_BSP_DrvInfo = {
    .BSP_API_Ptr        = &CAN1_BSP_API,
    .HW_Info.BaseAddr   = (CPU_ADDR)CAN1,
    .HW_Info.InfoExtPtr = DEF_NULL,
    .HW_Info.IF_Rx      = 1u,
    .HW_Info.IF_Tx      = 0u
};


/*
*********************************************************************************************************
*********************************************************************************************************
*                                            LOCAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/**
*********************************************************************************************************
*                                             BSP_CAN0_Open()
*
* @brief    Initializes the CAN IO module.
*
* @param    p_can   Pointer to a CAN register structure.
*********************************************************************************************************
*/

static void BSP_CAN0_Open (void)
{
    CMU_ClockEnable(cmuClock_HFPER, true);
    CMU_ClockEnable(cmuClock_CAN0, true);
    CMU_ClockEnable(cmuClock_GPIO, true);
    GPIO_PinModeSet(gpioPortC, 0u, gpioModeInput, 0u);
    GPIO_PinModeSet(gpioPortC, 1u, gpioModePushPull, 0u);
    CAN_SetRoute(CAN0, true, 0u, 0u);
}


/**
*********************************************************************************************************
*                                             BSP_CAN1_Open()
*
* @brief    Initializes the CAN IO module.
*********************************************************************************************************
*/

static void BSP_CAN1_Open (void)
{
    CMU_ClockEnable(cmuClock_HFPER, true);
    CMU_ClockEnable(cmuClock_CAN1, true);
    CMU_ClockEnable(cmuClock_GPIO, true);
    GPIO_PinModeSet(gpioPortA, 12u, gpioModeInput, 0u);
    GPIO_PinModeSet(gpioPortA, 13u, gpioModePushPull, 0u);
    CAN_SetRoute(CAN1, true, 5u, 5u);
}


/**
*********************************************************************************************************
*                                             BSP_CAN0_Close()
*
* @brief    De-initializes the CAN IO module.
*********************************************************************************************************
*/

static void BSP_CAN0_Close (void)
{
    CMU_ClockEnable(cmuClock_CAN0, false);
    CAN_SetRoute(CAN0, false, 0u, 0u);
    CPU_IntSrcDis(CAN0_IRQn + 16u);
}


/**
*********************************************************************************************************
*                                             BSP_CAN1_Close()
*
* @brief    De-initializes the CAN IO module.
*********************************************************************************************************
*/

static void BSP_CAN1_Close (void)
{
    CMU_ClockEnable(cmuClock_CAN1, false);
    CAN_SetRoute(CAN1, false, 5u, 5u);
    CPU_IntSrcDis(CAN1_IRQn + 16u);
}


/**
*********************************************************************************************************
*                                           BSP_CAN_TmrCfg()
*
* @brief    Initialize interrupt for Timer 0 module.
*
* @param    microsecond    Timer base time in microsecond.
*********************************************************************************************************
*/

static  void  BSP_CAN_TmrCfg  (CPU_INT32U  tmr_period)
{
    TIMER_Init_TypeDef init    = TIMER_INIT_DEFAULT;
    CPU_INT32U max_time_us     = 0u;
    CPU_INT32U max_timer_count = 0u;
    CPU_INT32U microsec        = 0u;
    CPU_INT32U top             = 0u;
    CPU_INT32U freq            = 0u;


    RTOS_ASSERT_CRITICAL((tmr_period >= 100u), RTOS_ERR_INVALID_ARG, ;);

    init.enable     = true;
    init.debugRun   = false;
    init.prescale   = timerPrescale1;
    init.clkSel     = timerClkSelHFPerClk;
    init.fallAction = timerInputActionReloadStart;
    init.riseAction = timerInputActionReloadStart;
    init.mode       = timerModeUp;
    init.oneShot    = false;
    init.sync       = false;

    CMU_ClockEnable(cmuClock_TIMER0, true);

    CPU_INT_SRC_HANDLER_SET_KA(TIMER0_IRQn + 16u,
                               BSP_CAN_Tmr_ISR_Handler);
    CPU_IntSrcEn(TIMER0_IRQn + 16u);

    TIMER_IntEnable(TIMER0, TIMER_IEN_OF);

    freq = CMU_ClockFreqGet(cmuClock_TIMER0);

    max_timer_count = TIMER_MaxCount(TIMER0);
                                                                /* Calculate the maximum period this timer can handle.  */
    max_time_us = ((CPU_INT64U)max_timer_count * DEF_TIME_NBR_uS_PER_SEC) / ((freq / (1u << timerPrescale1024)));

    if (microsec > max_time_us) {
                                                                /* If requested period is too high, take the max value. */
        top = max_timer_count;
    } else {
                                                                /* Calculate the TOP register value.                    */
        top = ((CPU_INT64U)(freq / (1u << (CPU_INT32U)init.prescale)) * tmr_period) / DEF_TIME_NBR_uS_PER_SEC;
                                                                /* Try finding the best prescaler.                      */
        while ((top            > max_timer_count                          ) &&
               (init.prescale != (TIMER_Prescale_TypeDef)timerPrescale1024)) {
            init.prescale += (TIMER_Prescale_TypeDef)timerPrescale2;
            top = ((CPU_INT64U)(freq / (1u << (CPU_INT32U)init.prescale)) * tmr_period) / DEF_TIME_NBR_uS_PER_SEC;
        }
    }

    TIMER_TopSet(TIMER0, top);

    TIMER_Init(TIMER0, &init);
}


/**
*********************************************************************************************************
*                                           BSP_CAN0_IntCtrl()
*
* @brief    Initialize interrupt for CAN IO module.
*
* @param    bus_handle    CANBus handle structure.
*********************************************************************************************************
*/

static  void  BSP_CAN0_IntCtrl (CAN_BUS_HANDLE  bus_handle)
{
    BSP_CAN0_BusHandle = bus_handle;

    CPU_INT_SRC_HANDLER_SET_KA(CAN0_IRQn + 16u,
                               BSP_CAN0_ISR_Handler);
    CPU_IntSrcEn(CAN0_IRQn + 16u);
}


/**
*********************************************************************************************************
*                                           BSP_CAN1_IntCtrl()
*
* @brief    Initialize interrupt for CAN IO module.
*
* @param    bus_handle    CANBus handle structure.
*********************************************************************************************************
*/

static  void  BSP_CAN1_IntCtrl (CAN_BUS_HANDLE  bus_handle)
{
    BSP_CAN1_BusHandle = bus_handle;

    CPU_INT_SRC_HANDLER_SET_KA(CAN1_IRQn + 16u,
                               BSP_CAN1_ISR_Handler);
    CPU_IntSrcEn(CAN1_IRQn + 16u);
}


/**
*********************************************************************************************************
*                                         BSP_CAN0_ISR_Handler()
*
* @brief    CAN0 module ISR handler.
*********************************************************************************************************
*/

static  void  BSP_CAN0_ISR_Handler (void)
{
    CanBus_ISRHandler(BSP_CAN0_BusHandle);
}


/**
*********************************************************************************************************
*                                         BSP_CAN1_ISR_Handler()
*
* @brief    CAN1 module ISR handler.
*********************************************************************************************************
*/

static  void  BSP_CAN1_ISR_Handler (void)
{
    CanBus_ISRHandler(BSP_CAN1_BusHandle);
}


/**
*********************************************************************************************************
*                                         BSP_CAN_Timer_ISR_Handler()
*
* @brief    Timer0 module ISR handler.
*********************************************************************************************************
*/

static  void  BSP_CAN_Tmr_ISR_Handler(void)
{
    CPU_INT32U  timer_get;


    timer_get = TIMER_IntGet(TIMER0);

    CanBus_TmrISRHandler();

    TIMER_IntClear(TIMER0, timer_get);
}
