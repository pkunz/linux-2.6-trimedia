#ifndef MV64360_PHY
#define MV64360_PHY

/*
 * PHY identifier
 */
#define PHY_ID_REVISION_MASK                    0xFFFFFFF0
#define PHY_ID_BROADCOM_BCM5421S                0x002060E0

/*
 * PHY generic register addresses
 */

#define DSC2_ENET_PHY_MII_CONTROL               0x00
#define DSC2_ENET_PHY_MII_STATUS                0x01
#define DSC2_ENET_PHY_PHY_ID_MSB                0x02
#define DSC2_ENET_PHY_PHY_ID_LSB                0x03
#define DSC2_ENET_PHY_AUTONEG_ADV               0x04
#define DSC2_ENET_PHY_AUTONEG_LPA               0x05
#define DSC2_ENET_PHY_AUTONEG_EXP               0x06
#define DSC2_ENET_PHY_NEXT_PAGE_TX              0x07
#define DSC2_ENET_PHY_LP_RX_NEXT_PAGE           0x08
#define DSC2_ENET_PHY_1000T_CONTROL             0x09
#define DSC2_ENET_PHY_1000T_STATUS              0x0A
#define DSC2_ENET_PHY_IEEE_EXT_STATUS           0x0F

/* PHY specific register addresses */

#define BCM5421S_PHY_EXT_CONTROL                0x10
#define BCM5421S_PHY_EXT_STATUS                 0x11
#define BCM5421S_PHY_AUX_STATUS                 0x19
#define BCM5421S_PHY_INT_STAT                   0x1A
#define BCM5421S_PHY_INT_MASK                   0x1B

/*
 * PHY generic registers bit definitions
 */
#define DSC2_ENET_PHY_CR_SPEED_SELECT_MSB       0x0040
#define DSC2_ENET_PHY_CR_COLL_TEST_ENABLE       0x0080
#define DSC2_ENET_PHY_CR_FULL_DUPLEX            0x0100
#define DSC2_ENET_PHY_CR_HALF_DUPLEX            0x0000
#define DSC2_ENET_PHY_CR_RESTART_AUTO_NEG       0x0200
#define DSC2_ENET_PHY_CR_ISOLATE                0x0400
#define DSC2_ENET_PHY_CR_POWER_DOWN             0x0800
#define DSC2_ENET_PHY_CR_AUTO_NEG_EN            0x1000
#define DSC2_ENET_PHY_CR_SPEED_SELECT_LSB       0x2000
#define DSC2_ENET_PHY_CR_LOOPBACK               0x4000
#define DSC2_ENET_PHY_CR_RESET                  0x8000

#define DSC2_ENET_PHY_CR_SPEED_SELECT_10        0x0000	/* 6/13 = 00 */
#define DSC2_ENET_PHY_CR_SPEED_SELECT_100       0x2000	/* 6/13 = 01 */
#define DSC2_ENET_PHY_CR_SPEED_SELECT_1000      0x0040	/* 6/13 = 10 */
#define DSC2_ENET_PHY_CR_SPEED_SELECT_MASK      0x2040
#define DSC2_ENET_PHY_ST_EXT_CAP                0x0001
#define DSC2_ENET_PHY_ST_JABBER                 0x0002
#define DSC2_ENET_PHY_ST_LINK_UP                0x0004
#define DSC2_ENET_PHY_ST_AUTO_NEG_CAP           0x0008
#define DSC2_ENET_PHY_ST_REMOTE_FAULT           0x0010
#define DSC2_ENET_PHY_ST_AUTO_NEG_DONE          0x0020
#define DSC2_ENET_PHY_ST_PREAMBLE_SUPPRESS      0x0040
#define DSC2_ENET_PHY_ST_EXT_STATUS             0x0100
#define DSC2_ENET_PHY_ST_100BASET2_HALF         0x0200
#define DSC2_ENET_PHY_ST_100BASET2_FULL         0x0400
#define DSC2_ENET_PHY_ST_10BASET_HALF           0x0800
#define DSC2_ENET_PHY_ST_10BASET_FULL           0x1000
#define DSC2_ENET_PHY_ST_100BASETX_HALF         0x2000
#define DSC2_ENET_PHY_ST_100BASETX_FULL         0x4000
#define DSC2_ENET_PHY_ST_100BASET4_CAP          0x8000

#define DSC2_ENET_PHY_ANA_SELECTOR_FIELD        0x0001
#define DSC2_ENET_PHY_ANA_10T_HD_CAPS           0x0020
#define DSC2_ENET_PHY_ANA_10T_FD_CAPS           0x0040
#define DSC2_ENET_PHY_ANA_100TX_HD_CAPS         0x0080
#define DSC2_ENET_PHY_ANA_100TX_FD_CAPS         0x0100
#define DSC2_ENET_PHY_ANA_100T4_CAPS            0x0200
#define DSC2_ENET_PHY_ANA_PAUSE                 0x0400
#define DSC2_ENET_PHY_ANA_ASM_DIR               0x0800
#define DSC2_ENET_PHY_ANA_REMOTE_FAULT          0x2000
#define DSC2_ENET_PHY_ANA_NEXT_PAGE             0x8000

#define DSC2_ENET_PHY_AUTONEG_LPA_10T_HD        0x0020
#define DSC2_ENET_PHY_AUTONEG_LPA_10T_FD        0x0040
#define DSC2_ENET_PHY_AUTONEG_LPA_100TX_HD      0x0080
#define DSC2_ENET_PHY_AUTONEG_LPA_100TX_FD      0x0100

#define DSC2_ENET_PHY_CR1000T_HD_CAPS           0x0100
#define DSC2_ENET_PHY_CR1000T_FD_CAPS           0x0200

#define DSC2_ENET_PHY_ST1000T_LPA_1000T_FD      0x0800

#define DSC2_ENET_PHY_IEEE_HD_CAPS              0x1000
#define DSC2_ENET_PHY_IEEE_FD_CAPS              0x2000

#define DSC2_ENET_MAX_PHY_REG_ADDRESS           0x1F

/* PHY specific registers bit definitions */

#define BCM5421S_PHY_AS_AUTONEG_DONE            0x8000

#define BCM5421S_PHY_INT_LINK_CHANGE            0x0002
#define BCM5421S_PHY_INT_SPEED_CHANGE           0x0004
#define BCM5421S_PHY_INT_DUPLEX_CHANGE          0x0008
#define BCM5421S_PHY_INT_LSD_CHANGE             0x000E	/* link/speed/duplex */

#endif				/* MV64360_PHY */
