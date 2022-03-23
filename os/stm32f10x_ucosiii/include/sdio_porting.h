#ifndef __SDIO_PORTING_H
#define __SDIO_PORTING_H

#include "stm32f10x.h"

typedef enum                                 /* SDIO specific error defines */
{
  SD_CMD_CRC_FAIL                    = (1),  /* Command response received (but CRC check failed) */
  SD_DATA_CRC_FAIL                   = (2),  /* Data bock sent/received (CRC check Failed) */
  SD_CMD_RSP_TIMEOUT                 = (3),  /* Command response timeout */
  SD_DATA_TIMEOUT                    = (4),  /* Data time out */
  SD_TX_UNDERRUN                     = (5),  /* Transmit FIFO under-run */
  SD_RX_OVERRUN                      = (6),  /* Receive FIFO over-run */
  SD_START_BIT_ERR                   = (7),  /* Start bit not detected on all data signals in widE bus mode */
  SD_CMD_OUT_OF_RANGE                = (8),  /* CMD's argument was out of range.*/
  SD_ADDR_MISALIGNED                 = (9),  /* Misaligned address */
  SD_BLOCK_LEN_ERR                   = (10), /* Transferred block length is not allowed for the card or the number of transferred bytes does not match the block length */
  SD_ERASE_SEQ_ERR                   = (11), /* An error in the sequence of erase command occurs.*/
  SD_BAD_ERASE_PARAM                 = (12), /* An Invalid selection for erase groups */
  SD_WRITE_PROT_VIOLATION            = (13), /* Attempt to program a write protect block */
  SD_LOCK_UNLOCK_FAILED              = (14), /* Sequence or password error has been detected in unlock command or if there was an attempt to access a locked card */
  SD_COM_CRC_FAILED                  = (15), /* CRC check of the previous command failed */
  SD_ILLEGAL_CMD                     = (16), /* Command is not legal for the card state */
  SD_CARD_ECC_FAILED                 = (17), /* Card internal ECC was applied but failed to correct the data */
  SD_CC_ERROR                        = (18), /* Internal card controller error */
  SD_GENERAL_UNKNOWN_ERROR           = (19), /* General or Unknown error */
  SD_STREAM_READ_UNDERRUN            = (20), /* The card could not sustain data transfer in stream read operation. */
  SD_STREAM_WRITE_OVERRUN            = (21), /* The card could not sustain data programming in stream mode */
  SD_CID_CSD_OVERWRITE               = (22), /* CID/CSD overwrite error */
  SD_WP_ERASE_SKIP                   = (23), /* only partial address space was erased */
  SD_CARD_ECC_DISABLED               = (24), /* Command has been executed without using internal ECC */
  SD_ERASE_RESET                     = (25), /* Erase sequence was cleared before executing because an out of erase sequence command was received */
  SD_AKE_SEQ_ERROR                   = (26), /* Error in sequence of authentication. */
  SD_INVALID_VOLTRANGE               = (27),
  SD_ADDR_OUT_OF_RANGE               = (28),
  SD_SWITCH_ERROR                    = (29),
  SD_SDIO_DISABLED                   = (30),
  SD_SDIO_FUNCTION_BUSY              = (31),
  SD_SDIO_FUNCTION_FAILED            = (32),
  SD_SDIO_UNKNOWN_FUNCTION           = (33),

  /* Standard error defines */
  SD_INTERNAL_ERROR, 
  SD_NOT_CONFIGURED,
  SD_REQUEST_PENDING, 
  SD_REQUEST_NOT_APPLICABLE, 
  SD_INVALID_PARAMETER,  
  SD_UNSUPPORTED_FEATURE,  
  SD_UNSUPPORTED_HW,  
  SD_ERROR,  
  SD_OK,  
} SD_Err;

typedef enum
{
  SD_NO_TRANSFER  = 0,
  SD_TRANSFER_IN_PROGRESS
} SDTransferState;

typedef struct
{
  uint16_t TransferredBytes;
  SD_Err TransferError;
  uint8_t  padding;
} SDLastTransferInfo;

/* Exported constants --------------------------------------------------------*/
/* SDIO Commands Index */
#define SDIO_GO_IDLE_STATE                       ((uint8_t)0)
#define SDIO_SEND_OP_COND                        ((uint8_t)1)
#define SDIO_ALL_SEND_CID                        ((uint8_t)2)
#define SDIO_SET_REL_ADDR                        ((uint8_t)3)
#define SDIO_SET_DSR                             ((uint8_t)4)
#define SDIO_SDIO_SEN_OP_COND                    ((uint8_t)5)
#define SDIO_HS_SWITCH                           ((uint8_t)6)
#define SDIO_SEL_DESEL_CARD                      ((uint8_t)7)
#define SDIO_HS_SEND_EXT_CSD                     ((uint8_t)8)
#define SDIO_SEND_CSD                            ((uint8_t)9)
#define SDIO_SEND_CID                            ((uint8_t)10)
#define SDIO_READ_DAT_UNTIL_STOP                 ((uint8_t)11) /* SD Card doesn't support it */
#define SDIO_STOP_TRANSMISSION                   ((uint8_t)12)
#define SDIO_SEND_STATUS                         ((uint8_t)13)
#define SDIO_HS_BUSTEST_READ                     ((uint8_t)14)
#define SDIO_GO_INACTIVE_STATE                   ((uint8_t)15)
#define SDIO_SET_BLOCKLEN                        ((uint8_t)16)
#define SDIO_READ_SINGLE_BLOCK                   ((uint8_t)17)
#define SDIO_READ_MULT_BLOCK                     ((uint8_t)18)
#define SDIO_HS_BUSTEST_WRITE                    ((uint8_t)19)
#define SDIO_WRITE_DAT_UNTIL_STOP                ((uint8_t)20) /* SD Card doesn't support it */
#define SDIO_SET_BLOCK_COUNT                     ((uint8_t)23) /* SD Card doesn't support it */
#define SDIO_WRITE_SINGLE_BLOCK                  ((uint8_t)24)
#define SDIO_WRITE_MULT_BLOCK                    ((uint8_t)25)
#define SDIO_PROG_CID                            ((uint8_t)26) /* reserved for manufacturers */
#define SDIO_PROG_CSD                            ((uint8_t)27)
#define SDIO_SET_WRITE_PROT                      ((uint8_t)28)
#define SDIO_CLR_WRITE_PROT                      ((uint8_t)29)
#define SDIO_SEND_WRITE_PROT                     ((uint8_t)30)
#define SDIO_SD_ERASE_GRP_START                  ((uint8_t)32) /* To set the address of the first write
                                                                  block to be erased. (For SD card only) */
#define SDIO_SD_ERASE_GRP_END                    ((uint8_t)33) /* To set the address of the last write block of the
                                                                  continuous range to be erased. (For SD card only) */
#define SDIO_ERASE_GRP_START                     ((uint8_t)35) /* To set the address of the first write block to be erased.
                                                                  (For MMC card only spec 3.31) */
#define SDIO_ERASE_GRP_END                       ((uint8_t)36) /* To set the address of the last write block of the
                                                                  continuous range to be erased. (For MMC card only spec 3.31) */
#define SDIO_ERASE                               ((uint8_t)38)
#define SDIO_FAST_IO                             ((uint8_t)39) /* SD Card doesn't support it */
#define SDIO_GO_IRQ_STATE                        ((uint8_t)40) /* SD Card doesn't support it */
#define SDIO_LOCK_UNLOCK                         ((uint8_t)42)
#define SDIO_APP_CMD                             ((uint8_t)55)
#define SDIO_GEN_CMD                             ((uint8_t)56)
#define SDIO_NO_CMD                              ((uint8_t)64)

/* SD Card Application Specific commands Index*/
#define SDIO_APP_SD_SET_BUSWIDTH                 ((uint8_t)6)  /* For SD Card only */
#define SDIO_SEND_IF_COND                        ((uint8_t)8)  /* For SD Card only */
#define SDIO_SD_APP_STAUS                        ((uint8_t)13) /* For SD Card only */
#define SDIO_SD_APP_SEND_NUM_WRITE_BLOCKS        ((uint8_t)22) /* For SD Card only */
#define SDIO_SD_APP_OP_COND                      ((uint8_t)41) /* For SD Card only */
#define SDIO_SD_APP_SET_CLR_CARD_DETECT          ((uint8_t)42) /* For SD Card only */
#define SDIO_SD_APP_SEND_SCR                     ((uint8_t)51) /* For SD Card only */
#define SDIO_SDIO_RW_DIRECT                      ((uint8_t)52) /* For SD I/O Card only */
#define SDIO_SDIO_RW_EXTENDED                    ((uint8_t)53) /* For SD I/O Card only */

/* Transmission Mode */
#define SD_DMA_MODE                              ((uint32_t)0x00000000)
#define SD_POLLING_MODE                          ((uint32_t)0x00000001)

/* Lock&Unlock Status */
#define SD_LOCK                                  ((uint8_t)0x05)
#define SD_UNLOCK                                ((uint8_t)0x02)

/* Command Class Supported */
#define SD_CCC_SWITCH                            ((uint32_t)0x00000400) /* Class 10 */
#define SD_CCC_IO_MODE                           ((uint32_t)0x00000200) /* Class 9 */
#define SD_CCC_APP_SPECIFIC                      ((uint32_t)0x00000100) /* Class 8 */
#define SD_CCC_LOCK_UNLOCK                       ((uint32_t)0x00000080) /* Class 7 */
#define SD_CCC_WRITE_PROT                        ((uint32_t)0x00000040) /* Class 6 */
#define SD_CCC_ERASE                             ((uint32_t)0x00000020) /* Class 5 */
#define SD_CCC_BLOCK_WRITE                       ((uint32_t)0x00000010) /* Class 4 */
#define SD_CCC_STREAM_WRITE                      ((uint32_t)0x00000008) /* Class 3 */
#define SD_CCC_BLOCK_READ                        ((uint32_t)0x00000004) /* Class 2 */
#define SD_CCC_STREAM_READ                       ((uint32_t)0x00000002) /* Class 1 */

/* Supported Memory Cards */
#define SDIO_STD_CAPACITY_SD_CARD_V1_1           ((uint32_t)0x0)
#define SDIO_STD_CAPACITY_SD_CARD_V2_0           ((uint32_t)0x1)
#define SDIO_HIGH_CAPACITY_SD_CARD               ((uint32_t)0x2)
#define SDIO_MULTIMEDIA_CARD                     ((uint32_t)0x3)
#define SDIO_SECURE_DIGITAL_IO_CARD              ((uint32_t)0x4)
#define SDIO_HIGH_SPEED_MULTIMEDIA_CARD          ((uint32_t)0x5)
#define SDIO_SECURE_DIGITAL_IO_COMBO_CARD        ((uint32_t)0x6)
#define SDIO_HIGH_CAPACITY_MMC_CARD              ((uint32_t)0x7)


/* Exported functions ------------------------------------------------------- */
void sdio_time_delay(uint32_t ms);
SD_Err sdio_card_init(void);
SD_Err sdio_read_byte(unsigned char func, unsigned long addr, unsigned char *data);
SD_Err sdio_write_byte(unsigned char func, unsigned long addr, unsigned char data);
SD_Err sdio_read_multi_data(unsigned char func, unsigned long src, unsigned long count, unsigned char *data);
SD_Err sdio_write_multi_data(unsigned char func, unsigned long src, unsigned long count, unsigned char *data);
SD_Err sdio_set_block_len(unsigned char func, unsigned long block_len);
SD_Err sdio_enable_func(unsigned char func);
SD_Err sdio_set_buswide(uint32_t WideMode);
SD_Err sdio_irq(void);

#endif /* __SDIO_PORTING_H */

