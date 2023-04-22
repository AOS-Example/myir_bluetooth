#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <stdlib.h>

#include "Bluetooth/libbluetooth/bluetooth_mock.hpp"
#include "bt.h"
#include "btresmgr.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "btgatts.h"
#include "btgattsrv_event.h"
#include "priv/btgatts_priv.h"
#include "stubs.h"
#ifdef __cplusplus
}
#endif

#include "TestSuite.h"

using namespace ::testing;
using bluetooth::libbluetooth::unittest::BluetoothMock;

void btgatts_callback( btgatts_instance_t *instance, btgatts_event_t evt, uint8_t *buf, size_t len, void *args ) {
}

void btgatts_cmd_callback( btgatts_instance_t *instance, btgatts_cmd_response_t cmd_rsp, uint64_t id,
                           bt_errcode_t errcode, const uint8_t *buf, size_t len, void *args ) {
}

TEST_F ( libbttest, btgatts_init_deinit_test ) {
    EXPECT_EQ( BT_STATUS_SUCCESS, btgatts_init() );
    EXPECT_EQ( BT_STATUS_IN_USE, btgatts_init() );
    EXPECT_EQ( BT_STATUS_SUCCESS, btgatts_deinit() );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btgatts_deinit() );
}

TEST_F( libbttest, btgatts_evt_handler_tests ) {
    EXPECT_EQ( 0, btgatts_init() );

    btgatts_attribute_t attribs[5] = { 0 };
    attribs[0].attr_type = BTGATTS_ATTRIBUTE_SERVICE;
    attribs[1].attr_type = BTGATTS_ATTRIBUTE_CHARACTERISTICS;
    attribs[2].attr_type = BTGATTS_ATTRIBUTE_DESCRIPTOR;
    attribs[3].attr_type = BTGATTS_ATTRIBUTE_INCLUDE_SERVICE;
    size_t cnt = 5;
    btgatts_instance_t handle{};
    btgatts_register_service( attribs, cnt, btgatts_callback, btgatts_cmd_callback, &handle, NULL );

    btgatts_service_t *instance = ( btgatts_service_t * )handle;
    btaddr_t btaddr = { 0 };

    if ( instance->btres.evt_cb ) {
        // static int gatts_event_handler (btres_t * btres, int fd, uint16_t msg, uint8_t * buf, size_t len, void *args)
        EXPECT_EQ( 0, instance->btres.evt_cb( &instance->btres, 1, BTGATTSRV_EVENT_CONNECTED, ( uint8_t * )&btaddr,
                                              sizeof( btaddr ), ( void * )instance ) );
        EXPECT_EQ( 0, instance->btres.evt_cb( &instance->btres, 1, BTGATTSRV_EVENT_DISCONNECTED, ( uint8_t * )&btaddr,
                                              sizeof( btaddr ), ( void * )instance ) );
        EXPECT_EQ( 0, instance->btres.evt_cb( &instance->btres, 1, BTGATTSRV_EVENT_READ_VALUE_REQ, ( uint8_t * )&btaddr,
                                              sizeof( btaddr ), ( void * )instance ) );
        EXPECT_EQ( 0, instance->btres.evt_cb( &instance->btres, 1, BTGATTSRV_EVENT_READ_LENGTH_REQ,
                                              ( uint8_t * )&btaddr, sizeof( btaddr ), ( void * )instance ) );
        EXPECT_EQ( 0, instance->btres.evt_cb( &instance->btres, 1, BTGATTSRV_EVENT_WRITE_VALUE, ( uint8_t * )&btaddr,
                                              sizeof( btaddr ), ( void * )instance ) );
        EXPECT_EQ( 0, instance->btres.evt_cb( &instance->btres, 1, BTGATTSRV_EVENT_WRITE_VALUE_REQ,
                                              ( uint8_t * )&btaddr, sizeof( btaddr ), ( void * )instance ) );
        EXPECT_EQ( 0, instance->btres.evt_cb( &instance->btres, 1, BTGATTSRV_EVENT_INDICATION_RSP, ( uint8_t * )&btaddr,
                                              sizeof( btaddr ), ( void * )instance ) );
    }

    btgatts_deregister_service( handle );

    EXPECT_EQ( 0, btgatts_deinit() );
}

TEST_F( libbttest, btgatts_call_tests_fail ) {
    btgatts_attribute_t attribs[5] = { 0 };
    size_t cnt = 5;
    btgatts_instance_t handle;
    bt_addr_t btaddr = { 0 };

    EXPECT_EQ( BT_STATUS_NO_TRANSPORT,
               btgatts_register_service( attribs, cnt, btgatts_callback, btgatts_cmd_callback,
                                         &handle, NULL ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btgatts_write_value_rsp( handle, &btaddr, 1, BT_ERRCODE_NO_ERROR ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btgatts_send_notification( handle, &btaddr, 1, 0, NULL ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btgatts_send_indication_req( handle, 1, &btaddr, 1, 0, NULL ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btgatts_read_length_rsp( handle, &btaddr, 1, 0 ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btgatts_read_value_rsp( handle, &btaddr, 1, BT_ERRCODE_NO_ERROR, 0, NULL ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btgatts_deregister_service( handle ) );
}

TEST_F( libbttest, btgatts_call_tests ) {
    EXPECT_EQ( 0, btgatts_init() );

    btgatts_attribute_t attribs[5] = { 0 };
    attribs[0].attr_type = BTGATTS_ATTRIBUTE_SERVICE;
    attribs[1].attr_type = BTGATTS_ATTRIBUTE_CHARACTERISTICS;
    attribs[2].attr_type = BTGATTS_ATTRIBUTE_DESCRIPTOR;
    attribs[3].attr_type = BTGATTS_ATTRIBUTE_INCLUDE_SERVICE;
    size_t cnt = 5;
    btgatts_instance_t handle;
    btgatts_register_service( attribs, cnt, btgatts_callback, btgatts_cmd_callback, &handle, NULL );
    bt_addr_t btaddr = { 0 };

    BluetoothMock *bluetoothMock = BluetoothMock::getInstance();
    EXPECT_CALL( *bluetoothMock, btgattsrv_write_value_rsp( _, _, _, _, _ ) ).WillOnce( Return( 0 ) );
    EXPECT_CALL( *bluetoothMock, btgattsrv_send_notification( _, _, _, _, _, _ ) ).WillOnce( Return( 0 ) );
    EXPECT_CALL( *bluetoothMock, btgattsrv_send_indication_req( _, _, _, _, _, _, _ ) ).WillOnce( Return( 0 ) );
    EXPECT_CALL( *bluetoothMock, btgattsrv_read_value_rsp( _, _, _, _, _, _, _ ) ).WillOnce( Return( 0 ) );
    EXPECT_CALL( *bluetoothMock, btgattsrv_read_length_rsp( _, _, _, _, _ ) ).WillOnce( Return( 0 ) );

    EXPECT_EQ( 0, btgatts_write_value_rsp( handle, &btaddr, 1, BT_ERRCODE_NO_ERROR ) );
    EXPECT_EQ( 0, btgatts_send_notification( handle, &btaddr, 1, 0, NULL ) );
    EXPECT_EQ( 0, btgatts_send_indication_req( handle, 1, &btaddr, 1, 0, NULL ) );
    EXPECT_EQ( 0, btgatts_read_length_rsp( handle, &btaddr, 1, 0 ) );
    EXPECT_EQ( 0, btgatts_read_value_rsp( handle, &btaddr, 1, BT_ERRCODE_NO_ERROR, 0, NULL ) );

    btgatts_deregister_service( handle );
    EXPECT_EQ( 0, btgatts_deinit() );
    BluetoothMock::removeInstance();
}

using except_cb_t = int ( * )( btres_t *btres, int fd );

TEST_F( libbttest, Test_btgatts_except ) {
    btres_t *btres = nullptr;

    except_cb_t btgatts_except = nullptr;
    EXPECT_CALL( *resMock, btres_add_conn( _, _, _, _, _ ) )
    .WillOnce( Invoke( [&btgatts_except, &btres]( btres_t *btres_in, int parent_fd, int fd, const char *progname,
    except_cb_t except ) {
        btgatts_except = except;
        btres = btres_in;
        return BTRES_RESULT_SUCCESS;
    } ) );

    EXPECT_EQ( 0, btgatts_init( ) ); // **
    btgatts_attribute_t attribs[5] = { 0 };
    attribs[0].attr_type = BTGATTS_ATTRIBUTE_SERVICE;
    attribs[1].attr_type = BTGATTS_ATTRIBUTE_CHARACTERISTICS;
    attribs[2].attr_type = BTGATTS_ATTRIBUTE_DESCRIPTOR;
    attribs[3].attr_type = BTGATTS_ATTRIBUTE_INCLUDE_SERVICE;
    size_t cnt = 5;
    btgatts_instance_t handle{};

    ASSERT_THAT( btgatts_register_service( attribs, cnt, btgatts_callback, btgatts_cmd_callback,
                                           &handle, NULL ),
                 Eq( BT_STATUS_SUCCESS ) );

    ASSERT_THAT( btgatts_except, Ne( nullptr ) );

    EXPECT_THAT(
        btgatts_except( btres, 0 ),
        Eq( 0 ) );  // **

    EXPECT_EQ( 0, btgatts_deinit() );
}