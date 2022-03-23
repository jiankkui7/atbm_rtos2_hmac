###############################################################################
#PLATFORM_XUNWEI    		1
#PLATFORM_SUN6I					2
#PLATFORM_FRIENDLY			3
#PLATFORM_SUN6I_64			4
#PLATFORM_HI3798M				5
#PLATFORM_HI3518E				6
#PLATFORM_X86PC         7
#PLATFORM_AMLOGIC				8
#PLATFORM_AMLOGIC_905X	9
#PLATFORM_ROCKCHIP      10
#PLATFORM_MSTAR					11
#PLATFORM_CDLINUX				12
#PLATFORM_AMLOGIC_S805	13
#PLATFORM_HIS_LINUX_3_4				14
#PLATFORM_ROCKCHIP_3229				15
#PLATFORM_ROCKCHIP_3229_ANDROID8		16
#PLATFORM_HS_IPC				17
#PLATFORM_SIGMASTAR                           18
#PLATFORM_HI3516EV200                         19
#PLATFORM_XUNWEI_2G                            20
#PLATFORM_NVT98517       21
export
platform ?=PLATFORM_SUN6I
#Android
#Linux
sys ?= linux
#arch:arm or arm64 or mips(NVT98517)
arch ?= arm

ifeq ($(platform),PLATFORM_NVT98517)
#KERDIR:=/wifi_prj/staff/zhouzhanchao/iTop4412_Kernel_3.0/
#KERDIR:=/home/xzq/svn_nvt_sdk/new_ipc_sdk/sdk_2.0/98517/6032i/NA51023_BSP_v1.1.1/linux-kernel/OUTPUT
KERDIR:=/wifi_prj/staff/wangsiyao/baichuan/linux-kernel/OUTPUT/
CROSS_COMPILE:=/wifi_prj/staff/wangsiyao/baichuan/mipsel-24kec-linux-uclibc-4.9-2017.07/usr/bin/mipsel-24kec-linux-uclibc-
ATBM_WIFI__EXT_CCFLAGS = -DATBM_WIFI_PLATFORM=21
arch = mips
endif

ifeq ($(platform),PLATFORM_HS_IPC)
KERDIR:=/wifi_prj/staff/jilechang/HS_IPC_1115/kernel
CROSS_COMPILE:=/wifi_prj/staff/jilechang/HS_IPC_1115/opt/arm-anykav200-crosstool/usr/bin/arm-anykav200-linux-uclibcgnueabi-
ATBM_WIFI__EXT_CCFLAGS = -DATBM_WIFI_PLATFORM=17
arch = arm
endif

ifeq ($(platform),PLATFORM_ROCKCHIP_3229_ANDROID8)
ifeq ($(sys),Android)
KERDIR:=/wifi_prj/staff/zhouzhanchao/rk3328_box_oreo_release_v1.0.0_20180319/kernel
CROSS_COMPILE:=/wifi_prj/staff/zhouzhanchao/rk3328_box_oreo_release_v1.0.0_20180319/prebuilts/gcc/linux-x86/aarch64/aarch64-linux-android-4.9/bin/aarch64-linux-android-
else
#KERDIR:=/wifi_prj/staff/zhouzhanchao/iTop4412_Kernel_3.0/
KERDIR:=/wifi_prj/wuping/project/linux/iTop4412_Kernel_3.0/
CROSS_COMPILE:=/usr/local/arm/arm-2009q3/bin/arm-none-linux-gnueabi-
endif
export
ATBM_WIFI__EXT_CCFLAGS = -DATBM_WIFI_PLATFORM=10
arch = arm64
endif

ifeq ($(platform),PLATFORM_ROCKCHIP_3229)
ifeq ($(sys),Android)
KERDIR:=/wifi_prj/staff/zhouzhanchao/rockchip_3229_kernel/kernel/
CROSS_COMPILE:=/wifi_prj/staff/zhouzhanchao/rockchip_3229_kernel/prebuilts/gcc/linux-x86/arm/arm-eabi-4.6/bin/arm-eabi-
else
#KERDIR:=/wifi_prj/staff/zhouzhanchao/iTop4412_Kernel_3.0/
KERDIR:=/wifi_prj/wuping/project/linux/iTop4412_Kernel_3.0/
CROSS_COMPILE:=/usr/local/arm/arm-2009q3/bin/arm-none-linux-gnueabi-
endif
export
ATBM_WIFI__EXT_CCFLAGS = -DATBM_WIFI_PLATFORM=10
endif

ifeq ($(platform),PLATFORM_HIS_LINUX_3_4)
KERDIR:=/wifi_prj/staff/zhouzhanchao/his_liux3_4/linux-3.4.y/
CROSS_COMPILE:=/wifi_prj/staff/zhouzhanchao/his_liux3_4/arm-hisiv300-linux/bin/arm-hisiv300-linux-uclibcgnueabi-
arch = arm
ATBM_WIFI__EXT_CCFLAGS = -DATBM_WIFI_PLATFORM=14
endif

ifeq ($(platform),PLATFORM_MSTAR)
ifeq ($(sys),Linux)
KERDIR:=/wifi_prj/staff/zhouzhanchao/mstar/linux3.18_i3/
CROSS_COMPILE:=/wifi_prj/staff/zhouzhanchao/mstar/arm-linux-gnueabihf-4.8.3-201404/bin/arm-linux-gnueabihf-
else
#KERDIR:=/wifi_prj/staff/zhouzhanchao/iTop4412_Kernel_3.0/
KERDIR:=/wifi_prj/wuping/project/linux/iTop4412_Kernel_3.0/
CROSS_COMPILE:=/usr/local/arm/arm-2009q3/bin/arm-none-linux-gnueabi-
endif
export
arch = arm
ATBM_WIFI__EXT_CCFLAGS = -DATBM_WIFI_PLATFORM=11
endif

ifeq ($(platform),PLATFORM_SIGMASTAR)
ifeq ($(sys),Linux)
KERDIR:=/wifi_prj/staff/futianhuang/sigma_star/linux-3.18/
CROSS_COMPILE:=/wifi_prj/staff/futianhuang/sigma_star/arm-linux-gnueabihf-4.8.3-201404/bin/arm-linux-gnueabihf-
else
#KERDIR:=/wifi_prj/staff/zhouzhanchao/iTop4412_Kernel_3.0/
#KERDIR:=/wifi_prj/wuping/project/linux/iTop4412_Kernel_3.0/
#CROSS_COMPILE:=/usr/local/arm/arm-2009q3/bin/arm-none-linux-gnueabi-
endif
export
arch = arm
ATBM_WIFI__EXT_CCFLAGS = -DATBM_WIFI_PLATFORM=18
endif

ifeq ($(platform),PLATFORM_ROCKCHIP)
ifeq ($(sys),Android)
KERDIR:=/wifi_prj/staff/zhouzhanchao/rockchip_new/kernel1/
CROSS_COMPILE:=/wifi_prj/staff/zhouzhanchao/rockchip_new/prebuilts/gcc/linux-x86/aarch64/aarch64-linux-android-4.9/bin/aarch64-linux-android-
else
#KERDIR:=/wifi_prj/staff/zhouzhanchao/iTop4412_Kernel_3.0/
KERDIR:=/wifi_prj/wuping/project/linux/iTop4412_Kernel_3.0/
CROSS_COMPILE:=/usr/local/arm/arm-2009q3/bin/arm-none-linux-gnueabi-
endif
export
arch = arm64
ATBM_WIFI__EXT_CCFLAGS = -DATBM_WIFI_PLATFORM=10 
endif

#
#xunwei platform params
#
ifeq ($(platform),PLATFORM_XUNWEI)
ifeq ($(sys),Android)
#KERDIR:=/wifi_prj/zhouzhanchao/android4_4_2/iTop4412_Kernel_3.0/
KERDIR:=/wifi_prj/wuping/project/android4_4_2/iTop4412_Kernel_3.0
CROSS_COMPILE:=/usr/local/arm/arm-2009q3/bin/arm-none-linux-gnueabi-
else
#KERDIR:=/wifi_prj/wuping/project/linux/iTop4412_Kernel_3.0/
KERDIR:=/wifi_prj/staff/panxuqiang/wifi_prj/iTop4412_Kernel_3.0/
#KERDIR:=/wifi_prj/staff/zhouzhanchao/iTop4412_Kernel_3.0/
CROSS_COMPILE:=/usr/local/arm/arm-2009q3/bin/arm-none-linux-gnueabi-
#KERDIR:=/wifi_prj/staff/jilechang/XUNWEI/iTop4412_Kernel_3.0/
#CROSS_COMPILE:=/wifi_prj/staff/jilechang/XUNWEI/arm-2009q3/bin/arm-none-linux-gnueabi-
endif
export
ATBM_WIFI__EXT_CCFLAGS = -DATBM_WIFI_PLATFORM=1
endif

ifeq ($(platform),PLATFORM_XUNWEI_2G)
ifeq ($(sys),Android)
#KERDIR:=/wifi_prj/zhouzhanchao/android4_4_2/iTop4412_Kernel_3.0/
KERDIR:=/wifi_prj/wuping/project/android4_4_2/iTop4412_Kernel_3.0
CROSS_COMPILE:=/usr/local/arm/arm-2009q3/bin/arm-none-linux-gnueabi-
else
#KERDIR:=/wifi_prj/wuping/project/linux/iTop4412_Kernel_3.0/
#KERDIR:=/wifi_prj/staff/panxuqiang/wifi_prj/iTop4412_Kernel_3.0/
#KERDIR:=/wifi_prj/staff/zhouzhanchao/iTop4412_Kernel_3.0/
#CROSS_COMPILE:=/usr/local/arm/arm-2009q3/bin/arm-none-linux-gnueabi-
KERDIR:=/wifi_prj/staff/jilechang/XUNWEI/iTop4412_Kernel_3.0/
CROSS_COMPILE:=/wifi_prj/staff/jilechang/XUNWEI/arm-2009q3/bin/arm-none-linux-gnueabi-
endif
export
ATBM_WIFI__EXT_CCFLAGS = -DATBM_WIFI_PLATFORM=20
endif

#
#allwinner 32 platform params
#
ifeq ($(platform),PLATFORM_SUN6I)
ifeq ($(sys),Android)
KERDIR:=/wifi_prj/staff/zhouzhanchao/android4_4_SIN/Source/lichee/linux-3.3
CROSS_COMPILE:=/wifi_prj/staff/zhouzhanchao/android4_4_SIN/Source/lichee/brandy/gcc-linaro/bin/arm-linux-gnueabi-
else
KERDIR:=/wifi_prj/staff/zhouzhanchao/Linux_sun6i/lichee/linux-3.3/
#KERDIR:=/wifi_prj/staff/panxuqiang/wifi_prj/branch/linux_sun6i/
CROSS_COMPILE:=/wifi_prj/staff/zhouzhanchao/Linux_sun6i/lichee/buildroot/output/external-toolchain/bin/arm-linux-gnueabi-
endif
export
ATBM_WIFI__EXT_CCFLAGS = -DATBM_WIFI_PLATFORM=2
endif

#
#allwinner 64 platform params
#
ifeq ($(platform),PLATFORM_SUN6I_64)
ifeq ($(sys),Android)
KERDIR:=/wifi_prj/staff/zhouzhanchao/android6_0_SIN/cqa64_android_v6.0/lichee/linux-3.10/
CROSS_COMPILE:=/wifi_prj/staff/zhouzhanchao/android6_0_SIN/cqa64_android_v6.0/lichee/out/sun50iw1p1/android/common/buildroot/external-toolchain/bin/aarch64-linux-gnu-
else
KERDIR:=/wifi_prj/staff/panxuqiang/64bi_driver/cqa64_linux_qt5.3.2/lichee/linux-3.10/
CROSS_COMPILE:=/wifi_prj/staff/panxuqiang/64bi_driver/cqa64_linux_qt5.3.2/lichee/brandy/armv8_toolchain/gcc-linaro-aarch64-linux-gnu-4.9-2014.09_linux/bin/aarch64-linux-gnu-
endif
arch:=arm64
export
ATBM_WIFI__EXT_CCFLAGS = -DATBM_WIFI_PLATFORM=4
endif

#
#HI3798M platform params
#
ifeq ($(platform),PLATFORM_HI3798M)
ifeq ($(sys),Android)
KERDIR:=/ssd-home/zhouzhanchao/Hi3798M-60-2/Hi3798M-60/out/target/product/Hi3798MV100/obj/KERNEL_OBJ/
CROSS_COMPILE:=/opt/hisi-linux/x86-arm/arm-hisiv200-linux/target/bin/arm-hisiv200-linux-
else
#KERDIR:=/wifi_prj/staff/panxuqiang/64bi_driver/cqa64_linux_qt5.3.2/lichee/linux-3.10/
endif
export
ATBM_WIFI__EXT_CCFLAGS = -DATBM_WIFI_PLATFORM=5
endif

ifeq ($(platform),PLATFORM_PCX86)
KERDIR:=/kernel/linux-lts-utopic-3.16.0/
export
ATBM_WIFI__EXT_CCFLAGS = -DATBM_WIFI_PLATFORM=7
arch:=x86
MAKEFILE_SUB = Makefile.build.local
endif
ifeq ($(platform),PLATFORM_AMLOGIC)
ifeq ($(sys),Android)
#KERDIR:=/wifi_prj/staff/zhouzhanchao/amlogic/release_s905_l/out/target/product/p200/obj/KERNEL_OBJ/
KERDIR:=/wifi_prj/staff/mengxuehong/s905l/S905L/out/target/product/p201_iptv/obj/KERNEL_OBJ/
CROSS_COMPILE:=/ssd-home/mengxuehong/buildTool1/gcc-linaro-aarch64-linux-gnu-4.9-2014.09_linux/bin/aarch64-linux-gnu-
else
#KERDIR:=/wifi_prj/staff/panxuqiang/64bi_driver/cqa64_linux_qt5.3.2/lichee/linux-3.10/
endif
export
ATBM_WIFI__EXT_CCFLAGS = -DATBM_WIFI_PLATFORM=8
arch:=arm64
endif

ifeq ($(platform),PLATFORM_AMLOGIC_S805)
ifeq ($(sys),Android)
KERDIR:=/wifi_prj/staff/zhouzhanchao/s805_env/KERNEL_OBJ/
CROSS_COMPILE:=/wifi_prj/staff/zhouzhanchao/s805_env/opt/gcc-linaro-arm-linux-gnueabihf/bin/arm-linux-gnueabihf-
else
#KERDIR:=/wifi_prj/staff/panxuqiang/64bi_driver/cqa64_linux_qt5.3.2/lichee/linux-3.10/
endif
export
ATBM_WIFI__EXT_CCFLAGS = -DATBM_WIFI_PLATFORM=8
arch:=arm
endif

ifeq ($(platform),PLATFORM_AMLOGIC_905X)
ifeq ($(sys),Android)
KERDIR=/wifi_prj/staff/mengxuehong/mengxuehong/aml0804/n-amlogic_0804/out/target/product/p281/obj/KERNEL_OBJ
#KERDIR:=/wifi_prj/staff/mengxuehong/mengxuehong/amlogic_sdk/release_s905_l/out/target/product/p201/obj/KERNEL_OBJ/
CROSS_COMPILE:=/wifi_prj/staff/mengxuehong/mengxuehong/amlogic_sdk/buildTool/gcc-linaro-aarch64-linux-gnu-4.9-2014.09_linux/bin/aarch64-linux-gnu-
else
#KERDIR:=/wifi_prj/staff/panxuqiang/64bi_driver/cqa64_linux_qt5.3.2/lichee/linux-3.10/
endif
export
ATBM_WIFI__EXT_CCFLAGS = -DATBM_WIFI_PLATFORM=9
arch:=arm64
endif

ifeq ($(platform),PLATFORM_HI3516EV200)
KERDIR:=/wifi_prj/staff/panxuqiang/wifi_prj/Hi3516EV200_SDK_V1.0.0.2/osdrv/opensource/kernel/linux-4.9.y
CROSS_COMPILE:=/opt/hisi-linux/x86-arm/arm-himix100-linux/bin/arm-himix100-linux-
export 
arch = arm
ATBM_WIFI__EXT_CCFLAGS = -DATBM_WIFI_PLATFORM=19
#ATBM_WIFI__EXT_CCFLAGS += -mcpu=cortex-a7 -mfloat-abi=softfp -mfpu=neon-vfpv4 -fno-aggressive-loop-optimizations
endif

TOPDIR ?= $(shell pwd)/../..

SDIO_BUS = y
USB_BUS = n
SPI_BUS = n

ifeq ($(SDIO_BUS),y)
TARGET = wifi_atbm6031_sdio
else
ifeq ($(USB_BUS),y)
TARGET = wifi_atbm6031_usb
else
TARGET = wifi_atbm6031_spi
endif
endif

INCLUDE += -I$(TOPDIR) \
		-I$(TOPDIR)/api \
		-I$(TOPDIR)/hal \
		-I$(TOPDIR)/hal/include \
		-I$(TOPDIR)/include \
		-I$(TOPDIR)/net/include \
		-I$(TOPDIR)/net/include/proto \
		-I$(TOPDIR)/net/wpa/crypto/include \
		-I$(TOPDIR)/net/wpa/wps/include \
		-I$(TOPDIR)/net/wpa/p2p/include \
		-I$(TOPDIR)/os/include	\
		-I$(TOPDIR)/os/linux/include

$(TARGET)-y += api/atbm_api.o

$(TARGET)-y += hal/atbm_ap.o hal/atbm_config.o hal/atbm_etf.o hal/atbm_init.o \
		hal/atbm_queue.o hal/atbm_skbuf.o hal/atbm_sta.o hal/atbm_task.o \
		hal/atbm_timer.o hal/atbm_txrx.o hal/atbm_wifi_driver_api.o hal/atbm_wsm.o \
		hal/smartconfig.o hal/atbm_main.o

$(TARGET)-y += net/atbm_connect.o net/atbm_key.o net/atbm_mgmt.o net/atbm_ratectrl.o \
		net/atbm_rc80211_minstrel.o net/atbm_rc80211_minstrel_ht.o net/atbm_rc80211_pid_algo.o \
		net/atbm_util.o
		
$(TARGET)-y += net/wpa/aes_core.o net/wpa/hostapd_main.o net/wpa/sha1.o \
		net/wpa/wpa_common.o net/wpa/wpa_event.o net/wpa/wpa_main.o

$(TARGET)-y += net/wpa/crypto/aes-cbc.o net/wpa/crypto/base64.o net/wpa/crypto/bignum.o \
		net/wpa/crypto/crypto_internal-modexp.o net/wpa/crypto/dh_group5.o \
		net/wpa/crypto/dh_groups.o net/wpa/crypto/sha256.o net/wpa/crypto/sha256-internal.o

$(TARGET)-y += net/wpa/wps/eap_common.o net/wpa/wps/eap_register.o net/wpa/wps/eap_server_wsc.o \
		net/wpa/wps/ieee802_11_common.o net/wpa/wps/uuid.o net/wpa/wps/wpabuf.o \
		net/wpa/wps/wps_attr_build.o net/wpa/wps/wps_attr_parse.o net/wpa/wps/wps_attr_process.o \
		net/wpa/wps/wps_common.o net/wpa/wps/wps_debug.o net/wpa/wps/wps_dev_attr.o \
		net/wpa/wps/wps_enrollee.o net/wpa/wps/wps_hostapd.o net/wpa/wps/wps_main.o \
		net/wpa/wps/wps_registrar.o net/wpa/wps/wps_sta.o net/wpa/wps/wps_validate.o

$(TARGET)-y += os/linux/atbm_os_api.o os/linux/atbm_os_atomic.o os/linux/atbm_os_mem.o \
		os/linux/atbm_os_msgQ.o os/linux/atbm_os_mutex.o os/linux/atbm_os_skbuf.o \
		os/linux/atbm_os_spinlock.o os/linux/atbm_os_thread.o os/linux/atbm_os_timer.o \
		os/linux/atbm_os_workqueue.o

ifeq ($(SDIO_BUS), y)
$(TARGET)-y += hal/sdio/atbm_dcxo_dpll.o hal/sdio/atbm_sdio.o hal/sdio/atbm_sdio_bh.o \
		hal/sdio/atbm_sdio_fwio.o hal/sdio/atbm_sdio_hwio.o
$(TARGET)-y += os/linux/atbm_os_sdio.o
INCLUDE += -I$(TOPDIR)/hal/sdio
ccflags-y += -DSDIO_BUS_USE
else
ifeq ($(USB_BUS), y)
$(TARGET)-y += hal/usb/atbm_usb.o hal/usb/atbm_usb_bh.o hal/usb/atbm_usb_fwio.o \
		hal/usb/atbm_usb_hwio.o
$(TARGET)-y += os/linux/atbm_os_usb.o
INCLUDE += -I$(TOPDIR)/hal/usb
ccflags-y += -DUSB_BUS_USE
else
ifeq ($(SPI_BUS), y)
$(TARGET)-y += hal/spi/atbm_spi.o hal/spi/atbm_spi_bh.o hal/spi/atbm_spi_fwio.o \
		hal/spi/atbm_spi_hwio.o
INCLUDE += -I$(TOPDIR)/hal/spi
ccflags-y += -DSPI_BUS_USE
endif
endif
endif

ccflags-y += -DATBM_SDIO_READ_ENHANCE=0 -DATBM_HW_CHIPER=0 -DBENTU_OS=0 -DMINSTREL_RATE_CONTROL=0
ccflags-y += -DLINUX_OS $(ATBM_WIFI__EXT_CCFLAGS) -fno-tree-scev-cprop -mlittle-endian
ccflags-y += $(INCLUDE)

KBUILD_CFLAGS  += -fno-tree-scev-cprop 

obj-m = $(TARGET).o

#build rule
.PHONY: all  target clean install 

all: install strip
	@echo "compare $(TARGET).ko success"

install:
	make ARCH=$(arch) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERDIR) M=$(TOPDIR) modules -j8

strip:
	$(CROSS_COMPILE)strip $(TOPDIR)/$(TARGET).ko --strip-unneeded

clean: 
	rm -rf $(TOPDIR)/$(TARGET).ko
	rm -rf $(TOPDIR)/*.symvers
	rm -rf $(TOPDIR)/*.order
	for i in $($(TARGET)-y); do rm -rf $(TOPDIR)/$$i; done




