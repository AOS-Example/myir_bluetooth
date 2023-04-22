#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <stdlib.h>
#include "TestSuite.h"

#include "btaudiogw.hpp"
#include "priv/btaudiogw_impl.hpp"

#ifdef __cplusplus
extern "C" {
#endif

#include "btaudiogw.h"

#ifdef __cplusplus
}
#endif

using namespace ::testing;
using Bluetooth::AudiogwImpl;
void btaudiogw_callback( btaudiogw_event_t evt, uint8_t *buf, size_t len, void *args ) {
}

void btaudiogw_cmd_callback( btaudiogw_cmd_response_t cmd_rsp, uint64_t id, bt_errcode_t errcode, const uint8_t *buf, size_t len, void *args ) {
}

TEST_F( libbttest, btaudiogw_connect_device_test ) {
    bt_addr_t addr{};
    EXPECT_EQ( BT_STATUS_SUCCESS, btaudiogw_init( btaudiogw_callback, btaudiogw_cmd_callback, nullptr ) );
    EXPECT_EQ( BT_STATUS_SUCCESS, btaudiogw_registerservice() );
    EXPECT_EQ( BT_STATUS_SUCCESS, btaudiogw_connect_device( &addr ) );
    EXPECT_EQ( BT_STATUS_SUCCESS, btaudiogw_connect_audio() );

    uint8_t level = 1;

    EXPECT_EQ( BT_STATUS_SUCCESS, btaudiogw_battery_level( level ) );
    EXPECT_EQ( BT_STATUS_SUCCESS, btaudiogw_roaming_status( level ) );
    EXPECT_EQ( BT_STATUS_SUCCESS, btaudiogw_signal_level( level ) );
    EXPECT_EQ( BT_STATUS_SUCCESS, btaudiogw_voice_rec_status( level ) );
    //const char *service_num = "012";
    const char *service_operator = "012";
    EXPECT_EQ( BT_STATUS_SUCCESS, btaudiogw_service_data( level, service_operator, service_operator ) );
    EXPECT_EQ( BT_STATUS_SUCCESS, btaudiogw_mfg_mod_data( service_operator, service_operator ) );
    EXPECT_EQ( BT_STATUS_SUCCESS, btaudiogw_call_state_idle() );
    uint8_t index = 0;
    const char *number = "0123456789";
    btaudiogw_call_state_t state{};
    btaudiogw_call_direction_t dir{};
    btaudiogw_call_mode_t mode{};
    uint8_t type = 0;
    EXPECT_EQ( BT_STATUS_SUCCESS, btaudiogw_update_call_status( index, number, state, dir, mode, type ) );

    EXPECT_EQ( BT_STATUS_SUCCESS, btaudiogw_disconnect_audio() );
    EXPECT_EQ( BT_STATUS_SUCCESS, btaudiogw_disconnect_device( &addr ) );
    EXPECT_EQ( BT_STATUS_SUCCESS, btaudiogw_deinit() );
}

TEST_F( libbttest, btaudiogw_no_connection_test ) {
    bt_addr_t addr{};
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btaudiogw_registerservice() );

    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btaudiogw_connect_device( &addr ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btaudiogw_connect_audio() );

    uint8_t level = 1;

    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btaudiogw_battery_level( level ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btaudiogw_roaming_status( level ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btaudiogw_signal_level( level ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btaudiogw_voice_rec_status( level ) );
    //const char *service_num = "012";
    const char *service_operator = "012";
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btaudiogw_service_data( level, service_operator, service_operator ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btaudiogw_mfg_mod_data( service_operator, service_operator ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btaudiogw_call_state_idle() );
    uint8_t index = 0;
    const char *number = "0123456789";
    btaudiogw_call_state_t state{};
    btaudiogw_call_direction_t dir{};
    btaudiogw_call_mode_t mode{};
    uint8_t type = 0;
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btaudiogw_update_call_status( index, number, state, dir, mode, type ) );

    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btaudiogw_disconnect_audio() );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btaudiogw_disconnect_device( &addr ) );
}

TEST_F( libbttest, btaudiogw_no_transport_test ) {
    bt_addr_t addr{};

    EXPECT_EQ( BT_STATUS_SUCCESS, btaudiogw_init( btaudiogw_callback, btaudiogw_cmd_callback, nullptr ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btaudiogw_connect_device( &addr ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btaudiogw_connect_audio() );

    uint8_t level = 1;

    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btaudiogw_battery_level( level ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btaudiogw_roaming_status( level ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btaudiogw_signal_level( level ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btaudiogw_voice_rec_status( level ) );
    //const char *service_num = "012";
    const char *service_operator = "012";
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btaudiogw_service_data( level, service_operator, service_operator ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btaudiogw_mfg_mod_data( service_operator, service_operator ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btaudiogw_call_state_idle() );
    uint8_t index = 0;
    const char *number = "0123456789";
    btaudiogw_call_state_t state{};
    btaudiogw_call_direction_t dir{};
    btaudiogw_call_mode_t mode{};
    uint8_t type = 0;
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btaudiogw_update_call_status( index, number, state, dir, mode, type ) );

    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btaudiogw_disconnect_audio() );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btaudiogw_disconnect_device( &addr ) );

    EXPECT_EQ( BT_STATUS_FAILED, btaudiogw_deinit() );
}

using BtmgrEvtHandler_t = int ( * )( btres_t *btres, int fd, uint16_t msg, uint8_t *buf, size_t len, void *args );
using except_cb_t = int ( * )( btres_t *btres, int fd );

TEST_F( libbttest, btaudiogw_event_handler_test ) {
    BtmgrEvtHandler_t AudioGwEventHandler = nullptr;
    btres_t *btres_out = nullptr;

    EXPECT_CALL( *resMock, btres_init( _, _, _, _, _ ) )
    .WillOnce( Invoke( [&AudioGwEventHandler, &btres_out]( btres_t *btres_in, jobq_t *jobq, handle_cmd_cb cmd_cb, handle_evt_cb evt_cb, void *args ) {
        AudioGwEventHandler = evt_cb;
        btres_out = btres_in;
        return BTRES_RESULT_SUCCESS;
    } ) )
    .WillRepeatedly( Return( BTRES_RESULT_SUCCESS ) );

    EXPECT_EQ( BT_STATUS_SUCCESS, btaudiogw_init( btaudiogw_callback, btaudiogw_cmd_callback, nullptr ) );

    except_cb_t AudioGwtExcept = nullptr;
    btres_t *btres_except = nullptr;
    EXPECT_CALL( *resMock, btres_add_conn( _, _, _, _, _ ) )
    .WillOnce( Invoke( [&AudioGwtExcept, &btres_except]( btres_t *btres, int parent_fd, int fd, const char *progname, except_cb_t except ) {
        AudioGwtExcept = except;
        btres_except = btres;
        return BTRES_RESULT_SUCCESS;
    } ) );

    EXPECT_EQ( BT_STATUS_SUCCESS, btaudiogw_registerservice() );

    ASSERT_NE( nullptr, AudioGwEventHandler );
    ASSERT_NE( nullptr, AudioGwtExcept );
    uint8_t buff[5] {};
    AudiogwImpl btaudiogw{};
    AudioGwEventHandler( btres_out, 0, BT_EVENT_COMMAND_RESPONSE, buff, sizeof( buff ), &btaudiogw );
    AudioGwtExcept( btres_except, 0 );
    btaudiogw_deinit();
}