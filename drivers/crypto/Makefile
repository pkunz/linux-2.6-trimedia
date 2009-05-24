
obj-$(CONFIG_CRYPTO_DEV_PADLOCK) += padlock.o

padlock-objs-$(CONFIG_CRYPTO_DEV_PADLOCK_AES) += padlock-aes.o

padlock-objs := padlock-generic.o $(padlock-objs-y)

obj-$(CONFIG_MPC8xxx_SEC2x)	+= sec2/
obj-$(CONFIG_MPC8xxx_SEC2xTEST)	+= sec2test/

