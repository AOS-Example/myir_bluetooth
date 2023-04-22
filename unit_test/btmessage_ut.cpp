#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <stdlib.h>

#include "TestSuite.h"
#include "bt.h"
#include "bt_utils.h"
#include "btlog.h"
#include "btmessage.h"
#include "stubs.h"

#include "priv/btmessage_impl.hpp"

#define TAG "BTMSG_UT"
using namespace ::testing;
using Bluetooth::MessageImpl;

void btmessage_callback( btmessage_event_t evt, uint8_t *buf, size_t len, void *args ) {
}

void btmessage_cmd_callback( btmessage_cmd_response_t cmd_rsp, uint64_t id, bt_errcode_t errcode, const uint8_t *buf,
                             size_t len, void *args ) {
}

TEST_F( libbttest, btmessage_calls_pre_init ) {
    EXPECT_CALL( *resMock, btres_sendcmd( _, _, _, _, _, _, _ ) ).WillRepeatedly( Return( BT_STATUS_SUCCESS ) );
    // Since we are only unit-testing function's local control logic,
    // all these calls are passed in trivial parameters.
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btmessage_set_read_status( 0, NULL, 0, 0, FALSE ) );

    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btmessage_send_message( 0, NULL, 0, NULL, 0, 0 ) );

    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btmessage_get_message( 0, NULL, 0, 0, 0 ) );

    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btmessage_get_message_listing( 0, NULL, 0, ( btmessage_status_t )0, 0, 0 ) );

    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btmessage_get_mas_info( NULL, 0, NULL ) );

    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btmessage_get_connected_list( NULL, NULL ) );

    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btmessage_get_local_state( NULL ) );

    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btmessage_connect( 0, NULL ) );

    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btmessage_disconnect( 0, NULL ) );

    // after this init call, (btmessage.fd < 0) will evaluates to true; Then all the following
    // calls will return a BT_STATUS_SUCCESS, instead of BT_STATUS_NO_TRANSPORT.
    EXPECT_EQ( 0, btmessage_init( btmessage_callback, NULL, NULL ) );
    bt_addr_t addr;
    uint8_t count = 2;
    btmessage_mas_instance_t instances[2];
    btmessage_local_state_t state;
    btmessage_mas_info_t info;

    EXPECT_EQ( BT_STATUS_SUCCESS, btmessage_set_read_status( 0, &addr, 0, 0, false ) );

    EXPECT_EQ( BT_STATUS_SUCCESS, btmessage_send_message( 0, &addr, 0, "addressTest", "subjectTest", "messageTest" ) );

    EXPECT_EQ( BT_STATUS_SUCCESS, btmessage_get_message( 0, &addr, 0, 0, 0 ) );

    EXPECT_EQ( BT_STATUS_SUCCESS, btmessage_get_message_listing( 0, &addr, 0, ( btmessage_status_t )0, 0, 0 ) );

    EXPECT_EQ( BT_STATUS_SUCCESS, btmessage_get_mas_info( &addr, 0, &info ) );

    EXPECT_EQ( BT_STATUS_SUCCESS, btmessage_get_connected_list( instances, &count ) );

    EXPECT_EQ( BT_STATUS_SUCCESS, btmessage_get_local_state( &state ) );

    EXPECT_EQ( BT_STATUS_IN_USE, btmessage_init( btmessage_callback, NULL, NULL ) );
    EXPECT_EQ( BT_STATUS_SUCCESS, btmessage_deinit() );
    EXPECT_EQ( BT_STATUS_FAILED, btmessage_deinit() );
}

TEST_F( libbttest, btmessage_Impl_calls_pre_init ) {
    MessageImpl messageImpl{};

    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, messageImpl.SetReadStatus( 0, NULL, 0, 0, FALSE ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, messageImpl.SendMessage( 0, NULL, 0, NULL, 0, 0 ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, messageImpl.GetMessage( 0, NULL, 0, 0, 0, 2000 ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, messageImpl.GetMessageListing( 0, NULL, 0, ( btmessage_status_t )0, 0, 0 ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, messageImpl.GetMasInfo( NULL, 0, NULL ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, messageImpl.GetConnectedList( NULL, NULL ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, messageImpl.GetLocalState( NULL ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, messageImpl.Connect( 0, NULL ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, messageImpl.Disconnect( 0, NULL ) );

    EXPECT_EQ( 0, messageImpl.Init( btmessage_callback, NULL, NULL ) );
    EXPECT_EQ( BT_STATUS_IN_USE, messageImpl.Init( btmessage_callback, NULL, NULL ) );
}

TEST_F( libbttest, btmessage_init_test ) {
    EXPECT_EQ( BT_STATUS_SUCCESS, btmessage_init( btmessage_callback, NULL, NULL ) );

    EXPECT_EQ( BT_STATUS_SUCCESS, btmessage_deinit() );

    EXPECT_CALL( *resMock, btres_openclient( _, _ ) ).WillOnce( Return( -1 ) ).RetiresOnSaturation();
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btmessage_init( btmessage_callback, NULL, NULL ) );

    btmessage_deinit();
}

TEST_F( libbttest, btmessage_call_tests ) {
    bt_addr_t addr;
    BTResMock *btrestMock = BTResMock::getInstance();
    EXPECT_CALL( *btrestMock, btres_sendcmd( _, _, _, _, _, _, _ ) ).WillRepeatedly( Return( BT_STATUS_SUCCESS ) );

    btmessage_init( btmessage_callback, NULL, NULL );
    EXPECT_EQ( BT_STATUS_SUCCESS, btmessage_connect( 1, &addr ) );
    EXPECT_EQ( BT_STATUS_SUCCESS, btmessage_disconnect( 1, &addr ) );

    btmessage_deinit();
    BTResMock::removeInstance();
}

using except_cb_t = int ( * )( btres_t *btres, int fd );

TEST_F( libbttest, Test_btmessage_MessageEvtHandler ) {

    handle_evt_cb MessageEvtHandler = nullptr;
    btres_t *btres = nullptr;
    MessageImpl *messageImpl = nullptr;

    EXPECT_CALL( *resMock, btres_init( _, _, _, _, _ ) )
    .WillOnce(
        Invoke( [&MessageEvtHandler, &btres, &messageImpl]( btres_t *btres_in, jobq_t *jobq, handle_cmd_cb cmd_cb,
    handle_evt_cb evt_cb, void *args ) {
        MessageEvtHandler = evt_cb;
        messageImpl = ( MessageImpl * )args;
        btres = btres_in;
        return BTRES_RESULT_SUCCESS;
    } ) );

    except_cb_t PhoneExcept = nullptr;
    btres_t *PhoneExcept_res = nullptr;
    EXPECT_CALL( *resMock, btres_add_conn( _, _, _, _, _ ) )
    .WillOnce( Invoke( [&PhoneExcept, &PhoneExcept_res]( btres_t *btres, int parent_fd, int fd,
    const char *progname, except_cb_t except ) {
        PhoneExcept = except;
        PhoneExcept_res = btres;
        return BTRES_RESULT_SUCCESS;
    } ) );

    EXPECT_EQ( 0, btmessage_init( btmessage_callback, btmessage_cmd_callback, NULL ) ); // **

    ASSERT_THAT( MessageEvtHandler, Ne( nullptr ) );
    ASSERT_THAT( PhoneExcept, Ne( nullptr ) );
    ASSERT_THAT( messageImpl, Ne( nullptr ) );

    bt_command_rsp_t buf{};
    EXPECT_THAT(
        MessageEvtHandler( btres, 0, BT_EVENT_COMMAND_RESPONSE, ( uint8_t * )&buf, sizeof( buf ), messageImpl ),
        Eq( 0 ) );  // **

    EXPECT_THAT(
        MessageEvtHandler( btres, 0, BT_CMD_RADIO_ON, ( uint8_t * )&buf, sizeof( buf ), messageImpl ),
        Eq( 0 ) );  // **

    EXPECT_THAT(
        PhoneExcept( PhoneExcept_res, 0 ),
        Eq( 0 ) );  // **

    EXPECT_EQ( 0, btmessage_deinit() );
}

TEST_F( libbttest, Test_btmessage_MessageEvtHandler_callbackNull ) {
    handle_evt_cb MessageEvtHandler = nullptr;
    btres_t *btres = nullptr;
    MessageImpl *messageImpl = nullptr;

    EXPECT_CALL( *resMock, btres_init( _, _, _, _, _ ) )
    .WillOnce(
        Invoke( [&MessageEvtHandler, &btres, &messageImpl]( btres_t *btres_in, jobq_t *jobq, handle_cmd_cb cmd_cb,
    handle_evt_cb evt_cb, void *args ) {
        MessageEvtHandler = evt_cb;
        messageImpl = ( MessageImpl * )args;
        btres = btres_in;
        return BTRES_RESULT_SUCCESS;
    } ) );

    EXPECT_EQ( 0, btmessage_init( NULL, NULL, NULL ) ); // **

    ASSERT_THAT( MessageEvtHandler, Ne( nullptr ) );
    ASSERT_THAT( messageImpl, Ne( nullptr ) );

    bt_command_rsp_t buf{};
    EXPECT_THAT(
        MessageEvtHandler( btres, 0, BT_EVENT_COMMAND_RESPONSE, ( uint8_t * )&buf, sizeof( buf ), messageImpl ),
        Eq( -1 ) );  // **

    EXPECT_EQ( 0, btmessage_deinit() );
}