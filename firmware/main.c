/*
 * TSDZ2 EBike wireless firmware
 *
 * Copyright (C) Casainho, 2020
 *
 * Released under the GPL License, Version 3
 */

#include <stdio.h>
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf.h"
#include "hardfault.h"
#include "app_error.h"
#include "app_timer.h"
#include "nrf_pwr_mgmt.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ant.h"
#include "ant_key_manager.h"
#include "ant_lev.h"
#include "pins.h"

#define LEV_HW_REVISION 1
#define LEV_MANUFACTURER_ID (UINT16_MAX - 1)
#define LEV_MODEL_NUMBER 1
#define LEV_SW_REVISION_MAJOR 1
#define LEV_SW_REVISION_MINOR 1
#define LEV_SERIAL_NUMBER 1234567
#define LEV_CHANNEL_NUM 0 // ?? seems can be any value from 0 to 8
#define CHAN_ID_TRANS_TYPE 5 // LEV table 4.2
#define CHAN_ID_DEV_NUM 1 // [1 - 65535], LEV table 4.2
#define ANTPLUS_NETWORK_NUM 0
#define ANT_LEV_ANT_OBSERVER_PRIO 1

void ant_lev_evt_handler(ant_lev_profile_t * p_profile, ant_lev_evt_t event);

LEV_SENS_CHANNEL_CONFIG_DEF(m_ant_lev,
                            LEV_CHANNEL_NUM,
                            CHAN_ID_TRANS_TYPE,
                            CHAN_ID_DEV_NUM,
                            ANTPLUS_NETWORK_NUM);
LEV_SENS_PROFILE_CONFIG_DEF(m_ant_lev,
                            ant_lev_evt_handler);

static ant_lev_profile_t m_ant_lev;

NRF_SDH_ANT_OBSERVER(m_ant_observer, ANT_LEV_ANT_OBSERVER_PRIO, ant_lev_sens_evt_handler, &m_ant_lev);

uint16_t cnt_1;

void ant_lev_evt_handler(ant_lev_profile_t * p_profile, ant_lev_evt_t event)
{
    nrf_pwr_mgmt_feed();

    switch (event)
    {
        case ANT_LEV_PAGE_1_UPDATED:
            break;

        case ANT_LEV_PAGE_2_UPDATED:
            break;

        case ANT_LEV_PAGE_3_UPDATED:
            break;

        case ANT_LEV_PAGE_4_UPDATED:
            break;

        case ANT_LEV_PAGE_5_UPDATED:
            break;

        case ANT_LEV_PAGE_34_UPDATED:
            break;

        case ANT_LEV_PAGE_16_UPDATED:
            if ((p_profile->page_16.travel_mode & 0x38) > 0)
                motor_power_enable(false);
            else
                motor_power_enable(true);

            // update the assist level
            p_profile->common.travel_mode_state = p_profile->page_16.travel_mode;

            break;

        case ANT_LEV_PAGE_80_UPDATED:
            break;

        case ANT_LEV_PAGE_81_UPDATED:
            break;

        case ANT_LEV_PAGE_REQUEST_SUCCESS:
            break;

        case ANT_LEV_PAGE_REQUEST_FAILED:
            break;

        default:
            break;
    }
}

static void profile_setup(void)
{
    uint32_t err_code;

    err_code = ant_lev_sens_init(&m_ant_lev,
                                 LEV_SENS_CHANNEL_CONFIG(m_ant_lev),
                                 LEV_SENS_PROFILE_CONFIG(m_ant_lev));
    APP_ERROR_CHECK(err_code);

    // fill manufacturer's common data page.
    m_ant_lev.page_80 = ANT_COMMON_page80(LEV_HW_REVISION,
                                          LEV_MANUFACTURER_ID,
                                          LEV_MODEL_NUMBER);
    // fill product's common data page.
    m_ant_lev.page_81 = ANT_COMMON_page81(LEV_SW_REVISION_MAJOR,
                                          LEV_SW_REVISION_MINOR,
                                          LEV_SERIAL_NUMBER);

    err_code = ant_lev_sens_open(&m_ant_lev);
    APP_ERROR_CHECK(err_code);
}

static void softdevice_setup(void)
{
    ret_code_t err_code = nrf_sdh_enable_request();
    APP_ERROR_CHECK(err_code);

    ASSERT(nrf_sdh_is_enabled());

    err_code = nrf_sdh_ant_enable();
    APP_ERROR_CHECK(err_code);

    err_code = ant_plus_key_set(ANTPLUS_NETWORK_NUM);
    APP_ERROR_CHECK(err_code);
}

static void log_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}

int main(void)
{
    static uint16_t cnt_2;

    log_init();
    pins_init();
    softdevice_setup();
    profile_setup();

    for (;;)
    {
        cnt_1 += 3;
        cnt_2++;
        __asm__("nop");

        NRF_LOG_FLUSH();
        // nrf_pwr_mgmt_run();
    }
}

