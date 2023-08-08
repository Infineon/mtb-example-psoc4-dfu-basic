/******************************************************************************
* File Name:   main.c
*
* Description: This is the source code for the PSoC4 MCU: Basic Device Firmware Upgrade (DFU)
* Example for ModusToolbox.
* This file implements the simple blinky application that can be bootloaded by the
* Bootloader.
*
* Related Document: See README.md
*
*
*******************************************************************************
* Copyright 2020-2023, Cypress Semiconductor Corporation (an Infineon company) or
* an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
*
* This software, including source code, documentation and related
* materials ("Software") is owned by Cypress Semiconductor Corporation
* or one of its affiliates ("Cypress") and is protected by and subject to
* worldwide patent protection (United States and foreign),
* United States copyright laws and international treaty provisions.
* Therefore, you may use this Software only as provided in the license
* agreement accompanying the software package from which you
* obtained this Software ("EULA").
* If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
* non-transferable license to copy, modify, and compile the Software
* source code solely for use in connection with Cypress's
* integrated circuit products.  Any reproduction, modification, translation,
* compilation, or representation of this Software except as specified
* above is prohibited without the express written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
* reserves the right to make changes to the Software without notice. Cypress
* does not assume any liability arising out of the application or use of the
* Software or any product or circuit described in the Software. Cypress does
* not authorize its products for use in any products where a malfunction or
* failure of the Cypress product may reasonably be expected to result in
* significant property damage, injury or death ("High Risk Product"). By
* including Cypress's product in a High Risk Product, the manufacturer
* of such system or application assumes all risk of such use and in doing
* so agrees to indemnify Cypress against all liability.
*******************************************************************************/

#include "cy_pdl.h"
#include "cybsp.h"
#include "cy_dfu.h"

/*******************************************************************************
* Macros
********************************************************************************/
/* App ID of bootloader. */
#define BOOTLOADER_ID                    (0u)
/* Define LED toggle frequency. */
#define LED_TOGGLE_INTERVAL_MS           (100u)

#ifndef TARGET_APP_CY8CKIT_040T
    /* User button interrupt priority. */
    #define SWITCH_INTR_PRIORITY         (3u)
#else
    /**/
    #define I2C_SUCCESS         (0UL)
    #define I2C_FAILURE         (1UL)

    /* User button interrupt priority. */
    #define I2C_INTR_PRIORITY            (3u)

    /*I2C packet Size*/
    #define PACKET_SIZE          (0x03u)

    /* Master write and read buffer of size three bytes */
    #define SL_RD_BUFFER_SIZE    (PACKET_SIZE)
    #define SL_WR_BUFFER_SIZE    (PACKET_SIZE)

    /* Start and end of packet markers */
    #define PACKET_SOP           (0x01u)
    #define PACKET_EOP           (0x17u)
    /*Switch Command*/
    #define SWITCH_TO_APP_CMD    (0xEAu)

    /* Command valid status */
    #define STS_CMD_DONE         (0x00u)
    #define STS_CMD_FAIL         (0xFFu)

    /* Packet positions */
    #define PACKET_SOP_POS       (0x00u)
    #define PACKET_STS_POS       (0x01u)
    #define PACKET_CMD_POS       (0x01u)
    #define PACKET_EOP_POS       (0x02u)

#endif

/*******************************************************************************
* Function Prototypes
********************************************************************************/
#ifndef TARGET_APP_CY8CKIT_040T
    static void user_button_event_handler(void);
#else
    static uint32_t I2C_Init();
    void I2C_InterruptHandler(void);
    static void SlaveCallback(uint32_t event);

#endif

/*******************************************************************************
* Global Variables
********************************************************************************/
/* Application signature. */
CY_SECTION(".cy_app_signature") __USED static const uint32_t cy_dfu_appSignature;


/******************************************************************************
 * Interrupt configuration structure
 *****************************************************************************/
#ifndef TARGET_APP_CY8CKIT_040T   
    const cy_stc_sysint_t switch_intr_config = {
        .intrSrc = CYBSP_USER_BTN_IRQ,                  /* Source of interrupt signal */
        .intrPriority = SWITCH_INTR_PRIORITY            /* Interrupt priority */
    };
#else  /* CY8CKIT-040T uses the I2C to trigger the switch to Bootloader action*/
    /* I2C read and write buffers */
    static uint8_t i2cReadBuffer [SL_RD_BUFFER_SIZE] = {PACKET_SOP, STS_CMD_FAIL,
                                                PACKET_EOP};
    static uint8_t i2cWriteBuffer[SL_WR_BUFFER_SIZE];


    static cy_stc_scb_i2c_context_t DFU_I2C_context;

    /*Configuring the I2C Interrupt*/
    const cy_stc_sysint_t DFU_I2C_SCB_IRQ_config = {
            .intrSrc = (IRQn_Type) DFU_I2C_IRQ,
            .intrPriority = I2C_INTR_PRIORITY
    };
#endif

volatile static bool is_user_event_detected;

/*******************************************************************************
* Function Name: main
********************************************************************************
* Summary:
* This is the main function for CM0 CPU.
*   1. It blinks LED at "LED_TOGGLE_INTERVAL_MS" interval.
*   2. If user event is detected, it will transfer control to
*      Bootloader.
*      The user event is triggered by the user button press for non CY8CKIT-040T
*      Kits, while for CY8CKIT-040T it is triggered by I2C command
*
* Parameters:
*  void
*
* Return:
*  int
*
*******************************************************************************/
int main(void)
{
    cy_rslt_t result;

    /* Initialize the device and board peripherals */
    result = cybsp_init() ;

    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    #ifndef TARGET_APP_CY8CKIT_040T
        /* Initialize and enable GPIO interrupt */
        result = Cy_SysInt_Init(&switch_intr_config, user_button_event_handler);
    #else
        /*Enable the I2C for CY8CKIT-040T*/
        result = I2C_Init();
        if(result != I2C_SUCCESS)
            while(1) {/*Infinite Loop on I2C initialization failure*/}
    #endif

    /* Enable global interrupts */
    __enable_irq();

    #ifndef TARGET_APP_CY8CKIT_040T
    /* Clearing and enabling the GPIO interrupt in NVIC */
    NVIC_ClearPendingIRQ(switch_intr_config.intrSrc);
    NVIC_EnableIRQ(switch_intr_config.intrSrc);
    #endif

    for (;;)
    {
        /* Invert the USER LED state */
        Cy_GPIO_Inv(CYBSP_USER_LED_PORT, CYBSP_USER_LED_PIN);

        /* Delay between LED toggles */
        Cy_SysLib_Delay(LED_TOGGLE_INTERVAL_MS);

        /* Switch to bootloader, if user button is pressed. */
        if (is_user_event_detected)
        {
            is_user_event_detected = false;
            Cy_DFU_ExecuteApp(BOOTLOADER_ID);
        }
    }
}

#ifndef TARGET_APP_CY8CKIT_040T
/*******************************************************************************
* Function Name: user_button_event_handler
********************************************************************************
* Summary:
*   User Button event handler.
*
*******************************************************************************/
static void user_button_event_handler(void)
{
    /* Clears the triggered pin interrupt */
    Cy_GPIO_ClearInterrupt(CYBSP_USER_BTN_PORT, CYBSP_USER_BTN_NUM);
    NVIC_ClearPendingIRQ(switch_intr_config.intrSrc);

    /* Set the flag */
    is_user_event_detected = true;
}

#else

/*******************************************************************************
* Function Name: I2C_Init
********************************************************************************
* Summary:
* This is the function for enabling the I2C block.
* This I2C block will be used by the CY8CKIT-040T as a replacement of the user
* button.
*
* Parameters:
*  void
*
* Return:
*  Status of initialization.
*
*******************************************************************************/
static uint32_t I2C_Init()
{
    cy_en_scb_i2c_status_t initI2Cstatus;
    cy_en_sysint_status_t syCYBSP_I2Cstatus;

    /*Initialize the DFU_I2C SCB*/
    initI2Cstatus = Cy_SCB_I2C_Init(DFU_I2C_HW, &DFU_I2C_config,
                                   &DFU_I2C_context);

    if(initI2Cstatus!=CY_SCB_I2C_SUCCESS)
    {
        return I2C_FAILURE;
    }

    /*Initialize the Interrupt Handler*/
    syCYBSP_I2Cstatus = Cy_SysInt_Init(&DFU_I2C_SCB_IRQ_config, &I2C_InterruptHandler);

    if(syCYBSP_I2Cstatus!=CY_SYSINT_SUCCESS)
    {
        return I2C_FAILURE;
    }

    /* Configure read buffer */
    Cy_SCB_I2C_SlaveConfigReadBuf(DFU_I2C_HW, i2cReadBuffer,
                                  SL_RD_BUFFER_SIZE, &DFU_I2C_context);

    /* Configure write buffer */
    Cy_SCB_I2C_SlaveConfigWriteBuf(DFU_I2C_HW, i2cWriteBuffer,
                                   SL_WR_BUFFER_SIZE, &DFU_I2C_context);

    /* Register Callback function for interrupt */
    Cy_SCB_I2C_RegisterEventCallback(DFU_I2C_HW,
                                    (cy_cb_scb_i2c_handle_events_t) SlaveCallback,
                                    &DFU_I2C_context);

    /*  Enable interrupt and I2C block */
    NVIC_EnableIRQ((IRQn_Type) DFU_I2C_SCB_IRQ_config.intrSrc);
    Cy_SCB_I2C_Enable(DFU_I2C_HW, &DFU_I2C_context);

    return I2C_SUCCESS;
}

/*******************************************************************************
* Function Name: SlaveCallback
********************************************************************************
* Summary:
* This is the callback function if the I2C block.
* Whenever a data is received on the I2C buffer, this callback functions checks
* for the SWITCH_TO_APP_CMD command, and sets the is_user_event_detected flag.
*
* Parameters:
*  event
*
* Return:
*  void
*
*******************************************************************************/
static void SlaveCallback(uint32_t event)
{
    /* Check write complete event */
    if (0UL != (CY_SCB_I2C_SLAVE_WR_CMPLT_EVENT & event))
    {

        /* Check for errors */
        if (0UL == (CY_SCB_I2C_SLAVE_ERR_EVENT & event))
        {
            /* Check packet length */
            if (PACKET_SIZE == Cy_SCB_I2C_SlaveGetWriteTransferCount(DFU_I2C_HW,
                                                                    &DFU_I2C_context))
            {
                /* Check start and end of packet markers */
                if ((i2cWriteBuffer[PACKET_SOP_POS] == PACKET_SOP) && (i2cWriteBuffer[PACKET_EOP_POS] == PACKET_EOP))
                {
                    /* Execute command */
                    /* Switch to BOOTLOADER if received command is SWITCH_TO_APP_CMD (0xEA)*/
                    if(i2cWriteBuffer[PACKET_CMD_POS] == SWITCH_TO_APP_CMD)
                    {
                        is_user_event_detected = true;
                    }

                    i2cReadBuffer[PACKET_STS_POS] = STS_CMD_DONE;
                }
            }
        }

        /* Configure write buffer for the next write */
        Cy_SCB_I2C_SlaveConfigWriteBuf(DFU_I2C_HW, i2cWriteBuffer,
                                       SL_WR_BUFFER_SIZE, &DFU_I2C_context);
    }

    /* Check read complete event */
    if (0UL != (CY_SCB_I2C_SLAVE_RD_CMPLT_EVENT & event))
    {
        /* Configure read buffer for the next read */
        i2cReadBuffer[PACKET_STS_POS] = STS_CMD_FAIL;
        Cy_SCB_I2C_SlaveConfigReadBuf(DFU_I2C_HW, i2cReadBuffer,
                                      SL_RD_BUFFER_SIZE, &DFU_I2C_context);
    }
}

/*******************************************************************************
* Function Name: I2C_InterruptHandler
********************************************************************************
* Summary:
*   The I2C interrupt handler .
*
*******************************************************************************/
void I2C_InterruptHandler(void)
{
    /* ISR implementation for I2C */
    Cy_SCB_I2C_SlaveInterrupt(DFU_I2C_HW, &DFU_I2C_context);
}

#endif


/* [] END OF FILE */
