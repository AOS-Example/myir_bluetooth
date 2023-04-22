#include <stdlib.h>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "bt.h"
#include "bt_utils.h"
#include "TestSuite.h"

using namespace ::testing;

TEST_F( libbttest, btutils_calls_success ) {

    // To cover all the local control paths, the following functions are called repeatedly with
    // parameter enumerations.

    uint16_t i;

    for ( i = 0; i < 18; i++ ) {
        EXPECT_NE ( ( const char * )NULL, bt_cmd_rsp_to_str( ( bt_cmd_response_t )( BT_CORE_MASK | i ) ) );
    }

    for ( i = 0; i < 24; i++ ) {
        EXPECT_NE ( ( const char * )NULL, bt_event_to_str( ( bt_event_t )( BT_CORE_MASK | i ) ) );
    }

    for ( i = 0; i < 34; i++ ) {
        EXPECT_NE ( ( const char * )NULL, bt_status_to_str( ( bt_status_t )i ) );
    }

    EXPECT_NE ( ( const char * )NULL, bt_status_to_str( ( bt_status_t )255 ) );

    for ( i = 0; i < 0xffff; i++ ) {
        EXPECT_NE ( ( const char * )NULL, bt_errcode_to_str( ( bt_errcode_t )i ) );
    }

    for ( i = 0; i < 18; i++ ) {
        EXPECT_NE ( ( const char * )NULL, btphone_cmd_rsp_to_str( ( btphone_cmd_response_t )( BT_HANDSFREE_MASK | i ) ) );
    }

    for ( i = 0; i < 22; i++ ) {
        EXPECT_NE ( ( const char * )NULL, btphone_event_to_str( ( btphone_event_t )( BT_HANDSFREE_MASK | i ) ) );
    }

    for ( i = 0; i < 8; i++ ) {
        EXPECT_NE ( ( const char * )NULL, btphone_call_state_to_str( ( btphone_call_state_t )i ) );
    }

    for ( i = 0; i < 3; i++ ) {
        EXPECT_NE ( ( const char * )NULL, btphone_call_direction_to_str( ( btphone_call_direction_t )i ) );
    }

    for ( i = 0; i < 3; i++ ) {
        EXPECT_NE ( ( const char * )NULL, btphone_call_multiparty_to_str( ( btphone_call_multiparty_t )i ) );
    }

    for ( i = 0; i < 12; i++ ) {
        EXPECT_NE ( ( const char * )NULL, btphone_pbk_index_to_str( ( btphone_pbk_index_t )i ) );
    }

    for ( i = 0; i < 10; i++ ) {
        EXPECT_NE ( ( const char * )NULL, btle_event_to_str( ( btle_event_t )( BT_LOW_ENERGY_MASK | i ) ) );
    }

    for ( i = 0; i < 6; i++ ) {
        EXPECT_NE ( ( const char * )NULL, btle_advert_type_to_str( ( btle_advert_type_t )i ) );
    }

    for ( i = 0; i < 6; i++ ) {
        EXPECT_NE ( ( const char * )NULL, btle_report_type_to_str( ( btle_report_type_t )i ) );
    }

    for ( i = 0; i < 3; i++ ) {
        EXPECT_NE ( ( const char * )NULL, btle_scan_type_to_str( ( btle_scan_type_t )i ) );
    }

    EXPECT_EQ ( ( const char * )NULL, bt_addr_to_str( NULL, NULL, BT_ADDR_STRING_LEN - 1 ) );

    bt_addr_t addr;
    memset( &addr, 1, sizeof( addr ) );
    addr.type = BT_ADDRTYPE_BREDR;
    char buf[128] = {0};
    EXPECT_NE ( ( const char * )NULL, bt_addr_to_str( &addr, buf, sizeof( buf ) ) );
}