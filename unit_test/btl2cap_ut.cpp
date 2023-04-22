#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <stdlib.h>

#include "TestSuite.h"

#include "btl2cap.h"
#include "btl2cap.hpp"
#include "priv/btl2cap_priv.h"
#include "btl2c_event.h"

#include "priv/btl2cap_impl.hpp"

using namespace ::testing;
using Bluetooth::L2cap;
using Bluetooth::L2capImpl;

void btl2cap_callback( btl2cap_event_t evt, uint8_t *buf, size_t len, void *args ) {
}

void btl2cap_cmd_callback( btl2cap_cmd_response_t cmd_rsp, uint64_t id,
                           bt_errcode_t errcode, const uint8_t *buf, size_t len, void *args ) {
}

TEST_F( libbttest, DISABLED_btl2cap_int_tests ) {
    EXPECT_EQ( 0, btl2cap_init() );
    EXPECT_EQ( BT_STATUS_IN_USE, btl2cap_init() );

    EXPECT_EQ( 0, btl2cap_deinit() );
    EXPECT_EQ( BT_STATUS_FAILED, btl2cap_deinit() );

    L2capImpl l2capImpl{};

    EXPECT_THAT( l2capImpl.Init(), Eq( BT_STATUS_SUCCESS ) );
    EXPECT_THAT( l2capImpl.Init(), Eq( BT_STATUS_IN_USE ) );
    EXPECT_THAT( l2capImpl.Deinit(), Eq( BT_STATUS_SUCCESS ) );

    EXPECT_CALL( *resMock, btres_openclient( _, _ ) ).WillOnce( Return( -1 ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btl2cap_init() );
}

TEST_F( libbttest, btl2cap_create ) {
    L2cap *l2cap = nullptr;
    EXPECT_THAT( L2cap::Create( &l2cap ), Eq( BT_STATUS_SUCCESS ) );
    EXPECT_THAT( L2cap::Destroy( l2cap ), Eq( BT_STATUS_SUCCESS ) );
    EXPECT_CALL( *resMock, btres_openclient( _, _ ) ).WillOnce( Return( -1 ) );
    EXPECT_THAT( L2cap::Create( &l2cap ), Eq( BT_STATUS_NO_TRANSPORT ) );
}

TEST_F( libbttest, btl2cap_call_fail ) {
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btl2cap_get_info( NULL, NULL ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btl2cap_register_server( NULL, NULL, NULL, NULL, NULL  ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btl2cap_connect( 0, NULL, 0, NULL, NULL, NULL ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btl2cap_send_packet( NULL, NULL, 0 ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btl2cap_disconnect( NULL ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btl2cap_unregister_server( 0 ) );
}

TEST_F( libbttest, btl2cap_impl_call_success ) {
    L2cap *l2cap = nullptr;
    uint16_t psm{};
    btl2cap_server_handle_t handle{};
    bt_addr_t addr{};
    btl2cap_connection_t cid{};
    std::vector<uint8_t> data( 100, 0 );

    EXPECT_THAT( L2cap::Create( &l2cap ), Eq( BT_STATUS_SUCCESS ) );
    EXPECT_EQ( BT_STATUS_SUCCESS, l2cap->RegisterServer( &psm, &handle, btl2cap_callback, btl2cap_cmd_callback, NULL  ) );
    EXPECT_EQ( BT_STATUS_NO_CONNECTION, l2cap->GetInfo( NULL, NULL ) );
    EXPECT_EQ( BT_STATUS_SUCCESS, l2cap->Connect( 0, &addr, psm, btl2cap_callback, btl2cap_cmd_callback, NULL ) );
    EXPECT_EQ( BT_STATUS_NO_CONNECTION, l2cap->SendPacket( &cid, data.data(), data.size() ) );
    EXPECT_EQ( BT_STATUS_NO_CONNECTION, l2cap->Disconnect( &cid ) );
    EXPECT_EQ( BT_STATUS_SUCCESS, l2cap->UnregisterServer( handle ) );
    EXPECT_THAT( L2cap::Destroy( l2cap ), Eq( BT_STATUS_SUCCESS ) );
}

TEST_F( libbttest, Test_L2CapEvtHandler ) {
    L2cap *l2cap = nullptr;
    handle_evt_cb L2CapEvtHandler = nullptr;
    btres_t *btres = nullptr;

    EXPECT_CALL( *resMock, btres_init( _, _, _, _, _ ) )
    .WillRepeatedly( Invoke( [&L2CapEvtHandler, &btres]( btres_t *btres_in, jobq_t *jobq, handle_cmd_cb cmd_cb,
    handle_evt_cb evt_cb, void *args ) {
        L2CapEvtHandler = evt_cb;
        btres = btres_in;
        return BTRES_RESULT_SUCCESS;
    } ) );

    EXPECT_THAT( L2cap::Create( &l2cap ), Eq( BT_STATUS_SUCCESS ) );

    ASSERT_THAT( L2CapEvtHandler, Ne( nullptr ) );

    handle_evt_cb L2capServerEvtHandler = nullptr;
    btres_t *btres_L2capServerEvtHandler = nullptr;
    btl2cap_server_t *server = nullptr;
    EXPECT_CALL( *resMock, btres_init( _, _, _, _, _ ) )
    .WillOnce(
        Invoke( [&L2capServerEvtHandler, &btres_L2capServerEvtHandler, &server](
    btres_t *btres_in, jobq_t *jobq, handle_cmd_cb cmd_cb, handle_evt_cb evt_cb, void *args ) {
        L2capServerEvtHandler = evt_cb;
        btres_in->evt_cb = evt_cb;
        btres_L2capServerEvtHandler = btres_in;
        server = ( btl2cap_server_t * )args;
        return BTRES_RESULT_SUCCESS;
    } ) );

    uint16_t psm{};
    btl2cap_server_handle_t handle{};
    EXPECT_EQ( BT_STATUS_SUCCESS, l2cap->RegisterServer( &psm, &handle, btl2cap_callback, btl2cap_cmd_callback, NULL  ) );

    BTResMock::removeInstance(); // Preven further expect_call from triggering

    ASSERT_THAT( L2capServerEvtHandler, Ne( nullptr ) );

    bt_command_rsp_t rsp{};
    rsp.cmd = BTL2CAP_RSP_CONNECT;
    EXPECT_THAT(
        L2capServerEvtHandler( btres_L2capServerEvtHandler, 0, BTL2C_EVENT_INCOMING_CONNECTION,
                               ( uint8_t * )&rsp, sizeof( rsp ), server ),
        Eq( 0 ) );

    EXPECT_THAT(
        L2CapEvtHandler( btres, 0, BT_EVENT_COMMAND_RESPONSE, ( uint8_t * )&rsp, sizeof( rsp ), l2cap ),
        Eq( 0 ) );  // **

    EXPECT_THAT(
        L2CapEvtHandler( btres, 0, BTL2CAP_EVENT_STACK_FAULT, ( uint8_t * )&rsp, sizeof( rsp ), l2cap ),
        Eq( 0 ) );  // **

    EXPECT_THAT( L2cap::Destroy( l2cap ), Eq( BT_STATUS_SUCCESS ) );
}

using except_cb_t = int ( * )( btres_t *btres, int fd );

void cb_func( btl2cap_event_t evt, uint8_t *buf, size_t len, void *args ) {
}

void cmd_cb_func( btl2cap_cmd_response_t cmd_rsp, uint64_t id, bt_errcode_t errcode, const uint8_t *buf, size_t len, void *args ) {
}

TEST_F( libbttest, Test_L2capExcept ) {
    L2cap *l2cap = nullptr;

    except_cb_t L2capExcept = nullptr;
    btres_t *res_L2capExcept = nullptr;
    EXPECT_CALL( *resMock, btres_add_conn( _, _, _, _, _ ) )
    .WillOnce( Invoke( [&L2capExcept, &res_L2capExcept]( btres_t *btres, int parent_fd, int fd, const char *progname, except_cb_t except ) {
        L2capExcept = except;
        res_L2capExcept = btres;
        return BTRES_RESULT_SUCCESS;
    } ) )
    .WillRepeatedly( Return( BT_STATUS_SUCCESS ) );

    EXPECT_EQ( BT_STATUS_SUCCESS, L2cap::Create( &l2cap ) );

    ASSERT_NE( nullptr, L2capExcept );
    ASSERT_NE( nullptr, l2cap );
    bt_addr_t addr{};

    EXPECT_CALL( *resMock, btres_sendcmd( _, _, _, _, _, _, _ ) ).WillOnce( Return( BT_STATUS_PENDING ) ).WillRepeatedly( Return( BT_STATUS_SUCCESS ) );

    l2cap->Connect( 0, &addr, 0, cb_func, cmd_cb_func, l2cap );

    btl2cap_server_handle_t handle{};
    uint16_t psm = 0;
    EXPECT_EQ( 0, L2capExcept( res_L2capExcept, 0 ) );
    l2cap->RegisterServer( &psm, &handle, cb_func, cmd_cb_func, l2cap );

    // EXPECT_EQ( 0, L2capExcept( res_L2capExcept, 0 ) );

    EXPECT_THAT( L2cap::Destroy( l2cap ), Eq( BT_STATUS_FAILED ) );
}

TEST_F( libbttest, Test_L2capServerEvtHandler ) {
    L2cap *l2cap = nullptr;

    EXPECT_THAT( L2cap::Create( &l2cap ), Eq( BT_STATUS_SUCCESS ) );

    handle_evt_cb L2capServerEvtHandler = nullptr;
    btres_t *btres_L2capServerEvtHandler = nullptr;
    btl2cap_server_t *server = nullptr;
    EXPECT_CALL( *resMock, btres_init( _, _, _, _, _ ) )
    .WillOnce(
        Invoke( [&L2capServerEvtHandler, &btres_L2capServerEvtHandler, &server](
    btres_t *btres_in, jobq_t *jobq, handle_cmd_cb cmd_cb, handle_evt_cb evt_cb, void *args ) {
        L2capServerEvtHandler = evt_cb;
        btres_in->evt_cb = evt_cb;
        btres_L2capServerEvtHandler = btres_in;
        server = ( btl2cap_server_t * )args;
        return BTRES_RESULT_SUCCESS;
    } ) );

    uint16_t psm{};
    btl2cap_server_handle_t handle{};
    EXPECT_EQ( BT_STATUS_SUCCESS, l2cap->RegisterServer( &psm, &handle, btl2cap_callback, btl2cap_cmd_callback, NULL  ) );

    BTResMock::removeInstance(); // Preven further expect_call from triggering

    ASSERT_THAT( L2capServerEvtHandler, Ne( nullptr ) );

    resMock = BTResMock::getInstance();

    handle_evt_cb ConnEvtHandler = nullptr;
    btres_t *btres_conn = nullptr;
    btl2cap_conn_t *conn = nullptr;
    EXPECT_CALL( *resMock, btres_init( _, _, _, _, _ ) )
    .WillOnce( Invoke( [&ConnEvtHandler, &btres_conn, &conn]( btres_t *btres_in, jobq_t *jobq, handle_cmd_cb cmd_cb,
    handle_evt_cb evt_cb, void *args ) {
        ConnEvtHandler = evt_cb;
        btres_in->evt_cb = evt_cb;
        btres_conn = btres_in;
        conn = ( btl2cap_conn_t * )args;
        return BTRES_RESULT_SUCCESS;
    } ) );

    bt_command_rsp_t rsp{};
    rsp.cmd = BTL2CAP_RSP_CONNECT;
    EXPECT_THAT(
        L2capServerEvtHandler( btres_L2capServerEvtHandler, 0, BTL2C_EVENT_INCOMING_CONNECTION,
                               ( uint8_t * )&rsp, sizeof( rsp ), server ),
        Eq( 0 ) );

    BTResMock::removeInstance(); // Preven further expect_call from triggering

    ASSERT_THAT( ConnEvtHandler, Ne( nullptr ) );

    EXPECT_THAT(
        ConnEvtHandler( btres_conn, 0, BTL2C_EVENT_DATA, ( uint8_t * )&rsp, sizeof( rsp ), conn ),
        Eq( 0 ) );

    btl2c_frag_pkt_t pkt{};
    EXPECT_THAT( ConnEvtHandler( btres_conn, 0, BTL2C_EVENT_FRAGMENT_DATA, ( uint8_t * )&pkt,
                                 sizeof( pkt ), conn ),
                 Eq( 0 ) );

    l2cap_link_t link{};
    EXPECT_THAT( ConnEvtHandler( btres_conn, 0, BTL2C_EVENT_DISCONNECTED, ( uint8_t * )&link,
                                 sizeof( link ), conn ),
                 Eq( 0 ) );

    EXPECT_THAT( L2cap::Destroy( l2cap ), Eq( BT_STATUS_SUCCESS ) );
}

TEST_F( libbttest, Test_L2capServerExcept ) {
    L2cap *l2cap = nullptr;

    EXPECT_THAT( L2cap::Create( &l2cap ), Eq( BT_STATUS_SUCCESS ) );

    except_cb_t L2capServerExcept = nullptr;
    btres_t *res_L2capServerExcept{};
    EXPECT_CALL( *resMock, btres_add_conn( _, _, _, _, _ ) )
    .WillOnce( Invoke( [&L2capServerExcept, &res_L2capServerExcept]( btres_t *btres, int parent_fd, int fd,
    const char *progname, except_cb_t except ) {
        L2capServerExcept = except;
        res_L2capServerExcept = btres;
        return BTRES_RESULT_SUCCESS;
    } ) );

    uint16_t psm{};
    btl2cap_server_handle_t handle{};
    EXPECT_EQ( BT_STATUS_SUCCESS, l2cap->RegisterServer( &psm, &handle, btl2cap_callback,
               btl2cap_cmd_callback, NULL ) );

    ASSERT_THAT( L2capServerExcept, Ne( nullptr ) );
    ASSERT_THAT( res_L2capServerExcept, Ne( nullptr ) );

    EXPECT_THAT( L2capServerExcept( res_L2capServerExcept, 0 ),
                 Eq( 0 ) );  // **

    EXPECT_THAT( L2cap::Destroy( l2cap ), Eq( BT_STATUS_SUCCESS ) );
}

// Test ConnExcept
TEST_F( libbttest, Test_L2cap_ConnExcept ) {
    L2cap *l2cap = nullptr;
    handle_evt_cb L2capServerEvtHandler = nullptr;
    btres_t *btres = nullptr;
    uint16_t psm{};
    btl2cap_server_handle_t handle{};
    btl2cap_server_t *server = nullptr;

    EXPECT_THAT( L2cap::Create( &l2cap ), Eq( BT_STATUS_SUCCESS ) );

    EXPECT_CALL( *resMock, btres_init( _, _, _, _, _ ) )
    .WillOnce(
        Invoke( [&L2capServerEvtHandler, &btres, &server]( btres_t *btres_in, jobq_t *jobq, handle_cmd_cb cmd_cb,
    handle_evt_cb evt_cb, void *args ) {
        L2capServerEvtHandler = evt_cb;
        btres_in->evt_cb = evt_cb;
        btres = btres_in;
        server = ( btl2cap_server_t * )args;
        return BTRES_RESULT_SUCCESS;
    } ) );

    EXPECT_EQ( BT_STATUS_SUCCESS, l2cap->RegisterServer( &psm, &handle, btl2cap_callback, btl2cap_cmd_callback, NULL  ) );

    BTResMock::removeInstance(); // Preven further expect_call from triggering

    ASSERT_THAT( L2capServerEvtHandler, Ne( nullptr ) );

    resMock = BTResMock::getInstance();

    except_cb_t ConnExcept = nullptr;
    btres_t *res_ConnExcept{};
    EXPECT_CALL( *resMock, btres_add_conn( _, _, _, _, _ ) )
    .WillOnce( Invoke( [&ConnExcept, &res_ConnExcept]( btres_t *btres, int parent_fd, int fd, const char *progname,
    except_cb_t except ) {
        ConnExcept = except;
        res_ConnExcept = btres;
        return BTRES_RESULT_SUCCESS;
    } ) );

    bt_command_rsp_t rsp{};
    rsp.cmd = BTL2CAP_RSP_CONNECT;
    EXPECT_THAT(
        L2capServerEvtHandler( btres, 0, BTL2C_EVENT_INCOMING_CONNECTION, ( uint8_t * )&rsp, sizeof( rsp ), server ),
        Eq( 0 ) );

    BTResMock::removeInstance(); // Preven further expect_call from triggering

    ASSERT_THAT( ConnExcept, Ne( nullptr ) );

    ASSERT_THAT( res_ConnExcept, Ne( nullptr ) );

    EXPECT_THAT( ConnExcept( res_ConnExcept, 0 ), Eq( 0 ) );

    EXPECT_THAT( L2cap::Destroy( l2cap ), Eq( BT_STATUS_SUCCESS ) );
}