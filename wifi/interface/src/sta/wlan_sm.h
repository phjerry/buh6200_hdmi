#ifndef __WLAN_SM_H__
#define __WLAN_SM_H__

#include "hi_wlan_sta.h"
#include "hi_wlan_p2p.h"

#define BASE     0x00020000
#define BASE_E   0x00024000

#define CMD_STA_OPEN                       BASE + 1
#define CMD_STA_CLOSE                      BASE + 2
#define CMD_STA_START                      BASE + 3
#define CMD_STA_STOP                       BASE + 4
#define CMD_STA_SCAN                       BASE + 5
#define CMD_STA_CONNECT                    BASE + 6
#define CMD_STA_DISCONNECT                 BASE + 7
#define CMD_P2P_OPEN                       BASE + 8
#define CMD_P2P_CLOSE                      BASE + 9
#define CMD_P2P_START                      BASE + 10
#define CMD_P2P_STOP                       BASE + 11
#define CMD_P2P_FIND                       BASE + 12
#define CMD_P2P_CONNECT                    BASE + 13
#define CMD_P2P_DISCONNECT                 BASE + 14
#define CMD_P2P_SET_DEVICE_INFO            BASE + 15
#define CMD_P2P_LISTEN                     BASE + 16
#define CMD_P2P_GROUP_ADD                  BASE + 17
#define CMD_STA_P2P_CLOSE                  BASE + 18
#define CMD_STA_P2P_START                  BASE + 19
#define CMD_P2P_FLUSH                      BASE + 20
#define CMD_P2P_SET_DEVICE_NAME            BASE + 21
#define CMD_END                            BASE + 1024


#define SCAN_RESULTS_EVENT                 BASE_E + 2
#define STA_CONNECTED_EVENT                BASE_E + 3
#define STA_CONNECTING_EVENT               BASE_E + 4
#define STA_DISCONNECTED_EVENT             BASE_E + 5
#define P2P_DEVICE_FOUND_EVENT             BASE_E + 21
#define P2P_DEVICE_LOST_EVENT              BASE_E + 22
#define P2P_GO_NEGOTIATION_REQUEST_EVENT   BASE_E + 23
#define P2P_GO_NEGOTIATION_SUCCESS_EVENT   BASE_E + 25
#define P2P_GO_NEGOTIATION_FAILURE_EVENT   BASE_E + 26
#define P2P_GROUP_FORMATION_SUCCESS_EVENT  BASE_E + 27
#define P2P_GROUP_FORMATION_FAILURE_EVENT  BASE_E + 28
#define P2P_GROUP_STARTED_EVENT            BASE_E + 29
#define P2P_GROUP_REMOVED_EVENT            BASE_E + 30
#define P2P_INVITATION_RECEIVED_EVENT      BASE_E + 31
#define P2P_INVITATION_RESULT_EVENT        BASE_E + 32
#define P2P_PROV_DISC_PBC_REQ_EVENT        BASE_E + 33
#define P2P_PROV_DISC_PBC_RSP_EVENT        BASE_E + 34
#define P2P_PROV_DISC_ENTER_PIN_EVENT      BASE_E + 35
#define P2P_PROV_DISC_SHOW_PIN_EVENT       BASE_E + 36
#define P2P_FIND_STOPPED_EVENT             BASE_E + 37
#define P2P_SERV_DISC_RESP_EVENT           BASE_E + 38
#define P2P_PROV_DISC_FAILURE_EVENT        BASE_E + 39
#define AP_STA_DISCONNECTED_EVENT          BASE_E + 41
#define AP_STA_CONNECTED_EVENT             BASE_E + 42
#define SUPP_STOPPED_EVENT                 BASE_E + 50
#define DRIVER_STOPPED_EVENT               BASE_E + 51
#define WPS_EVENT_TIMEOUT                  BASE_E + 52
#define WPS_EVENT_OVERLAP                  BASE_E + 53
#define EVENT_END                          BASE_E + 1024


typedef enum {
    WLAN_STATE_CLOSED,
    WLAN_STATE_DRIVER_LOADDED,
    WLAN_STATE_STA_STARTED,
    WLAN_STATE_STA_CONNECTED,
    WLAN_STATE_P2P_STARTED,
    WLAN_STATE_P2P_USER_AUTHORIZING_INVITATION,
    WLAN_STATE_P2P_USER_AUTHORIZING_JOIN,
    WLAN_STATE_P2P_GROUP_NEGOTIATION,
    WLAN_STATE_P2P_GROUP_CREATED,
    WLAN_STATE_BUTT,
} WLAN_STATE_E;

/** configured network information */
typedef struct hiHI_WLAN_P2P_ProvDisc_S
{
    hi_wlan_p2p_device_info device;
    hi_wlan_wps_method wps_method;
    hi_char pin[9];
} HI_WLAN_P2P_ProvDisc_S;

typedef struct {
    WLAN_STATE_E state;
    hi_s32 (*process)(hi_s32 msg, hi_void *data,
                        hi_char *reply, hi_s32 *len);
} WLAN_SM_S;

hi_s32 wlan_sm_init(hi_void);

hi_void wlan_sm_deinit(hi_void);

hi_s32 wlan_sm_send_message(hi_s32 msg, hi_void *data,
                              hi_char *reply, hi_s32 *len);

hi_s32 wlan_state_Closed(hi_s32 msg, hi_void *data,
                           hi_char *reply, hi_s32 *len);

hi_s32 wlan_state_DriverLoadded(hi_s32 msg, hi_void *data,
                           hi_char *reply, hi_s32 *len);

hi_s32 wlan_state_sta_Started(hi_s32 msg, hi_void *data,
                           hi_char *reply, hi_s32 *len);

hi_s32 wlan_state_sta_Connected(hi_s32 msg, hi_void *data,
                           hi_char *reply, hi_s32 *len);

hi_s32 wlan_state_p2p_Started(hi_s32 msg, hi_void *data,
                           hi_char *reply, hi_s32 *len);

hi_s32 wlan_state_p2p_UserAuthorizingInvitation(hi_s32 msg, hi_void *data,
                           hi_char *reply, hi_s32 *len);

hi_s32 wlan_state_p2p_UserAuthorizingJoin(hi_s32 msg, hi_void *data,
                           hi_char *reply, hi_s32 *len);

hi_s32 wlan_state_p2p_GroupCreated(hi_s32 msg, hi_void *data,
                           hi_char *reply, hi_s32 *len);

hi_s32 wlan_state_p2p_GroupNegotiation(hi_s32 msg, hi_void *data,
                           hi_char *reply, hi_s32 *len);

hi_s32 wlan_sm_register_callback(hi_wlan_sta_event_callback pfnEventCb);
hi_s32 wlan_sm_unregister_callback_given(hi_wlan_sta_event_callback pfnEventCb);
hi_s32 wlan_sm_unregister_callback(hi_void);

hi_s32 wlan_sm_sta_scan_results(hi_char *results, hi_u32 resultsSize);

hi_s32 wlan_sm_sta_connection_status(hi_wlan_sta_connection_status *con);

hi_s32 wlan_sm_p2p_get_peers(hi_wlan_p2p_device_info *list, hi_u32 *num);

hi_s32 wlan_sm_p2p_get_persistent_groups(hi_wlan_p2p_group_info *grouplist,
                                           hi_u32 *num);

hi_s32 wlan_sm_p2p_delete_persistent_group(hi_wlan_p2p_group_info *group);

hi_s32 wlan_sm_start_wps_pbc(hi_char *bssid);
hi_s32 wlan_sm_start_wps_pbc_p2p(hi_char *bssid);

hi_s32 wlan_sm_start_wps_pin_keypad(hi_char *pin);
hi_s32 wlan_sm_start_wps_pin_keypad_p2p(hi_char *pin);

#endif /* __WLAN_SM_H__ */
