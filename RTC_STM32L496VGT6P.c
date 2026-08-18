// #############################################################################
// #### Copyright ##############################################################
// #############################################################################

/*
 * Copyright 2024 BaSSeM
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

// #############################################################################
// #### Description ############################################################
// #############################################################################

// #############################################################################
// #### Control Include(s) #####################################################
// #############################################################################

#include "Platform.h"

// #############################################################################
// #### Control Macro(s) #######################################################
// #############################################################################

#ifndef DEBUG
    #define DEBUG
#endif

#ifdef DEBUG
    #undef DEBUG
#endif

// #############################################################################
// #### File Guard #############################################################
// #############################################################################

#ifdef STM32L496xx

// #############################################################################
// #### Include(s) #############################################################
// #############################################################################

    #include "../../RTC_Internal.h"
    #include "RTC_STM32L496VGT6P.h"

    #include "stm32l4xx.h"
    #include "stm32l4xx_hal_rtc.h"

// #############################################################################
// #### Private Macro(s) #######################################################
// #############################################################################

    #define RTC_STM32L496VGT6P_TICKS_PER_MS 2

// #############################################################################
// #### Private Type(s) ########################################################
// #############################################################################

typedef enum RTC_STM32L496VGT6P_Event
{
    RTC_STM32L496VGT6P_Event_None = 0,
    RTC_STM32L496VGT6P_Event_InterruptWakeup = UTIL_BIT( 0 ),
    RTC_STM32L496VGT6P_Event_InterruptAlarm = UTIL_BIT( 1 ),
} RTC_STM32L496VGT6P_Event_t;

typedef struct RTC_STM32L496VGT6P_Instance
{
    RTC_HandleTypeDef RTCx;
    RTC_STM32L496VGT6P_Timestamp_t Timestamp;
    RTC_STM32L496VGT6P_Event_t Event;
} RTC_STM32L496VGT6P_Instance_t;

typedef struct RTC_STM32L496VGT6P_Context
{
    TIM_Timestamp_t Timestamp;
    RTC_STM32L496VGT6P_Instance_t Instance[ RTC_STM32L496VGT6P_Count ];
} RTC_STM32L496VGT6P_Context_t;

// #############################################################################
// #### Private Method(s) Prototype ############################################
// #############################################################################

void HAL_RTCEx_WakeUpTimerEventCallback( RTC_HandleTypeDef * hrtc );
void RTC_WKUP_IRQHandler( void );
void RTC_Alarm_IRQHandler( void );

static RTC_STM32L496VGT6P_Status_t RTC_STM32L496VGT6P_Instance_GetTimestamp( RTC_STM32L496VGT6P_t RTCx, RTC_STM32L496VGT6P_Timestamp_t * Timestamp );
static RTC_STM32L496VGT6P_Status_t RTC_STM32L496VGT6P_Instance_SetTimestamp( RTC_STM32L496VGT6P_t RTCx, RTC_STM32L496VGT6P_Timestamp_t Timestamp );
static RTC_STM32L496VGT6P_Status_t RTC_STM32L496VGT6P_Instance_SetAlarm( RTC_STM32L496VGT6P_t RTCx, RTC_STM32L496VGT6P_Timestamp_t Timestamp );

static RTC_STM32L496VGT6P_Status_t RTC_STM32L496VGT6P_Instance_Commit( RTC_STM32L496VGT6P_t RTCx );

static RTC_STM32L496VGT6P_Status_t RTC_STM32L496VGT6P_Instance_Initialize( RTC_STM32L496VGT6P_t RTCx );
static RTC_STM32L496VGT6P_Status_t RTC_STM32L496VGT6P_Instance_Cycle( RTC_STM32L496VGT6P_t RTCx );
static RTC_STM32L496VGT6P_Status_t RTC_STM32L496VGT6P_Instance_DeInitialize( RTC_STM32L496VGT6P_t RTCx );

static RTC_STM32L496VGT6P_Status_t RTC_STM32L496VGT6P_Context_Initialize( void );
static RTC_STM32L496VGT6P_Status_t RTC_STM32L496VGT6P_Context_Cycle( void );
static RTC_STM32L496VGT6P_Status_t RTC_STM32L496VGT6P_Context_DeInitialize( void );

// #############################################################################
// #### Private Variable(s) ####################################################
// #############################################################################

static RTC_STM32L496VGT6P_Context_t RTC_STM32L496VGT6P_Context;

// #############################################################################
// #### Private Method(s) ######################################################
// #############################################################################

void HAL_RTCEx_WakeUpTimerEventCallback( RTC_HandleTypeDef * hrtc )
{
}

void RTC_WKUP_IRQHandler( void )
{
    RTC_STM32L496VGT6P_Instance_t * Instance = &RTC_STM32L496VGT6P_Context.Instance[ RTC_STM32L496VGT6P_1 ];

    Instance->Event |= RTC_STM32L496VGT6P_Event_InterruptWakeup;

    HAL_RTCEx_WakeUpTimerIRQHandler( &Instance->RTCx );
}

void RTC_Alarm_IRQHandler( void )
{
    RTC_STM32L496VGT6P_Instance_t * Instance = &RTC_STM32L496VGT6P_Context.Instance[ RTC_STM32L496VGT6P_1 ];

    Instance->Event |= RTC_STM32L496VGT6P_Event_InterruptAlarm;

    HAL_RTC_AlarmIRQHandler( &Instance->RTCx );
}

static RTC_STM32L496VGT6P_Status_t RTC_STM32L496VGT6P_Instance_GetTimestamp( RTC_STM32L496VGT6P_t RTCx, RTC_STM32L496VGT6P_Timestamp_t * Timestamp )
{
    RTC_STM32L496VGT6P_Status_t Status = RTC_STM32L496VGT6P_Status_Success;

    do
    {
        RTC_Trace( "%s( RTC=%d, Timestamp=%p )", __FUNCTION__, RTCx, Timestamp );

        RTC_STM32L496VGT6P_Instance_t * Instance = &RTC_STM32L496VGT6P_Context.Instance[ RTCx ];

        // TODO Either update RTC using the context timestamp
        // TODO or get the new timestamp into the context
        // FIXME For now use GET ONLY

        HAL_StatusTypeDef HAL_Status = HAL_OK;
        RTC_TimeTypeDef sTimeStamp;
        RTC_DateTypeDef sTimeStampDate;

        if ( ( HAL_Status = HAL_RTC_GetTime( &Instance->RTCx, &sTimeStamp, RTC_FORMAT_BIN ) ) != HAL_OK )
        {
            // FIXME
            RTC_Warning( "RTC=%d HAL_RTC_GetTime Failed, Status %d", RTCx, HAL_Status );
        }

        if ( ( HAL_Status = HAL_RTC_GetDate( &Instance->RTCx, &sTimeStampDate, RTC_FORMAT_BIN ) ) != HAL_OK )
        {
            // FIXME
            RTC_Warning( "RTC=%d HAL_RTC_GetDate Failed, Status %d", RTCx, HAL_Status );
        }

        // FIXME
        //    if ( HAL_RTCEx_GetTimeStamp( &Instance->RTCx, &sTimeStamp, &sTimeStampDate, RTC_FORMAT_BIN ) != HAL_OK )
        //    {
        //      // FIXME
        //    }

        Instance->Timestamp.Year = 2000 + sTimeStampDate.Year;
        Instance->Timestamp.Month = RTC_Bcd2ToByte( sTimeStampDate.Month );
        Instance->Timestamp.Day = sTimeStampDate.Date;

        Instance->Timestamp.Hour = sTimeStamp.Hours;
        Instance->Timestamp.Minute = sTimeStamp.Minutes;
        Instance->Timestamp.Second = sTimeStamp.Seconds;

        // FIXME
        double subseconds = sTimeStamp.SecondFraction - sTimeStamp.SubSeconds;
        subseconds /= sTimeStamp.SecondFraction + 1;
        Instance->Timestamp.Millisecond = ( uint32_t ) UTIL_SecondToMillisecond( subseconds ) % 1000;
        Instance->Timestamp.Microsecond = ( uint32_t ) UTIL_MillisecondToMicrosecond( UTIL_SecondToMillisecond( subseconds ) ) % 1000;

        Instance->Timestamp.Weekday = sTimeStampDate.WeekDay;

        *Timestamp = Instance->Timestamp;
    }
    while ( 0 );

    return Status;
}

static RTC_STM32L496VGT6P_Status_t RTC_STM32L496VGT6P_Instance_SetTimestamp( RTC_STM32L496VGT6P_t RTCx, RTC_STM32L496VGT6P_Timestamp_t Timestamp )
{
    RTC_STM32L496VGT6P_Status_t Status = RTC_STM32L496VGT6P_Status_Success;

    do
    {
        RTC_Trace( "%s( RTCx=%d, Timestamp={Weekday=%d, Year=%d, Month=%d, Day=%d, Hour=%d, Minute=%d, Second=%d, Millisecond=%d, Microsecond=%d} )", __FUNCTION__, RTCx, Timestamp.Weekday, Timestamp.Year, Timestamp.Month, Timestamp.Day, Timestamp.Hour, Timestamp.Minute, Timestamp.Second, Timestamp.Millisecond, Timestamp.Microsecond );

        RTC_STM32L496VGT6P_Instance_t * Instance = &RTC_STM32L496VGT6P_Context.Instance[ RTCx ];

        RTC_DateTypeDef sTimeStampDate;
        RTC_TimeTypeDef sTimeStamp;

        sTimeStamp.Hours = Timestamp.Hour;
        sTimeStamp.Minutes = Timestamp.Minute;
        sTimeStamp.Seconds = Timestamp.Second;
        sTimeStamp.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
        sTimeStamp.StoreOperation = RTC_STOREOPERATION_RESET;
        if ( HAL_RTC_SetTime( &Instance->RTCx, &sTimeStamp, RTC_FORMAT_BIN ) != HAL_OK )
        {
            Status = RTC_STM32L496VGT6P_Status_Error;
            break;
        }
        sTimeStampDate.WeekDay = Timestamp.Weekday;
        sTimeStampDate.Month = RTC_ByteToBcd2( Timestamp.Month );
        sTimeStampDate.Date = Timestamp.Day;
        sTimeStampDate.Year = Timestamp.Year - 2000;

        if ( HAL_RTC_SetDate( &Instance->RTCx, &sTimeStampDate, RTC_FORMAT_BIN ) != HAL_OK )
        {
            Status = RTC_STM32L496VGT6P_Status_Error;
            break;
        }
    }
    while ( 0 );

    return Status;
}

static RTC_STM32L496VGT6P_Status_t RTC_STM32L496VGT6P_Instance_SetAlarm( RTC_STM32L496VGT6P_t RTCx, RTC_STM32L496VGT6P_Timestamp_t Timestamp )
{
    RTC_STM32L496VGT6P_Status_t Status = RTC_STM32L496VGT6P_Status_Success;
    HAL_StatusTypeDef HAL_Status = HAL_OK;

    do
    {
        RTC_Trace( "%s( RTCx=%d, Timestamp={Weekday=%d, Year=%d, Month=%d, Day=%d, Hour=%d, Minute=%d, Second=%d, Millisecond=%d, Microsecond=%d} )", __FUNCTION__, RTCx, Timestamp.Weekday, Timestamp.Year, Timestamp.Month, Timestamp.Day, Timestamp.Hour, Timestamp.Minute, Timestamp.Second, Timestamp.Millisecond, Timestamp.Microsecond );

        RTC_STM32L496VGT6P_Instance_t * Instance = &RTC_STM32L496VGT6P_Context.Instance[ RTCx ];

        RTC_AlarmTypeDef sAlarm;

        UTIL_MemorySetZero( &sAlarm, sizeof( sAlarm ) );

        sAlarm.AlarmTime.Hours = Timestamp.Hour;
        sAlarm.AlarmTime.Minutes = Timestamp.Minute;
        sAlarm.AlarmTime.Seconds = Timestamp.Second;
        // @note sub-seconds register value calculation breakdown:
        // @ref ss_val = ( PREDIV_S - SSR ) / ( PREDIV_S + 1 )
        // SSR = PREDIV_S - ss_val * (PREDIV_S + 1)
        //     = ( PREDIV_S + 1 ) - 1 - ss_val * (PREDIV_S + 1)
        //     = ( PREDIV_S + 1 ) - 1 - ss_val * (PREDIV_S + 1)
        //     = ( PREDIV_S + 1 ) * ( 1 - ss_val ) - 1
        sAlarm.AlarmTime.SubSeconds = ( 1 + ( Instance->RTCx.Instance->PRER & RTC_PRER_PREDIV_S ) ) * ( 1 - UTIL_MillisecondToSecond( Timestamp.Millisecond ) - UTIL_MillisecondToSecond( UTIL_MicrosecondToMillisecond( Timestamp.Microsecond ) ) ) - 1;
        sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
        sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;

        sAlarm.AlarmMask = RTC_ALARMMASK_NONE; // @note all timestamp fields MUST match

        sAlarm.SubSeconds = sAlarm.AlarmTime.SubSeconds;
        sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_NONE;   // @note all timestamp sub-seconds fields MUST match
        sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE; // @note alarm is being selected on date, could be set on weekdays as well `RTC_ALARMDATEWEEKDAYSEL_WEEKDAY`
        switch ( sAlarm.AlarmDateWeekDaySel )
        {
            case RTC_ALARMDATEWEEKDAYSEL_DATE:
                sAlarm.AlarmDateWeekDay = Timestamp.Day;
                break;

            case RTC_ALARMDATEWEEKDAYSEL_WEEKDAY:
                sAlarm.AlarmDateWeekDay = Timestamp.Weekday;
                break;

            default:
                break;
        }

        sAlarm.Alarm = RTC_ALARM_A;

        if ( ( HAL_Status = HAL_RTC_SetAlarm_IT( &Instance->RTCx, &sAlarm, RTC_FORMAT_BIN ) ) != HAL_OK )
        {
            Status = RTC_STM32L496VGT6P_Status_Error;
            break;
        }
    }
    while ( 0 );

    return Status;
}

static RTC_STM32L496VGT6P_Status_t RTC_STM32L496VGT6P_Instance_Commit( RTC_STM32L496VGT6P_t RTCx )
{
    RTC_STM32L496VGT6P_Status_t Status = RTC_STM32L496VGT6P_Status_Success;

    do
    {
        RTC_Trace( "%s( RTCx=%d )", __FUNCTION__, RTCx );

        RTC_STM32L496VGT6P_Instance_t * Instance = &RTC_STM32L496VGT6P_Context.Instance[ RTCx ];

        UTIL_UNUSED( Instance );

        // FIXME Keep CubeMX generated configurations as is for now
    #if 0
        HAL_StatusTypeDef HAL_Status = HAL_ERROR;
        if ( ( HAL_Status = HAL_RTC_Init( &Instance->RTCx ) ) != HAL_OK )
        {
            Status = RTC_STM32L496VGT6P_Status_Error;
            break;
        }
    #endif
    }
    while ( 0 );

    return Status;
}

static RTC_STM32L496VGT6P_Status_t RTC_STM32L496VGT6P_Instance_Initialize( RTC_STM32L496VGT6P_t RTCx )
{
    RTC_STM32L496VGT6P_Status_t Status = RTC_STM32L496VGT6P_Status_Success;

    do
    {
        RTC_Trace( "%s( RTCx=%d )", __FUNCTION__, RTCx );

        RTC_STM32L496VGT6P_Instance_t * Instance = &RTC_STM32L496VGT6P_Context.Instance[ RTCx ];

        Instance->Event = RTC_STM32L496VGT6P_Event_None;

        // FIXME Is there any RTC configuration to be committed ?
        if ( ( Status = RTC_STM32L496VGT6P_Instance_Commit( RTCx ) ) != RTC_STM32L496VGT6P_Status_Success )
        {
            break;
        }
    }
    while ( 0 );

    return Status;
}

static RTC_STM32L496VGT6P_Status_t RTC_STM32L496VGT6P_Instance_Cycle( RTC_STM32L496VGT6P_t RTCx )
{
    RTC_STM32L496VGT6P_Status_t Status = RTC_STM32L496VGT6P_Status_Success;

    do
    {
        RTC_Trace( "%s( RTCx=%d )", __FUNCTION__, RTCx );

        RTC_STM32L496VGT6P_Instance_t * Instance = &RTC_STM32L496VGT6P_Context.Instance[ RTCx ];
        RTC_STM32L496VGT6P_Event_t Event = Instance->Event; // CAUTION: Has to copy events occurred at the early start of the cycle, so as to be cleared at the end of the cycle,
                                                            //          which let events occurs after that for the next cycle call
        Instance->Event &= ~Event;                          //          Clear captured events

        if ( ( Event & RTC_STM32L496VGT6P_Event_InterruptWakeup ) == RTC_STM32L496VGT6P_Event_InterruptWakeup )
        {
            Event &= ~RTC_STM32L496VGT6P_Event_InterruptWakeup;
            RTC_Debug( "Wake-up Interrupt: RTCx=%d", RTCx );

            // TODO Invoke Callback
        }

        if ( ( Event & RTC_STM32L496VGT6P_Event_InterruptAlarm ) == RTC_STM32L496VGT6P_Event_InterruptAlarm )
        {
            Event &= ~RTC_STM32L496VGT6P_Event_InterruptAlarm;
            RTC_Debug( "Wake-up Alarm: RTCx=%d", RTCx );

            // TODO Invoke Callback
        }

        if ( Event )
        {
            RTC_Warning( "Not handled events %X: RTCx=%d", Event, RTCx );
        }

        // TODO Either update RTC using the context timestamp
        // TODO or get the new timestamp into the context
        // FIXME For now use GET ONLY

        HAL_StatusTypeDef HAL_Status = HAL_OK;
        RTC_TimeTypeDef sTimeStamp;
        RTC_DateTypeDef sTimeStampDate;

        if ( ( HAL_Status = HAL_RTC_GetTime( &Instance->RTCx, &sTimeStamp, RTC_FORMAT_BIN ) ) != HAL_OK )
        {
            // FIXME
            RTC_Warning( "RTC=%d HAL_RTC_GetTime Failed, Status %d", RTCx, HAL_Status );
        }

        if ( ( HAL_Status = HAL_RTC_GetDate( &Instance->RTCx, &sTimeStampDate, RTC_FORMAT_BIN ) ) != HAL_OK )
        {
            // FIXME
            RTC_Warning( "RTC=%d HAL_RTC_GetDate Failed, Status %d", RTCx, HAL_Status );
        }

        Instance->Timestamp.Year = 2000 + sTimeStampDate.Year;
        Instance->Timestamp.Month = RTC_Bcd2ToByte( sTimeStampDate.Month );
        Instance->Timestamp.Day = sTimeStampDate.Date;

        Instance->Timestamp.Hour = sTimeStamp.Hours;
        Instance->Timestamp.Minute = sTimeStamp.Minutes;
        Instance->Timestamp.Second = sTimeStamp.Seconds;

        // @note sub-seconds value calculation breakdown:
        // @ref ss_val = ( PREDIV_S - SSR ) / ( PREDIV_S + 1 )
        double subseconds = sTimeStamp.SecondFraction - sTimeStamp.SubSeconds;
        subseconds /= sTimeStamp.SecondFraction + 1;
        Instance->Timestamp.Millisecond = ( uint32_t ) UTIL_SecondToMillisecond( subseconds ) % 1000;
        Instance->Timestamp.Microsecond = ( uint32_t ) UTIL_MillisecondToMicrosecond( UTIL_SecondToMillisecond( subseconds ) ) % 1000;

        Instance->Timestamp.Weekday = sTimeStampDate.WeekDay;
    }
    while ( 0 );

    return Status;
}

static RTC_STM32L496VGT6P_Status_t RTC_STM32L496VGT6P_Instance_DeInitialize( RTC_STM32L496VGT6P_t RTCx )
{
    RTC_STM32L496VGT6P_Status_t Status = RTC_STM32L496VGT6P_Status_Success;

    do
    {
        RTC_Trace( "%s( RTCx=%d )", __FUNCTION__, RTCx );

        // FIXME De-Initialize RTC
        Status = RTC_STM32L496VGT6P_Status_NotSupported;
    }
    while ( 0 );

    return Status;
}

static RTC_STM32L496VGT6P_Status_t RTC_STM32L496VGT6P_Context_Initialize( void )
{
    RTC_STM32L496VGT6P_Status_t Status = RTC_STM32L496VGT6P_Status_Success;

    do
    {
        RTC_Trace( "%s( void )", __FUNCTION__ );

        // FIXME Remove the usage of `MX_RTC_Init()`
        // FIXME Keep CubeMX generated configurations as is for now
    #if 1
        extern RTC_HandleTypeDef hrtc;
        extern void MX_RTC_Init( void );
        MX_RTC_Init( );
        RTC_STM32L496VGT6P_Context.Instance[ RTC_STM32L496VGT6P_1 ].RTCx = hrtc;
    #endif

        // TODO Verify importance or usage of wake-up timer events
        // for ( RTC_STM32L496VGT6P_t RTC_x = RTC_STM32L496VGT6P_1; RTC_x < RTC_STM32L496VGT6P_Count; ++RTC_x )
        // {
        //     RTC_STM32L496VGT6P_Instance_t * Instance = &RTC_STM32L496VGT6P_Context.Instance[ RTC_x ];
        //
        //     HAL_StatusTypeDef HAL_Status = HAL_ERROR;
        //     if ( ( HAL_Status = HAL_RTCEx_SetWakeUpTimer_IT( &Instance->RTCx, 1000 * RTC_STM32L496VGT6P_TICKS_PER_MS, RTC_WAKEUPCLOCK_RTCCLK_DIV16 ) ) != HAL_OK )
        //     {
        //         Status = RTC_STM32L496VGT6P_Status_Error;
        //         break;
        //     }
        // }
    }
    while ( 0 );

    return Status;
}

static RTC_STM32L496VGT6P_Status_t RTC_STM32L496VGT6P_Context_Cycle( void )
{
    RTC_STM32L496VGT6P_Status_t Status = RTC_STM32L496VGT6P_Status_Success;

    do
    {
        RTC_Trace( "%s( void )", __FUNCTION__ );

        UTIL_UNUSED( RTC_STM32L496VGT6P_Context );
    }
    while ( 0 );

    return Status;
}

static RTC_STM32L496VGT6P_Status_t RTC_STM32L496VGT6P_Context_DeInitialize( void )
{
    RTC_STM32L496VGT6P_Status_t Status = RTC_STM32L496VGT6P_Status_Success;

    do
    {
        RTC_Trace( "%s( void )", __FUNCTION__ );

        UTIL_UNUSED( RTC_STM32L496VGT6P_Context );
    }
    while ( 0 );

    return Status;
}

// #############################################################################
// #### Public Method(s) #######################################################
// #############################################################################

RTC_STM32L496VGT6P_Status_t RTC_STM32L496VGT6P_Initialize( RTC_STM32L496VGT6P_t RTCx )
{
    RTC_STM32L496VGT6P_Status_t Status = RTC_STM32L496VGT6P_Status_Success;

    do
    {
        RTC_Trace( "%s( RTCx=%d )", __FUNCTION__, RTCx );

        if ( ( Status = RTC_STM32L496VGT6P_Context_Initialize( ) ) != RTC_STM32L496VGT6P_Status_Success )
        {
            break;
        }

        Status = RTC_STM32L496VGT6P_Instance_Initialize( RTCx );
    }
    while ( 0 );

    return Status;
}

RTC_STM32L496VGT6P_Status_t RTC_STM32L496VGT6P_Cycle( RTC_STM32L496VGT6P_t RTCx )
{
    RTC_STM32L496VGT6P_Status_t Status = RTC_STM32L496VGT6P_Status_Success;
    do
    {
        RTC_Trace( "%s( RTCx=%d )", __FUNCTION__, RTCx );

        if ( ( Status = RTC_STM32L496VGT6P_Context_Cycle( ) ) != RTC_STM32L496VGT6P_Status_Success )
        {
            break;
        }

        Status = RTC_STM32L496VGT6P_Instance_Cycle( RTCx );
    }
    while ( 0 );

    return Status;
}

RTC_STM32L496VGT6P_Status_t RTC_STM32L496VGT6P_DeInitialize( RTC_STM32L496VGT6P_t RTCx )
{
    RTC_STM32L496VGT6P_Status_t Status = RTC_STM32L496VGT6P_Status_Success;
    do
    {
        RTC_Trace( "%s( RTCx=%d )", __FUNCTION__, RTCx );

        if ( ( Status = RTC_STM32L496VGT6P_Instance_DeInitialize( RTCx ) ) != RTC_STM32L496VGT6P_Status_Success )
        {
            break;
        }

        Status = RTC_STM32L496VGT6P_Context_DeInitialize( );
    }
    while ( 0 );

    return Status;
}

RTC_STM32L496VGT6P_Status_t RTC_STM32L496VGT6P_GetTimestamp( RTC_STM32L496VGT6P_t RTCx, RTC_STM32L496VGT6P_Timestamp_t * Timestamp )
{
    RTC_STM32L496VGT6P_Status_t Status = RTC_STM32L496VGT6P_Status_Success;

    do
    {
        RTC_Trace( "%s( RTCx=%d, Timestamp=%p )", __FUNCTION__, RTCx, Timestamp );

        Status = RTC_STM32L496VGT6P_Instance_GetTimestamp( RTCx, Timestamp );
    }
    while ( 0 );

    return Status;
}

RTC_STM32L496VGT6P_Status_t RTC_STM32L496VGT6P_SetTimestamp( RTC_STM32L496VGT6P_t RTCx, RTC_STM32L496VGT6P_Timestamp_t Timestamp )
{
    RTC_STM32L496VGT6P_Status_t Status = RTC_STM32L496VGT6P_Status_Success;

    do
    {
        RTC_Trace( "%s( RTCx=%d, Timestamp={Weekday=%d, Year=%d, Month=%d, Day=%d, Hour=%d, Minute=%d, Second=%d, Millisecond=%d, Microsecond=%d} )", __FUNCTION__, RTCx, Timestamp.Weekday, Timestamp.Year, Timestamp.Month, Timestamp.Day, Timestamp.Hour, Timestamp.Minute, Timestamp.Second, Timestamp.Millisecond, Timestamp.Microsecond );

        Status = RTC_STM32L496VGT6P_Instance_SetTimestamp( RTCx, Timestamp );
    }
    while ( 0 );

    return Status;
}

RTC_STM32L496VGT6P_Status_t RTC_STM32L496VGT6P_SetAlarm( RTC_STM32L496VGT6P_t RTCx, RTC_STM32L496VGT6P_Timestamp_t Timestamp )
{
    RTC_STM32L496VGT6P_Status_t Status = RTC_STM32L496VGT6P_Status_Success;

    do
    {
        RTC_Trace( "%s( RTCx=%d, Timestamp={Weekday=%d, Year=%d, Month=%d, Day=%d, Hour=%d, Minute=%d, Second=%d, Millisecond=%d, Microsecond=%d} )", __FUNCTION__, RTCx, Timestamp.Weekday, Timestamp.Year, Timestamp.Month, Timestamp.Day, Timestamp.Hour, Timestamp.Minute, Timestamp.Second, Timestamp.Millisecond, Timestamp.Microsecond );

        Status = RTC_STM32L496VGT6P_Instance_SetAlarm( RTCx, Timestamp );
    }
    while ( 0 );

    return Status;
}

// #############################################################################
// #### Public Variable(s) #####################################################
// #############################################################################

// #############################################################################
// #### File Guard #############################################################
// #############################################################################

#endif /* STM32L496xx */

// #############################################################################
// #### END OF FILE ############################################################
// #############################################################################
