#
# Makefile for the Linux networking core.
#

obj-y := sock.o request_sock.o skbuff.o iovec.o datagram.o stream.o scm.o \
	 gen_stats.o gen_estimator.o

obj-$(CONFIG_SYSCTL) += sysctl_net_core.o

obj-y		     += dev.o dst.o \
			neighbour.o rtnetlink.o utils.o link_watch.o

obj-$(CONFIG_XFRM) += flow.o
obj-$(CONFIG_ETHTOOL) += ethtool.o
obj-$(CONFIG_SYSFS) += net-sysfs.o
obj-$(CONFIG_NET_DIVERT) += dv.o
obj-$(CONFIG_NET_PKTGEN) += pktgen.o
obj-$(CONFIG_NET_RADIO) += wireless.o
obj-$(CONFIG_NETPOLL) += netpoll.o
obj-$(CONFIG_NET_SK_FILTER) += filter.o
obj-$(CONFIG_NET_DEV_MULTICAST) += dev_mcast.o
