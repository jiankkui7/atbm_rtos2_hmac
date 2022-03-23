#ifndef ATBM_OS_API_H
#define ATBM_OS_API_H

#include "atbm_type.h"

#include "atbm_os_sdio.h"

#include "atbm_os_mem.h"

#define HZ 100

#ifndef atbm_packed
#define atbm_packed //__attribute__ ((packed))
#endif //__packed

struct __una_u16 { atbm_uint16 x; } atbm_packed ;
struct __una_u32  { atbm_uint32 x; } atbm_packed ;

#define __builtin_return_address(x) (x)


static __INLINE atbm_uint16 __get_unaligned_cpu16(const atbm_void *p)
{
 	const struct __una_u16 *ptr = (const struct __una_u16 *)p;
 	return ptr->x;
}

static __INLINE  atbm_uint32 __get_unaligned_cpu32 (const atbm_void *p)
{
	 const struct __una_u32  *ptr = (const struct __una_u32  *)p;
	 return ptr->x;
}

static __INLINE atbm_uint32 get_unaligned_le32(const atbm_void *p)
{
	return __get_unaligned_cpu32 ((const atbm_uint8 *)p);
}

static __INLINE atbm_void *strdup(const char *str)
{
	char *tmp = 0;
	unsigned int len = strlen(str);
	tmp = (char *)atbm_kmalloc(len+1, GFP_KERNEL);
	if (tmp == 0){
		return 0;
	}

	atbm_memcpy(tmp, str, len+1);
	return tmp;
}

int atbmwifi_event_OsCallback(atbm_void *prv,int eventid,atbm_void *param);
#if ATBM_USB_BUS
int atbm_usb_register_init(void);
int atbm_usb_register_deinit(void);
#elif ATBM_SDIO_BUS
int atbm_sdio_register_init(void);
int atbm_sdio_register_deinit(void);
#endif

#endif //ATBM_OS_API_H
