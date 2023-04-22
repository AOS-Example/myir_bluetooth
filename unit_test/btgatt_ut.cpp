#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <stdlib.h>
#include "TestSuite.h"
#include "Bluetooth/libbttools/bttools_mock.hpp"
#include "priv/btgatt_impl.hpp"
#include "btgatt.hpp"
#ifdef __cplusplus
extern "C" {
#endif

#include "btgattconn_event.h"
#include "btgatt.h"
#include "btgattconn.h"

#ifdef __cplusplus
}
#endif

using namespace ::testing;
using Bluetooth::Gatt;
using Bluetooth::GattImpl;
using bluetooth::libbttools::unittest::BTToolsMock;

class BtGattTest : public libbttest {
public:
    void SetUp() {
        libbttest::SetUp();
        toolsMock = BTToolsMock::getInstance();
    }
    void TearDown() {
        libbttest::TearDown();
        BTToolsMock::removeInstance();
    }

protected:
    BTToolsMock *toolsMock = nullptr;
};
void btgatt_callback( Gatt *gatt, btgatt_event_t evt, uint8_t *buf, size_t len, void *args ) {
}

void btgatt_cmd_callback( Gatt *gatt, btgatt_cmd_response_t cmd_rsp, uint64_t id, bt_errcode_t errcode, const uint8_t *buf, size_t len, void *args ) {
}

using BtmgrEvtHandler_t = int ( * )( btres_t *btres, int fd, uint16_t msg, uint8_t *buf, size_t len, void *args );
using except_cb_t = int ( * )( btres_t *btres, int fd );

TEST_F( BtGattTest, btgatt_GattEventHandler_test ) {
    GattImpl *gatt = new GattImpl();
    ASSERT_NE( nullptr, gatt );
    bt_addr_t addr{};
    EXPECT_CALL( *resMock, btres_openclient( _, _ ) ).Times( AtLeast( 1 ) ).WillRepeatedly( Return( 1 ) );
    BtmgrEvtHandler_t GattEventHandler = nullptr;
    btres_t *btres_out = nullptr;

    EXPECT_CALL( *resMock, btres_init( _, _, _, _, _ ) )
    .WillOnce( Return( BTRES_RESULT_SUCCESS ) )
    .WillOnce( Invoke( [&GattEventHandler, &btres_out]( btres_t *btres_in, jobq_t *jobq, handle_cmd_cb cmd_cb, handle_evt_cb evt_cb, void *args ) {
        GattEventHandler = evt_cb;
        btres_out = btres_in;
        return BTRES_RESULT_SUCCESS;
    } ) );

    except_cb_t GattExcept = nullptr;
    btres_t *btres_except = nullptr;
    EXPECT_CALL( *resMock, btres_add_conn( _, _, _, _, _ ) )
    .WillOnce( Invoke( [&GattExcept, &btres_except]( btres_t *btres, int parent_fd, int fd, const char *progname, except_cb_t except ) {
        GattExcept = except;
        btres_except = btres;
        return BTRES_RESULT_SUCCESS;
    } ) );

    EXPECT_EQ( BT_STATUS_SUCCESS, gatt->Init( &addr, btgatt_callback, btgatt_cmd_callback, gatt ) );
    ASSERT_NE( nullptr, GattEventHandler );
    ASSERT_NE( nullptr, GattExcept );
    uint16_t msg = BTGATTCONN_EVENT_CONNECTED;
    bt_errcode_t error_code{};

    // Test GattEventHandle
    EXPECT_CALL( *toolsMock, reqlist_remove_req( _, _, _, _, _, _ ) ).WillRepeatedly( Return( 0 ) );
    EXPECT_EQ( 0, GattEventHandler( btres_out, 0, msg, ( uint8_t * )&error_code, sizeof( error_code ), gatt ) );
    msg = BTGATTCONN_EVENT_DISCONNECTED;
    EXPECT_EQ( 0, GattEventHandler( btres_out, 0, msg, ( uint8_t * )&error_code, sizeof( error_code ), gatt ) );

    btgattconn_notification_t notification_buff{};
    msg = BTGATTCONN_EVENT_NOTIFICATION;
    EXPECT_EQ( -1, GattEventHandler( btres_out, 0, msg, ( uint8_t * )&notification_buff, sizeof( notification_buff ), gatt ) );
    msg = BTGATTCONN_EVENT_INDICATION;
    EXPECT_EQ( -1, GattEventHandler( btres_out, 0, msg, ( uint8_t * )&notification_buff, sizeof( notification_buff ), gatt ) );

    btgattconn_read_value_rsp_t rsp{};
    msg = BTGATTCONN_EVENT_READ_VALUE_RSP;
    EXPECT_EQ( -1, GattEventHandler( btres_out, 0, msg, ( uint8_t * )&rsp, sizeof( rsp ), gatt ) );
    msg = BTGATTCONN_EVENT_WRITE_VALUE_RSP;
    EXPECT_EQ( -1, GattEventHandler( btres_out, 0, msg, ( uint8_t * )&rsp, sizeof( rsp ), gatt ) );

    msg = BTGATTCONN_EVENT_CACHE_REFRESHED;
    EXPECT_EQ( -1, GattEventHandler( btres_out, 0, msg, ( uint8_t * )&error_code, sizeof( error_code ), gatt ) );
    msg = BTGATTCONN_EVENT_CACHE_CHANGED;
    EXPECT_EQ( -1, GattEventHandler( btres_out, 0, msg, ( uint8_t * )&error_code, sizeof( error_code ), gatt ) );

    // test GattExcept
    ASSERT_NE( nullptr, btres_except );
    btres_except->args = ( void * )gatt;
    GattExcept( btres_except, 0 );

    if ( gatt != nullptr ) {
        delete gatt;
        gatt = nullptr;
    }
}

TEST_F( BtGattTest, btgatt_Read_Write_test ) {
    GattImpl *gatt = new GattImpl();
    bt_addr_t addr{};
    ASSERT_NE( nullptr, gatt );

    EXPECT_CALL( *resMock, btres_openclient( _, _ ) ).Times( AtLeast( 1 ) ).WillRepeatedly( Return( 1 ) );

    EXPECT_CALL( *resMock, btres_sendcmd( _, _, _, _, _, _, _ ) )
    .WillOnce( Return( BTMGR_STATUS_SUCCESS ) )
    .WillOnce( Return( BT_STATUS_PENDING ) )
    .WillRepeatedly( Return( BTMGR_STATUS_SUCCESS ) );
    EXPECT_EQ( BT_STATUS_PENDING, gatt->Init( &addr, btgatt_callback, btgatt_cmd_callback, gatt ) );

    uint16_t size = 0;
    gatt->GetAttributeListSize( &size );

    btgatt_attribute_t attribute[4] {};
    size = 4;
    attribute[0].attr_type = BTGATT_ATTRIBUTE_SERVICE;
    attribute[1].attr_type = BTGATT_ATTRIBUTE_CHARACTERISTICS;
    attribute[2].attr_type = BTGATT_ATTRIBUTE_INCLUDE_SERVICE;
    attribute[3].attr_type = BTGATT_ATTRIBUTE_DESCRIPTOR;

    EXPECT_CALL( *resMock, btres_sendcmd( _, _, _, _, _, _, _ ) )
    .WillOnce( Invoke( []( btres_t *btres, int fd, uint16_t msg, uint8_t *buf, size_t len, uint8_t *rsp, size_t *rsplen ) {
        btgattconn_attribute_t *attr = ( btgattconn_attribute_t * )rsp;
        attr[0].attr_type = BTGATT_ATTRIBUTE_SERVICE;
        attr[1].attr_type = BTGATT_ATTRIBUTE_CHARACTERISTICS;
        attr[2].attr_type = BTGATT_ATTRIBUTE_INCLUDE_SERVICE;
        attr[3].attr_type = BTGATT_ATTRIBUTE_DESCRIPTOR;
        return BTMGR_STATUS_SUCCESS;
    } ) )
    .WillRepeatedly( Return( BTMGR_STATUS_SUCCESS ) );

    EXPECT_EQ( BT_STATUS_SUCCESS, gatt->GetAttributeList( attribute, &size ) );

    // Test Write Value
    uint8_t data[5] {};
    EXPECT_EQ( BT_STATUS_SUCCESS, gatt->WriteValue( 0, 0, 0, sizeof( data ), data ) );

    // Test Read Value
    EXPECT_EQ( BT_STATUS_SUCCESS, gatt->ReadValue( 0, 0, 0 ) );

    if ( gatt != nullptr ) {
        delete gatt;
        gatt = nullptr;
    }
}