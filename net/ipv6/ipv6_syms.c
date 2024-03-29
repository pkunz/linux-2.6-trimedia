
#include <linux/config.h>
#include <linux/module.h>
#include <net/protocol.h>
#include <net/ipv6.h>
#include <net/addrconf.h>
#include <net/ip6_route.h>
#include <net/xfrm.h>

EXPORT_SYMBOL(__ipv6_addr_type);
EXPORT_SYMBOL(icmpv6_send);
EXPORT_SYMBOL(icmpv6_statistics);
EXPORT_SYMBOL(icmpv6_err_convert);
EXPORT_SYMBOL(ndisc_mc_map);
EXPORT_SYMBOL(register_inet6addr_notifier);
EXPORT_SYMBOL(unregister_inet6addr_notifier);
EXPORT_SYMBOL(ip6_pol_route_output);
EXPORT_SYMBOL(addrconf_lock);
EXPORT_SYMBOL(ipv6_setsockopt);
EXPORT_SYMBOL(ipv6_getsockopt);
EXPORT_SYMBOL(inet6_register_protosw);
EXPORT_SYMBOL(inet6_unregister_protosw);
EXPORT_SYMBOL(inet6_add_protocol);
EXPORT_SYMBOL(inet6_del_protocol);
EXPORT_SYMBOL(ip6_xmit);
EXPORT_SYMBOL(inet6_release);
EXPORT_SYMBOL(inet6_bind);
EXPORT_SYMBOL(inet6_getname);
EXPORT_SYMBOL(inet6_ioctl);
EXPORT_SYMBOL(ipv6_get_saddr);
EXPORT_SYMBOL(ipv6_chk_addr);
EXPORT_SYMBOL(in6_dev_finish_destroy);
#ifdef CONFIG_IPV6_MIP6
EXPORT_SYMBOL(ipv6_find_tlv);
#endif
#ifdef CONFIG_XFRM
EXPORT_SYMBOL(xfrm6_rcv);
#ifdef CONFIG_XFRM_ENHANCEMENT
EXPORT_SYMBOL(__xfrm6_rcv_one);
#endif
#endif
EXPORT_SYMBOL(ip6_pol_route_lookup);
/* EXPORT_SYMBOL(ip6_dst_lookup); */
EXPORT_SYMBOL(ipv6_push_nfrag_opts);
#ifdef CONFIG_IPV6_MULTIPLE_TABLES
EXPORT_SYMBOL(rt6_rule_lookup);
EXPORT_SYMBOL(rt6_tables);
#else
EXPORT_SYMBOL(ip6_routing_table);
#endif /* CONFIG_IPV6_MULTIPLE_TABLES */
EXPORT_SYMBOL(in6addr_any);
