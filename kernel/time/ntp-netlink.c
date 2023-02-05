/* SPDX-License-Identifier: GPL-2.0-only */
#include <linux/kernel.h>
#include <net/genetlink.h>
#include <uapi/linux/ntp-netlink.h>

static int ntp_disrupt_notify_doit(struct sk_buff *skb, struct genl_info *info)
{
	/* This is a one-way protocol, so if we receive messages, just drop them on
        the floor. */
	(void)skb;
	(void)info;
	return 0;
}

static struct nla_policy ntp_disrupt_genl_policy[] = {
	[NTP_DISRUPT_A_CLKVERSION]              = { .type = NLA_U32 },
	[NTP_DISRUPT_A_HARDWARE_CHANGED]        = { .type = NLA_FLAG },
};

static struct genl_ops ntp_genl_ops[] = {
	{ 
                .cmd            = NTP_MSG_DISRUPT_NOTIFY,
	        .doit           = ntp_disrupt_notify_doit,
                .policy         = ntp_disrupt_genl_policy,
                .maxattr        = ARRAY_SIZE(ntp_disrupt_genl_policy) - 1
        }
};

enum {
        NTP_MCGRP_DISRUPT
};

static struct genl_multicast_group ntp_genl_mcgrps[] = {
	[NTP_MCGRP_DISRUPT] = { .name = NTP_MCGRP_DISRUPT_NAME  }
};

static struct genl_family ntp_genl_family = {
	.name           = NTP_GENL_NAME,
	.version        = NTP_GENL_VERSION,
        .netnsok        = true,
        .ops            = ntp_genl_ops,
        .n_ops          = ARRAY_SIZE(ntp_genl_ops),
	.mcgrps         = ntp_genl_mcgrps,
        .n_mcgrps       = ARRAY_SIZE(ntp_genl_mcgrps),
};

void ntp_disrupt_notify(u32 clkversion, bool hardware_changed)
{
	struct sk_buff *skb;
	void *msg_head;
	int error;

	skb = genlmsg_new(NLMSG_GOODSIZE, GFP_KERNEL);
	if (skb == NULL) {
		pr_err("genlmsg_new() failed\n");
		return;
	}

	msg_head = genlmsg_put(skb, 0, 0, &ntp_genl_family, 0,
			       NTP_MSG_DISRUPT_NOTIFY);
	if (msg_head == NULL) {
		pr_err("genlmsg_put() failed\n");
		kfree_skb(skb);
		return;
	}

	error = nla_put_u32(skb, NTP_DISRUPT_A_CLKVERSION, clkversion);
	if (error) {
		pr_err("nla_put_u32() failed: %d\n", error);
		kfree_skb(skb);
		return;
	}

	if (hardware_changed) {
		error = nla_put_flag(skb, NTP_DISRUPT_A_HARDWARE_CHANGED);
		if (error) {
			pr_err("nla_put_u32() failed: %d\n", error);
			kfree_skb(skb);
			return;
		}
	}

	genlmsg_end(skb, msg_head);
	error = genlmsg_multicast_allns(&ntp_genl_family, skb, 0, NTP_MCGRP_DISRUPT,
					GFP_KERNEL);
	if (error != 0 && error != -ESRCH) {
		pr_err("genlmsg_mulitcast_allns() failed: %d\n", error);
	}

	kfree_skb(skb);
}

static int __init ntp_netlink_init(void)
{
	int error;

	error = genl_register_family(&ntp_genl_family);
	if (error) {
		pr_err("genl_register_family() failed: %d\n", error);
	}
	return error;
}

subsys_initcall(ntp_netlink_init);