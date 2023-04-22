#include <stdlib.h>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "bt.h"
#include "priv/bt_priv.h"
#include "priv/bt_impl.hpp"
#include "btmgr_constants.h"
#include "btmgr_event.h"
#include "btrfs_event.h"
#include "TestSuite.h"
#include "stubs.h"
#include "btresmgr.h"

using namespace ::testing;

typedef struct {
    jobq_t *jobq;
    btres_t btres;
    int fd;
    bt_callback_t cb;
    bt_cmd_callback_t cmd_cb;
    void *args;

    link_t servers;
    pthread_mutex_t server_mutex;
} bt_t;

void bt_callback( bt_event_t event, const uint8_t *data, size_t len, void *args ) {
}

void bt_cmd_callback( bt_cmd_response_t cmd_rsp, uint64_t id, bt_errcode_t errcode, const uint8_t *data, size_t len, void *args ) {
}

using Bluetooth::BtImpl;

using BtmgrEvtHandler_t = int ( * )( btres_t *btres, int fd, uint16_t msg, uint8_t *buf, size_t len,
                                     void *args );

TEST_F( libbttest, bt_init_success ) {
    BtmgrEvtHandler_t BtmgrEvtHandler = nullptr;
    btres_t *btres = nullptr;
    BtImpl *bt = nullptr;

    EXPECT_CALL( *resMock, btres_init( _, _, _, _, _ ) )
    .WillOnce( Invoke( [&BtmgrEvtHandler, &bt, &btres]( btres_t *btres_in, jobq_t *jobq, handle_cmd_cb cmd_cb,
    handle_evt_cb evt_cb, void *args ) {
        BtmgrEvtHandler = evt_cb;
        bt = ( BtImpl * )args;
        btres = btres_in;
        return BTRES_RESULT_SUCCESS;
    } ) );

    EXPECT_EQ ( 0, bt_init( bt_callback, bt_cmd_callback, NULL ) );

    ASSERT_THAT( BtmgrEvtHandler, Ne( nullptr ) );
    ASSERT_THAT( bt, Ne( nullptr ) );

    bt_command_rsp_t buf{};
    EXPECT_THAT( BtmgrEvtHandler( btres, 0, BT_EVENT_COMMAND_RESPONSE, ( uint8_t * )&buf,
                                  sizeof( buf ), bt ),
                 Eq( 0 ) );  // **

    EXPECT_THAT( BtmgrEvtHandler( btres, 0, BT_CMD_RADIO_ON, ( uint8_t * )&buf,
                                  sizeof( buf ), bt ),
                 Eq( 0 ) ); // **

    EXPECT_CALL( *resMock, btres_init( _, _, _, _, _ ) ).WillRepeatedly( Return( BTRES_RESULT_SUCCESS ) );
    EXPECT_EQ ( BT_STATUS_IN_USE, bt_init( bt_callback, bt_cmd_callback, NULL ) );

    EXPECT_EQ ( 0, bt_deinit() );
}

TEST_F( libbttest, bt_init_success_btCallbackNull ) {
    BtmgrEvtHandler_t BtmgrEvtHandler = nullptr;
    btres_t *btres = nullptr;
    BtImpl *bt = nullptr;

    EXPECT_CALL( *resMock, btres_init( _, _, _, _, _ ) )
    .WillOnce( Invoke( [&BtmgrEvtHandler, &bt, &btres]( btres_t *btres_in, jobq_t *jobq, handle_cmd_cb cmd_cb,
    handle_evt_cb evt_cb, void *args ) {
        BtmgrEvtHandler = evt_cb;
        bt = ( BtImpl * )args;
        btres = btres_in;
        return BTRES_RESULT_SUCCESS;
    } ) );

    EXPECT_EQ ( 0, bt_init( NULL, NULL, NULL ) ); // **

    ASSERT_THAT( BtmgrEvtHandler, Ne( nullptr ) );
    ASSERT_THAT( bt, Ne( nullptr ) );

    bt_command_rsp_t buf{};
    EXPECT_THAT( BtmgrEvtHandler( btres, 0, BT_EVENT_COMMAND_RESPONSE, ( uint8_t * )&buf,
                                  sizeof( buf ), bt ),
                 Eq( -1 ) );  // **

    EXPECT_EQ ( BT_STATUS_IN_USE, bt_init( bt_callback, bt_cmd_callback, NULL ) );

    EXPECT_EQ ( 0, bt_deinit() );
}

TEST_F( libbttest, bt_calls_success ) {

    bt_init( bt_callback, bt_cmd_callback, NULL );

    EXPECT_EQ ( BT_STATUS_SUCCESS, bt_radio_on( 0 ) );

    EXPECT_EQ ( BT_STATUS_SUCCESS, bt_radio_off( 0 ) );

    bt_local_state_t state;
    EXPECT_EQ ( BT_STATUS_SUCCESS, bt_get_local_state( &state ) );

    bt_addr_t addr;
    memset( &addr, 0, sizeof( addr ) );
    addr.type = BT_ADDRTYPE_BREDR;
    EXPECT_EQ ( BT_STATUS_SUCCESS, bt_delete_device( 0, &addr ) );

    EXPECT_EQ ( BT_STATUS_SUCCESS, bt_set_discoverable( 0, TRUE ) );

    EXPECT_EQ ( BT_STATUS_SUCCESS, bt_set_discoverable_mode( 0, BT_DISCOVERABLE_MODE ) );

    EXPECT_EQ ( BT_STATUS_SUCCESS, bt_start_inquiry( 0 ) );

    EXPECT_EQ ( BT_STATUS_SUCCESS, bt_cancel_inquiry( 0 ) );

    EXPECT_EQ ( BT_STATUS_SUCCESS, bt_pair_device( 0, &addr ) );

    EXPECT_EQ ( BT_STATUS_SUCCESS, bt_cancel_pairing( 0, &addr ) );

    EXPECT_EQ ( BT_STATUS_SUCCESS, bt_pair_dual_device( 0, &addr ) );
    EXPECT_EQ ( BT_STATUS_SUCCESS, bt_cancel_pairing( 0, &addr ) );

    EXPECT_EQ ( BT_STATUS_SUCCESS, bt_confirm_pairing( &addr, TRUE ) );

    EXPECT_EQ ( BT_STATUS_SUCCESS, bt_set_pincode( &addr, "012345" ) );

    EXPECT_EQ ( BT_STATUS_SUCCESS, bt_set_passkey( &addr, 2 ) );

    bt_addr_t addrs[10];
    uint8_t cnt = ( uint8_t )( sizeof( addrs ) / sizeof( addrs[0] ) );
    EXPECT_EQ ( BT_STATUS_SUCCESS, bt_get_connected_list( addrs, &cnt ) );

    EXPECT_EQ ( BT_STATUS_SUCCESS, bt_get_paired_list( addrs, &cnt ) );

    cnt = 10;
    size_t len = sizeof( bt_device_t ) + ( cnt * sizeof( bt_uuid_t ) );
    bt_device_t *info = ( bt_device_t * )malloc( len );
    memset( info, 0, len );
    info->edr.cnt = cnt;

    EXPECT_EQ ( BTRES_RESULT_SUCCESS, bt_get_device_info( &addr, info, cnt ) );
    free( info );

    bt_uuid_t uuids[15];
    cnt = 15;
    EXPECT_EQ ( BT_STATUS_SUCCESS, bt_get_connected_services( addrs, uuids, &cnt ) );

    bt_uuid_t uuid;
    uuid.len = 16;
    uuid.uuid16 = 1234;
    EXPECT_EQ ( BT_STATUS_SUCCESS, bt_search_services( 0, &addr, &uuid ) );

    addr.type = BT_ADDRTYPE_BREDR;
    EXPECT_EQ ( BT_STATUS_SUCCESS, bt_connect_service( addr.handle, &addr, &uuid, 0 ) );

    EXPECT_EQ ( BT_STATUS_SUCCESS, bt_disconnect_service( addr.handle, &addr, &uuid, 0 ) );

    uint64_t deviceId = 0;
    EXPECT_EQ ( BT_STATUS_SUCCESS, bt_connect_device( deviceId, &addr ) );

    EXPECT_EQ ( BT_STATUS_SUCCESS, bt_disconnect_device( deviceId, &addr ) );

    EXPECT_EQ ( BT_STATUS_SUCCESS, bt_set_conmgr_state( true ) );

    char name[256] = {0};
    EXPECT_EQ ( BT_STATUS_SUCCESS, bt_set_name( name ) );

    EXPECT_EQ ( BT_STATUS_SUCCESS, bt_set_device_priority( &addr, 3 ) );

    uuid.len = 128;

    for ( int i = 0; i < 16; i++ ) {
        uuid.uuid128[i] = i;
    }

    EXPECT_EQ ( BT_STATUS_SUCCESS, bt_connect_spp( addr.handle, &addr, &uuid, 0 ) );

    uint8_t channel = 0;
    EXPECT_EQ ( BTMGR_STATUS_SUCCESS, bt_register_spp_server( &uuid, NULL, 0, &channel ) );
    EXPECT_EQ ( BT_STATUS_SUCCESS, bt_deregister_spp_server( channel ) );

    EXPECT_EQ ( BTMGR_STATUS_SUCCESS, bt_register_rfcomm_server( &channel ) );

    EXPECT_EQ ( BT_STATUS_SUCCESS, bt_deregister_rfcomm_server( channel ) );

    EXPECT_EQ ( BT_STATUS_SUCCESS, bt_connect_rfcomm( 0, &addr, channel ) );

    EXPECT_EQ ( BT_STATUS_SUCCESS, bt_register_eir_service( &uuid ) );

    EXPECT_EQ ( BT_STATUS_SUCCESS, bt_deregister_eir_service( &uuid ) );

    EXPECT_EQ ( BT_STATUS_INVALID_PARM, bt_inject_device( 0, NULL, NULL, NULL, 0, 0, 0, 0 ) );

    uint8_t linkkey[BT_LINK_KEY_LEN];
    EXPECT_EQ ( BT_STATUS_SUCCESS, bt_inject_device( 0, addrs, linkkey, NULL, 0, 0, 0, 0 ) );

    uint32_t handle = 0;
    EXPECT_EQ ( BT_STATUS_SUCCESS, bt_add_service_record( NULL, 0, &handle ) );
    EXPECT_EQ ( BT_STATUS_SUCCESS, bt_remove_service_record( handle ) );

    EXPECT_EQ ( BTMGR_STATUS_SUCCESS, bt_register_spp_server( &uuid, NULL, 0, &channel ) );

    bt_deinit();
}

TEST_F( libbttest, bt_calls_fail ) {
    EXPECT_CALL( *resMock, btres_openclient( _, _ ) ).WillOnce( Return( -1 ) ).RetiresOnSaturation();
    bt_init( bt_callback, bt_cmd_callback, NULL );

    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt_deregister_spp_server( 0 ) );

    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt_radio_on( 0 ) );

    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt_radio_off( 0 ) );

    bt_local_state_t state;
    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt_get_local_state( &state ) );

    bt_addr_t addr;
    memset( &addr, 0, sizeof( addr ) );
    addr.type = BT_ADDRTYPE_BREDR;
    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt_delete_device( 0, &addr ) );

    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt_set_discoverable( 0, TRUE ) );

    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt_set_discoverable_mode( 0, BT_DISCOVERABLE_MODE ) );

    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt_start_inquiry( 0 ) );

    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt_cancel_inquiry( 0 ) );

    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt_pair_device( 0, &addr ) );

    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt_cancel_pairing( 0, &addr ) );

    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt_pair_dual_device( 0, &addr ) );
    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt_cancel_pairing( 0, &addr ) );

    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt_confirm_pairing( &addr, TRUE ) );

    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt_set_pincode( &addr, "012345" ) );

    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt_set_passkey( &addr, 2 ) );

    bt_addr_t addrs[10];
    uint8_t cnt = ( uint8_t )( sizeof( addrs ) / sizeof( addrs[0] ) );
    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt_get_connected_list( addrs, &cnt ) );

    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt_get_paired_list( addrs, &cnt ) );

    cnt = 10;
    bt_device_t *info = ( bt_device_t * )malloc( sizeof( bt_device_t ) + ( cnt * sizeof( bt_uuid_t ) ) );
    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt_get_device_info( &addr, info, cnt ) );
    free( info );

    bt_uuid_t uuids[15];
    cnt = 15;
    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt_get_connected_services( addrs, uuids, &cnt ) );

    bt_uuid_t uuid;
    uuid.len = 16;
    uuid.uuid16 = 1234;
    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt_search_services( 0, &addr, &uuid ) );

    addr.type = BT_ADDRTYPE_BREDR;
    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt_connect_service( addr.handle, &addr, &uuid, 0 ) );

    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt_disconnect_service( addr.handle, &addr, &uuid, 0 ) );

    uint64_t deviceId = 0;
    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt_connect_device( deviceId, &addr ) );

    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt_disconnect_device( deviceId, &addr ) );

    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt_set_conmgr_state( true ) );

    char name[256] = {0};
    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt_set_name( name ) );

    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt_set_device_priority( &addr, 3 ) );

    uuid.len = 128;

    for ( int i = 0; i < 16; i++ ) {
        uuid.uuid128[i] = i;
    }

    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt_connect_spp( addr.handle, &addr, &uuid, 0 ) );

    uint8_t channel = 0;
    uint8_t sdp[128] = {0};
    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt_register_spp_server( &uuid, sdp, 128, &channel ) );

    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt_register_spp_server( &uuid, NULL, 0, NULL ) );

    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt_register_spp_server( &uuid, NULL, 0, &channel ) );

    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt_register_rfcomm_server( &channel ) );
    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt_deregister_rfcomm_server( channel ) );

    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt_connect_rfcomm( 0, &addr, channel ) );

    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt_inject_device( 0, NULL, NULL, NULL, 0, 0, 0, 0 ) );

    uint32_t handle = 0;
    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt_add_service_record( NULL, 0, &handle ) );
    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt_remove_service_record( handle ) );

    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt_deregister_spp_server( channel ) );

    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt_register_eir_service( NULL ) );

    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt_deregister_eir_service( NULL ) );

    bt_deinit();
}

TEST_F( libbttest, bt_calls_fail_testBtImpl ) {

    BtImpl bt{};

    EXPECT_CALL( *resMock, btres_openclient( _, _ ) ).WillOnce( Return( -1 ) ).RetiresOnSaturation();

    EXPECT_THAT( bt.InitVersion( BT_API_VERSION, bt_callback, bt_cmd_callback, NULL ), Eq( BT_STATUS_NOT_READY ) );

    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt.DeregisterRfcommServer( 0 ) );

    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt.RadioOn( 0 ) );

    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt.RadioOff( 0 ) );

    bt_local_state_t state;
    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt.GetLocalState( &state ) );

    bt_addr_t addr;
    memset( &addr, 0, sizeof( addr ) );
    addr.type = BT_ADDRTYPE_BREDR;
    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt.DeleteDevice( 0, &addr ) );

    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt.SetDiscoverable( 0, TRUE ) );

    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt.SetDiscoverableMode( 0, BT_DISCOVERABLE_MODE ) );

    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt.StartInquiry( 0 ) );

    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt.CancelInquiry( 0 ) );

    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt.PairDevice( 0, &addr ) );

    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt.CancelPairing( 0, &addr ) );

    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt.PairDualDevice( 0, &addr ) );
    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt.CancelPairing( 0, &addr ) );

    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt.ConfirmPairing( &addr, TRUE ) );

    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt.SetPincode( &addr, "012345" ) );

    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt.SetPasskey( &addr, 2 ) );

    bt_addr_t addrs[10];
    uint8_t cnt = ( uint8_t )( sizeof( addrs ) / sizeof( addrs[0] ) );
    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt.GetConnectedList( addrs, &cnt ) );

    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt.GetPairedList( addrs, &cnt ) );

    cnt = 10;
    bt_device_t *info = ( bt_device_t * )malloc( sizeof( bt_device_t ) + ( cnt * sizeof( bt_uuid_t ) ) );
    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt.GetDeviceInfo( &addr, info, cnt ) );
    free( info );

    bt_uuid_t uuids[15];
    cnt = 15;
    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt.GetConnectedServices( addrs, uuids, &cnt ) );

    bt_uuid_t uuid;
    uuid.len = 16;
    uuid.uuid16 = 1234;
    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt.SearchServices( 0, &addr, &uuid ) );

    addr.type = BT_ADDRTYPE_BREDR;
    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt.ConnectService( addr.handle, &addr, &uuid, 0 ) );

    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt.DisconnectService( addr.handle, &addr, &uuid, 0 ) );

    uint64_t deviceId = 0;
    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt.ConnectDevice( deviceId, &addr ) );

    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt.DisconnectDevice( deviceId, &addr ) );

    // EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt.SetConmgrState( true ) );

    char name[256] = {0};
    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt.SetName( name ) );

    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt.SetDevicePriority( &addr, 3 ) );

    uuid.len = 128;

    for ( int i = 0; i < 16; i++ ) {
        uuid.uuid128[i] = i;
    }

    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt.ConnectSpp( addr.handle, &addr, &uuid, 0 ) );

    uint8_t channel = 0;
    uint8_t sdp[128] = {0};
    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt.RegisterSppServer( &uuid, sdp, 128, &channel ) );

    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt.RegisterSppServer( &uuid, NULL, 0, NULL ) );

    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt.RegisterSppServer( &uuid, NULL, 0, &channel ) );

    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt.RegisterRfcommServer( &channel ) );
    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt.DeregisterRfcommServer( channel ) );

    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt.ConnectRfcomm( 0, &addr, channel ) );

    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt.InjectDevice( 0, NULL, NULL, NULL, 0, 0, 0, 0 ) );

    uint32_t handle = 0;
    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt.AddServiceRecord( NULL, 0, &handle ) );
    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt.RemoveServiceRecord( handle ) );

    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt.DeregisterSppServer( channel ) );

    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt.RegisterEirService( NULL ) );

    EXPECT_EQ ( BT_STATUS_NO_TRANSPORT, bt.DeregisterEirService( NULL ) );

    bt_deinit();
}

TEST_F ( libbttest, bt_except_test ) {
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
    ASSERT_EQ( BT_STATUS_SUCCESS, bt_init( bt_callback, bt_cmd_callback, NULL ) );
    exceptFunc( btres, 1 );
    bt_deinit();
    BTResMock::removeInstance();
}

TEST_F ( libbttest, btmgr_evt_handler_test ) {
    // btres_t *btres;
    // BTResMock *btrestMock = BTResMock::getInstance();
    // EXPECT_CALL( *btrestMock, btres_openclient( _, _ ) ).Times( 1 )
    // .WillOnce( Return( 1 ) );
    // EXPECT_CALL( *btrestMock, btres_register_events( _, _, _ ) ).Times( 1 )
    // .WillOnce( Return( 0 ) );
    // EXPECT_CALL( *btrestMock, btres_add_conn( _, _, _, _, _ ) ).Times( 1 )
    // .WillOnce( DoAll( SaveArg<0>( &btres ),
    //                   Return( BTRES_RESULT_SUCCESS ) ) );
    // ASSERT_EQ( BT_STATUS_SUCCESS, bt_init( bt_callback, bt_cmd_callback, NULL ) );

    // void *bt = reinterpret_cast<char *>( btres ) - reinterpret_cast<size_t>( &( static_cast<bt_t *>( 0 ) )->btres );
    // bt_command_rsp_t rsp;
    // int result = btres->evt_cb( btres, 1, BT_EVENT_DISCOVERABLE,
    //                             reinterpret_cast<uint8_t *>( &rsp ), 0, bt );
    // EXPECT_EQ( result, 0 );

    // result = btres->evt_cb( btres, 1, BT_EVENT_COMMAND_RESPONSE,
    //                         reinterpret_cast<uint8_t *>( &rsp ), 0, bt );
    // EXPECT_EQ( result, 0 );

    // bt_deinit();
    // BTResMock::removeInstance();
}

class LibbtBtSppServerTest : public libbttest {
public:
    void SetUp() {
        libbttest::SetUp();
        EXPECT_CALL( *resMock, btres_openclient( _, _ ) ).Times( 1 )
        .WillOnce( Return( 1 ) );
        EXPECT_CALL( *resMock, btres_register_events( _, _, _ ) ).Times( 1 )
        .WillOnce( Return( 0 ) );
        EXPECT_CALL( *resMock, btres_add_conn( _, _, _, _, _ ) ).Times( 1 )
        .WillOnce( Return( BTRES_RESULT_SUCCESS ) );

        EXPECT_EQ( BT_STATUS_SUCCESS, bt_init( bt_callback, bt_cmd_callback, NULL ) );

        mChannel = 0;
    }

    bt_status_t registerSppServer() {
        bt_uuid_t uuid = {0};
        uuid.len = 32;
        uint8_t sdp[] = {1, 2, 3};
        uint16_t sdpLen = sizeof( sdp );
        mChannel = ( reinterpret_cast<size_t>( &uuid ) & 0xFF ) + 1;

        return bt_register_spp_server( &uuid, sdp, sdpLen, &mChannel );
    }

    void setupSuccessRegisterMocks( btres_t **btBtres, btres_t **sppBtres, int ( **sppExcept ) ( btres_t *, int ) ) {
        EXPECT_CALL( *resMock, btres_sendcmd( _, _, _, _, _, _, _ ) ).Times( 1 )
        .WillOnce( DoAll( SaveArg<0>( btBtres ),
                          Return( BTMGR_STATUS_SUCCESS ) ) );
        EXPECT_CALL( *resMock, btres_openclient( _, _ ) ).Times( 1 )
        .WillOnce( Return( 1 ) );
        EXPECT_CALL( *resMock, btres_register_events( _, _, _ ) ).Times( 1 )
        .WillOnce( Return( 0 ) );
        EXPECT_CALL( *resMock, btres_add_conn( _, _, _, _, _ ) ).Times( 1 )
        .WillOnce( DoAll( SaveArg<0>( sppBtres ),
                          SaveArg<4>( sppExcept ),
                          Return( BTRES_RESULT_SUCCESS ) ) );
    }

    void TearDown() {
        if ( mChannel ) {
            bt_deregister_spp_server( mChannel );
        }

        EXPECT_EQ( BT_STATUS_SUCCESS, bt_deinit() );
        libbttest::TearDown();
    }

    uint8_t mChannel;
};

TEST_F ( LibbtBtSppServerTest, bt_register_spp_server_null_channel_test ) {
    bt_uuid_t uuid = {0};
    uuid.len = 32;

    EXPECT_EQ( BT_STATUS_INVALID_PARM, bt_register_spp_server( &uuid, nullptr, 0, nullptr ) );
}

TEST_F ( LibbtBtSppServerTest, bt_register_spp_server_open_client_fail_test ) {
    EXPECT_CALL( *resMock, btres_sendcmd( _, _, _, _, _, _, _ ) ).Times( 1 )
    .WillOnce( Return( BTMGR_STATUS_SUCCESS ) );

    EXPECT_CALL( *resMock, btres_openclient( _, _ ) ).Times( 1 )
    .WillOnce( Return( -1 ) );

    EXPECT_EQ( BT_STATUS_FAILED, registerSppServer() );
}

TEST_F ( LibbtBtSppServerTest, btspp_evt_handler_test ) {
    btres_t *btBtres;
    btres_t *sppBtres;
    handle_evt_cb BtsppEvtHandler = nullptr;
    int ( *exceptFunc ) ( btres_t *, int );
    setupSuccessRegisterMocks( &btBtres, &sppBtres, &exceptFunc );
    BtImpl *bt = nullptr;
    EXPECT_CALL( *resMock, btres_init( _, _, _, _, _ ) )
    .WillOnce( Invoke( [&btBtres, &bt, &BtsppEvtHandler]( btres_t *btres_in, jobq_t *jobq, handle_cmd_cb cmd_cb, handle_evt_cb evt_cb, void *args ) {
        BtsppEvtHandler = evt_cb;
        btres_in->evt_cb = evt_cb;
        btBtres = btres_in;
        bt = ( BtImpl * )args;
        return BTRES_RESULT_SUCCESS;
    } ) );

    ASSERT_EQ( BTMGR_STATUS_SUCCESS, registerSppServer() );

    ASSERT_NE( nullptr, btBtres );
    ASSERT_NE( nullptr, BtsppEvtHandler );
    ASSERT_NE( nullptr, bt );

    service_info_t info;
    info.btaddr.handle = reinterpret_cast<size_t>( &info );
    info.service.len = 128;
    info.instance = 11;
    info.reason = 2;
    info.respath[0] = '\0';

    int result = 0;
    result = BtsppEvtHandler( btBtres, 1, BTRFS_EVENT_SERVICE_CONNECTED, reinterpret_cast<uint8_t *>( &info ), sizeof( info ), bt );
    EXPECT_EQ( result, 0 );
    result = BtsppEvtHandler( btBtres, 1, BTRFS_EVENT_SERVICE_DISCONNECTED, reinterpret_cast<uint8_t *>( &info ), sizeof( info ), bt );
    EXPECT_EQ( result, 0 );
}

TEST_F ( LibbtBtSppServerTest, btspp_spp_except_test ) {
    btres_t *btBtres;
    btres_t *sppBtres;
    int ( *exceptFunc ) ( btres_t *, int );
    setupSuccessRegisterMocks( &btBtres, &sppBtres, &exceptFunc );
    ASSERT_EQ( BTMGR_STATUS_SUCCESS, registerSppServer() );
    ASSERT_NE( nullptr, exceptFunc );
    exceptFunc( sppBtres, 1 );
}
