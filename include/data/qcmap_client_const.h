#ifndef _QCMAP_CLIENT_CONST_H_
#define _QCMAP_CLIENT_CONST_H_
const char *hostapd_basecfg_str[]=
{
"Page",
"mask",
"basecfg_ap_index",
"basecfg_ssid",
"basecfg_broadcast",
"basecfg_auth_type",
"basecfg_wpa_pairwise",
"basecfg_wep_key_status",
"basecfg_wep_default_key",
"basecfg_wpa_passphrase",
"basecfg_wep_key0",
"basecfg_wep_key1",
"basecfg_wep_key2",
"basecfg_wep_key3",
"basecfg_result",
};

const char *wpa_supplicant_cfg_str[]=
{
"Page",
"mask",
"basecfg_ap_index",
"basecfg_ssid",
"basecfg_wep_key_status"
"basecfg_auth_type",
"basecfg_wpa_passphrase",
"basecfg_result",
};

const char *hostapd_extcfg_str[]=
{
"Page",
"mask",
"extcfg_ap_index",
"extcfg_country",
"extcfg_channel",
"extcfg_hw_mode",
"extcfg_ap_isolate",
"extcfg_ht_capab",
"extcfg_result",
};

const char *dhcp_config_str[]=
{
"Page",
"Mask",
"lan_gw_addrs",
"lan_sub",
"lan_dhcp",
"lan_dhcp_start",
"lan_dhcp_end",
"lan_dhcp_lease",
"lan_config_result",
};

const char *default_profile_str[]=
{
"Page",
"mask",
"default_profile_index",
"default_profile_result",
};

const char *map_status_string[] =
{
	"Page",
	"mask",
	"map_status",
	"result",
};

const char *net_status_string[] =
{
	"Page",
	"mask",
	"net_status",
	"result",
};

const char *sta_ip_string[] =
{
	"Page",
	"mask",
	"sta_ip",
	"result",
};

#ifdef BUILD_VERSION_WIFI_RTL
const char *rtl_stainit_status_string[] =
{
	"Page",
	"mask",
	"map_status",
	"result",
};
#endif

#if 0
const char *firewall_string[] =
{
  "handle",
  "fw_entry_enabled",
  "IPversion",
  "nxthdrprotovld",
  "nxthdrproto",
  "tcpudpsrcvld",
  "tcpudpsrcport",
  "tcpudpsrcrange",
  "tcpudpdstvld",
  "tcpudpdstport",
  "tcpudpdstrange",
  "icmptypvld",
  "icmptyp",
  "icmpcode",
  "espspivld",
  "espspi",
  "ipsrcaddrvld",
  "ipsrcaddr",
  "ipsrcsubnet",
  "ipdstaddrvld",
  "ipdstaddr",
  "ipdstsubnet",  
  "iptosvld",
  "iptosvlv",
  "iptosmsk",
  "ip6srcvld",
  "ip6srcaddr",
  "ip6srclen",
  "ip6trsclfvld",
  "ip6trsclfvlv",
  "ip6trsclfmsk"
};

const char *nat_settings_str[]=
{
"Page",
"mask",
"nattype",
"nattype_result",
"dmzip",
"dmzip_result",
"ipsec",
"ipsec_result",
"pptp",
"pptp_result",
"l2tp",
"l2tp_result",
"gen_timeout",
"gen_timeout_result",
"icmp_timeout",
"icmp_timeout_result",
"tcp_timeout",
"tcp_timeout_result",
"udp_timeout",
"udp_timeout_result"
};

const char *get_dmz_str[]=
{
"Page",
"Mask",
"get_dmz_valid",
"get_dmz_ip",
"get_dmz_result",
};

const char *delete_dmz_str[]=
{
"Page",
"Mask",
"delete_dmz_result",
};

const char *lan_config_str[]=
{
"Page",
"Mask",
"lan_config_result",
"wlan_status_result",
"wlan_config_result",
"lan_gw_addrs",
"lan_sub",
"lan_dhcp",
"lan_dhcp_start",
"lan_dhcp_end",
"lan_dhcp_lease",
"wlan_status",
"wlan_mode",
"wlan_access",
"ap_sta_dhcp",
"ap_sta_ip",
"ap_sta_gw_addr",
"ap_sta_dns",
"ap_sta_sub"
};
const char *WWAN_v6_string[]=
{
  "Page",
  "mask",
  "Enable",
  "Enable_result",
  "backhaul",
  "backhaul_result",
  "Policy_result"
};

const char *WWAN_statistic_string[]=
{
"Bytes_Rx",
"Bytes_Tx",
"Packets_Rx",
"Packets_Tx",
"Packet_Drop_Rx",
"Packet_Drop_Tx"
};

const char *WWAN_Status_String[]=
{
"error",
// for V4
"connecting ",
"connecting_fail",
"connected",
"disconnecting",
"disconnecting_fail",
"disconnected",
// for v6
"connecting ",
"connecting_fail",
"connected",
"disconnecting",
"disconnecting_fail",
"disconnected"
};
const char *STA_String[]=
{
"notused",
"connected",
"disconnected"
};

const char *get_wwan_stats_string[]=
{
"Page",
"reset",
"ip_family",
"Result",
"Bytes_Rx",
"Bytes_Tx",
"Packets_Rx",
"Packets_Tx",
"Packet_Drop_Rx",
"Packet_Drop_Tx",
"Config_error",
"Public_IP",
"Primary_DNS",
"Secondary_DNS",
"backhaul_result",
"backhaul",
"station_result",
"station_mode"
};
const char *get_data_rate_string[]=
{
"Page",
"Result",
"Bytes_Tx",
"Bytes_Tx",
"Bytes_Tx_max",
"Bytes_Rx_max"
};

const char *ipv6_state_string[]=
{
"Page",
"enable_error",
"enable",
"V6status_result",
"V6status",
"policy_result",
"Tech_pref",
"profile_id_3gpp",
"profile_id_3gpp2"
};

const char *mobileap_str[]=
{
"Page",
"mask",
"autoconnect",
"autoconnect_result",
"roaming",
"roaming_result"
};
const char *get_media_dir_str[]=
{
"Page",
"Result",
"Dir"
};

const char *set_media_dir_str[]=
{
"Page",
"Operation",
"Dir",
"Result"
};

const char *upnp_dlna_str[]=
{
"Page",
"mask",
"upnp_result",
"upnp_enable",
"dlna_result",
"dlna_enable",
"mdns_result",
"mdns_enable"
};

const char *wwan_ipv4_str[]=
{
"Page",
"mask",
"enable",
"enable_result",
"backhaul",
"backhaul_result",
"tech_result",
"tech_pref",
"profile_id_3gpp",
"profile_id_3gpp2",
};

const char *hostapd_macfilter_str[]=
{
"Page",
"mask",
"macfilter_flag",
"macfilter_count",
"macfilter_set_result",
"macfilter_addr0",
"macfilter_addr1",
"macfilter_addr2",
"macfilter_addr3",
"macfilter_addr4",
"macfilter_addr5",
"macfilter_addr6",
"macfilter_addr7",
"macfilter_addr8",
"macfilter_addr9",
"macfilter_addr10",
"macfilter_addr11",
"macfilter_addr12",
"macfilter_addr13",
"macfilter_addr14",
"macfilter_addr15",
"macfilter_addr16",
"macfilter_addr17",
"macfilter_addr18",
"macfilter_addr19",
"macfilter_get_result",
};

const char *createprofile_str[]=
{
"Page",
"mask",
"create_profile_index",
"create_profile_profilename",
"create_profile_apnname",
"create_profile_username",
"create_profile_password",
"create_profile_result",
};

const char *modifyprofile_str[]=
{
"Page",
"mask",
"modify_profile_index",
"modify_profile_profilename",
"modify_profile_apnname",
"modify_profile_username",
"modify_profile_password",
"modify_profile_result",
};
const char *deleteprofile_str[]=
{
"Page",
"mask",
"delete_profile_index",
"delete_profile_result",
};
const char *getprofile_str[]=
{
"Page",
"mask",
"get_profile_index",
//"get_profile_profilename",
"get_profile_apnname",
"get_profile_username",
"get_profile_password",
"get_profile_result",
};

const char *getprofile_list_str[]=
{
"Page",
"mask",
"getprofile_list_protect_index",
"getprofile_list_default_index",
"getprofile_list_number",
"getprofile_list_profilename_1",
"getprofile_list_profilename_2",
"getprofile_list_profilename_3",
"getprofile_list_profilename_4",
"getprofile_list_profilename_5",
"getprofile_list_profilename_6",
"getprofile_list_profilename_7",
"getprofile_list_profilename_8",
"getprofile_list_profilename_9",
"getprofile_list_profilename_10",
"getprofile_list_profilename_11",
"getprofile_list_profilename_12",
"getprofile_list_profilename_13",
"getprofile_list_profilename_14",
"getprofile_list_profilename_15",
"getprofile_list_profilename_16",

"getprofile_list_profileindex_1",
"getprofile_list_profileindex_2",
"getprofile_list_profileindex_3",
"getprofile_list_profileindex_4",
"getprofile_list_profileindex_5",
"getprofile_list_profileindex_6",
"getprofile_list_profileindex_7",
"getprofile_list_profileindex_8",
"getprofile_list_profileindex_9",
"getprofile_list_profileindex_10",
"getprofile_list_profileindex_11",
"getprofile_list_profileindex_12",
"getprofile_list_profileindex_13",
"getprofile_list_profileindex_14",
"getprofile_list_profileindex_15",
"getprofile_list_profileindex_16",
"getprofile_list_result",
};

const char *get_dms_status_str[]=
{
"Page",
"mask",
"get_dms_status_imei",
"get_dms_status_imsi",
"get_dms_status_hwrev",
"get_dms_status_swversion",
"get_dms_status_uimstate",
"get_dms_status_powerstate",
"get_dms_status_battery_lvl",
"get_dms_status_wlan_mac",
"get_dms_status_wan_ip",
"get_dms_status_result",
};

const char *get_head_status_str[]=
{
"Page",
"mask",
"get_head_status_mms",
"get_head_status_signal",
"get_head_status_wan",
"get_head_status_wlan",
"get_head_status_charging",
"get_head_battery_lvl",
"get_head_net_traffic_rx",
"get_head_net_traffic_tx",
"get_head_plmn",
"get_head_wlan_clients",
"get_dms_status_result",
};

const char *get_firewall_status_str[]=
{
"Page",
"mask",
"get_firewall_state",
"get_firewall_packets_allowed",
"get_firewall_result",
};

const char *ping_state_str[]=
{
"Page",
"mask",
"ping_state_ping_disabled",
"ping_state_result",
};

const char *sim_pin_state_str[]=
{
"Page",
"mask",
"sim_pin1_state",
"sim_pin1_verify_left",
"sim_pin1_unblock_left",
"sim_pin_result",
};

const char *sim_pin_protection_str[]=
{
"Page",
"mask",
"sim_pin_protection_id",
"sim_pin_protection_state",
"sim_pin_protection_enabled",
"sim_pin_protection_value",
"sim_pin_protection_result",
};

const char *sim_pin_verify_str[]=
{
"Page",
"mask",
"sim_pin_verify_id",
"sim_pin_verify_state",
"sim_pin_verify_value",
"sim_pin_verify_result",
};
const char *sim_pin_change_str[]=
{
"Page",
"mask",
"sim_pin_change_id",
"sim_pin_change_state",
"sim_pin_change_old_value",
"sim_pin_change_new_value",
"sim_pin_change_result",
};

const char *sim_pin_unblock_str[]=
{
"Page",
"mask",
"sim_pin_unblock_id",
"sim_pin_unblock_state",
"sim_pin_unblock_puk_value",
"sim_pin_unblock_pin_value",
"sim_pin_unblock_result",
};
const char *system_preference_str[]=
{
"Page",
"mask",
"system_preference_mode",
"system_preference_network",
"system_preference_support",
"system_preference_result",
};

const char *Supported_preference_str[]=
{
"Page",
"mask",
"Supported_preference_mode",
"supported_preference_result",
};

const char *wms_list_string[] =
{
  "message_index",
  "tag_type"
};

const char *wms_read_string[] =
{
  "Page",
  "mask",
  "storage_type",
  "storage_index",
  "ims_valid",
  "ims",
  "mode_valid",
  "mode",
  "tag_type",
  "format",
  "phone_num",
  "time",
  "data_len",
  "data_value",
  "encode",
  "result"
};


const char *wms_delete_string[] =
{
  "Page",
  "mask",
  "index_valid",
  "index",
  "storage",
  "mode_valid",
  "mode",
  "tag_valid",
  "tag",
  "result"
};

const char *wms_report_string[] =
{
  "Page",
  "mask",
  "report_mt_message",
  "result"
};


const char *wms_send_from_string[] =
{
  "Page",
  "mask",
  "storage_type",
  "storage_index",
  "result"
};

const char *wms_store_max_string[] =
{
  "Page",
  "mask",
  "storage_type",
  "storage_index",
  "result"
};
const char *wms_modify_tag_string[] =
{
  "Page",
  "mask",
  "tag_type",
  "storage_type",
  "storage_index",
  "result"
};

const char *wms_mem_status_string[] =
{
  "Page",
  "mask",
  "memory_valid",
  "memory",
  "result"
};

#endif
const char *firewall_string[] =
{
  "handle",
  "IPversion",
 "nxthdrprotovld",
  "nxthdrproto",
  "tcpudpsrcvld",
  "tcpudpsrcport",
  "tcpudpsrcrange",
  "tcpudpdstvld",
  "tcpudpdstport",
  "tcpudpdstrange",
  "icmptypvld",
  "icmptyp",
  "icmpcodevld",
  "icmpcode",
  "espspivld",
  "espspi",
  "ipsrcaddrvld",
  "ipsrcaddr",
  "ipsrcsubnet",
  "iptosvld",
  "iptosvlv",
  "iptosmsk",
  "ip6srcvld",
  "ip6srcaddr",
  "ip6srclen",
  "ip6trsclfvld",
  "ip6trsclfvlv",
  "ip6trsclfmsk"
};

const char *nat_settings_str[]=
{
"Page",
"mask",
"nattype",
"nattype_result",
"dmzip",
"dmzip_result",
"ipsec",
"ipsec_result",
"pptp",
"pptp_result",
"l2tp",
"l2tp_result",
"gen_timeout",
"gen_timeout_result",
"icmp_timeout",
"icmp_timeout_result",
"tcp_timeout",
"tcp_timeout_result",
"udp_timeout",
"udp_timeout_result",
"wwan_access",
"wwan_access_result"
};

const char *lan_config_str[]=
{
"Page",
"Mask",
"lan_config_result",
"wlan_status_result",
"wlan_config_result",
"lan_gw_addrs",
"lan_sub",
"lan_dhcp",
"lan_dhcp_start",
"lan_dhcp_end",
"lan_dhcp_lease",
"wlan_status",
"wlan_mode",
"wlan_access",
"ap_sta_dhcp",
"ap_sta_ip",
"ap_sta_gw_addr",
"ap_sta_dns",
"ap_sta_sub"
};
const char *WWAN_v6_string[]=
{
  "Page",
  "mask",
  "Enable",
  "Enable_result",
  "backhaul",
  "backhaul_result",
  "Policy_result"
};

const char *WWAN_statistic_string[]=
{
"Bytes_Rx",
"Bytes_Tx",
"Packets_Rx",
"Packets_Tx",
"Packet_Drop_Rx",
"Packet_Drop_Tx"
};

const char *WWAN_Status_String[]=
{
"error",
// for V4
"connecting ",
"connecting_fail",
"connected",
"disconnecting",
"disconnecting_fail",
"disconnected",
// for v6
"connecting ",
"connecting_fail",
"connected",
"disconnecting",
"disconnecting_fail",
"disconnected"
};
const char *STA_String[]=
{
"notused",
"connected",
"disconnected"
};

const char *get_wwan_stats_string[]=
{
"Page",
"reset",
"ip_family",
"Result",
"Bytes_Rx",
"Bytes_Tx",
"Packets_Rx",
"Packets_Tx",
"Packet_Drop_Rx",
"Packet_Drop_Tx",
"Config_error",
"Public_IP",
"Primary_DNS",
"Secondary_DNS",
"backhaul_result",
"backhaul",
"station_result",
"station_mode"
};
const char *get_data_rate_string[]=
{
"Page",
"Result",
"Bytes_Tx",
"Bytes_Tx",
"Bytes_Tx_max",
"Bytes_Rx_max"
};

const char *ipv6_state_string[]=
{
"Page",
"enable_error",
"enable",
"V6status_result",
"V6status",
"policy_result",
"Tech_pref",
"profile_id_3gpp",
"profile_id_3gpp2"
};

const char *mobileap_str[]=
{
"Page",
"mask",
"autoconnect",
"autoconnect_result",
"roaming",
"roaming_result"
};
const char *get_media_dir_str[]=
{
"Page",
"Result",
"Dir"
};

const char *set_media_dir_str[]=
{
"Page",
"Operation",
"Dir",
"Result"
};

const char *upnp_dlna_str[]=
{
"Page",
"mask",
"upnp_result",
"upnp_enable",
"dlna_result",
"dlna_enable",
"mdns_result",
"mdns_enable"
};

const char *wwan_ipv4_str[]=
{
"Page",
"mask",
"enable",
"enable_result",
"backhaul",
"backhaul_result",
"tech_result",
"tech_pref",
"profile_id_3gpp",
"profile_id_3gpp2"
};

const char *upnp_dlna_notify_str[]=
{
"Page",
"mask",
"upnp_interval_result",
"upnp_interval",
"dlna_interval_result",
"dlna_interval"
};
#endif