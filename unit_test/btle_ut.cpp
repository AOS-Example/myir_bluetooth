#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <stdlib.h>

#include "TestSuite.h"
#include "bt.h"
#include "btle.h"
#include "priv/bt_priv.h"
#include "priv/btle_impl.hpp"
#include "btmgr_event.h"

using namespace ::testing;
using Bluetooth::BtleImpl;
typedef struct {
    jobq_t *jobq;

    btres_t btres;
    int fd;
    btle_callback_t cb;
    btle_cmd_callback_t cmd_cb;
    void *args;
} btle_t;

void btle_callback( btle_event_t evt, const uint8_t *buf, size_t len, void *args ) {
}

void btle_cmd_callback( btle_cmd_response_t cmd_rsp, uint64_t id, bt_errcode_t errcode, const uint8_t *buf, size_t len,
                        void *args ) {
}

TEST_F( libbttest, btle_calls_fail ) {
    uint64_t id = 1;
    btle_privacy_type_t privacy{};

    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btle_set_privacy( id, privacy ) );

    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btle_start_scan( id, BTLE_SCAN_ACTIVE, 2, 2, 1, 12, 12, 12 ) );

    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btle_stop_scan( id ) );

    uint8_t data[3] = { 0 };
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btle_add_vendor_data( id, BTLE_FEATURE_MASK, 244, data ) );

    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btle_remove_vendor_data( BTLE_FEATURE_MASK ) );

    bt_uuid_t uuid = { 0 };
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btle_add_service_data( id, &uuid, 223, data ) );

    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btle_remove_service_data( &uuid ) );

    EXPECT_EQ( BT_STATUS_FAILED, btle_deinit() );
}

TEST_F( libbttest, ScanHandler_test ) {
    uint64_t id = 1;
    handle_evt_cb ScanHandler_test = nullptr;
    btres_t *btres = nullptr;
    BtleImpl *btleImpl = nullptr;
    EXPECT_EQ( 0, btle_init( NULL, NULL, NULL ) );
    EXPECT_CALL( *resMock, btres_init( _, _, _, _, _ ) )
    .WillOnce( Invoke( [&ScanHandler_test, &btres, &btleImpl]( btres_t *btres_in, jobq_t *jobq, handle_cmd_cb cmd_cb, handle_evt_cb evt_cb, void *args ) {
        ScanHandler_test = evt_cb;
        btleImpl = ( BtleImpl * )args;
        btres = btres_in;
        return BTRES_RESULT_SUCCESS;
    } ) );

    EXPECT_EQ( BT_STATUS_SUCCESS, btle_start_scan( id, BTLE_SCAN_ACTIVE, 2, 2, 1, 12, 12, 12 ) );

    ASSERT_NE( nullptr, ScanHandler_test );

    uint16_t msg = BTMGR_EVENT_LE_REPORT;
    le_report_t buff{};
    le_ext_report_t le_ext{};

    ScanHandler_test( btres, 0, msg, ( uint8_t * )&buff, sizeof( buff ), btleImpl );

    msg = BTMGR_EVENT_LE_EXT_REPORT;
    ScanHandler_test( btres, 0, msg, ( uint8_t * )&le_ext, sizeof( le_ext ), btleImpl );
    EXPECT_EQ( 0, btle_deinit() );
}

TEST_F( libbttest, btle_impl_calls_fail ) {
    uint64_t id = 1;
    btle_privacy_type_t privacy{};
    uint8_t data[3] = { 0 };
    bt_uuid_t uuid = { 0 };
    bt_addr_t addr{};
    BtleImpl btleImpl{};

    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btleImpl.SetPrivacy( id, privacy ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btleImpl.StartScan( id, BTLE_SCAN_ACTIVE, 2, 2, 1, 12, 12, 12 ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btleImpl.StopScan( id ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btleImpl.AddVendorData( id, BTLE_FEATURE_MASK, 244, data ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btleImpl.RemoveVendorData( BTLE_FEATURE_MASK ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btleImpl.AddServiceData( id, &uuid, 223, data ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btleImpl.RemoveServiceData( &uuid ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btleImpl.SetConnSearchParms( 0, 0, 0, 0, 0, 0, 0 ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btleImpl.SetConnParms( 0, 0, 0, 0, 0, 0, 0 ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btleImpl.UpdateConnParms( 0, nullptr, 0, 0, 0, 0, 0, 0 ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btleImpl.AddScanAddrFilter( &addr ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btleImpl.AddScanManufactureFilter( 1 ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btleImpl.AddScanNameFilter( "nameTest" ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btleImpl.AddScanServiceFilter( &uuid ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btleImpl.RemoveScanAddrFilter( &addr ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btleImpl.RemoveScanManufactureFilter( 1 ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btleImpl.StartAdvert( 1, BTLE_ADVERT_CONNECTABLE_UNDIRECTED, NULL, 0, 0, 0, 0, 0, 0 ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btleImpl.StopAdvert( 1 ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btleImpl.RemoveScanNameFilter( "nameTest" ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btleImpl.RemoveScanServiceFilter( &uuid ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btleImpl.AddScanProductFilter( 0 ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btleImpl.RemoveScanProductFilter( 0 ) );

    EXPECT_EQ( BT_STATUS_FAILED, btleImpl.Deinit() );
}

TEST_F( libbttest, btle_int_tests ) {
    BtleImpl btleImpl{};

    EXPECT_THAT( btleImpl.InitVersion( 0, btle_callback, btle_cmd_callback, NULL ), Eq( BT_STATUS_SUCCESS ) );
    EXPECT_THAT( btleImpl.InitVersion( 0, btle_callback, btle_cmd_callback, NULL ), Eq( BT_STATUS_IN_USE ) );
    EXPECT_THAT( btleImpl.Deinit(), Eq( BT_STATUS_SUCCESS ) );

    EXPECT_CALL( *resMock, btres_openclient( _, _ ) ).WillOnce( Return( -1 ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btle_init( btle_callback, btle_cmd_callback, NULL ) );
}

TEST_F( libbttest, btle_call_tests ) {
    EXPECT_EQ( 0, btle_init( btle_callback, btle_cmd_callback, NULL ) );
    EXPECT_EQ( BT_STATUS_IN_USE, btle_init( btle_callback, btle_cmd_callback, NULL ) );

    uint64_t id = 1;
    EXPECT_EQ( 0, btle_set_privacy( id, BTLE_PRIVACY_PUBLIC ) );

    EXPECT_EQ( 0, btle_start_scan( id, BTLE_SCAN_ACTIVE, 2, 2, 1, 12, 12, 12 ) );

    EXPECT_EQ( 0, btle_start_advert( id, BTLE_ADVERT_CONNECTABLE_DIRECTED, NULL, 1, 3, 12, 1, 2, 7 ) );

    uint8_t data[3] = { 0 };
    EXPECT_EQ( 0, btle_add_vendor_data( id, BTLE_FEATURE_MASK, 3, data ) );

    EXPECT_EQ( 0, btle_remove_vendor_data( BTLE_FEATURE_MASK ) );

    bt_uuid_t uuid = { 0 };
    EXPECT_EQ( 0, btle_add_service_data( id, &uuid, 3, data ) );

    EXPECT_EQ( 0, btle_remove_service_data( &uuid ) );
    EXPECT_EQ( BT_STATUS_SUCCESS, btle_add_scan_product_filter( 0 ) );
    EXPECT_EQ( BT_STATUS_SUCCESS, btle_remove_scan_product_filter( 0 ) );
    EXPECT_EQ( BT_STATUS_SUCCESS, btle_stop_extended_advert_set( 0, 0 ) );
    btle_advert_set_parms_t set_parms{ 0 };
    EXPECT_EQ( BT_STATUS_SUCCESS, btle_update_extended_advert_set_parms( 0, 0, &set_parms ) );
    uint8_t advert_data[64] = { 0 };
    uint16_t advert_len = sizeof( advert_data );
    uint8_t advert_priv_data[64] = { 0 };
    uint16_t advert_priv_len = sizeof( advert_priv_data );
    uint8_t scan_rsp_data[64] = { 0 };
    uint16_t scan_rsp_len = sizeof( scan_rsp_len );
    uint8_t scan_rsp_priv_data[64] = { 0 };
    uint16_t scan_rsp_priv_len = sizeof( scan_rsp_priv_data );
    uint8_t set_handle = 0;
    EXPECT_EQ( BT_STATUS_SUCCESS,
               btle_update_extended_advert_set_data(
                   0, 0, advert_len, ( uint8_t * )&advert_data, advert_priv_len, ( uint8_t * )&advert_priv_data,
                   scan_rsp_len, ( uint8_t * )&scan_rsp_data, scan_rsp_priv_len, ( uint8_t * )&scan_rsp_priv_data ) );
    EXPECT_EQ( BT_STATUS_SUCCESS, btle_start_extended_advert_set(
                   0, &set_parms, advert_len, ( uint8_t * )&advert_data, advert_priv_len,
                   ( uint8_t * )&advert_priv_data, scan_rsp_len, ( uint8_t * )&scan_rsp_data,
                   scan_rsp_priv_len, ( uint8_t * )&scan_rsp_priv_data, &set_handle ) );

    EXPECT_EQ( 0, btle_stop_scan( id ) );
    EXPECT_EQ( 0, btle_stop_advert( id ) );

    EXPECT_EQ( 0, btle_deinit() );
}

class LibbtBtleSimpleTest : public libbttest {
public:
    void SetUp() {
        libbttest::SetUp();
    }

    void btleInitSuccess( btres_t **btres, int ( **exceptFunc )( btres_t *, int ) ) {
        EXPECT_CALL( *resMock, btres_openclient( _, _ ) ).Times( 1 ).WillOnce( Return( 1 ) );
        EXPECT_CALL( *resMock, btres_register_events( _, _, _ ) ).Times( 1 ).WillOnce( Return( 0 ) );
        EXPECT_CALL( *resMock, btres_add_conn( _, _, _, _, _ ) )
        .Times( 1 )
        .WillOnce( DoAll( SaveArg<0>( btres ), SaveArg<4>( exceptFunc ), Return( BTRES_RESULT_SUCCESS ) ) );

        ASSERT_EQ( BT_STATUS_SUCCESS, btle_init( btle_callback, btle_cmd_callback, NULL ) );
    }

    void TearDown() {
        btle_deinit();
        libbttest::TearDown();
    }

};

TEST_F( LibbtBtleSimpleTest, btle_conn_parms_fail_test ) {
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btle_set_conn_search_parms( 0, 0, 0, 0, 0, 0, 0 ) );

    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btle_set_conn_parms( 0, 0, 0, 0, 0, 0, 0 ) );

    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btle_update_conn_parms( 0, nullptr, 0, 0, 0, 0, 0, 0 ) );
}

TEST_F( LibbtBtleSimpleTest, btle_conn_parms_success_test ) {
    int ( *exceptFunc )( btres_t *, int );
    btres_t *btres;
    btleInitSuccess( &btres, &exceptFunc );

    EXPECT_CALL( *resMock, btres_sendcmd( _, _, _, _, _, _, _ ) ).Times( 1 ).WillOnce( Return( BT_STATUS_SUCCESS ) );
    EXPECT_EQ( BT_STATUS_SUCCESS, btle_set_conn_search_parms( 0, 0, 0, 0, 0, 0, 0 ) );

    EXPECT_CALL( *resMock, btres_sendcmd( _, _, _, _, _, _, _ ) ).Times( 1 ).WillOnce( Return( BT_STATUS_SUCCESS ) );
    EXPECT_EQ( BT_STATUS_SUCCESS, btle_set_conn_parms( 0, 0, 0, 0, 0, 0, 0 ) );

    EXPECT_CALL( *resMock, btres_sendcmd( _, _, _, _, _, _, _ ) ).Times( 1 ).WillOnce( Return( BT_STATUS_SUCCESS ) );
    bt_addr_t addr;
    EXPECT_EQ( BT_STATUS_SUCCESS, btle_update_conn_parms( 0, &addr, 0, 0, 0, 0, 0, 0 ) );
}

TEST_F( LibbtBtleSimpleTest, btle_except_test ) {
    int ( *exceptFunc )( btres_t *, int );
    btres_t *btres;
    btleInitSuccess( &btres, &exceptFunc );

    exceptFunc( btres, 1 );
}

TEST_F( LibbtBtleSimpleTest, btle_evt_handler_test ) {
    // int ( *exceptFunc )( btres_t *, int );
    // btres_t *btres;
    // btleInitSuccess( &btres, &exceptFunc );

    // void *btle = reinterpret_cast<char *>( btres ) - reinterpret_cast<size_t>( &( static_cast<btle_t *>( 0 ) )->btres );
    // bt_command_rsp_t rsp;
    // int result = btres->evt_cb( btres, 1, BT_EVENT_DISCOVERABLE, reinterpret_cast<uint8_t *>( &rsp ), 0, btle );
    // EXPECT_EQ( result, 0 );

    // result = btres->evt_cb( btres, 1, BT_EVENT_COMMAND_RESPONSE, reinterpret_cast<uint8_t *>( &rsp ), 0, btle );
    // EXPECT_EQ( result, 0 );
}

TEST_F( LibbtBtleSimpleTest, btle_add_remove_filters ) {
    int ( *exceptFunc )( btres_t *, int );
    btres_t *btres;

    bt_addr_t addr{};
    bt_uuid_t uuid = { 0 };
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btle_add_scan_addr_filter( &addr ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btle_add_scan_manufacture_filter( 1 ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btle_add_scan_name_filter( "nameTest" ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btle_add_scan_service_filter( &uuid ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btle_remove_scan_addr_filter( &addr ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btle_remove_scan_manufacture_filter( 1 ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btle_start_advert( 1, BTLE_ADVERT_CONNECTABLE_UNDIRECTED, NULL, 0, 0, 0, 0, 0, 0 ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btle_stop_advert( 1 ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btle_remove_scan_name_filter( "nameTest" ) );
    EXPECT_EQ( BT_STATUS_NO_TRANSPORT, btle_remove_scan_service_filter( &uuid ) );

    btleInitSuccess( &btres, &exceptFunc );

    EXPECT_CALL( *resMock, btres_sendcmd( _, _, _, _, _, _, _ ) ).WillRepeatedly( Return( BT_STATUS_SUCCESS ) );

    EXPECT_EQ( BT_STATUS_SUCCESS, btle_add_scan_addr_filter( &addr ) );
    EXPECT_EQ( BT_STATUS_SUCCESS, btle_add_scan_manufacture_filter( 1 ) );
    EXPECT_EQ( BT_STATUS_SUCCESS, btle_add_scan_name_filter( "nameTest" ) );
    EXPECT_EQ( BT_STATUS_SUCCESS, btle_add_scan_service_filter( &uuid ) );
    EXPECT_EQ( BT_STATUS_SUCCESS, btle_remove_scan_addr_filter( &addr ) );
    EXPECT_EQ( BT_STATUS_SUCCESS, btle_remove_scan_manufacture_filter( 1 ) );
    EXPECT_EQ( BT_STATUS_SUCCESS, btle_remove_scan_name_filter( "nameTest" ) );
    EXPECT_EQ( BT_STATUS_SUCCESS, btle_remove_scan_service_filter( &uuid ) );
}

TEST_F( libbttest, Test_btle_BtleEvtHandler ) {
    handle_evt_cb BtleEvtHandler = nullptr;
    btres_t *btres = nullptr;
    BtleImpl *btleImpl = nullptr;

    EXPECT_CALL( *resMock, btres_init( _, _, _, _, _ ) )
    .WillOnce( Invoke( [&BtleEvtHandler, &btres, &btleImpl]( btres_t *btres_in, jobq_t *jobq, handle_cmd_cb cmd_cb,
    handle_evt_cb evt_cb, void *args ) {
        BtleEvtHandler = evt_cb;
        btleImpl = ( BtleImpl * )args;
        btres = btres_in;
        return BTRES_RESULT_SUCCESS;
    } ) );

    EXPECT_EQ( 0, btle_init( btle_callback, btle_cmd_callback, NULL ) ); // **

    ASSERT_THAT( BtleEvtHandler, Ne( nullptr ) );
    ASSERT_THAT( btleImpl, Ne( nullptr ) );

    bt_command_rsp_t buf{};
    EXPECT_THAT(
        BtleEvtHandler( btres, 0, BT_EVENT_COMMAND_RESPONSE, ( uint8_t * )&buf, sizeof( buf ), btleImpl ),
        Eq( 0 ) );  // **

    EXPECT_THAT(
        BtleEvtHandler( btres, 0, BT_CMD_RADIO_ON, ( uint8_t * )&buf, sizeof( buf ), btleImpl ),
        Eq( 0 ) );  // **

    EXPECT_EQ( 0, btle_deinit() );
}

TEST_F( libbttest, Test_btle_BtleEvtHandler_callbackNull ) {
    handle_evt_cb BtleEvtHandler = nullptr;
    btres_t *btres = nullptr;
    BtleImpl *btleImpl = nullptr;

    EXPECT_CALL( *resMock, btres_init( _, _, _, _, _ ) )
    .WillOnce( Invoke( [&BtleEvtHandler, &btres, &btleImpl]( btres_t *btres_in, jobq_t *jobq, handle_cmd_cb cmd_cb,
    handle_evt_cb evt_cb, void *args ) {
        BtleEvtHandler = evt_cb;
        btleImpl = ( BtleImpl * )args;
        btres = btres_in;
        return BTRES_RESULT_SUCCESS;
    } ) );

    EXPECT_EQ( 0, btle_init( NULL, NULL, NULL ) ); // **

    ASSERT_THAT( BtleEvtHandler, Ne( nullptr ) );
    ASSERT_THAT( btleImpl, Ne( nullptr ) );

    bt_command_rsp_t buf{};
    EXPECT_THAT(
        BtleEvtHandler( btres, 0, BT_EVENT_COMMAND_RESPONSE, ( uint8_t * )&buf, sizeof( buf ), btleImpl ),
        Eq( -1 ) );  // **

    EXPECT_EQ( 0, btle_deinit() );
}