#include "sdio_porting.h"
#include "os.h"
#include <stdio.h>

/* Private define --------------------------------------------------------------*/
#define NULL 0
#define SDIO_STATIC_FLAGS               ((uint32_t)0x00c007FF)
#define SDIO_FIFO_ADDR                  ((uint32_t)0x40018080)

/* Mask for errors Card Status R1 */
#define SD_R1_ADDR_OUT_OF_RANGE         ((uint32_t)0x80000000)
#define SD_R1_ADDR_MISALIGNED           ((uint32_t)0x40000000)
#define SD_R1_BLOCK_LEN_ERR             ((uint32_t)0x20000000)
#define SD_R1_ERASE_SEQ_ERR             ((uint32_t)0x10000000)
#define SD_R1_BAD_ERASE_PARAM           ((uint32_t)0x08000000)
#define SD_R1_WRITE_PROT_VIOLATION      ((uint32_t)0x04000000)
#define SD_R1_LOCK_UNLOCK_FAILED        ((uint32_t)0x01000000)
#define SD_R1_COM_CRC_FAILED            ((uint32_t)0x00800000)
#define SD_R1_ILLEGAL_CMD               ((uint32_t)0x00400000)
#define SD_R1_CARD_ECC_FAILED           ((uint32_t)0x00200000)
#define SD_R1_CC_ERROR                  ((uint32_t)0x00100000)
#define SD_R1_GENERAL_UNKNOWN_ERROR     ((uint32_t)0x00080000)
#define SD_R1_STREAM_READ_UNDERRUN      ((uint32_t)0x00040000)
#define SD_R1_STREAM_WRITE_OVERRUN      ((uint32_t)0x00020000)
#define SD_R1_CID_CSD_OVERWRIETE        ((uint32_t)0x00010000)
#define SD_R1_WP_ERASE_SKIP             ((uint32_t)0x00008000)
#define SD_R1_CARD_ECC_DISABLED         ((uint32_t)0x00004000)
#define SD_R1_ERASE_RESET               ((uint32_t)0x00002000)
#define SD_R1_AKE_SEQ_ERROR             ((uint32_t)0x00000008)
#define SD_R1_ERRORBITS                 ((uint32_t)0xFDFFE008)

/* Masks for R6 Response */
#define SD_R6_COM_CRC_FAILED            ((uint32_t)0x00008000)
#define SD_R6_ILLEGAL_CMD               ((uint32_t)0x00004000)
#define SD_R6_GENERAL_UNKNOWN_ERROR     ((uint32_t)0x00002000)

/* DSM data timeout */
#define SD_DATATIMEOUT                  ((uint32_t)0x00800000)

/* Masks for SCR Register */
#define SD_0TO7BITS                     ((uint32_t)0x000000FF)
#define SD_8TO15BITS                    ((uint32_t)0x0000FF00)
#define SD_16TO23BITS                   ((uint32_t)0x00FF0000)
#define SD_24TO31BITS                   ((uint32_t)0xFF000000)

#define SD_MAX_DATA_LENGTH              ((uint32_t)0x01FFFFFF)

#define SD_HALFFIFO                     ((uint32_t)0x00000008)
#define SD_HALFFIFOBYTES                ((uint32_t)0x00000020)

#define SD_ALLZERO                      ((uint32_t)0x00000000)

/* Clock division factor used for init the card*/
#define SDIO_INIT_CLK_DIV               ((uint8_t)0xB2)

#define CCCR_IO_ABORT 0x06

#define SDIO_FUN_NUM_MASK                (3<<28)
#define SDIO_FUN_NUM_OFFSET               28
#define SDIO_MP_MASK                     (1<<27)
#define SDIO_OCR_MASK                    (0xFFFFFF)
#define SDIO_DEFAULT_VOLTAGE             (0x00FF8000)

#define SDIO_MAX_BLOCK_LEN              2048

/* Card Common Control Registers (CCCR) */

#define CCCR_SDIO_REVISION                  0x00
#define CCCR_SD_SPECIFICATION_REVISION      0x01
#define CCCR_IO_ENABLE                      0x02
#define CCCR_IO_READY                       0x03
#define CCCR_INT_ENABLE                     0x04
#define CCCR_INT_PENDING                    0x05
#define CCCR_IO_ABORT                       0x06
#define CCCR_BUS_INTERFACE_CONTOROL         0x07
#define CCCR_CARD_CAPABILITY	            0x08
#define CCCR_COMMON_CIS_POINTER             0x09 /*0x09-0x0B*/
#define CCCR_FN0_BLOCK_SIZE	                0x10 /*0x10-0x11*/

#define SDIO_CCCR_INTEN_IENM (1<<0)
#define SDIO_CCCR_INTEN_IEN1 (1<<1)

#define SD_STATUS_POWERUP      (1<<31)
#define SD_DEFAULT_VOLTAGE     (0x00FF8000)

#define CMD52_READ             (0)
#define CMD52_WRITE            (1)
#define CMD52_READ_AFTER_WRITE (1)
#define CMD52_NORMAL_WRITE     (0)

#define CMD53_WRITE        (1<<31)
#define CMD53_BLOCKMODE    (1<<27)
#define CMD53_INCREMENTING (1<<26)
#define CMD53_TIMEOUT      (10000000)

#define SDIO_SET_CMD52_ARG(arg,rw,func,raw,address,writedata) \
	 (arg) = (((rw) & 1) << 31) 		  | \
			 (((func) & 0x7) << 28) 	  | \
			 (((raw) & 1) << 27)		  | \
			 (1 << 26)					  | \
			 (((address) & 0x1FFFF) << 9) | \
			 (1 << 8)					  | \
			 ((writedata) & 0xFF)

SDIO_InitTypeDef SDIO_InitStructure;
SDIO_CmdInitTypeDef SDIO_CmdInitStructure;
SDIO_DataInitTypeDef SDIO_DataInitStructure;

static unsigned long s_block_length = 0;

static SD_Err sdio_check_err_with_crc(void)
{
    SD_Err Status = SD_OK;
    uint32_t status;

    status = SDIO->STA;

    while (!(status & (SDIO_FLAG_CCRCFAIL | SDIO_FLAG_CMDREND | SDIO_FLAG_CTIMEOUT)))
    {
        status = SDIO->STA;
    }

    if (status & SDIO_FLAG_CTIMEOUT)
    {
        Status = SD_CMD_RSP_TIMEOUT;
        SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);
        return(Status);
    }
    else if (status & SDIO_FLAG_CCRCFAIL)
    {
        Status = SD_CMD_CRC_FAIL;
        SDIO_ClearFlag(SDIO_FLAG_CCRCFAIL);
        return(Status);
    }

    /* Clear all the static flags */
    SDIO_ClearFlag(SDIO_STATIC_FLAGS);
    return(Status);
}

static SD_Err sdio_check_err_without_crc(void)
{
    SD_Err Status = SD_OK;
    uint32_t status;

    status = SDIO->STA;

    while (!(status & (SDIO_FLAG_CCRCFAIL | SDIO_FLAG_CMDREND | SDIO_FLAG_CTIMEOUT)))
    {
        status = SDIO->STA;
    }

    if (status & SDIO_FLAG_CTIMEOUT)
    {
        Status = SD_CMD_RSP_TIMEOUT;
        SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);
        return(Status);
    }

    /* Clear all the static flags */
    SDIO_ClearFlag(SDIO_STATIC_FLAGS);
    return(Status);
}

static SD_Err sdio_check_err_R1(uint8_t cmd)
{
    SD_Err Status = SD_OK;
    uint32_t status;
    uint32_t response_r1;

    status = SDIO->STA;

    while (!(status & (SDIO_FLAG_CCRCFAIL | SDIO_FLAG_CMDREND | SDIO_FLAG_CTIMEOUT)))
    {
        status = SDIO->STA;
    }

    if (status & SDIO_FLAG_CTIMEOUT)
    {
        Status = SD_CMD_RSP_TIMEOUT;
        SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);
        return(Status);
    }
    else if (status & SDIO_FLAG_CCRCFAIL)
    {
        Status = SD_CMD_CRC_FAIL;
        SDIO_ClearFlag(SDIO_FLAG_CCRCFAIL);
        return(Status);
    }

    /* Check response received is of desired command */
    if (SDIO_GetCommandResponse() != cmd)
    {
        Status = SD_ILLEGAL_CMD;
        return(Status);
    }

    /* Clear all the static flags */
    SDIO_ClearFlag(SDIO_STATIC_FLAGS);

    /* We have received response, retrieve it for analysis  */
    response_r1 = SDIO_GetResponse(SDIO_RESP1);

    if ((response_r1 & SD_R1_ERRORBITS) == SD_ALLZERO)//no error
    {
        return(Status);
    }

    if (response_r1 & SD_R1_ADDR_OUT_OF_RANGE)
    {
        return(SD_ADDR_OUT_OF_RANGE);
    }

    if (response_r1 & SD_R1_ADDR_MISALIGNED)
    {
        return(SD_ADDR_MISALIGNED);
    }

    if (response_r1 & SD_R1_BLOCK_LEN_ERR)
    {
        return(SD_BLOCK_LEN_ERR);
    }

    if (response_r1 & SD_R1_ERASE_SEQ_ERR)
    {
        return(SD_ERASE_SEQ_ERR);
    }

    if (response_r1 & SD_R1_BAD_ERASE_PARAM)
    {
        return(SD_BAD_ERASE_PARAM);
    }

    if (response_r1 & SD_R1_WRITE_PROT_VIOLATION)
    {
        return(SD_WRITE_PROT_VIOLATION);
    }

    if (response_r1 & SD_R1_LOCK_UNLOCK_FAILED)
    {
        return(SD_LOCK_UNLOCK_FAILED);
    }

    if (response_r1 & SD_R1_COM_CRC_FAILED)
    {
        return(SD_COM_CRC_FAILED);
    }

    if (response_r1 & SD_R1_ILLEGAL_CMD)
    {
        return(SD_ILLEGAL_CMD);
    }

    if (response_r1 & SD_R1_CARD_ECC_FAILED)
    {
        return(SD_CARD_ECC_FAILED);
    }

    if (response_r1 & SD_R1_CC_ERROR)
    {
        return(SD_CC_ERROR);
    }

    if (response_r1 & SD_R1_GENERAL_UNKNOWN_ERROR)
    {
    	return(SD_GENERAL_UNKNOWN_ERROR);
    }

    if (response_r1 & SD_R1_STREAM_READ_UNDERRUN)
    {
        return(SD_STREAM_READ_UNDERRUN);
    }

    if (response_r1 & SD_R1_STREAM_WRITE_OVERRUN)
    {
    	return(SD_STREAM_WRITE_OVERRUN);
    }

    if (response_r1 & SD_R1_CID_CSD_OVERWRIETE)
    {
        return(SD_CID_CSD_OVERWRITE);
    }

    if (response_r1 & SD_R1_WP_ERASE_SKIP)
    {
        return(SD_WP_ERASE_SKIP);
    }

    if (response_r1 & SD_R1_CARD_ECC_DISABLED)
    {
        return(SD_CARD_ECC_DISABLED);
    }

    if (response_r1 & SD_R1_ERASE_RESET)
    {
        return(SD_ERASE_RESET);
    }

    if (response_r1 & SD_R1_AKE_SEQ_ERROR)
    {
        return(SD_AKE_SEQ_ERROR);
    }

    return(Status);
}

static SD_Err sdio_check_err_R4(uint32_t *ocr)
{
    SD_Err Status = SD_OK;
    uint32_t status;
	uint32_t response = 0;

    status = SDIO->STA;

    while (!(status & (SDIO_FLAG_CCRCFAIL | SDIO_FLAG_CMDREND | SDIO_FLAG_CTIMEOUT)))
    {
        status = SDIO->STA;
    }

    if (status & SDIO_FLAG_CTIMEOUT)
    {
        Status = SD_CMD_RSP_TIMEOUT;
        SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);
        return(Status);
    }

	response = SDIO_GetResponse(SDIO_RESP1);
	if (0 == (response & SDIO_FUN_NUM_MASK))
	{
		return SD_ERROR;
	}

	if (0 == (response & 0xFF8000))
	{
		printf("ocr invalid\n");
		return SD_ERROR;
	}
	
	*ocr = response & SDIO_OCR_MASK;
	SDIO_ClearFlag(SDIO_STATIC_FLAGS);
    return(Status);
}

static SD_Err sdio_check_err_R6(uint8_t cmd, uint16_t *prca)
{
    SD_Err Status = SD_OK;
    uint32_t status;
    uint32_t response_r1;

    status = SDIO->STA;

    while (!(status & (SDIO_FLAG_CCRCFAIL | SDIO_FLAG_CMDREND | SDIO_FLAG_CTIMEOUT)))
    {
        status = SDIO->STA;
    }

    if (status & SDIO_FLAG_CTIMEOUT)
    {
        Status = SD_CMD_RSP_TIMEOUT;
        SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);
        return(Status);
    }
    else if (status & SDIO_FLAG_CCRCFAIL)
    {
        Status = SD_CMD_CRC_FAIL;
        SDIO_ClearFlag(SDIO_FLAG_CCRCFAIL);
        return(Status);
    }

    /* Check response received is of desired command */
    if (SDIO_GetCommandResponse() != cmd)
    {
        Status = SD_ILLEGAL_CMD;
        return(Status);
    }

    /* Clear all the static flags */
    SDIO_ClearFlag(SDIO_STATIC_FLAGS);

    /* We have received response, retrieve it.  */
    response_r1 = SDIO_GetResponse(SDIO_RESP1);

    if (SD_ALLZERO == (response_r1 & (SD_R6_GENERAL_UNKNOWN_ERROR | SD_R6_ILLEGAL_CMD | SD_R6_COM_CRC_FAILED)))
    {
        *prca = (uint16_t) (response_r1 >> 16);
        return(Status);
    }

    if (response_r1 & SD_R6_GENERAL_UNKNOWN_ERROR)
    {
        return(SD_GENERAL_UNKNOWN_ERROR);
    }
    
    if (response_r1 & SD_R6_ILLEGAL_CMD)
    {
        return(SD_ILLEGAL_CMD);
    }
    
    if (response_r1 & SD_R6_COM_CRC_FAILED)
    {
        return(SD_COM_CRC_FAILED);
    }

    return(Status);
}

void sdio_time_delay(uint32_t ms)
{
	OS_ERR err;
	OSTimeDly(ms,0,&err);
}

static void sdio_gpio_configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	int i = 0;

    /* Enable GPIOC and GPIOD clock and GPIOB reset clock*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE);

	GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_RESET);

	/* Configure PB12 wifi reset */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

	sdio_time_delay(100);

	GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_SET);

    /* Configure relative ports(PC8, PC9, PC10, PC11, PC12: D0, D1, D2, D3, CLK pin) */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    /* Configure PD2(CMD line) */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
}

static void sdio_dma_tx_configuration(uint32_t *BufferSRC, uint32_t BufferSize)
{
    DMA_InitTypeDef DMA_InitStructure;

    DMA_ClearFlag(DMA2_FLAG_TC4 | DMA2_FLAG_TE4 | DMA2_FLAG_HT4 | DMA2_FLAG_GL4);

    /* DMA2 Channel4 disable */
    DMA_Cmd(DMA2_Channel4, DISABLE);

    /* DMA2 Channel4 Config */
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)SDIO_FIFO_ADDR;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)BufferSRC;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = BufferSize / 4;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA2_Channel4, &DMA_InitStructure);

    /* DMA2 Channel4 enable */
    DMA_Cmd(DMA2_Channel4, ENABLE);
}

static void sdio_dma_rx_configuration(uint32_t *BufferDST, uint32_t BufferSize)
{
    DMA_InitTypeDef DMA_InitStructure;

    DMA_ClearFlag(DMA2_FLAG_TC4 | DMA2_FLAG_TE4 | DMA2_FLAG_HT4 | DMA2_FLAG_GL4);

    /* DMA2 Channel4 disable */
    DMA_Cmd(DMA2_Channel4, DISABLE);

    /* DMA2 Channel4 Config */
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)SDIO_FIFO_ADDR;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)BufferDST;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = BufferSize / 4;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA2_Channel4, &DMA_InitStructure);

    /* DMA2 Channel4 enable */
    DMA_Cmd(DMA2_Channel4, ENABLE);
}

SD_Err sdio_card_init(void)
{
    SD_Err Status = SD_OK;
	uint32_t cnt = 0;
	uint32_t i = 0;
	uint32_t response = 0;
	uint32_t ocr = 0;
	uint32_t rca = 0x01;

	/* Open Periph Clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE);

    /* Configure GPIO about SDIO interface */
    sdio_gpio_configuration();

    /* Enable the SDIO and DMA2 Clock */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_SDIO | RCC_AHBPeriph_DMA2, ENABLE);

    /* Deinitialize the SDIO */
    SDIO_DeInit();
	
	/* Configure the SDIO peripheral */
	/* HCLK = 72MHz, SDIOCLK = 72MHz, SDIO_CK = HCLK/(178 + 2) = 400 KHz */
	SDIO_InitStructure.SDIO_ClockDiv = SDIO_INIT_CLK_DIV;
	SDIO_InitStructure.SDIO_ClockEdge = SDIO_ClockEdge_Rising;
	SDIO_InitStructure.SDIO_ClockBypass = SDIO_ClockBypass_Disable;
	SDIO_InitStructure.SDIO_ClockPowerSave = SDIO_ClockPowerSave_Disable;
	SDIO_InitStructure.SDIO_BusWide = SDIO_BusWide_1b;
	SDIO_InitStructure.SDIO_HardwareFlowControl = SDIO_HardwareFlowControl_Disable;
	SDIO_Init(&SDIO_InitStructure);

	/* Configure Power State to ON */
	SDIO_SetPowerState(SDIO_PowerState_ON);

	/* Enable SDIO Clock */
	SDIO_ClockCmd(ENABLE);

	/* Enable SDIO Operation */
	SDIO_SetSDIOOperation(ENABLE);

	sdio_time_delay(100);

	/* Send CMD5 to inquire voltage range */
	SDIO_CmdInitStructure.SDIO_Argument = 0;
	SDIO_CmdInitStructure.SDIO_CmdIndex = SDIO_SDIO_SEN_OP_COND;
	SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
	SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
	SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
	SDIO_SendCommand(&SDIO_CmdInitStructure);

	Status = sdio_check_err_R4(&ocr);
	if (Status != SD_OK)
	{
		printf("SDIO_SDIO_SEN_OP_COND sdio_check_err_R4 OK\n");
		return (Status);
	}

	do/* Send CMD5 to set power voltage */
	{
		SDIO_CmdInitStructure.SDIO_Argument = ocr & SD_DEFAULT_VOLTAGE;
		SDIO_CmdInitStructure.SDIO_CmdIndex = SDIO_SDIO_SEN_OP_COND;
		SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
		SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
		SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
		SDIO_SendCommand(&SDIO_CmdInitStructure);

		Status = sdio_check_err_without_crc();
		if (Status != SD_OK)
		{
			printf("SDIO_SDIO_SEN_OP_COND sdio_check_err_without_crc err\n");
			return(Status);
		}

		response = SDIO_GetResponse(SDIO_RESP1);
	} while ((!(response & SD_STATUS_POWERUP)) && (i++ < 10000));

	if (i >= 10000)
	{
		return SD_ERROR;
	}

	/* Check if sdio powerup */
	if (SDIO_GetPowerState() == SDIO_PowerState_OFF)
    {
    	printf("SDIO_GetPowerState not OPEN\n");
        Status = SD_REQUEST_NOT_APPLICABLE;
        return(Status);
    }

    /* Send CMD3 to set the RCA */
    SDIO_CmdInitStructure.SDIO_Argument = 0x00;
    SDIO_CmdInitStructure.SDIO_CmdIndex = SDIO_SET_REL_ADDR;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);
    
    /* Check CMD errors */
    Status = sdio_check_err_R6(SDIO_SET_REL_ADDR, (uint16_t *)(&rca));
    if (SD_OK != Status)
    {
    	printf("SDIO_SET_REL_ADDR sdio_check_err_R6 err\n");
        return(Status);
    }

	 /* Send CMD7 SDIO_SEL_DESEL_CARD */
    SDIO_CmdInitStructure.SDIO_Argument =  (uint32_t)(rca << 16);
    SDIO_CmdInitStructure.SDIO_CmdIndex = SDIO_SEL_DESEL_CARD;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);

    Status = sdio_check_err_R1(SDIO_SEL_DESEL_CARD);
	if (Status != SD_OK)
	{
		printf("SD_Select_Deselect_Card sdio_check_err_R1 err\n");
		return (Status);
	}

    /* Configure the SDIO peripheral */
    /* HCLK = SDIOCLK = 72 MHz, SDIO_CLK = HCLK/(2 + 1) = 24 MHz */  
    SDIO_InitStructure.SDIO_ClockDiv = 0x01;
    SDIO_InitStructure.SDIO_ClockEdge = SDIO_ClockEdge_Rising;
    SDIO_InitStructure.SDIO_ClockBypass = SDIO_ClockBypass_Disable;
    SDIO_InitStructure.SDIO_ClockPowerSave = SDIO_ClockPowerSave_Disable;
    SDIO_InitStructure.SDIO_BusWide = SDIO_BusWide_1b;
    SDIO_InitStructure.SDIO_HardwareFlowControl = SDIO_HardwareFlowControl_Disable;
    SDIO_Init(&SDIO_InitStructure);

    return(Status);
}

static int sdio_check_error(const char *msg_title)
{
	int err = 0;

	if (SDIO_GetFlagStatus(SDIO_FLAG_CCRCFAIL) == SET)
	{
		SDIO_ClearFlag(SDIO_FLAG_CCRCFAIL);
		err++;
	}
	if (SDIO_GetFlagStatus(SDIO_FLAG_CTIMEOUT) == SET)
	{
		SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);
		err++;
	}
	if (SDIO_GetFlagStatus(SDIO_FLAG_DCRCFAIL) == SET)
	{
		SDIO_ClearFlag(SDIO_FLAG_DCRCFAIL);
		err++;
	}
	if (SDIO_GetFlagStatus(SDIO_FLAG_DTIMEOUT) == SET)
	{
		SDIO_ClearFlag(SDIO_FLAG_DTIMEOUT);
		err++;
	}
	if (SDIO_GetFlagStatus(SDIO_FLAG_STBITERR) == SET)
	{
		SDIO_ClearFlag(SDIO_FLAG_STBITERR);
		err++;
	}
	if (SDIO_GetFlagStatus(SDIO_FLAG_TXUNDERR) == SET)
	{
		SDIO_ClearFlag(SDIO_FLAG_TXUNDERR);
		err++;
	}
	if (SDIO_GetFlagStatus(SDIO_FLAG_RXOVERR) == SET)
	{
		SDIO_ClearFlag(SDIO_FLAG_RXOVERR);
		err++;
	}

	if (DMA_GetFlagStatus(DMA2_FLAG_TE4) == SET)
	{
		DMA_ClearFlag(DMA2_FLAG_TE4);
		err++;
	}

	return err;
}

SD_Err sdio_read_byte(unsigned char func, unsigned long addr, unsigned char *data)
{
	uint32_t argument;
	SD_Err Status = SD_OK;

	SDIO_SET_CMD52_ARG(argument, CMD52_READ, func, 0, addr, 0x0);

	/* Send CMD52 to read the SDIO card */
	SDIO_CmdInitStructure.SDIO_Argument = argument;
	SDIO_CmdInitStructure.SDIO_CmdIndex = SDIO_SDIO_RW_DIRECT;
	SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
	SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
	SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
	SDIO_SendCommand(&SDIO_CmdInitStructure);

	Status = sdio_check_err_with_crc();
	if (Status != SD_OK)
	{
		printf("SDIO_SDIO_RW_DIRECT sdio_check_err_with_crc err\n");
		return(Status);
	}

	*data = SDIO_GetResponse(SDIO_RESP1);
	return Status;
}

SD_Err sdio_write_byte(unsigned char func, unsigned long addr, unsigned char data)
{
	uint32_t argument;
	SD_Err Status = SD_OK;

	SDIO_SET_CMD52_ARG(argument, CMD52_WRITE, func, CMD52_NORMAL_WRITE, addr, data);

	/* Send CMD52 to write the SDIO card */
	SDIO_CmdInitStructure.SDIO_Argument = argument;
	SDIO_CmdInitStructure.SDIO_CmdIndex = SDIO_SDIO_RW_DIRECT;
	SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
	SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
	SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
	SDIO_SendCommand(&SDIO_CmdInitStructure);

	Status = sdio_check_err_with_crc();
	if (Status != SD_OK)
	{
		printf("SDIO_SDIO_RW_DIRECT sdio_check_err_with_crc err\n");
		return(Status);
	}

	return Status;
}

static int sdio_trans_busy()
{
	if (SDIO_GetFlagStatus(SDIO_FLAG_TXACT)==SET || SDIO_GetFlagStatus(SDIO_FLAG_RXACT)==SET)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

static int blocksize_trans(int block)
{
	switch (block)
	{
		case 1:
			return SDIO_DataBlockSize_1b;
		case 2:
			return SDIO_DataBlockSize_2b;
		case 4:
			return SDIO_DataBlockSize_4b;
		case 8:
			return SDIO_DataBlockSize_8b;
		case 16:
			return SDIO_DataBlockSize_16b;
		case 32:
			return SDIO_DataBlockSize_32b;
		case 64:
			return SDIO_DataBlockSize_64b;
		case 128:
			return SDIO_DataBlockSize_128b;
		case 256:
			return SDIO_DataBlockSize_256b;
		case 512:
			return SDIO_DataBlockSize_512b;
		case 1024:
			return SDIO_DataBlockSize_1024b;
		case 2048:
			return SDIO_DataBlockSize_2048b;
		case 4096:
			return SDIO_DataBlockSize_4096b;
		case 8192:
			return SDIO_DataBlockSize_8192b;
		case 16384:
			return SDIO_DataBlockSize_16384b;
		default:
			return 0xFF;
	}
}

SD_Err sdio_read_multi_data(unsigned char func, unsigned long src, unsigned long count, unsigned char *data)
{
    unsigned long blocks = 0;
    unsigned long i = 0;
	SD_Err err = SD_OK;

	if (func > 7)
	{
		printf("error cmd53 for read: %d\n", func);
		return SD_ERROR;
	}

	if ((uint32_t)data & 3)
	{
		printf("data must 4 byte aligned\n");
		return SD_ERROR;
	}

	if (count == 0)
	{
		printf("count == 0\n");
		return SD_ERROR;
	}

	if (sdio_trans_busy())
	{
		printf("sdio_trans_busy\n");
		return SD_ERROR;
	}

	if (count > 256)
	{
		blocks = count/s_block_length;
		if (count%s_block_length)
		{
			blocks++;
		}
		count = blocks*s_block_length;
	}
	else
	{
		count = (count+3) & ~3;
	}

    SDIO_DMACmd(ENABLE);
	sdio_dma_rx_configuration((uint32_t *)data, count);

	if (blocks)
	{
		SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
		SDIO_CmdInitStructure.SDIO_Argument = (func << 28) | (src << 9) | (blocks & 0x1ff) | (CMD53_BLOCKMODE|CMD53_INCREMENTING);
	}
	else
	{
		SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Stream;
		SDIO_CmdInitStructure.SDIO_Argument = (func << 28) | (src << 9) | (count & 0x1ff) | CMD53_INCREMENTING;
	}

	SDIO_CmdInitStructure.SDIO_CmdIndex = SDIO_SDIO_RW_EXTENDED;
	SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
	SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
	SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
	SDIO_SendCommand(&SDIO_CmdInitStructure);

	SDIO_ITConfig(SDIO_IT_SDIOIT, DISABLE);

	SDIO_DataInitStructure.SDIO_DataTimeOut = SD_DATATIMEOUT;
	SDIO_DataInitStructure.SDIO_DataLength = count;
	SDIO_DataInitStructure.SDIO_DataBlockSize = blocksize_trans(s_block_length);
	SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToSDIO;
	SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
	SDIO_DataConfig(&SDIO_DataInitStructure);

	i = 0;
	while (SDIO_GetFlagStatus(SDIO_FLAG_CMDACT) == SET || SDIO_GetFlagStatus(SDIO_FLAG_DATAEND) == RESET)
	{
		if (sdio_check_error(__func__))
		{
			err = SD_ERROR;
			printf("sdio_check_error err\n");
			break;
		}

		i++;
		if (i == CMD53_TIMEOUT)
		{
			err = SD_DATA_TIMEOUT;
			printf("CMD53 timeout err\n");
			break;
		}
	}

	SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Disable;
	SDIO_DataConfig(&SDIO_DataInitStructure);

	SDIO_ITConfig(SDIO_IT_SDIOIT, ENABLE);
	
	DMA_ClearFlag(DMA2_FLAG_TC4 | DMA2_FLAG_TE4 | DMA2_FLAG_HT4 | DMA2_FLAG_GL4);
	DMA_Cmd(DMA2_Channel4, DISABLE);

	SDIO_ClearFlag(SDIO_STATIC_FLAGS);

	return err;
}

SD_Err sdio_write_multi_data(unsigned char func, unsigned long src, unsigned long count, unsigned char *data)
{
    unsigned long blocks = 0;
    unsigned long i = 0;
	SD_Err err = SD_OK;

	if (func > 7)
	{
		printf("error cmd53 for write: %d\n", func);
		return SD_ERROR;
	}

	if ((uint32_t)data & 3)
	{
		printf("data must 4 byte aligned\n");
		return SD_ERROR;
	}

	if (count == 0)
	{
		printf("count == 0\n");
		return SD_ERROR;
	}

	if (sdio_trans_busy())
	{
		printf("sdio_trans_busy\n");
		return SD_ERROR;
	}

	if (count > 256)
	{
		blocks = count/s_block_length;
		if (count%s_block_length)
		{
			blocks++;
		}
		count = blocks*s_block_length;
	}
	else
	{
		count = (count+3) & ~3;
	}

	if (blocks)
	{
		SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
		SDIO_CmdInitStructure.SDIO_Argument = (func << 28) | (src << 9) | (blocks & 0x1ff) | (CMD53_WRITE|CMD53_BLOCKMODE|CMD53_INCREMENTING);
	}
	else
	{
		SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Stream;
		SDIO_CmdInitStructure.SDIO_Argument = (func << 28) | (src << 9) | (count & 0x1ff) | (CMD53_WRITE|CMD53_INCREMENTING);
	}

	SDIO_CmdInitStructure.SDIO_CmdIndex = SDIO_SDIO_RW_EXTENDED;
	SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
	SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
	SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
	SDIO_SendCommand(&SDIO_CmdInitStructure);

	i = 0;
	do
	{
	  if (i == 3)
	  {
		printf("send CMD53 err\n");
		return SD_ERROR;
	  }
	  
	  if (i != 0)
	  {
		SDIO_SendCommand(&SDIO_CmdInitStructure);
	  }

	  i++;
	  
	  while (SDIO_GetFlagStatus(SDIO_FLAG_CMDACT) == SET);
	  sdio_check_error(__func__);
	} while (SDIO_GetFlagStatus(SDIO_FLAG_CMDREND) == RESET);

	SDIO_ClearFlag(SDIO_FLAG_CMDREND);

	SDIO_DMACmd(ENABLE);
	sdio_dma_tx_configuration((uint32_t *)data, count);

	SDIO_ITConfig(SDIO_IT_SDIOIT, DISABLE);

	SDIO_DataInitStructure.SDIO_DataTimeOut = SD_DATATIMEOUT;
	SDIO_DataInitStructure.SDIO_DataLength = count;
	SDIO_DataInitStructure.SDIO_DataBlockSize = blocksize_trans(s_block_length);
	SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToCard;
	SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
	SDIO_DataConfig(&SDIO_DataInitStructure);

	i = 0;
	while (SDIO_GetFlagStatus(SDIO_FLAG_DATAEND) == RESET)
	{
		if (sdio_check_error(__func__))
		{
			err = SD_ERROR;
			printf("sdio_check_error err\n");
			break;
		}

		i++;
		if (i == CMD53_TIMEOUT)
		{
			err = SD_DATA_TIMEOUT;
			printf("CMD53 timeout err\n");
			break;
		}
	}

	SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Disable;
	SDIO_DataConfig(&SDIO_DataInitStructure);

	SDIO_ITConfig(SDIO_IT_SDIOIT, ENABLE);
	
	DMA_ClearFlag(DMA2_FLAG_TC4 | DMA2_FLAG_TE4 | DMA2_FLAG_HT4 | DMA2_FLAG_GL4);
	DMA_Cmd(DMA2_Channel4, DISABLE);

	SDIO_ClearFlag(SDIO_STATIC_FLAGS);

	return err;
}

SD_Err sdio_set_block_len(unsigned char func, unsigned long block_len)
{
	if (func > 7 || block_len > SDIO_MAX_BLOCK_LEN)
	{
		return SD_ERROR;
	}

	if (blocksize_trans(block_len) == 0xFF)
	{
		printf("not support blocksize:%d\n", block_len);
		return SD_ERROR;
	}

	if (sdio_write_byte(0, (func*0x100 + CCCR_FN0_BLOCK_SIZE + 1), (block_len/256)) != SD_OK)
	{
		return SD_ERROR;
	}

	if (sdio_write_byte(0, (func*0x100 + CCCR_FN0_BLOCK_SIZE), (block_len%256)) != SD_OK)
	{
		return SD_ERROR;
	}

	s_block_length = block_len;
	return SD_OK;
}

SD_Err sdio_enable_func(unsigned char func)
{
	unsigned char temp = 0;

	if (sdio_read_byte(0, CCCR_IO_ENABLE, &temp) != SD_OK)
	{
		printf("enable_func: sdio_read_byte enable err\n");
		return SD_ERROR;
	}

	temp |= 1<<func;

	if (sdio_write_byte(0, CCCR_IO_ENABLE, temp) != SD_OK)
	{
		printf("enable_func: sdio_write_byte enable err\n");
		return SD_ERROR;
	}
	
	while (1)
	{
		if (sdio_read_byte(0, CCCR_IO_READY, &temp) != SD_OK)
		{
			printf("enable_func: sdio_read_byte ready err\n");
			return SD_ERROR;
		}
		if (temp & (1<<func))
		{
			break;
		}
	}

	sdio_write_byte(0, CCCR_INT_ENABLE, SDIO_CCCR_INTEN_IEN1|SDIO_CCCR_INTEN_IENM);

	return SD_OK;
}

SD_Err sdio_set_buswide(uint32_t WideMode)
{
	uint8_t temp;

	if (sdio_read_byte(0, CCCR_BUS_INTERFACE_CONTOROL, &temp) != SD_OK)
	{
		return SD_ERROR;
	}

	if (SDIO_BusWide_4b == WideMode)
    {
        temp &= ~(0x3<<0);
		temp |= (0x2<<0);
    }
    else if (SDIO_BusWide_1b == WideMode)
    {
        temp &= ~(0x3<<0);
		temp |= (0x0<<0);
    }
	else
	{
		return SD_ERROR;
	}

	if (sdio_write_byte(0, CCCR_BUS_INTERFACE_CONTOROL, temp) != SD_OK)
	{
		return SD_ERROR;
	}

	if (sdio_read_byte(0, CCCR_BUS_INTERFACE_CONTOROL, &temp) != SD_OK)
	{
		return SD_ERROR;
	}

	if (SDIO_BusWide_4b == WideMode)
    {
        if ((temp & 0x3) == 0x2)
        {
			return SD_OK;
		}
    }
    else if (SDIO_BusWide_1b == WideMode)
    {
        if ((temp & 0x3) == 0x0)
        {
			return SD_OK;
		}
    }

    return SD_ERROR;
}

static SD_Err sdio_stop_transfer(void)
{
    SD_Err Status = SD_OK;

    /* Send CMD12 STOP_TRANSMISSION  */
    SDIO_CmdInitStructure.SDIO_Argument = 0x0;
    SDIO_CmdInitStructure.SDIO_CmdIndex = SDIO_STOP_TRANSMISSION;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);

    Status = sdio_check_err_R1(SDIO_STOP_TRANSMISSION);

    return(Status);
}

