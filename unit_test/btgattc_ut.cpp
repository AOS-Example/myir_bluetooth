#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <stdlib.h>

#include "bt.h"
#include "btgattclt_event.h"
#include "btgattclt_priv.h"
#include "btresmgr.h"
#include "mocks/Bluetooth/libbluetooth/btgattclt_mock.hpp"
#include "Bluetooth/libbttools/bttools_mock.hpp"
#include "priv/btgattc_impl.hpp"
#include "reqlist.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "btgattc.h"
#include "priv/btgattc_priv.h"
#include "stubs.h"
#ifdef __cplusplus
}
#endif

#include "TestSuite.h"

using namespace ::testing;
using Bluetooth::GattcImpl;
using bluetooth::libbluetooth::unittest::BtgattcltMock;

typedef struct _btgattc_conn_t {
    node_t node;

    btres_t btres;

    int fd;

    btgattc_callback_t cb;
    btgattc_cmd_callback_t cmd_cb;

    void *args;

    uint64_t id; /* ID of the Connect Request */

    reqlist_t reqlist;

    btgattc_service_inst_t connect;

    GattcImpl *parent;
} btgattc_conn_t;

void btgattc_callback( btgattc_instance_t instance, btgattc_event_t evt, uint8_t *buf, size_t len, void *args ) {
}

void btgattc_cmd_callback( btgattc_instance_t instance, btgattc_cmd_response_t cmd_rsp, uint64_t id,
                           bt_errcode_t errcode, const uint8_t *buf, size_t len, void *args ) {
}

TEST_F( libbttest, btgattc_init_deinit_test ) {
    EXPECT_EQ( BT_STATUS_SUCCESS, btgattc_init() );
    EXPECT_EQ( BT_STATUS_IN_USE, btgattc_init() );
    EXPECT_EQ( BT_STATUS_SUCCESS, btgattc_deinit() );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btgattc_deinit() );

    EXPECT_CALL( *resMock, btres_openclient( _, _ ) ).WillOnce( Return( -1 ) );
    EXPECT_THAT( btgattc_init(), Eq( BT_STATUS_FAILED ) );
}

TEST_F( libbttest, btgattc_except_test ) {
    /* I don't know how to fix this problem.  The problem is that we are
     * not properly using the btres_init for the BtgattcImpl so that args
     * field is not populated as it is in normal production.  Because of this
     * we crash when we start using that in the except call
     */
#if 0
    int ( *exceptFunc ) ( btres_t *, int );
    btres_t *btres;
    BTResMock *btrestMock = BTResMock::getInstance();
    EXPECT_CALL( *btrestMock, btres_openclient( _, _ ) ).Times( 1 )
    .WillOnce( Return( 1 ) );
    EXPECT_CALL( *btrestMock, btres_register_events( _, _, _ ) ).Times( 1 )
    .WillOnce( Return( 0 ) );
    EXPECT_CALL( *btrestMock, btres_add_conn( _, _, _, _, _ ) ).Times( 1 )
    .WillOnce( DoAll( SaveArg<0>( &btres ),
                      SaveArg<4>( &exceptFunc ),
                      Return( BTRES_RESULT_SUCCESS ) ) );
    EXPECT_EQ( BT_STATUS_SUCCESS, btgattc_init() );
    exceptFunc( btres, 1 );
    btgattc_deinit();
    BTResMock::removeInstance();
#endif
}

TEST_F( libbttest, btgattc_disconnect_fail_test ) {
    btgattc_conn_t conn = { 0 };
    conn.fd = -1;

    bt_status_t status = btgattc_disconnect( static_cast<btgattc_instance_t>( &conn ) );
    EXPECT_EQ( status, BT_STATUS_NO_TRANSPORT );
}

TEST_F( libbttest, btgattc_connect_service_test ) {
    bt_status_t status = btgattc_connect_service( 0, NULL, 0, NULL, NULL, NULL, NULL );
    EXPECT_EQ( status, BT_STATUS_NO_TRANSPORT );
}

TEST_F( libbttest, btgattc_connect_service_handle_test ) {
    bt_status_t status = btgattc_connect_service_handle( 0, NULL, 0, NULL, NULL, NULL, NULL );
    EXPECT_EQ( status, BT_STATUS_NO_TRANSPORT );
}

TEST_F( libbttest, btgattc_read_value_no_transport_test ) {
    btgattc_conn_t conn = { 0 };
    conn.fd = -1;

    bt_status_t status = btgattc_read_value( static_cast<btgattc_instance_t>( &conn ), 0, 0, 0 );
    EXPECT_EQ( status, BT_STATUS_NO_TRANSPORT );
}

TEST_F( libbttest, btgattc_write_value_no_transport_test ) {
    btgattc_conn_t conn = { 0 };
    conn.fd = -1;

    bt_status_t status = btgattc_write_value( static_cast<btgattc_instance_t>( &conn ), 0, 0, 0, nullptr );
    EXPECT_EQ( status, BT_STATUS_NO_TRANSPORT );
}

TEST_F( libbttest, btgattc_get_attribs_no_transport_test ) {
    btgattc_conn_t conn = { 0 };
    conn.fd = -1;

    bt_status_t status = btgattc_get_attribs( static_cast<btgattc_instance_t>( &conn ), nullptr, nullptr );
    EXPECT_EQ( status, BT_STATUS_NO_TRANSPORT );
}

TEST_F( libbttest, btgattc_get_info_no_transport_test ) {
    btgattc_conn_t conn = { 0 };
    conn.fd = -1;

    bt_status_t status = btgattc_get_info( static_cast<btgattc_instance_t>( &conn ), nullptr );
    EXPECT_EQ( status, BT_STATUS_NO_TRANSPORT );
}
using BtmgrEvtHandler_t = int ( * )( btres_t *btres, int fd, uint16_t msg, uint8_t *buf, size_t len, void *args );
using bluetooth::libbttools::unittest::BTToolsMock;
class LibbtBtgattcConnectionTest : public libbttest {
public:
    void SetUp() {
        libbttest::SetUp();
        toolsMock = BTToolsMock::getInstance();
        ON_CALL( *resMock, btres_openclient( _, _ ) ).WillByDefault( Return( 1 ) );

        EXPECT_EQ( BT_STATUS_SUCCESS, btgattc_init() );
        btgattclt = BtgattcltMock::getInstance();
        mInstance = nullptr;
    }

    bt_status_t connectToService() {
        uint64_t id = 1111;
        bt_addr_t addr = { 0 };
        addr.type = BT_ADDRTYPE_BREDR;
        bt_uuid_t uuid = { 0 };
        uuid.len = 32;

        return btgattc_connect_service( id, &addr, &uuid, btgattc_callback, btgattc_cmd_callback, nullptr, &mInstance );
    }

    void createValidConnectionToService() {
        EXPECT_CALL( *resMock, btres_sendcmd( _, _, BTGATTC_CMD_CONNECT_SERVICE, _, _, _, _ ) )
        .WillOnce( Return( BT_STATUS_PENDING ) )
        .WillRepeatedly( Return( BT_STATUS_SUCCESS ) );

        ASSERT_EQ( BT_STATUS_PENDING, connectToService() );
        ASSERT_NE( nullptr, mInstance );
        ( ( btgattc_conn_t * )mInstance )->fd = 1;
    }

    void TearDown() {
        if ( mInstance ) {
            btgattc_disconnect( mInstance );
        }

        BtgattcltMock::removeInstance();
        BTResMock::removeInstance();
        EXPECT_EQ( BT_STATUS_SUCCESS, btgattc_deinit() );
        libbttest::TearDown();
        BTToolsMock::removeInstance();
    }

    btgattc_instance_t mInstance = nullptr;

protected:
    BtgattcltMock *btgattclt = nullptr;
    BTToolsMock *toolsMock = nullptr;
};

#ifdef __QNX__
TEST_F( libbttest, btgattc_main_event_handler_test ) {
    btres_t *btres;
    int result = 0;
    btgattc_instance_t instance = nullptr;
    EXPECT_CALL( *resMock, btres_init( _, _, _, _, _ ) )
    .WillOnce( Invoke( [&btres, &instance]( btres_t *btres_in, jobq_t *jobq, handle_cmd_cb cmd_cb,
    handle_evt_cb evt_cb, void *args ) {
        btres = btres_in;
        instance = ( btgattc_instance_t )args;
        return BTRES_RESULT_SUCCESS;
    } ) )
    .WillRepeatedly( Return( 0 ) );
    EXPECT_CALL( *resMock, btres_openclient( _, _ ) ).WillOnce( Return( 1 ) );
    EXPECT_EQ( BT_STATUS_SUCCESS, btgattc_init() );
    EXPECT_NE( nullptr, instance );

    uint64_t id = reinterpret_cast<size_t>( instance );
    bt_addr_t addr = { 0 };
    addr.type = BT_ADDRTYPE_BREDR;
    ON_CALL( *resMock, btres_sendcmd( _, _, BTGATTC_CMD_CONNECT_SERVICE, _, _, _, _ ) ).WillByDefault( Return( BT_STATUS_PENDING ) );
    EXPECT_EQ( BT_STATUS_PENDING, btgattc_connect_service_handle( id, &addr, 0, btgattc_callback, btgattc_cmd_callback,
               NULL, &instance ) );
    bt_command_rsp_t rsp{};

    rsp.id = reinterpret_cast<size_t>( instance );
    rsp.cmd = BTGATTC_RSP_CONNECT_SERVICE;
    rsp.errcode = BT_ERRCODE_NO_ERROR;
    result = btres->evt_cb( btres, 1, BT_EVENT_COMMAND_RESPONSE, reinterpret_cast<uint8_t *>( &rsp ), sizeof( rsp ), instance );
    EXPECT_EQ( result, 0 );
    EXPECT_EQ( BT_STATUS_SUCCESS, btgattc_disconnect( instance ) );
    EXPECT_EQ( BT_STATUS_SUCCESS, btgattc_deinit() );
}
#endif

TEST_F( LibbtBtgattcConnectionTest, btgattc_connect_service_handle_fail_test ) {
    EXPECT_CALL( *resMock, btres_sendcmd( _, _, _, _, _, _, _ ) ).Times( 1 ).WillOnce( Return( BT_STATUS_FAILED ) );
    EXPECT_EQ( BT_STATUS_FAILED, connectToService() );
}

TEST_F( LibbtBtgattcConnectionTest, btgattc_event_handler_command_response_default_test ) {
    EXPECT_CALL( *resMock, btres_sendcmd( _, _, _, _, _, _, _ ) ).WillRepeatedly( Return( BT_STATUS_PENDING ) );
    ASSERT_EQ( BT_STATUS_PENDING, connectToService() );
    btgattc_conn_t *conn = static_cast<btgattc_conn_t *>( mInstance );
    int result = conn->btres.evt_cb( &( conn->btres ), conn->fd, BTGATTCLT_EVENT_NOTIFICATION, nullptr, 0, conn );
    EXPECT_EQ( 0, result );
}

TEST_F( LibbtBtgattcConnectionTest, btgattc_event_handler_command_response_open_client_fail_test ) {
    EXPECT_CALL( *resMock, btres_sendcmd( _, _, _, _, _, _, _ ) ).Times( 1 ).WillOnce( Return( BT_STATUS_PENDING ) );

    ASSERT_EQ( BT_STATUS_PENDING, connectToService() );
    btgattc_conn_t *conn = static_cast<btgattc_conn_t *>( mInstance );
    bt_command_rsp_t rsp = { 0 };
    rsp.cmd = BTGATTC_RSP_CONNECT_SERVICE;
    rsp.errcode = BT_ERRCODE_NO_ERROR;

    int result = conn->btres.evt_cb( &( conn->btres ), conn->fd, BT_EVENT_COMMAND_RESPONSE,
                                     reinterpret_cast<uint8_t *>( &rsp ), 0, conn );
    /* Note: conn is freed inside the SUT */
    conn = nullptr;
    mInstance = nullptr;

    EXPECT_EQ( 0, result );
}

TEST_F( LibbtBtgattcConnectionTest, btgattc_event_handler_command_response_disconnect_test ) {
    EXPECT_CALL( *resMock, btres_sendcmd( _, _, _, _, _, _, _ ) ).WillRepeatedly( Return( BT_STATUS_PENDING ) );

    ASSERT_EQ( BT_STATUS_PENDING, connectToService() );

    btgattc_conn_t *conn = static_cast<btgattc_conn_t *>( mInstance );
    EXPECT_CALL( *resMock, btres_closeclient( _, _ ) ).Times( AtMost( 1 ) );

    int result = conn->btres.evt_cb( &( conn->btres ), conn->fd, BTGATTCLT_EVENT_DISCONNECTED, nullptr, 0, conn );
    /* Note: conn is freed inside the SUT */
    conn = nullptr;
    mInstance = nullptr;

    EXPECT_EQ( 0, result );
}

TEST_F( LibbtBtgattcConnectionTest, btgattc_get_info_test ) {
    createValidConnectionToService();

    EXPECT_CALL( *btgattclt, getInfo( _, _, _ ) ).Times( 1 ).WillOnce( Return( BTMGR_STATUS_SUCCESS ) );

    btgattc_info_t info;
    EXPECT_EQ( BT_STATUS_SUCCESS, btgattc_get_info( mInstance, &info ) );
}

TEST_F( LibbtBtgattcConnectionTest, btgattc_get_attribs_test ) {
    createValidConnectionToService();

    btgattclt_attribs_t srtAttrs[4] = { 0 };
    btgattc_attribute_t dstAttrs[4] = { 0 };
    srtAttrs[0].attr_type = BTGATTC_ATTRIBUTE_SERVICE;
    srtAttrs[1].attr_type = BTGATTC_ATTRIBUTE_CHARACTERISTICS;
    srtAttrs[2].attr_type = BTGATTC_ATTRIBUTE_DESCRIPTOR;
    srtAttrs[3].attr_type = BTGATTC_ATTRIBUTE_INCLUDE_SERVICE;
    size_t cnt = 4;

    EXPECT_CALL( *btgattclt, getAttribs( _, _, _, _ ) )
    .Times( 1 )
    .WillOnce( DoAll( SetArrayArgument<2>( srtAttrs, &srtAttrs[4] ), Return( BT_STATUS_SUCCESS ) ) );

    EXPECT_EQ( BT_STATUS_SUCCESS, btgattc_get_attribs( mInstance, dstAttrs, &cnt ) );
}

TEST_F( LibbtBtgattcConnectionTest, btgattc_connect_service_handle_success ) {
    uint64_t id = 1111;
    bt_addr_t addr = { 0 };
    addr.type = BT_ADDRTYPE_BREDR;

    bt_status_t status =
        btgattc_connect_service_handle( id, &addr, 0, btgattc_callback, btgattc_cmd_callback, NULL, &mInstance );
    EXPECT_EQ( status, BT_STATUS_SUCCESS );
}

TEST_F( LibbtBtgattcConnectionTest, btgattc_read_value_test ) {
    uint64_t id = 10;
    uint16_t handle = 20;
    uint16_t offset = 0;

    EXPECT_EQ( BT_STATUS_NO_CONNECTION, btgattc_read_value( mInstance, id, handle, offset ) );

    createValidConnectionToService();
    EXPECT_EQ( BT_STATUS_SUCCESS, btgattc_read_value( mInstance, id, handle, offset ) );

    btgattc_conn_t *conn = static_cast<btgattc_conn_t *>( mInstance );
    btgattclt_read_value_rsp_t rsp = { 0 };
    rsp.handle = handle;
    rsp.len = 0;
    rsp.offset = 0;
    EXPECT_CALL( *toolsMock, reqlist_remove_req( _, _, _, _, _, _ ) ).WillRepeatedly( Return( 0 ) );
    int result = conn->btres.evt_cb( &( conn->btres ), conn->fd, BTGATTCLT_EVENT_READ_VALUE_RSP,
                                     reinterpret_cast<uint8_t *>( &rsp ), 0, conn );
    EXPECT_EQ( 0, result );
}

TEST_F( LibbtBtgattcConnectionTest, btgattc_write_value_test ) {
    uint64_t id = 11;
    uint16_t handle = 21;
    uint8_t data[] = { 't', 'e', 's', 't' };
    uint16_t len = sizeof( data );

    EXPECT_EQ( BT_STATUS_NO_CONNECTION, btgattc_write_value( mInstance, id, handle, len, data ) );

    createValidConnectionToService();
    EXPECT_EQ( BT_STATUS_SUCCESS, btgattc_write_value( mInstance, id, handle, len, data ) );

    btgattc_conn_t *conn = static_cast<btgattc_conn_t *>( mInstance );
    btgattclt_write_value_rsp_t rsp = { 0 };
    rsp.handle = handle;
    rsp.errcode = BT_ERRCODE_NO_ERROR;

    int result = conn->btres.evt_cb( &( conn->btres ), conn->fd, BTGATTCLT_EVENT_WRITE_VALUE_RSP,
                                     reinterpret_cast<uint8_t *>( &rsp ), 0, conn );
    EXPECT_EQ( 0, result );
}

TEST_F( LibbtBtgattcConnectionTest, DISABLED_btgattc_conn_except_test ) {
    int ( *exceptFunc )( btres_t *, int );
    EXPECT_CALL( *resMock, btres_add_conn( _, _, _, _, _ ) )
    .Times( 1 )
    .WillOnce( DoAll( SaveArg<4>( &exceptFunc ), Return( BTRES_RESULT_SUCCESS ) ) );

    createValidConnectionToService();
    btgattc_conn_t *conn = static_cast<btgattc_conn_t *>( mInstance );
    exceptFunc( &( conn->btres ), 1 );
    /* Note: conn is freed inside the SUT */
    conn = nullptr;
    mInstance = nullptr;
}