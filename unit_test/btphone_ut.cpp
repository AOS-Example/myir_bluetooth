#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <stdlib.h>

#include "priv/btphone_impl.hpp"
extern "C" {
#include "priv/btphone_v1_priv.h"
}

#include "TestSuite.h"
#include "bt.h"
#include "btphone.h"
#include "btphone.hpp"

using namespace ::testing;
using Bluetooth::Phone;
using Bluetooth::PhoneImpl;

void btphone_callback( btphone_event_t evt, uint8_t *buf, size_t len, void *args ) {
}

void btphone_cmd_callback( btphone_cmd_response_t cmd_rsp, uint64_t id, bt_errcode_t errcode, const uint8_t *buf,
                           size_t len, void *args ) {
}

TEST_F( libbttest, btphone_calls_fail ) {
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btphone_hangup( 0 ) );

    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btphone_hangup_index( 0, 0 ) );

    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btphone_hangup_all( 0 ) );

    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btphone_answer( 0 ) );

    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btphone_reject( 0 ) );

    bt_addr_t addr;
    memset( &addr, 0, sizeof( addr ) );
    addr.type = BT_ADDRTYPE_BREDR;
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btphone_dial( 0, &addr, "123-123-123" ) );

    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btphone_swap_calls( 0 ) );

    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btphone_merge_calls( 0 ) );

    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btphone_redial( 0 ) );

    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btphone_voice_recognition( 0, &addr, TRUE ) );

    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btphone_send_dtmf( 0, '5' ) );

    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btphone_audio( 0, TRUE ) );

    btphone_call_t btphoneCall{};
    size_t callListCnt = 0;
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btphone_get_call_list( 0, &btphoneCall, &callListCnt ) );

    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btphone_emergency_dial( 0, "999-999-999" ) );

    bt_addr_t addrs[10];
    uint8_t cnt = ( uint8_t )( sizeof( addrs ) / sizeof( addrs[0] ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btphone_get_connected_list( addrs, &cnt ) );

    btphone_local_state_t state;
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btphone_get_local_state( &state ) );

    btphone_device_t info;
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btphone_get_device_info( &addr, &info ) );

    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btphone_download_contacts( 0, &addr ) );

    btphone_contact_sort_t sort = BTPHONE_CONTACT_SORT_ID;
    uint16_t offset = 0;
    uint16_t count = 100;
    btphone_contact_filter_mask_t filter = BTPHONE_CONTACT_FILTER_MASK_NO_FILTER;
    addr.type = BT_ADDRTYPE_BREDR;
    btphone_pbk_index_t pbk = ( btphone_pbk_index_t )3;
    btphone_contact_name_t *contacts = ( btphone_contact_name_t * )calloc( count, sizeof( btphone_contact_name_t ) );
    uint16_t cnt16 = 0;
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btphone_get_contact_list( &addr, pbk, sort, filter, offset, contacts, &cnt16 ) );
    free( contacts );

    btphone_smart_search_t smart;
    memset( &smart, 0, sizeof( smart ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btphone_get_search_info( 0, &addr, pbk, sort, filter, "111", &smart ) );

    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btphone_search( 0, &addr, pbk, sort, filter, offset, "111", contacts, &cnt16 ) );

    int32_t id = 6;
    btphone_contact_t contact;
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btphone_get_contact( &addr, pbk, id, &contact ) );

    EXPECT_EQ( BT_STATUS_FAILED, btphone_deinit() );
}

TEST_F( libbttest, btphone_calls_fail_btphone_impl ) {
    PhoneImpl phoneImpl{};

    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, phoneImpl.Hangup( 0 ) );

    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, phoneImpl.HangupIndex( 0, 0 ) );

    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, phoneImpl.HangupAll( 0 ) );

    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, phoneImpl.Answer( 0 ) );

    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, phoneImpl.Reject( 0 ) );

    bt_addr_t addr;
    memset( &addr, 0, sizeof( addr ) );
    addr.type = BT_ADDRTYPE_BREDR;
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, phoneImpl.Dial( 0, &addr, "123-123-123" ) );

    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, phoneImpl.SwapCalls( 0 ) );

    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, phoneImpl.MergeCalls( 0 ) );

    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, phoneImpl.Redial( 0 ) );

    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, phoneImpl.VoiceRecognition( 0, &addr, TRUE ) );

    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, phoneImpl.SendDtmf( 0, '5' ) );

    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, phoneImpl.Audio( 0, TRUE ) );

    btphone_call_t btphoneCall{};
    size_t callListCnt = 0;
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, phoneImpl.GetCallList( 0, &btphoneCall, &callListCnt ) );

    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, phoneImpl.EmergencyDial( 0, "999-999-999" ) );

    btphone_state_t state = BTPHONE_STATE_ACTIVE;
    const char *number = "0123456789";
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, phoneImpl.SetPbkState( &addr, BTPHONE_PBK_STATE_IDLE ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, phoneImpl.DeviceState( &addr, BTPHONE_STATE_ACTIVE ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, phoneImpl.GetDeviceState( &addr, &state ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, phoneImpl.Connect( 1, &addr ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, phoneImpl.Disconnect( 1, &addr ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, phoneImpl.DialContact( 1, &addr, number, BTPHONE_PBK_INDEX_INCALLS, 1 ) );

    bt_addr_t addrs[10];
    uint8_t cnt = ( uint8_t )( sizeof( addrs ) / sizeof( addrs[0] ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, phoneImpl.GetConnectedList( addrs, &cnt ) );

    btphone_local_state_t lstate;
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, phoneImpl.GetLocalState( &lstate ) );

    btphone_device_t info;
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, phoneImpl.GetDeviceInfo( &addr, &info ) );

    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, phoneImpl.DownloadContacts( 0, &addr ) );

    btphone_contact_sort_t sort = BTPHONE_CONTACT_SORT_ID;
    uint16_t offset = 0;
    uint16_t count = 100;
    btphone_contact_filter_mask_t filter = BTPHONE_CONTACT_FILTER_MASK_NO_FILTER;
    addr.type = BT_ADDRTYPE_BREDR;
    btphone_pbk_index_t pbk = ( btphone_pbk_index_t )3;
    btphone_contact_name_t *contacts = ( btphone_contact_name_t * )calloc( count, sizeof( btphone_contact_name_t ) );
    uint16_t cnt16 = 0;
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, phoneImpl.GetContactList( &addr, pbk, sort, BTPHONE_CONTACT_NAME_START_ALPHA, filter, offset, contacts, &cnt16 ) );
    free( contacts );

    btphone_smart_search_t smart;
    memset( &smart, 0, sizeof( smart ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, phoneImpl.GetSearchInfo( 0, &addr, pbk, sort, filter, "111", &smart ) );

    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, phoneImpl.Search( 0, &addr, pbk, sort, BTPHONE_CONTACT_NAME_START_ALPHA, filter, offset, "111", contacts,
               &cnt16 ) );

    int32_t id = 6;
    btphone_contact_t contact;
    size_t count_size = 100;
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, phoneImpl.GetContact( &addr, pbk, id, &contact ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, phoneImpl.GetContactPhoto( 0, &addr, pbk, id ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, phoneImpl.GetContactSet( &addr, pbk, sort, BTPHONE_CONTACT_NAME_START_ALPHA, filter, offset, &contact, &cnt16 ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, phoneImpl.DeleteContacts( 0, &addr ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, phoneImpl.SendHfIndicator( 0, &addr, 0 ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, phoneImpl.DeviceGetCallList( 0, &addr, &btphoneCall, &count_size ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, phoneImpl.DeviceHangup( 0, &addr ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, phoneImpl.DeviceAnswer( 0, &addr ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, phoneImpl.DeviceSwapCalls( 0, &addr ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, phoneImpl.DeviceMergeCalls( 0, &addr ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, phoneImpl.DeviceSendDtmf( 0, &addr, 0x00 ) );
}

TEST_F( libbttest, btphone_init_test ) {
    EXPECT_EQ( 0, btphone_init( btphone_callback, btphone_cmd_callback, NULL ) );
    EXPECT_EQ( BT_STATUS_IN_USE, btphone_init( btphone_callback, btphone_cmd_callback, NULL ) );

    EXPECT_EQ( 0, btphone_deinit() );
    EXPECT_EQ( BT_STATUS_FAILED, btphone_deinit() );

    EXPECT_CALL( *resMock, btres_openclient( _, _ ) ).WillOnce( Return( -1 ) );
    EXPECT_EQ( BT_STATUS_NOT_READY, btphone_init( btphone_callback, btphone_cmd_callback, NULL ) );
}

using except_cb_t = int ( * )( btres_t *btres, int fd );

TEST_F( libbttest, Test_btphone_EvtHandler ) {
    handle_evt_cb EvtHandler = nullptr;
    btres_t *btres = nullptr;
    PhoneImpl *phoneImpl = nullptr;

    EXPECT_CALL( *resMock, btres_init( _, _, _, _, _ ) )
    .WillOnce( Invoke( [&EvtHandler, &btres, &phoneImpl]( btres_t *btres_in, jobq_t *jobq, handle_cmd_cb cmd_cb,
    handle_evt_cb evt_cb, void *args ) {
        EvtHandler = evt_cb;
        phoneImpl = ( PhoneImpl * )args;
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

    EXPECT_EQ( 0, btphone_init( btphone_callback, btphone_cmd_callback, NULL ) ); // **

    ASSERT_THAT( EvtHandler, Ne( nullptr ) );
    ASSERT_THAT( PhoneExcept, Ne( nullptr ) );
    ASSERT_THAT( phoneImpl, Ne( nullptr ) );

    bt_command_rsp_t buf{};
    EXPECT_THAT(
        EvtHandler( btres, 0, BT_EVENT_COMMAND_RESPONSE, ( uint8_t * )&buf, sizeof( buf ), phoneImpl ),
        Eq( 0 ) );  // **

    EXPECT_THAT(
        EvtHandler( btres, 0, BT_CMD_RADIO_ON, ( uint8_t * )&buf, sizeof( buf ), phoneImpl ),
        Eq( 0 ) );  // **

    EXPECT_THAT(
        PhoneExcept( PhoneExcept_res, 0 ),
        Eq( 0 ) );  // **

    EXPECT_EQ( 0, btphone_deinit() );
}

TEST_F( libbttest, Test_btphone_EvtHandler_callbackNull ) {
    handle_evt_cb EvtHandler = nullptr;
    btres_t *btres = nullptr;
    PhoneImpl *phoneImpl = nullptr;

    EXPECT_CALL( *resMock, btres_init( _, _, _, _, _ ) )
    .WillOnce( Invoke( [&EvtHandler, &btres, &phoneImpl]( btres_t *btres_in, jobq_t *jobq, handle_cmd_cb cmd_cb,
    handle_evt_cb evt_cb, void *args ) {
        EvtHandler = evt_cb;
        phoneImpl = ( PhoneImpl * )args;
        btres = btres_in;
        return BTRES_RESULT_SUCCESS;
    } ) );

    EXPECT_EQ( 0, btphone_init( NULL, NULL, NULL ) ); // **

    ASSERT_THAT( EvtHandler, Ne( nullptr ) );
    ASSERT_THAT( phoneImpl, Ne( nullptr ) );

    bt_command_rsp_t buf{};
    EXPECT_THAT(
        EvtHandler( btres, 0, BT_EVENT_COMMAND_RESPONSE, ( uint8_t * )&buf, sizeof( buf ), phoneImpl ),
        Eq( -1 ) );  // **

    EXPECT_EQ( 0, btphone_deinit() );
}

TEST_F( libbttest, btphone_calls_success ) {
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

    btphone_call_t btphoneCall{};
    size_t callListCnt = 0;
    EXPECT_EQ( BT_STATUS_SUCCESS, btphone_get_call_list( 0, &btphoneCall, &callListCnt ) );

    EXPECT_EQ( BT_STATUS_SUCCESS, btphone_emergency_dial( 0, "999-999-999" ) );

    bt_addr_t addrs[10];
    uint8_t cnt = ( uint8_t )( sizeof( addrs ) / sizeof( addrs[0] ) );
    EXPECT_EQ( BT_STATUS_SUCCESS, btphone_get_connected_list( addrs, &cnt ) );

    btphone_local_state_t state;
    EXPECT_EQ( BT_STATUS_SUCCESS, btphone_get_local_state( &state ) );

    btphone_device_t info;
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
    btphone_contact_t contact;
    size_t count_size = 100;
    EXPECT_EQ( BT_STATUS_SUCCESS, btphone_get_contact( &addr, pbk, id, &contact ) );

    EXPECT_EQ( BT_STATUS_SUCCESS, btphone_get_contact_photo( 0, &addr, pbk, id ) );
    EXPECT_EQ( BT_STATUS_SUCCESS, btphone_get_contact_set( &addr, pbk, sort, filter, offset, &contact, &count ) );
    EXPECT_EQ( BT_STATUS_SUCCESS, btphone_delete_contacts( id, &addr ) );
    EXPECT_EQ( BT_STATUS_SUCCESS, btphone_send_hf_indicator( id, &addr, 0 ) );
    EXPECT_EQ( BT_STATUS_SUCCESS, btphone_device_get_call_list( id, &addr, &btphoneCall, &count_size ) );
    EXPECT_EQ( BT_STATUS_SUCCESS, btphone_device_hangup( id, &addr ) );
    EXPECT_EQ( BT_STATUS_SUCCESS, btphone_device_answer( id, &addr ) );
    EXPECT_EQ( BT_STATUS_SUCCESS, btphone_device_swap_calls( id, &addr ) );
    EXPECT_EQ( BT_STATUS_SUCCESS, btphone_device_merge_calls( id, &addr ) );
    EXPECT_EQ( BT_STATUS_SUCCESS, btphone_device_send_dtmf( id, &addr, 0x00 ) );

    btphone_deinit();
}

TEST_F( libbttest, btphone_call_tests ) {
    bt_addr_t addr;
    btphone_state_t state = BTPHONE_STATE_ACTIVE;
    const char *number = "0123456789";
    EXPECT_CALL( *resMock, btres_sendcmd( _, _, _, _, _, _, _ ) ).WillRepeatedly( Return( BT_STATUS_SUCCESS ) );
    btphone_call_t btphoneCall{};
    size_t count_size = 0;
    btphone_pbk_index_t pbk = ( btphone_pbk_index_t )3;
    btphone_contact_sort_t sort = BTPHONE_CONTACT_SORT_ID;
    btphone_contact_filter_mask_t filter = BTPHONE_CONTACT_FILTER_MASK_NO_FILTER;
    uint16_t offset = 0;
    btphone_contact_t contact{};
    uint16_t count = 100;
    btphone_contact_start_letter_t letter{};
    btphone_alpha_index_t alpha{};
    uint16_t photo_size = 0;
    btphone_contact_photo_t photo{};
    char search_str[8] {};
    char first[8] {};
    char last[8] {};
    bt_filter_mask_t mask{};
    btphone_contact_name_t contact_name{};
    uint8_t search_attr = 0;

    btphone_init( btphone_callback, btphone_cmd_callback, NULL );
    EXPECT_EQ( BT_STATUS_SUCCESS, btphone_set_pbk_state( &addr, BTPHONE_PBK_STATE_IDLE ) );
    EXPECT_EQ( BT_STATUS_SUCCESS, btphone_device_state( &addr, BTPHONE_STATE_ACTIVE ) );
    EXPECT_EQ( BT_STATUS_SUCCESS, btphone_get_device_state( &addr, &state ) );
    EXPECT_EQ( BT_STATUS_SUCCESS, btphone_connect( 1, &addr ) );
    EXPECT_EQ( BT_STATUS_SUCCESS, btphone_device_redial( 1, &addr ) );
    EXPECT_EQ( BT_STATUS_SUCCESS, btphone_device_audio( 1, &addr, true ) );
    EXPECT_EQ( BT_STATUS_SUCCESS, btphone_disconnect( 1, &addr ) );
    EXPECT_EQ( BT_STATUS_SUCCESS, btphone_dial_contact( 1, &addr, number, BTPHONE_PBK_INDEX_INCALLS, 1 ) );
    EXPECT_EQ( BT_STATUS_SUCCESS, btphone_get_contact_first_letter_indexes( &addr, pbk, sort, letter, filter, offset, &alpha, &count ) );
    EXPECT_EQ( BT_STATUS_SUCCESS, btphone_get_fav_photo( &addr, pbk, 0, photo_size, &photo, offset ) );

    EXPECT_EQ( BT_STATUS_SUCCESS, btphone_get_contact_photo_vcardsearch( 0, &addr, pbk, search_attr, search_str, first, last ) );
    EXPECT_EQ( BT_STATUS_SUCCESS, btphone_get_contact_count( &addr, pbk, 0, &count ) );
    EXPECT_EQ( BT_STATUS_SUCCESS, btphone_set_filter_mask( &mask ) );
    EXPECT_EQ( BT_STATUS_SUCCESS, btphone_get_contact_set_ex( &addr, pbk, sort, letter, filter, offset, &contact, &count ) );
    EXPECT_EQ( BT_STATUS_SUCCESS, btphone_get_contact_list_ex( &addr, pbk, sort, letter, filter, offset, &contact_name, &count ) );
    EXPECT_EQ( BT_STATUS_SUCCESS, btphone_search_ex( 0, &addr, pbk, sort, letter, filter, offset, number, &contact_name, &count ) );

    btphone_deinit();
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btphone_set_pbk_state( &addr, BTPHONE_PBK_STATE_IDLE ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btphone_device_state( &addr, BTPHONE_STATE_ACTIVE ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btphone_get_device_state( &addr, &state ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btphone_connect( 1, &addr ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btphone_disconnect( 1, &addr ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btphone_device_answer( 1, &addr ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btphone_device_swap_calls( 1, &addr ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btphone_device_hangup( 1, &addr ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btphone_device_merge_calls( 1, &addr ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btphone_device_redial( 1, &addr ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btphone_send_hf_indicator( 1, &addr, 0 ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btphone_device_send_dtmf( 1, &addr, 0x00 ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btphone_device_audio( 1, &addr, true ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btphone_delete_contacts( 1, &addr ) );

    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btphone_get_contact_set( &addr, pbk, sort, filter, offset, &contact, &count ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btphone_get_contact_first_letter_indexes( &addr, pbk, sort, letter, filter, offset, &alpha, &count ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btphone_get_fav_photo( &addr, pbk, 0, photo_size, &photo, offset ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btphone_get_contact_photo( 0, &addr, pbk, 0 ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btphone_device_get_call_list( 0, &addr, &btphoneCall, &count_size ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btphone_dial_contact( 1, &addr, number, BTPHONE_PBK_INDEX_INCALLS, 1 ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btphone_get_contact_photo_vcardsearch( 0, &addr, pbk, search_attr, search_str, first, last ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btphone_get_contact_count( &addr, pbk, 0, &count ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btphone_set_filter_mask( &mask ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btphone_get_contact_set_ex( &addr, pbk, sort, letter, filter, offset, &contact, &count ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btphone_get_contact_list_ex( &addr, pbk, sort, letter, filter, offset, &contact_name, &count ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btphone_search_ex( 0, &addr, pbk, sort, letter, filter, offset, number, &contact_name, &count ) );
}

TEST_F( libbttest, btphone_v1_device_test ) {
    btphone_device_t device{};
    btphone_device_v1_t device_v1{};
    btphone_v1_device( &device, &device_v1 );
}