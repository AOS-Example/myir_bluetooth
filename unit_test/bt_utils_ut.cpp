#include "TestSuite.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "bt_utils.h"
#include "priv/btaudiogw_priv.h"
#include "btle.h"
#include "priv/btle_priv.h"
#include "priv/btmedia_priv.h"
#include "btmessage.h"
#include "priv/btmessage_priv.h"
#include "btresmgr.h"

const char *btaudiogw_cmd_rsp_to_str( btphone_cmd_response_t cmd_rsp );

#ifdef __cplusplus
}
#endif

TEST_F( libbttest, bt_uuid16_to_str_test ) {
    EXPECT_STREQ( "UNKNOWN", bt_uuid16_to_str( 0x0000 ) );
    EXPECT_STREQ( "SDP", bt_uuid16_to_str( 0x0001 ) );
    EXPECT_STRNE( "UNKNOWN", bt_uuid16_to_str( 0x0001 ) );
}

TEST_F( libbttest, bt_uuid_to_str_test ) {
    EXPECT_EQ( NULL, bt_uuid_to_str( NULL, NULL, 40 ) );

    bt_uuid_t uuid = { 0 };

    uuid.uuid16 = 1234;
    uuid.uuid32 = 12345678;
    uuid.uuid128[0] = 1;
    uuid.uuid128[1] = 2;
    uuid.uuid128[2] = 3;
    uuid.uuid128[3] = 4;
    uuid.uuid128[4] = 5;
    uuid.uuid128[5] = 6;
    uuid.uuid128[6] = 7;
    uuid.uuid128[7] = 8;
    uuid.uuid128[8] = 9;
    uuid.uuid128[9] = 10;
    uuid.uuid128[10] = 11;
    uuid.uuid128[11] = 12;
    uuid.uuid128[12] = 13;
    uuid.uuid128[13] = 14;
    uuid.uuid128[14] = 15;
    uuid.uuid128[15] = 16;

    char buf[45] = { 0 };

    uuid.len = 16;
    EXPECT_STREQ( "0x0201", bt_uuid_to_str( &uuid, buf, 44 ) );

    uuid.len = 32;
    EXPECT_STREQ( "0x04030201", bt_uuid_to_str( &uuid, buf, 44 ) );

    uuid.len = 64;
    EXPECT_STREQ( "Unknown (64)", bt_uuid_to_str( &uuid, buf, 44 ) );

    uuid.len = 128;
    EXPECT_STREQ( "01020304-0506-0708-090a-0b0c0d0e0f10", bt_uuid_to_str( &uuid, buf, 44 ) );
}

TEST_F( libbttest, btaudiogw_cmd_rsp_to_str_test ) {
    EXPECT_STREQ( "UNKNOWN", btaudiogw_cmd_rsp_to_str( BTPHONE_RSP_HANGUP ) );
}

TEST_F( libbttest, btle_cmd_to_str_test ) {
    EXPECT_STREQ( "BTLE_CMD_START_SCAN", btle_cmd_to_str( BTLE_CMD_START_SCAN ) );
}

TEST_F( libbttest, btle_cmd_rsp_to_str_test ) {
    EXPECT_STREQ( "BTLE_RSP_START_SCAN", btle_cmd_rsp_to_str( BTLE_RSP_START_SCAN ) );
    EXPECT_STREQ( "BTLE_RSP_UPDATE_CONN_PARMS", btle_cmd_rsp_to_str( BTLE_RSP_UPDATE_CONN_PARMS ) );
}

TEST_F( libbttest, btphone_number_type_to_str_test ) {
    EXPECT_STREQ( "BTPHONE_NUMBER_TYPE_INVALID", btphone_number_type_to_str( BTPHONE_NUMBER_TYPE_INVALID ) );
    EXPECT_STREQ( "BTPHONE_NUMBER_TYPE_MULTIPLE", btphone_number_type_to_str( BTPHONE_NUMBER_TYPE_MULTIPLE ) );
}

TEST_F( libbttest, btphone_contact_type_to_str_test ) {
    EXPECT_STREQ( "BTPHONE_CONTACT_TYPE_PHONEBOOK", btphone_contact_type_to_str( BTPHONE_CONTACT_TYPE_PHONEBOOK ) );
    EXPECT_STREQ( "BTPHONE_CONTACT_TYPE_MISSED", btphone_contact_type_to_str( BTPHONE_CONTACT_TYPE_MISSED ) );
}

TEST_F( libbttest, btmessage_cmd_rsp_to_str_test ) {
    EXPECT_STREQ( "BTMESSAGE_RSP_GET_MESSAGE_LISTING", btmessage_cmd_rsp_to_str( BTMESSAGE_RSP_GET_MESSAGE_LISTING ) );
    EXPECT_STREQ( "BTMESSAGE_RSP_DISCONNECT", btmessage_cmd_rsp_to_str( BTMESSAGE_RSP_DISCONNECT ) );
}

TEST_F( libbttest, btmessage_cmd_to_str_test ) {
    EXPECT_STREQ( "BTMESSAGE_CMD_GET_CONNECTED_LIST", btmessage_cmd_to_str( BTMESSAGE_CMD_GET_CONNECTED_LIST ) );
    EXPECT_STREQ( "BTMESSAGE_CMD_DISCONNECT", btmessage_cmd_to_str( BTMESSAGE_CMD_DISCONNECT ) );
}

TEST_F( libbttest, btmessage_event_to_str_test ) {
    EXPECT_STREQ( "BTMESSAGE_EVENT_STACK_FAULT", btmessage_event_to_str( BTMESSAGE_EVENT_STACK_FAULT ) );
    EXPECT_STREQ( "BTMESSAGE_EVENT_CONVERSATION_CHANGED",
                  btmessage_event_to_str( BTMESSAGE_EVENT_CONVERSATION_CHANGED ) );
}

TEST_F( libbttest, btmedia_cmd_rsp_to_str_test ) {
    EXPECT_STREQ( "BTMEDIA_RSP_SEND_PLAY", btmedia_cmd_rsp_to_str( BTMEDIA_RSP_SEND_PLAY ) );
    EXPECT_STREQ( "BTMEDIA_RSP_DISCONNECT", btmedia_cmd_rsp_to_str( BTMEDIA_RSP_DISCONNECT ) );
}

TEST_F( libbttest, btmedia_cmd_to_str_test ) {
    EXPECT_STREQ( "BTMEDIA_CMD_SEND_PLAY", btmedia_cmd_to_str( BTMEDIA_CMD_SEND_PLAY ) );
    EXPECT_STREQ( "BTMEDIA_CMD_DISCONNECT", btmedia_cmd_to_str( BTMEDIA_CMD_DISCONNECT ) );
}

TEST_F( libbttest, btmedia_event_to_str_test ) {
    EXPECT_STREQ( "BTMEDIA_EVENT_STACK_FAULT", btmedia_event_to_str( BTMEDIA_EVENT_STACK_FAULT ) );
    EXPECT_STREQ( "BTMEDIA_EVENT_SHUFFLE_STATE", btmedia_event_to_str( BTMEDIA_EVENT_SHUFFLE_STATE ) );
}

TEST_F( libbttest, btmedia_player_major_to_str_test ) {
    EXPECT_STREQ( "BTMEDIA_MAJOR_PLAYER_AUDIO", btmedia_player_major_to_str( BTMEDIA_MAJOR_PLAYER_AUDIO ) );
    EXPECT_STREQ( "BTMEDIA_MAJOR_PLAYER_VID_BCAST", btmedia_player_major_to_str( BTMEDIA_MAJOR_PLAYER_VID_BCAST ) );
}

TEST_F( libbttest, btmedia_player_sub_to_str_test ) {
    EXPECT_STREQ( "BTMEDIA_PLAYER_SUBTYPE_NONE", btmedia_player_sub_to_str( BTMEDIA_PLAYER_SUBTYPE_NONE ) );
    EXPECT_STREQ( "BTMEDIA_PLAYER_SUBTYPE_PODCAST", btmedia_player_sub_to_str( BTMEDIA_PLAYER_SUBTYPE_PODCAST ) );
}

TEST_F( libbttest, btmedia_browse_type_to_str_test ) {
    EXPECT_STREQ( "BTMEDIA_BROWSE_TYPE_FOLDER_MIXED", btmedia_browse_type_to_str( BTMEDIA_BROWSE_TYPE_FOLDER_MIXED ) );
    EXPECT_STREQ( "BTMEDIA_BROWSE_TYPE_MEDIA_VIDEO", btmedia_browse_type_to_str( BTMEDIA_BROWSE_TYPE_MEDIA_VIDEO ) );
}

TEST_F( libbttest, btmedia_play_state_to_str_test ) {
    EXPECT_STREQ( "BTMEDIA_PLAY_STATE_STOPPED", btmedia_play_state_to_str( BTMEDIA_PLAY_STATE_STOPPED ) );
    EXPECT_STREQ( "BTMEDIA_PLAY_STATE_ERROR", btmedia_play_state_to_str( BTMEDIA_PLAY_STATE_ERROR ) );
}

TEST_F( libbttest, btmedia_player_feature_to_str_test ) {
    EXPECT_STREQ( "BTMEDIA_PLAYER_0_FEAT_SELECT", btmedia_player_feature_to_str( 0, 0 ) );
    EXPECT_STREQ( "BTMEDIA_PLAYER_0_FEAT_LEFT_UP", btmedia_player_feature_to_str( 0, 7 ) );
    EXPECT_STREQ( "BTMEDIA_PLAYER_8_SEARCH_WHEN_ADDRESSED", btmedia_player_feature_to_str( 8, 0 ) );
    EXPECT_STREQ( "BTMEDIA_PLAYER_8_UID_COVER_ART", btmedia_player_feature_to_str( 8, 4 ) );
    EXPECT_STREQ( "Unknown feature", btmedia_player_feature_to_str( 8, 5 ) );
    EXPECT_STREQ( "Unknown feature", btmedia_player_feature_to_str( 16, 5 ) );
    EXPECT_STREQ( "Unknown feature", btmedia_player_feature_to_str( 0, 8 ) );
}

TEST_F( libbttest, btgattc_cmd_rsp_to_str_test ) {
    EXPECT_STREQ( "BTGATTC_RSP_CONNECT_SERVICE", btgattc_cmd_rsp_to_str( BTGATTC_RSP_CONNECT_SERVICE ) );
    EXPECT_STREQ( "BTGATTC_RSP_WRITE_VALUE", btgattc_cmd_rsp_to_str( BTGATTC_RSP_WRITE_VALUE ) );
}

TEST_F( libbttest, btgattc_event_to_str_test ) {
    EXPECT_STREQ( "BTGATTC_EVENT_STACK_FAULT", btgattc_event_to_str( BTGATTC_EVENT_STACK_FAULT ) );
    EXPECT_STREQ( "BTGATTC_EVENT_INDICATION", btgattc_event_to_str( BTGATTC_EVENT_INDICATION ) );
}

TEST_F( libbttest, btgattc_attr_type_to_str_test ) {
    EXPECT_STREQ( "BTGATTC_ATTRIBUTE_SERVICE", btgattc_attr_type_to_str( BTGATTC_ATTRIBUTE_SERVICE ) );
    EXPECT_STREQ( "BTGATTC_ATTRIBUTE_INCLUDE_SERVICE", btgattc_attr_type_to_str( BTGATTC_ATTRIBUTE_INCLUDE_SERVICE ) );
}

TEST_F( libbttest, btgattc_service_type_to_str_test ) {
    EXPECT_STREQ( "BTGATTC_SERVICE_TYPE_SECONDARY", btgattc_service_type_to_str( BTGATTC_SERVICE_TYPE_SECONDARY ) );
    EXPECT_STREQ( "BTGATTC_SERVICE_TYPE_PRIMARY", btgattc_service_type_to_str( BTGATTC_SERVICE_TYPE_PRIMARY ) );
}

TEST_F( libbttest, btaudiogw_get_cmd_evt_map_test ) {
    const btres_map_t *map;
    EXPECT_EQ( 0, btaudiogw_get_cmd_map( &map ) );
    EXPECT_EQ( 15, btaudiogw_get_evt_map( &map ) );
}