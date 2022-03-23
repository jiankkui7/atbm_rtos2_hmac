#include "atbm_hal.h"

#include "atbm_os_sdio.h"
#include "sdio_porting.h"

#define SDIO_LOCK_INIT()						atbm_os_mutexLockInit(&sdio_lock)
#define SDIO_LOCK()								atbm_os_mutexLock(&sdio_lock,0)
#define SDIO_UNLOCK()							atbm_os_mutexUnLock(&sdio_lock)

static struct atbm_sdio_func sdio_func0;
static atbm_mutex sdio_lock;

struct atbm_sdio_cccr {
	unsigned int		sdio_vsn;
	unsigned int		sd_vsn;
	unsigned int		multi_block:1,
	low_speed:1,
	wide_bus:1,
	high_power:1,
	high_speed:1,
	disable_cd:1;
};

struct atbm_sdio_cccr cccr;

extern struct atbmwifi_common g_hw_prv;
static int irq_rx_coming = 0;

void SDIO_IRQHandler(void)
{
	OSIntEnter();
	if (SDIO_GetITStatus(SDIO_IT_SDIOIT) != RESET)
    {
		irq_rx_coming = 1;
		if (sdio_func0.irq_handle)
		{
			sdio_func0.irq_handle(&sdio_func0);
			irq_rx_coming = 0;
		}
        SDIO_ClearITPendingBit(SDIO_IT_SDIOIT);
    }
	OSIntExit();
}

int atbm_sdio_read_cccr(void)
{
	SD_Err err;
	int cccr_vsn;
	unsigned char data;

	atbm_memset(&cccr, 0, sizeof(struct atbm_sdio_cccr));

	err = sdio_read_byte(0, ATBM_SDIO_CCCR_CCCR, &data);
	if (err != SD_OK)
	{
		printf("sdio_read_byte CCCR err\n");
		return -1;
	}

	cccr_vsn = data & 0x0f;

	if (cccr_vsn > SDIO_CCCR_REV_1_20)
	{
		printf("unrecognised CCCR structure version %d\n", cccr_vsn);
		return -1;
	}

	cccr.sdio_vsn = (data & 0xf0) >> 4;

	err = sdio_read_byte(0, ATBM_SDIO_CCCR_CAPS, &data);
	if (err != SD_OK)
	{
		printf("sdio_read_byte CAPS err\n");
		return -1;
	}

	if (data & SDIO_CCCR_CAP_SMB)
	{
		cccr.multi_block = 1;
	}
	if (data & SDIO_CCCR_CAP_LSC)
	{
		cccr.low_speed = 1;
	}
	if (data & SDIO_CCCR_CAP_4BLS)
	{
		cccr.wide_bus = 1;
	}

	if (cccr_vsn >= SDIO_CCCR_REV_1_10)
	{
		err = sdio_read_byte(0, ATBM_SDIO_CCCR_POWER, &data);
		if (err != SD_OK)
		{
			printf("sdio_read_byte POWER err\n");
			return -1;
		}

		if (data & SDIO_POWER_SMPC)
		{
			cccr.high_power = 1;
		}
	}

	if (cccr_vsn >= SDIO_CCCR_REV_1_20)
	{
		err = sdio_read_byte(0, ATBM_SDIO_CCCR_SPEED, &data);
		if (err != SD_OK)
		{
			printf("sdio_read_byte SPEED err\n");
			return -1;
		}

		if (data & ATBM_SDIO_SPEED_SHS)
		{
			cccr.high_speed = 1;
		}

		err = sdio_read_byte(0, 7, &data);
		if (err != SD_OK)
		{
			printf("sdio_read_byte 7 err\n");
			return -1;
		}

		data |= 0x20;

		err = sdio_write_byte(0, 7, data);
		if (err != SD_OK)
		{
			printf("sdio_write_byte 7 err\n");
			return -1;
		}
	}

	return 0;
}

int atbm_sdio_register(struct atbm_sdio_driver *sdio_driver)
{
	int ret = 0;
	atbm_sdio_read_cccr();
	SDIO_LOCK_INIT();
	SDIO_ITConfig(SDIO_IT_SDIOIT, ENABLE);
	sdio_func0.num = 1;
	sdio_func0.irq_handle = NULL;
	sdio_func0.en = 0;
	sdio_func0.priv = NULL;
	sdio_func0.blocksize = 512;
	sdio_set_block_len(sdio_func0.num, sdio_func0.blocksize);

	return sdio_driver->probe_func(&sdio_func0, sdio_driver->id_table);
}

void atbm_sdio_deregister(struct atbm_sdio_driver *sdio_driver)
{
	sdio_driver->discon_func(&sdio_func0);
}

void atbm_sdio_claim_host(struct atbm_sdio_func *func)
{
	SDIO_LOCK();
}

void atbm_sdio_release_host(struct atbm_sdio_func *func)
{
	SDIO_UNLOCK();
}

atbm_int32 atbm_sdio_enable_func(struct atbm_sdio_func *func)
{
	func->en = 1;
	sdio_enable_func(func->num);
	return 0;
}

void atbm_sdio_disable_func(struct atbm_sdio_func *func)
{
	func->en = 0;
}

void atbm_sdio_set_drvdata(struct atbm_sdio_func *func,void *priv)
{
	func->priv = priv;
}

void *atbm_sdio_get_drvdata(struct atbm_sdio_func *func)
{
	return func->priv;
}

int atbm_sdio_claim_irq(struct atbm_sdio_func *func,void (*irq_handle)(struct atbm_sdio_func *func))
{
	func->irq_handle = irq_handle;
	if (irq_rx_coming)
	{
		func->irq_handle(func);
		irq_rx_coming = 0;
	}
	return 0;
}

int atbm_sdio_release_irq(struct atbm_sdio_func *func)
{
	func->irq_handle = NULL;
	return 0;
}

int __atbm_sdio_memcpy_fromio(struct atbm_sdio_func *func,void *dst,unsigned int addr,int count)
{
	int ret = 0;

	if (func->en == 0)
	{
		wifi_printk(WIFI_DBG_ERROR,"atbmwifi sdio driver func not enable\n");
		return -1;
	}

	if((((unsigned int)dst) & 3)||(count & 3)){
		wifi_printk(WIFI_DBG_ERROR,"Read Data & Len must be 4 align,dst=0x%08x,count=%d",(unsigned int )dst,count);
	}

	if(sdio_read_multi_data(func->num, addr, count, (unsigned char *)dst) != SD_OK)
	{
		wifi_printk(WIFI_DBG_ERROR,"sdio_read_multi_data fail\n");
		ret = -1;
	}

	return ret;
}

int __atbm_sdio_memcpy_toio(struct atbm_sdio_func *func,unsigned int addr,void *src,int count)
{
	int ret = 0;

	if (func->en == 0)
	{
		wifi_printk(WIFI_DBG_ERROR,"atbmwifi sdio driver func not enable\n");
		return -1;
	}

	if((((unsigned int)src) & 3)||(count & 3)){
		wifi_printk(WIFI_DBG_ERROR,"Write Data & Len must be 4 align,src=0x%08x,count=%d",(unsigned int )src,count);
	}

	if (sdio_write_multi_data(func->num, addr, count, (unsigned char *)src) != SD_OK)
	{
		wifi_printk(WIFI_DBG_ERROR,"sdio_write_multi_data fail\n");
		ret = -1;
	}

	return ret;
}

unsigned char atbm_sdio_f0_readb(struct atbm_sdio_func *func,unsigned int addr,int *retval)
{
	unsigned char date = 0;
	bool ret;

	ret = sdio_read_byte(func->num, addr, &date);
	*retval = 0;
	if(ret == false)
		*retval = -1;

	return date;
}

void atbm_sdio_f0_writeb(struct atbm_sdio_func *func,unsigned char regdata,unsigned int addr,int *retval)
{
	bool ret;

	ret = sdio_write_byte(func->num, addr, regdata);
	*retval = 0;
	if(ret == false)
		*retval = -1;

	return;
}

atbm_uint32 atbm_sdio_alignsize(struct atbm_sdio_func *func,atbm_uint32 size)
{
	atbm_uint32 block_size = func->blocksize;
	return (size%block_size == 0)?size:(size+block_size-size%block_size);
}

int  atbm_sdio_set_blocksize(struct atbm_sdio_func *func,int blocksize)
{
	return (sdio_set_block_len(func->num, blocksize) != SD_OK);
}

