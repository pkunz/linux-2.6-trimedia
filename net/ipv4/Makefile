#
# Makefile for the Linux TCP/IP (INET) layer.
#

obj-y     := route.o protocol.o \
	     ip_input.o ip_fragment.o ip_forward.o ip_options.o \
	     ip_output.o ip_sockglue.o inet_hashtables.o \
	     inet_timewait_sock.o inet_connection_sock.o \
	     tcp.o tcp_input.o tcp_output.o tcp_timer.o tcp_ipv4.o \
	     tcp_minisocks.o tcp_cong.o \
	     datagram.o raw.o udp.o arp.o icmp.o devinet.o af_inet.o \
	     sysctl_net_ipv4.o fib_frontend.o fib_semantics.o netfilter.o

obj-$(CONFIG_IP_FIB_HASH) += fib_hash.o
obj-$(CONFIG_IP_FIB_TRIE) += fib_trie.o
obj-$(CONFIG_IGMP) += igmp.o
obj-$(CONFIG_INETPEER) += inetpeer.o
obj-$(CONFIG_PROC_FS) += proc.o
obj-$(CONFIG_IP_MULTIPLE_TABLES) += fib_rules.o
obj-$(CONFIG_IP_MROUTE) += ipmr.o
obj-$(CONFIG_NET_IPIP) += ipip.o
obj-$(CONFIG_NET_IPGRE) += ip_gre.o
obj-$(CONFIG_SYN_COOKIES) += syncookies.o
obj-$(CONFIG_INET_AH) += ah4.o
obj-$(CONFIG_INET_ESP) += esp4.o
obj-$(CONFIG_INET_IPCOMP) += ipcomp.o
obj-$(CONFIG_INET_TUNNEL) += xfrm4_tunnel.o 
obj-$(CONFIG_IP_PNP) += ipconfig.o
obj-$(CONFIG_IP_ROUTE_MULTIPATH_RR) += multipath_rr.o
obj-$(CONFIG_IP_ROUTE_MULTIPATH_RANDOM) += multipath_random.o
obj-$(CONFIG_IP_ROUTE_MULTIPATH_WRANDOM) += multipath_wrandom.o
obj-$(CONFIG_IP_ROUTE_MULTIPATH_DRR) += multipath_drr.o
obj-$(CONFIG_NETFILTER)	+= netfilter/
obj-$(CONFIG_IP_VS) += ipvs/
obj-$(CONFIG_INET_DIAG) += inet_diag.o 
obj-$(CONFIG_IP_ROUTE_MULTIPATH_CACHED) += multipath.o
obj-$(CONFIG_INET_TCP_DIAG) += tcp_diag.o
obj-$(CONFIG_TCP_CONG_BIC) += tcp_bic.o
obj-$(CONFIG_TCP_CONG_WESTWOOD) += tcp_westwood.o
obj-$(CONFIG_TCP_CONG_HSTCP) += tcp_highspeed.o
obj-$(CONFIG_TCP_CONG_HYBLA) += tcp_hybla.o
obj-$(CONFIG_TCP_CONG_HTCP) += tcp_htcp.o
obj-$(CONFIG_TCP_CONG_VEGAS) += tcp_vegas.o
obj-$(CONFIG_TCP_CONG_SCALABLE) += tcp_scalable.o

obj-$(CONFIG_XFRM) += xfrm4_policy.o xfrm4_state.o xfrm4_input.o \
		      xfrm4_output.o
