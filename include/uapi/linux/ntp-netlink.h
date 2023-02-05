/* SPDX-License-Identifier: GPL-2.0-only WITH Linux-syscall-note */
/*
 * include/uapi/linux/ethtool_netlink.h - netlink interface for NTP disruption notifications
 */

#ifndef _UAPI_LINUX_NTP_NETLINK_H_
#define _UAPI_LINUX_NTP_NETLINK_H_

enum {
	NTP_DISRUPT_A_UNSPEC,
	NTP_DISRUPT_A_CLKVERSION,               /* u32 */
	NTP_DISRUPT_A_HARDWARE_CHANGED,         /* flag */

	__NTP_DISRUPT_A_CNT,
        NTP_DISRUPT_A_MAX = (__NTP_DISRUPT_A_CNT - 1)
};

enum {
	NTP_MSG_UNSPEC,
	NTP_MSG_DISRUPT_NOTIFY,

	__NTP_MSG_CNT,
        NTP_MSG_MAX = (__NTP_MSG_CNT - 1)
};

#define NTP_GENL_NAME "ntp"
#define NTP_GENL_VERSION 1

#define NTP_MCGRP_DISRUPT_NAME "disrupt"

#endif