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

typedef struct RTC_STM32L496VGT6P_InstanceContext
{
    RTC_HandleTypeDef RTCx;
    RTC_STM32L496VGT6P_Timestamp_t Timestamp;
    RTC_STM32L496VGT6P_Event_t Event;
} RTC_STM32L496VGT6P_InstanceContext_t;

typedef struct RTC_STM32L496VGT6P_Context
{
    TIM_Timestamp_t Timestamp;
    RTC_STM32L496VGT6P_InstanceContext_t Context[ RTC_STM32L496VGT6P_Count ];
} RTC_STM32L496VGT6P_Context_t;

// #############################################################################
// #### Private Method(s) Prototype ############################################
// #############################################################################

void HAL_RTCEx_WakeUpTimerEventCallback( RTC_HandleTypeDef * hrtc );
void RTC_WKUP_IRQHandler( void );
void RTC_Alarm_IRQHandler( void );

static RTC_STM32L496VGT6P_Status_t RTC_STM32L496VGT6P_Instance_GetTimestamp( RTC_STM32L496VGT6P_Instance_t * Instance, RTC_STM32L496VGT6P_Timestamp_t * Timestamp );
static RTC_STM32L496VGT6P_Status_t RTC_STM32L496VGT6P_Instance_SetTimestamp( RTC_STM32L496VGT6P_Instance_t * Instance, RTC_STM32L496VGT6P_Timestamp_t Timestamp );

static RTC_STM32L496VGT6P_Status_t RTC_STM32L496VGT6P_Instance_Commit( RTC_STM32L496VGT6P_Instance_t * Instance );

static RTC_STM32L496VGT6P_Status_t RTC_STM32L496VGT6P_Instance_Initialize( RTC_STM32L496VGT6P_Instance_t * Instance );
static RTC_STM32L496VGT6P_Status_t RTC_STM32L496VGT6P_Instance_Cycle( RTC_STM32L496VGT6P_Instance_t * Instance );
static RTC_STM32L496VGT6P_Status_t RTC_STM32L496VGT6P_Instance_DeInitialize( RTC_STM32L496VGT6P_Instance_t * Instance );

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
    RTC_STM32L496VGT6P_InstanceContext_t * Context = &RTC_STM32L496VGT6P_Context.Context[ RTC_STM32L496VGT6P_1 ];

    Context->Event |= RTC_STM32L496VGT6P_Event_InterruptWakeup;

    HAL_RTCEx_WakeUpTimerIRQHandler( &Context->RTCx );
}

void RTC_Alarm_IRQHandler( void )
{
    RTC_STM32L496VGT6P_InstanceContext_t * Context = &RTC_STM32L496VGT6P_Context.Context[ RTC_STM32L496VGT6P_1 ];

    Context->Event |= RTC_STM32L496VGT6P_Event_InterruptAlarm;
}

static RTC_STM32L496VGT6P_Status_t RTC_STM32L496VGT6P_Instance_GetTimestamp( RTC_STM32L496VGT6P_Instance_t * Instance, RTC_STM32L496VGT6P_Timestamp_t * Timestamp )
{
    RTC_STM32L496VGT6P_Status_t Status = RTC_STM32L496VGT6P_Status_Success;

    do
    {
        RTC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );

        // TODO Either update RTC using the context timestamp
        // TODO or get the new timestamp into the context
        // FIXME For now use GET ONLY

        RTC_DateTypeDef sTimeStampDate;
        RTC_TimeTypeDef sTimeStamp;

        if ( HAL_RTC_GetDate( &Instance->Context->RTCx, &sTimeStampDate, RTC_FORMAT_BIN ) != HAL_OK )
        {
            // FIXME
        }

        if ( HAL_RTC_GetTime( &Instance->Context->RTCx, &sTimeStamp, RTC_FORMAT_BIN ) != HAL_OK )
        {
            // FIXME
        }

        // FIXME
        //    if ( HAL_RTCEx_GetTimeStamp( &Instance->Context->RTCx, &sTimeStamp, &sTimeStampDate, RTC_FORMAT_BIN ) != HAL_OK )
        //    {
        //      // FIXME
        //    }

        Instance->Context->Timestamp.Year = 2000 + sTimeStampDate.Year;
        Instance->Context->Timestamp.Month = RTC_Bcd2ToByte( sTimeStampDate.Month );
        Instance->Context->Timestamp.Day = sTimeStampDate.Date;

        Instance->Context->Timestamp.Hour = sTimeStamp.Hours;
        Instance->Context->Timestamp.Minute = sTimeStamp.Minutes;
        Instance->Context->Timestamp.Second = sTimeStamp.Seconds;

        // FIXME
        double subseconds = sTimeStamp.SecondFraction - sTimeStamp.SubSeconds;
        subseconds /= sTimeStamp.SecondFraction + 1;
        Instance->Context->Timestamp.Millisecond = ( uint32_t ) UTIL_SecondToMillisecond( subseconds ) % 1000;
        Instance->Context->Timestamp.Microsecond = ( uint32_t ) UTIL_MillisecondToMicrosecond( UTIL_SecondToMillisecond( subseconds ) ) % 1000;

        Instance->Context->Timestamp.Weekday = sTimeStampDate.WeekDay;

        *Timestamp = Instance->Context->Timestamp;
    }
    while ( 0 );

    return Status;
}

static RTC_STM32L496VGT6P_Status_t RTC_STM32L496VGT6P_Instance_SetTimestamp( RTC_STM32L496VGT6P_Instance_t * Instance, RTC_STM32L496VGT6P_Timestamp_t Timestamp )
{
    RTC_STM32L496VGT6P_Status_t Status = RTC_STM32L496VGT6P_Status_Success;

    do
    {
        RTC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );

        RTC_DateTypeDef sTimeStampDate;
        RTC_TimeTypeDef sTimeStamp;

        sTimeStamp.Hours = Timestamp.Hour;
        sTimeStamp.Minutes = Timestamp.Minute;
        sTimeStamp.Seconds = Timestamp.Second;
        sTimeStamp.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
        sTimeStamp.StoreOperation = RTC_STOREOPERATION_RESET;
        if ( HAL_RTC_SetTime( &Instance->Context->RTCx, &sTimeStamp, RTC_FORMAT_BIN ) != HAL_OK )
        {
            Status = RTC_STM32L496VGT6P_Status_Error;
            break;
        }
        sTimeStampDate.WeekDay = Timestamp.Weekday;
        sTimeStampDate.Month = RTC_ByteToBcd2( Timestamp.Month );
        sTimeStampDate.Date = Timestamp.Day;
        sTimeStampDate.Year = Timestamp.Year - 2000;

        if ( HAL_RTC_SetDate( &Instance->Context->RTCx, &sTimeStampDate, RTC_FORMAT_BIN ) != HAL_OK )
        {
            Status = RTC_STM32L496VGT6P_Status_Error;
            break;
        }
    }
    while ( 0 );

    return Status;
}

static RTC_STM32L496VGT6P_Status_t RTC_STM32L496VGT6P_Instance_Commit( RTC_STM32L496VGT6P_Instance_t * Instance )
{
    RTC_STM32L496VGT6P_Status_t Status = RTC_STM32L496VGT6P_Status_Success;

    do
    {
        RTC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );

        if ( Instance == NULL )
        {
            Status = RTC_STM32L496VGT6P_Status_ArgumentInvalid;
            break;
        }

        if ( Instance->Context == NULL )
        {
            Status = RTC_STM32L496VGT6P_Status_Error;
            break;
        }

        // FIXME Keep CubeMX generated configurations as is for now
    #if 0
    HAL_StatusTypeDef HAL_Status = HAL_ERROR;
    if ( ( HAL_Status = HAL_RTC_Init( &Instance->Context->RTCx ) ) != HAL_OK )
    {
      Status = RTC_STM32L496VGT6P_Status_Error;
      break;
    }
    #endif
    }
    while ( 0 );

    return Status;
}

static RTC_STM32L496VGT6P_Status_t RTC_STM32L496VGT6P_Instance_Initialize( RTC_STM32L496VGT6P_Instance_t * Instance )
{
    RTC_STM32L496VGT6P_Status_t Status = RTC_STM32L496VGT6P_Status_Success;

    do
    {
        RTC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );

        if ( Instance == NULL )
        {
            Status = RTC_STM32L496VGT6P_Status_ArgumentInvalid;
            break;
        }

        RTC_STM32L496VGT6P_InstanceContext_t * Context = &RTC_STM32L496VGT6P_Context.Context[ Instance->RTCx ];

        Instance->Context = Context;

        if ( ( Status = RTC_STM32L496VGT6P_Instance_Commit( Instance ) ) != RTC_STM32L496VGT6P_Status_Success )
        {
            break;
        }
    }
    while ( 0 );

    return Status;
}

static RTC_STM32L496VGT6P_Status_t RTC_STM32L496VGT6P_Instance_Cycle( RTC_STM32L496VGT6P_Instance_t * Instance )
{
    RTC_STM32L496VGT6P_Status_t Status = RTC_STM32L496VGT6P_Status_Success;

    do
    {
        RTC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );

        // TODO Either update RTC using the context timestamp
        // TODO or get the new timestamp into the context
        // FIXME For now use GET ONLY

        RTC_DateTypeDef sTimeStampDate;
        RTC_TimeTypeDef sTimeStamp;

        if ( HAL_RTC_GetDate( &Instance->Context->RTCx, &sTimeStampDate, RTC_FORMAT_BIN ) != HAL_OK )
        {
            // FIXME
        }

        if ( HAL_RTC_GetTime( &Instance->Context->RTCx, &sTimeStamp, RTC_FORMAT_BIN ) != HAL_OK )
        {
            // FIXME
        }

        // FIXME
        //    if ( HAL_RTCEx_GetTimeStamp( &Instance->Context->RTCx, &sTimeStamp, &sTimeStampDate, RTC_FORMAT_BIN ) != HAL_OK )
        //    {
        //      // FIXME
        //    }

        Instance->Context->Timestamp.Year = 2000 + sTimeStampDate.Year;
        Instance->Context->Timestamp.Month = RTC_Bcd2ToByte( sTimeStampDate.Month );
        Instance->Context->Timestamp.Day = sTimeStampDate.Date;

        Instance->Context->Timestamp.Hour = sTimeStamp.Hours;
        Instance->Context->Timestamp.Minute = sTimeStamp.Minutes;
        Instance->Context->Timestamp.Second = sTimeStamp.Seconds;

        // FIXME
        double subseconds = sTimeStamp.SecondFraction - sTimeStamp.SubSeconds;
        subseconds /= sTimeStamp.SecondFraction + 1;
        Instance->Context->Timestamp.Millisecond = ( uint32_t ) UTIL_SecondToMillisecond( subseconds ) % 1000;
        Instance->Context->Timestamp.Microsecond = ( uint32_t ) UTIL_MillisecondToMicrosecond( UTIL_SecondToMillisecond( subseconds ) ) % 1000;

        Instance->Context->Timestamp.Weekday = sTimeStampDate.WeekDay;
    }
    while ( 0 );

    return Status;
}

static RTC_STM32L496VGT6P_Status_t RTC_STM32L496VGT6P_Instance_DeInitialize( RTC_STM32L496VGT6P_Instance_t * Instance )
{
    RTC_STM32L496VGT6P_Status_t Status = RTC_STM32L496VGT6P_Status_Success;

    do
    {
        RTC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );

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
        RTC_STM32L496VGT6P_Context.Context[ RTC_STM32L496VGT6P_1 ].RTCx = hrtc;
    #endif

        for ( RTC_STM32L496VGT6P_t RTC_x = RTC_STM32L496VGT6P_1; RTC_x < RTC_STM32L496VGT6P_Count; ++RTC_x )
        {
            RTC_STM32L496VGT6P_InstanceContext_t * Context = &RTC_STM32L496VGT6P_Context.Context[ RTC_x ];

            HAL_StatusTypeDef HAL_Status = HAL_ERROR;
            if ( ( HAL_Status = HAL_RTCEx_SetWakeUpTimer_IT( &Context->RTCx, 1000 * RTC_STM32L496VGT6P_TICKS_PER_MS, RTC_WAKEUPCLOCK_RTCCLK_DIV16 ) ) != HAL_OK )
            {
                Status = RTC_STM32L496VGT6P_Status_Error;
                break;
            }
        }
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
    }
    while ( 0 );

    return Status;
}

// #############################################################################
// #### Public Method(s) #######################################################
// #############################################################################

RTC_STM32L496VGT6P_Status_t RTC_STM32L496VGT6P_Initialize( RTC_STM32L496VGT6P_Instance_t * Instance )
{
    RTC_STM32L496VGT6P_Status_t Status = RTC_STM32L496VGT6P_Status_Success;

    do
    {
        RTC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );

        if ( ( Status = RTC_STM32L496VGT6P_Context_Initialize( ) ) != RTC_STM32L496VGT6P_Status_Success )
        {
            break;
        }

        Status = RTC_STM32L496VGT6P_Instance_Initialize( Instance );
    }
    while ( 0 );

    return Status;
}

RTC_STM32L496VGT6P_Status_t RTC_STM32L496VGT6P_Cycle( RTC_STM32L496VGT6P_Instance_t * Instance )
{
    RTC_STM32L496VGT6P_Status_t Status = RTC_STM32L496VGT6P_Status_Success;
    do
    {
        RTC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );

        if ( ( Status = RTC_STM32L496VGT6P_Context_Cycle( ) ) != RTC_STM32L496VGT6P_Status_Success )
        {
            break;
        }

        Status = RTC_STM32L496VGT6P_Instance_Cycle( Instance );
    }
    while ( 0 );

    return Status;
}

RTC_STM32L496VGT6P_Status_t RTC_STM32L496VGT6P_DeInitialize( RTC_STM32L496VGT6P_Instance_t * Instance )
{
    RTC_STM32L496VGT6P_Status_t Status = RTC_STM32L496VGT6P_Status_Success;
    do
    {
        RTC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );

        if ( ( Status = RTC_STM32L496VGT6P_Instance_DeInitialize( Instance ) ) != RTC_STM32L496VGT6P_Status_Success )
        {
            break;
        }

        Status = RTC_STM32L496VGT6P_Context_DeInitialize( );
    }
    while ( 0 );

    return Status;
}

RTC_STM32L496VGT6P_Status_t RTC_STM32L496VGT6P_GetTimestamp( RTC_STM32L496VGT6P_Instance_t * Instance, RTC_STM32L496VGT6P_Timestamp_t * Timestamp )
{
    RTC_STM32L496VGT6P_Status_t Status = RTC_STM32L496VGT6P_Status_Success;

    do
    {
        RTC_Trace( "%s( Instance=%p, Timestamp=%p )", __FUNCTION__, Instance, Timestamp );

        Status = RTC_STM32L496VGT6P_Instance_GetTimestamp( Instance, Timestamp );
    }
    while ( 0 );

    return Status;
}

RTC_STM32L496VGT6P_Status_t RTC_STM32L496VGT6P_SetTimestamp( RTC_STM32L496VGT6P_Instance_t * Instance, RTC_STM32L496VGT6P_Timestamp_t Timestamp )
{
    RTC_STM32L496VGT6P_Status_t Status = RTC_STM32L496VGT6P_Status_Success;

    do
    {
        RTC_Trace( "%s( Instance=%p, Timestamp=%p )", __FUNCTION__, Instance, Timestamp );

        Status = RTC_STM32L496VGT6P_Instance_SetTimestamp( Instance, Timestamp );
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
