#
# Makefile for arch/trimedia/boot
#
obj-y += head.o
CFLAGS += -g

targets := TMLinux 

clean-files := TMLinux*.out

#
# FIXME: TCS-5.2 does not support dynamically loading, so I have to generate a tmlinux using tcs-4.61
#
$(obj)/TMLinux: vmlinux FORCE
	$(Q)$(CC) -tmld $(LDFLAGS) -- $(CFLAGS) $(BOOTTYPE) -o $(obj)/TMLinux-$(TARGET)-$(HOST)$(FLOAT)$(ENDIAN).out $< $(appmain) $(initapp) $(libs-usr)
	$(Q)wine /opt/NXP/TCS4.61/bin/tmcc -tmld $(LDFLAGS) -- $(CFLAGS) $(BOOTTYPE) -o $(obj)/TMLinux-tcs4.61-$(TARGET)-$(HOST)$(FLOAT)$(ENDIAN).out $< $(appmain) $(initapp) $(libs-usr)

clean:
	@$(RM) -fr TMLinux-*.out
