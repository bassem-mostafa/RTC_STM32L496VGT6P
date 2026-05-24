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

/**
 *  @file
 *
 *  @brief Platform RTC STM32L496VGT6P Driver
 */

// #############################################################################
// #### Control Include(s) #####################################################
// #############################################################################

// #############################################################################
// #### Control Macro(s) #######################################################
// #############################################################################

// #############################################################################
// #### File Guard #############################################################
// #############################################################################

/**
 *  @addtogroup Platform_RTC_Driver
 *
 *  @{
 */

/**
 *  @defgroup Platform_RTC_STM32L496VGT6P STM32L496VGT6P
 *
 *  @{
 */

#ifndef RTC_STM32L496VGT6P_H_
    #define RTC_STM32L496VGT6P_H_

    #ifdef __cplusplus
extern "C"
{
    #endif /* __cplusplus */

    // #############################################################################
    // #### Include(s) #############################################################
    // #############################################################################

    // #############################################################################
    // #### Public Macro(s) ########################################################
    // #############################################################################

    // #############################################################################
    // #### Public Type(s) #########################################################
    // #############################################################################

    /**
     *  @brief RTC STM32L496VGT6P Operation Status Type
     *
     *  @enum RTC_STM32L496VGT6P_Status_t
     */
    typedef enum RTC_STM32L496VGT6P_Status
    {
        RTC_STM32L496VGT6P_Status_Success = 0,     ///< Success
        RTC_STM32L496VGT6P_Status_ArgumentInvalid, ///< Argument Invalid
        RTC_STM32L496VGT6P_Status_NotSupported,    ///< Not Supported
        RTC_STM32L496VGT6P_Status_Error,           ///< General Error
        RTC_STM32L496VGT6P_Status_Busy,            ///< Busy
        RTC_STM32L496VGT6P_Status_Timeout,         ///< Timeout
    } RTC_STM32L496VGT6P_Status_t;

    /**
     *  @brief RTC STM32L496VGT6P Type
     *
     *  @enum RTC_STM32L496VGT6P_t
     */
    typedef enum RTC_STM32L496VGT6P
    {
        RTC_STM32L496VGT6P_1 = 0, ///< RTC 1
        RTC_STM32L496VGT6P_Count, ///< Count
    } RTC_STM32L496VGT6P_t;

    /**
     *  @brief RTC STM32L496VGT6P Time-stamp Type
     */
    typedef struct RTC_STM32L496VGT6P_Timestamp
    {
        uint16_t Year;
        uint8_t Month;
        uint8_t Day;

        uint8_t Hour;
        uint8_t Minute;
        uint8_t Second;

        uint16_t Millisecond;
        uint16_t Microsecond;

        uint8_t Weekday; // Monday = 1, ... Sunday = 7
    } RTC_STM32L496VGT6P_Timestamp_t;

    // #############################################################################
    // #### Public Method(s) #######################################################
    // #############################################################################

    /**
     *  @brief Initializes specified RTC STM32L496VGT6P Instance
     *
     *  @param[in] RTCx Peripheral
     *
     *  @return RTC_STM32L496VGT6P_Status_t
     */
    RTC_STM32L496VGT6P_Status_t RTC_STM32L496VGT6P_Initialize( RTC_STM32L496VGT6P_t RTCx );

    /**
     *  @brief Cycles specified RTC STM32L496VGT6P Instance
     *
     *  @param[in] RTCx Peripheral
     *
     *  @return RTC_STM32L496VGT6P_Status_t
     */
    RTC_STM32L496VGT6P_Status_t RTC_STM32L496VGT6P_Cycle( RTC_STM32L496VGT6P_t RTCx );

    /**
     *  @brief De-initializes specified RTC STM32L496VGT6P Instance
     *
     *  @param[in] RTCx Peripheral
     *
     *  @return RTC_STM32L496VGT6P_Status_t
     */
    RTC_STM32L496VGT6P_Status_t RTC_STM32L496VGT6P_DeInitialize( RTC_STM32L496VGT6P_t RTCx );

    /**
     *  @brief Get time-stamp from specified RTC STM32L496VGT6P Instance
     *
     *  @param[in]  RTCx      Peripheral
     *  @param[out] Timestamp Time-stamp
     *
     *  @return RTC_STM32L496VGT6P_Status_t
     */
    RTC_STM32L496VGT6P_Status_t RTC_STM32L496VGT6P_GetTimestamp( RTC_STM32L496VGT6P_t RTCx, RTC_STM32L496VGT6P_Timestamp_t * Timestamp );

    /**
     *  @brief Set time-stamp to specified RTC STM32L496VGT6P Instance
     *
     *  @param[in] RTCx      Peripheral
     *  @param[in] Timestamp Time-stamp
     *
     *  @return RTC_STM32L496VGT6P_Status_t
     */
    RTC_STM32L496VGT6P_Status_t RTC_STM32L496VGT6P_SetTimestamp( RTC_STM32L496VGT6P_t RTCx, RTC_STM32L496VGT6P_Timestamp_t Timestamp );

    // TODO Add More APIs

    // #############################################################################
    // #### Public Variable(s) #####################################################
    // #############################################################################

    // #############################################################################
    // #### File Guard #############################################################
    // #############################################################################

    #ifdef __cplusplus
} /* extern "C" */
    #endif /* __cplusplus */

#endif /* RTC_STM32L496VGT6P_H_ */

/**
 *  @}
 *
 *  @}
 */

// #############################################################################
// #### END OF FILE ############################################################
// #############################################################################
