#include "TestSuite.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>

#include "bt_common.h"
#include "btphone.h"
#include "priv/btphone_v0_priv.h"

void btphone_callback( btphone_event_t evt, uint8_t *buf, size_t len, void *args ) {
}

#ifdef __cplusplus
}
#endif

#ifndef strlcpy
#define strlcpy(d, s, size) \
    do { \
        strncpy(d, s, size); \
        d[size-1] = 0; \
    } while (0)
#endif

TEST_F( libbttest, bt_v0_uuid_test ) {
    bt_uuid_t uuid = { 0 };
    bt_uuid_v0_t uuid_v0 = { 0 };

    uuid.uuid128[0] = 1;
    uuid.uuid128[1] = 2;
    uuid.uuid128[2] = 3;
    uuid.uuid128[3] = 4;
    uuid.uuid128[4] = 5;
    uuid.len = 5;

    bt_v0_uuid( &uuid, &uuid_v0 );

    EXPECT_EQ( 5, uuid_v0.len );
    EXPECT_EQ( 5, uuid_v0.uuid128[4] );
    EXPECT_EQ( 2, uuid_v0.uuid128[1] );
}

TEST_F( libbttest, btphone_v0_contact_test ) {
    btphone_contact_t contact = { 0 };
    btphone_contact_v0_t contact_v0 = { 0 };

    contact.id = 1;

    strlcpy( contact.name.first, "Name Test First", 16 );
    strlcpy( contact.name.last, "Name Test Last", 15 );

    contact.date.year = 2020;
    contact.date.month = 9;
    contact.date.day = 30;

    btphone_v0_contact( &contact, &contact_v0 );

    EXPECT_EQ( 1, contact_v0.id );
    EXPECT_STREQ( "Name Test First", contact_v0.name.first );
    EXPECT_STREQ( "Name Test Last", contact_v0.name.last );
    EXPECT_EQ( 2020, contact_v0.date.year );
    EXPECT_EQ( 9, contact_v0.date.month );
    EXPECT_EQ( 30, contact_v0.date.day );
}

TEST_F( libbttest, btphone_v0_pbk_progress_test ) {
    btphone_pbk_progress_t progress = { 0 };
    btphone_pbk_progress_v0_t progress_v0 = { 0 };

    progress.addr.handle = 1;
    progress.pbk_idx = BTPHONE_PBK_INDEX_LOCAL;
    progress.size = 32;
    progress.dl = 30;

    btphone_v0_pbk_progress( &progress, &progress_v0 );

    EXPECT_EQ( 1, progress_v0.addr.handle );
    EXPECT_EQ( BTPHONE_PBK_INDEX_LOCAL, progress_v0.pbk_idx );
    EXPECT_EQ( 32, progress_v0.size );
    EXPECT_EQ( 30, progress_v0.dl );
}

TEST_F( libbttest, btphone_v0_contact_name_test ) {
    btphone_contact_name_t name = { 0 };
    btphone_contact_name_v0_t name_v0 = { 0 };

    strlcpy( name.firstname, "FirstName", 10 );
    strlcpy( name.name, "Name", 5 );

    btphone_v0_contact_name( &name, &name_v0 );

    EXPECT_STREQ( "FirstName", name_v0.firstname );
    EXPECT_STREQ( "Name", name_v0.name );
}

TEST_F( libbttest, btphone_v0_device_test ) {
    btphone_device_t device{};
    btphone_device_v0_t device_v0 = { 0 };

    device.brsf.threeway = 1;
    device.chld.release_held = 1;
    device.signal = 95;
    device.held = 50;
    device.pb_dl[1] = 35;

    btphone_v0_device( &device, &device_v0 );

    EXPECT_EQ( device_v0.brsf.threeway, 1 );
    EXPECT_EQ( device_v0.chld.release_held, 1 );
    EXPECT_EQ( device_v0.signal, 95 );
    EXPECT_EQ( device_v0.held, 50 );
    EXPECT_EQ( device_v0.pb_dl[1], 35 );
}

TEST_F( libbttest, btphone_v0_call_test ) {
    btphone_call_t call = { 0 };
    btphone_call_v0_t call_v0 = { 0 };

    call.idx = 1;
    call.addr.handle = 2;

    strlcpy( call.number, "0123456789", 11 );
    strlcpy( call.name, "Name", 5 );
    strlcpy( call.firstname, "First Name", 11 );

    btphone_v0_call( &call, &call_v0 );

    EXPECT_EQ( 1, call_v0.idx );
    EXPECT_EQ( 2, call_v0.addr.handle );
    EXPECT_STREQ( "0123456789", call_v0.number );
    EXPECT_STREQ( "Name", call_v0.name );
    EXPECT_STREQ( "First Name", call_v0.firstname );
}

TEST_F( libbttest, btphone_v0_call_indicator_test ) {
    btphone_call_indicator_t indicator{};
    btphone_call_indicator_v0_t indicator_v0{};

    indicator.setup = BTPHONE_CALL_SETUP_INCOMING;
    indicator.call = BTPHONE_CALL_ACTIVE;
    indicator.held = BTPHONE_CALL_HELD;

    btphone_v0_call_indicator( &indicator, &indicator_v0 );

    EXPECT_EQ( indicator_v0.setup, BTPHONE_CALL_SETUP_INCOMING );
    EXPECT_EQ( indicator_v0.call, BTPHONE_CALL_ACTIVE );
    EXPECT_EQ( indicator_v0.held, BTPHONE_CALL_HELD );
}

TEST_F( libbttest, btphone_v0_callback_test ) {
    btphone_call_indicator_t indicator{};

    btphone_v0_callback( btphone_callback, BTPHONE_EVENT_CALL_HELD, ( uint8_t * )&indicator, 0, NULL );
    btphone_v0_callback( btphone_callback, BTPHONE_EVENT_DISCONNECTED, ( uint8_t * )&indicator, 0, NULL );
    btphone_v0_callback( btphone_callback, BTPHONE_EVENT_CONNECTED, ( uint8_t * )&indicator, 0, NULL );
}

void btphone_callback( btphone_event_t evt, uint8_t *buf, size_t len, void *args );

void btphone_cmd_callback( btphone_cmd_response_t cmd_rsp, uint64_t id, bt_errcode_t errcode,
                           const uint8_t *buf, size_t len, void *args );

#undef btphone_init // This make btphone_init init V0; Please keep this test at the bottom of the file

TEST_F( libbttest, btphone_v0_calls_success ) {
    btphone_init( btphone_callback, btphone_cmd_callback, NULL );

    EXPECT_EQ( BT_STATUS_SUCCESS, btphone_hangup( 0 ) );

    EXPECT_EQ( BT_STATUS_SUCCESS, btphone_hangup_index( 0, 0 ) );

    EXPECT_EQ( BT_STATUS_SUCCESS, btphone_hangup_all( 0 ) );

    EXPECT_EQ( BT_STATUS_SUCCESS, btphone_answer( 0 ) );

    EXPECT_EQ( BT_STATUS_SUCCESS, btphone_reject( 0 ) );

    bt_addr_t addr;
    memset( &addr, 0, sizeof( addr ) );
    addr.type = BT_ADDRTYPE_BREDR;
    EXPECT_EQ( BT_STATUS_SUCCESS, btphone_dial( 0, &addr, "123-123-123" ) );

    EXPECT_EQ( BT_STATUS_SUCCESS, btphone_swap_calls( 0 ) );

    EXPECT_EQ( BT_STATUS_SUCCESS, btphone_merge_calls( 0 ) );

    EXPECT_EQ( BT_STATUS_SUCCESS, btphone_redial( 0 ) );

    EXPECT_EQ( BT_STATUS_SUCCESS, btphone_voice_recognition( 0, &addr, TRUE ) );

    EXPECT_EQ( BT_STATUS_SUCCESS, btphone_send_dtmf( 0, '5' ) );

    EXPECT_EQ( BT_STATUS_SUCCESS, btphone_audio( 0, TRUE ) );

    EXPECT_EQ( BT_STATUS_SUCCESS, btphone_emergency_dial( 0, "999-999-999" ) );

    bt_addr_t addrs[10];
    memset( addrs, 0, sizeof( addrs ) );

    uint8_t cnt = ( uint8_t )( sizeof( addrs ) / sizeof( addrs[0] ) );
    EXPECT_EQ( BT_STATUS_SUCCESS, btphone_get_connected_list( addrs, &cnt ) );

    btphone_local_state_t state{};
    EXPECT_EQ( BT_STATUS_SUCCESS, btphone_get_local_state( &state ) );

    btphone_device_t info{};
    EXPECT_EQ( BT_STATUS_SUCCESS, btphone_get_device_info( &addr, &info ) );

    EXPECT_EQ( BT_STATUS_SUCCESS, btphone_download_contacts( 0, &addr ) );

    btphone_contact_sort_t sort = BTPHONE_CONTACT_SORT_ID;
    uint16_t offset = 0;
    uint16_t count = 100;
    btphone_contact_filter_mask_t filter = BTPHONE_CONTACT_FILTER_MASK_NO_FILTER;
    addr.type = BT_ADDRTYPE_BREDR;
    btphone_pbk_index_t pbk = ( btphone_pbk_index_t )3;
    btphone_contact_name_t *contacts = ( btphone_contact_name_t * )calloc( count, sizeof( btphone_contact_name_t ) );
    uint16_t cnt16 = 0;
    EXPECT_EQ( BT_STATUS_SUCCESS, btphone_get_contact_list( &addr, pbk, sort, filter, offset, contacts, &cnt16 ) );

    btphone_smart_search_t smart;
    memset( &smart, 0, sizeof( smart ) );
    EXPECT_EQ( BT_STATUS_SUCCESS, btphone_get_search_info( 0, &addr, pbk, sort, filter, "111", &smart ) );

    EXPECT_EQ( BT_STATUS_SUCCESS, btphone_search( 0, &addr, pbk, sort, filter, offset, "111", contacts, &cnt16 ) );

    free( contacts );

    int32_t id = 6;
    btphone_contact_v0_t contact{};
    EXPECT_EQ( BT_STATUS_SUCCESS,
               btphone_get_contact( &addr, pbk, id, ( btphone_contact_t * )&contact ) );

    btphone_deinit();
}