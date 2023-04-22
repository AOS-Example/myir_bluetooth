#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <stdlib.h>

#include "TestSuite.h"
#include "bt.h"
#include "btmedia.h"

#include "priv/btmedia_impl.hpp"

using namespace ::testing;
using Bluetooth::MediaImpl;

void btmedia_callback( btmedia_event_t evt, uint8_t *buf, size_t len, void *args ) {
}

void btmedia_cmd_callback( btmedia_cmd_response_t cmd_rsp, uint64_t id, bt_errcode_t errcode, const uint8_t *buf,
                           size_t len, void *args ) {
}

TEST_F( libbttest, btmedia_calls_fail ) {
    bt_addr_t addr;
    memset( &addr, 0, sizeof( addr ) );

    btmedia_repeat_t state = BTMEDIA_REPEAT_SINGLE;
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btmedia_set_repeat( 0, state ) );

    btmedia_shuffle_t stateShuffle = BTMEDIA_SHUFFLE_ALL;
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btmedia_set_shuffle( 0, stateShuffle ) );

    uint64_t uid = 10;
    btmedia_thumbnail_t thumbnail;
    memset( &thumbnail, 0, sizeof( thumbnail ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btmedia_get_thumbnail( 0, uid, &addr, &thumbnail ) );

    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btmedia_cancel_thumbnail( 0, &addr, uid ) );

    uint16_t player_id = 3;
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btmedia_player_set_active( 0, &addr, player_id ) );

    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btmedia_player_set_browse( 0, &addr, player_id ) );

    addr.type = BT_ADDRTYPE_BREDR;
    uint16_t cnt = 0;
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btmedia_player_count( 0, &cnt ) );

    uint16_t offset = 0;
    btmedia_player_t players[3];
    uint16_t count = 3;

    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btmedia_player_get_list( 0, offset, players, &count ) );

    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btmedia_browse_up( 0 ) );

    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btmedia_browse_down( 0, uid ) );

    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btmedia_browse_root( 0 ) );

    offset = 0;
    count = 4;
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btmedia_browse_get_list( 0, offset, count ) );

    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btmedia_browse_get_path( 0 ) );

    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btmedia_browse_play_item( 0, 1, 2 ) );

    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btmedia_nowplaying_play_item( 0, 1, 2 ) );

    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btmedia_nowplaying_count( 0, &count ) );

    offset = 0;
    count = 4;
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btmedia_nowplaying_get_list( 0, offset, count ) );

    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btmedia_set_active_source( 0, &addr ) );

    uint8_t cnt8 = 0;
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btmedia_get_connected_list( &addr, &cnt8 ) );

    btmedia_local_info_t loc_info;
    memset( &loc_info, 0, sizeof( loc_info ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btmedia_get_local_info( &loc_info ) );

    btmedia_device_t dev_info;
    memset( &dev_info, 0, sizeof( dev_info ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btmedia_get_device_info( &addr, &dev_info ) );

    btmedia_state_t mediaState{};
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btmedia_device_state( &addr, mediaState ) );
}

TEST_F( libbttest, btmedia_impl_calls_fail ) {
    MediaImpl mediaImpl{};
    bt_addr_t addr{};

    btmedia_repeat_t state = BTMEDIA_REPEAT_SINGLE;
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, mediaImpl.SetRepeat( 0, state ) );

    btmedia_shuffle_t stateShuffle = BTMEDIA_SHUFFLE_ALL;
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, mediaImpl.SetShuffle( 0, stateShuffle ) );

    uint64_t handle = 10;
    btmedia_thumbnail_t thumbnail;
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, mediaImpl.GetThumbnail( 0, handle, &addr, &thumbnail ) );

    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, mediaImpl.CancelThumbnail( 0, handle, &addr ) );

    uint16_t player_id = 3;
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, mediaImpl.PlayerSetActive( 0, &addr, player_id ) );

    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, mediaImpl.PlayerSetBrowse( 0, &addr, player_id ) );

    addr.type = BT_ADDRTYPE_BREDR;
    uint16_t cnt = 0;
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, mediaImpl.PlayerCount( 0, &cnt ) );

    uint16_t offset = 0;
    btmedia_player_t players[3];
    uint16_t count = 3;

    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, mediaImpl.PlayerGetList( 0, offset, players, &count ) );

    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, mediaImpl.BrowseUp( 0 ) );

    uint64_t uid = 12345678;
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, mediaImpl.BrowseDown( 0, uid ) );

    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, mediaImpl.BrowseRoot( 0 ) );

    offset = 0;
    count = 4;
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, mediaImpl.BrowseGetList( 0, offset, count ) );

    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, mediaImpl.BrowseGetPath( 0 ) );

    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, mediaImpl.BrowsePlayItem( 0, 1, 2 ) );

    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, mediaImpl.NowplayingPlayItem( 0, 1, 2 ) );

    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, mediaImpl.NowplayingCount( 0, &count ) );

    offset = 0;
    count = 4;
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, mediaImpl.NowplayingGetList( 0, offset, count ) );

    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, mediaImpl.SetActiveSource( 0, &addr ) );

    uint8_t cnt8 = 0;
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, mediaImpl.GetConnectedList( &addr, &cnt8 ) );

    btmedia_local_info_t loc_info;
    memset( &loc_info, 0, sizeof( loc_info ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, mediaImpl.GetLocalInfo( &loc_info ) );

    btmedia_device_t dev_info;
    memset( &dev_info, 0, sizeof( dev_info ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, mediaImpl.GetDeviceInfo( &addr, &dev_info ) );

    btmedia_state_t mediaState{};
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, mediaImpl.DeviceState( &addr, mediaState ) );
}

TEST_F( libbttest, btmedia_init_test ) {
    EXPECT_EQ( 0, btmedia_init( btmedia_callback, btmedia_cmd_callback, NULL ) );
    EXPECT_EQ( BT_STATUS_IN_USE, btmedia_init( btmedia_callback, btmedia_cmd_callback, NULL ) );

    EXPECT_EQ( 0, btmedia_deinit() );
    EXPECT_EQ( BT_STATUS_FAILED, btmedia_deinit() );

    EXPECT_CALL( *resMock, btres_openclient( _, _ ) ).WillOnce( Return( -1 ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btmedia_init( btmedia_callback, btmedia_cmd_callback, NULL ) );
}

TEST_F( libbttest, btmedia_calls_success ) {
    btmedia_init( btmedia_callback, btmedia_cmd_callback, NULL );

    bt_addr_t addr;
    memset( &addr, 0, sizeof( addr ) );

    btmedia_repeat_t state = BTMEDIA_REPEAT_SINGLE;
    EXPECT_EQ( BT_STATUS_SUCCESS, btmedia_set_repeat( 0, state ) );

    btmedia_shuffle_t stateShuffle = BTMEDIA_SHUFFLE_ALL;
    EXPECT_EQ( BT_STATUS_SUCCESS, btmedia_set_shuffle( 0, stateShuffle ) );

    uint64_t uid = 52;
    btmedia_thumbnail_t thumbnail;
    memset( &thumbnail, 0, sizeof( thumbnail ) );
    EXPECT_EQ( BT_STATUS_SUCCESS, btmedia_get_thumbnail( 0, uid,  &addr, &thumbnail ) );

    EXPECT_EQ( BT_STATUS_SUCCESS, btmedia_cancel_thumbnail( 0, &addr, uid ) );

    uint16_t player_id = 3;
    EXPECT_EQ( BT_STATUS_SUCCESS, btmedia_player_set_active( 0, &addr, player_id ) );

    EXPECT_EQ( BT_STATUS_SUCCESS, btmedia_player_set_browse( 0, &addr, player_id ) );

    addr.type = BT_ADDRTYPE_BREDR;
    uint16_t cnt = 0;
    EXPECT_EQ( BT_STATUS_SUCCESS, btmedia_player_count( 0, &cnt ) );

    uint16_t offset = 0;
    btmedia_player_t players[3];
    uint16_t count = 3;

    EXPECT_EQ( BT_STATUS_SUCCESS, btmedia_player_get_list( 0, offset, players, &count ) );

    EXPECT_EQ( BT_STATUS_SUCCESS, btmedia_browse_up( 0 ) );

    EXPECT_EQ( BT_STATUS_SUCCESS, btmedia_browse_down( 0, uid ) );

    EXPECT_EQ( BT_STATUS_SUCCESS, btmedia_browse_root( 0 ) );

    offset = 0;
    count = 4;
    EXPECT_EQ( BT_STATUS_SUCCESS, btmedia_browse_get_list( 0, offset, count ) );

    EXPECT_EQ( BT_STATUS_SUCCESS, btmedia_browse_get_path( 0 ) );

    EXPECT_EQ( BT_STATUS_SUCCESS, btmedia_browse_play_item( 0, 1, 2 ) );

    EXPECT_EQ( BT_STATUS_SUCCESS, btmedia_nowplaying_play_item( 0, 1, 2 ) );

    EXPECT_EQ( BT_STATUS_SUCCESS, btmedia_nowplaying_count( 0, &count ) );

    offset = 0;
    count = 4;
    EXPECT_EQ( BT_STATUS_SUCCESS, btmedia_nowplaying_get_list( 0, offset, count ) );

    EXPECT_EQ( BT_STATUS_SUCCESS, btmedia_set_active_source( 0, &addr ) );

    uint8_t cnt8 = 0;
    EXPECT_EQ( BT_STATUS_SUCCESS, btmedia_get_connected_list( &addr, &cnt8 ) );

    btmedia_local_info_t loc_info;
    memset( &loc_info, 0, sizeof( loc_info ) );
    EXPECT_EQ( BT_STATUS_SUCCESS, btmedia_get_local_info( &loc_info ) );

    btmedia_device_t dev_info;
    memset( &dev_info, 0, sizeof( dev_info ) );
    EXPECT_EQ( BT_STATUS_SUCCESS, btmedia_get_device_info( &addr, &dev_info ) );

    btmedia_state_t mediaState{};
    EXPECT_EQ( BT_STATUS_SUCCESS, btmedia_device_state( &addr, mediaState ) );

    btmedia_deinit();
}

TEST_F( libbttest, btmedia_call_tests ) {
    bt_addr_t addr = { 0 };
    btmedia_state_t state;
    EXPECT_CALL( *resMock, btres_sendcmd( _, _, _, _, _, _, _ ) ).WillRepeatedly( Return( BT_STATUS_SUCCESS ) );

    ASSERT_EQ( 0, btmedia_init( btmedia_callback, btmedia_cmd_callback, NULL ) );
    EXPECT_EQ( BT_STATUS_SUCCESS, btmedia_send_play( 1 ) );
    EXPECT_EQ( BT_STATUS_SUCCESS, btmedia_send_pause( 1 ) );
    EXPECT_EQ( BT_STATUS_SUCCESS, btmedia_send_next( 1 ) );
    EXPECT_EQ( BT_STATUS_SUCCESS, btmedia_send_previous( 1 ) );
    EXPECT_EQ( BT_STATUS_SUCCESS, btmedia_hold_fast_forward( 1 ) );
    EXPECT_EQ( BT_STATUS_SUCCESS, btmedia_release_fast_forward( 1 ) );
    EXPECT_EQ( BT_STATUS_SUCCESS, btmedia_hold_rewind( 1 ) );
    EXPECT_EQ( BT_STATUS_SUCCESS, btmedia_release_rewind( 1 ) );
    EXPECT_EQ( BT_STATUS_SUCCESS, btmedia_get_device_state( &addr, &state ) );
    EXPECT_EQ( BT_STATUS_SUCCESS, btmedia_connect( 1, &addr ) );
    EXPECT_EQ( BT_STATUS_SUCCESS, btmedia_disconnect( 1, &addr ) );

    btmedia_deinit();

    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btmedia_send_play( 1 ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btmedia_send_pause( 1 ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btmedia_send_next( 1 ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btmedia_send_previous( 1 ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btmedia_hold_fast_forward( 1 ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btmedia_release_fast_forward( 1 ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btmedia_hold_rewind( 1 ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btmedia_release_rewind( 1 ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btmedia_get_device_state( &addr, &state ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btmedia_connect( 1, &addr ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btmedia_disconnect( 1, &addr ) );

    MediaImpl mediaImpl{};
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, mediaImpl.SendPlay( 1 ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, mediaImpl.SendPause( 1 ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, mediaImpl.SendNext( 1 ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, mediaImpl.SendPrevious( 1 ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, mediaImpl.HoldFastForward( 1 ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, mediaImpl.ReleaseFastForward( 1 ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, mediaImpl.HoldRewind( 1 ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, mediaImpl.ReleaseRewind( 1 ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, mediaImpl.GetDeviceState( &addr, &state ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, mediaImpl.Connect( 1, &addr ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, mediaImpl.Disconnect( 1, &addr ) );
}

using except_cb_t = int ( * )( btres_t *btres, int fd );

TEST_F( libbttest, Test_btmedia_MediaEvtHandler ) {
    handle_evt_cb MediaEvtHandler = nullptr;
    btres_t *btres = nullptr;
    MediaImpl *mediaImpl = nullptr;

    EXPECT_CALL( *resMock, btres_init( _, _, _, _, _ ) )
    .WillOnce(
        Invoke( [&MediaEvtHandler, &btres, &mediaImpl]( btres_t *btres_in, jobq_t *jobq, handle_cmd_cb cmd_cb,
    handle_evt_cb evt_cb, void *args ) {
        MediaEvtHandler = evt_cb;
        mediaImpl = ( MediaImpl * )args;
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

    EXPECT_EQ( 0, btmedia_init( btmedia_callback, btmedia_cmd_callback, NULL ) ); // **

    ASSERT_THAT( MediaEvtHandler, Ne( nullptr ) );
    ASSERT_THAT( PhoneExcept, Ne( nullptr ) );
    ASSERT_THAT( mediaImpl, Ne( nullptr ) );

    bt_command_rsp_t buf{};
    EXPECT_THAT(
        MediaEvtHandler( btres, 0, BT_EVENT_COMMAND_RESPONSE, ( uint8_t * )&buf, sizeof( buf ), mediaImpl ),
        Eq( 0 ) );  // **

    EXPECT_THAT(
        MediaEvtHandler( btres, 0, BT_CMD_RADIO_ON, ( uint8_t * )&buf, sizeof( buf ), mediaImpl ),
        Eq( 0 ) );  // **

    EXPECT_THAT(
        PhoneExcept( PhoneExcept_res, 0 ),
        Eq( 0 ) );  // **

    EXPECT_EQ( 0, btmedia_deinit() );
}

TEST_F( libbttest, Test_btmedia_MediaEvtHandler_callbackNull ) {

    handle_evt_cb MediaEvtHandler = nullptr;
    btres_t *btres = nullptr;
    MediaImpl *mediaImpl = nullptr;

    EXPECT_CALL( *resMock, btres_init( _, _, _, _, _ ) )
    .WillOnce(
        Invoke( [&MediaEvtHandler, &btres, &mediaImpl]( btres_t *btres_in, jobq_t *jobq, handle_cmd_cb cmd_cb,
    handle_evt_cb evt_cb, void *args ) {
        MediaEvtHandler = evt_cb;
        mediaImpl = ( MediaImpl * )args;
        btres = btres_in;
        return BTRES_RESULT_SUCCESS;
    } ) );

    EXPECT_EQ( 0, btmedia_init( NULL, NULL, NULL ) ); // **

    ASSERT_THAT( MediaEvtHandler, Ne( nullptr ) );
    ASSERT_THAT( mediaImpl, Ne( nullptr ) );

    bt_command_rsp_t buf{};
    EXPECT_THAT(
        MediaEvtHandler( btres, 0, BT_EVENT_COMMAND_RESPONSE, ( uint8_t * )&buf, sizeof( buf ), mediaImpl ),
        Eq( -1 ) );  // **

    EXPECT_EQ( 0, btmedia_deinit() );
}