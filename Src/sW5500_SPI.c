/* Includes ------------------------------------------------------------------*/
#include "sW5500_SPI.h"

/* Variable ------------------------------------------------------------------*/
int8_t result = FALSE;

/* Network ------------------------------------------------------------------*/
wiz_NetInfo gWIZNETINFO = { .mac = {0x00, 0x08, 0xdc, 0xab, 0xcd, 0xef},
                            .ip = {192, 168, 0, 59},
                            .sn = {255, 255, 255, 0},
                            .gw = {192, 168, 0, 1},
                            .dns = {0, 0, 0, 0},
                            .dhcp = NETINFO_DHCP };
u8 gDATABUF[DATA_BUF_SIZE];
u8 gDHCP_RETRY = 0;
u8 ET_INIT_FLAG = FALSE;

/* Function ------------------------------------------------------------------*/
void Init_W5500_SPI(void)
{
	SPI_InitTypeDef SPI_InitStructure;
	GPIO_InitTypeDef    GPIO_InitStructure;

    /* Configure SPI1 pins: RESET, SCK, MISO and MOSI ---------------------------------*/
    GPIO_InitStructure.GPIO_Pin = W5500_SPI_SCK_PIN | W5500_SPI_MOSI_PIN | W5500_SPI_MISO_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = W5500_SPI_CS_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* SOFTWARE W5500 RESET
    GPIO_InitStructure.GPIO_Pin = W5500_RESET;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_SetBits(GPIOB, W5500_RESET);
    DelayMs(10);
    GPIO_ResetBits(GPIOB, W5500_RESET);
    DelayMs(10);
    GPIO_SetBits(GPIOB, W5500_RESET);
    DelayMs(10);
    */

	/* SPI_SLAVE configuration -------------------------------------------------*/
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &SPI_InitStructure);

    /* Enable SPI_SLAVE */
    SPI_Cmd(SPI1, ENABLE);
}

u8 wizchip_rw(u8 byte)
{
    // !< Loop while DR register in not emplty
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);

    //!< Send byte through the SPI1 peripheral
    SPI_I2S_SendData(SPI1, byte);

    //!< Wait to receive a byte
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);

    //!< Return the byte read from the SPI bus
    return SPI_I2S_ReceiveData(SPI1);
}

void wizchip_select(void)
{
    GPIO_ResetBits(GPIOA, W5500_SPI_CS_PIN);
}

void wizchip_deselect(void)
{
    GPIO_SetBits(GPIOA, W5500_SPI_CS_PIN);
}

void wizchip_write(u8 wb)
{
    wizchip_rw(wb);
}

u8 wizchip_read(void)
{
    return wizchip_rw(0xFF);
}

void W5500_Init(void)
{   
    /* wizchip initialize*/
    uint8_t memsize[2][8] = { {2,2,2,2,2,2,2,2},{2,2,2,2,2,2,2,2}};

    /*!< Deselect the FLASH: Chip Select high */
    wizchip_deselect();

    // Wiznet
    reg_wizchip_spi_cbfunc(wizchip_read, wizchip_write);
    reg_wizchip_cs_cbfunc(wizchip_select, wizchip_deselect);

    // Initialize w5500
    if(ctlwizchip(CW_INIT_WIZCHIP,(void*)memsize) == -1) {
        printf("WIZCHIP Initialized fail.\r\n");
        return;
    }
}

void Net_Conf()
{
    ctlnetwork(CN_SET_NETINFO, (void*) &gWIZNETINFO);
}

void display_Net_Info()
{
    wiz_NetInfo gWizInfo;

    ctlnetwork(CN_GET_NETINFO, (void*) &gWizInfo);
    printf("MAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n", gWizInfo.mac[0], gWizInfo.mac[1], gWizInfo.mac[2], gWizInfo.mac[3], gWizInfo.mac[4], gWizInfo.mac[5]);
    printf("IP: %d.%d.%d.%d\r\n", gWizInfo.ip[0], gWizInfo.ip[1], gWizInfo.ip[2], gWizInfo.ip[3]);
    printf("GW: %d.%d.%d.%d\r\n", gWizInfo.gw[0], gWizInfo.gw[1], gWizInfo.gw[2], gWizInfo.gw[3]);
    printf("SN: %d.%d.%d.%d\r\n", gWizInfo.sn[0], gWizInfo.sn[1], gWizInfo.sn[2], gWizInfo.sn[3]);
    printf("DNS: %d.%d.%d.%d\r\n", gWizInfo.dns[0], gWizInfo.dns[1], gWizInfo.dns[2], gWizInfo.dns[3]);
    printf("MODE: %s\r\n", gWizInfo.dhcp == NETINFO_DHCP ? "DHCP" : "STATIC");
}

void wizchip_ipassign(void)
{
    getIPfromDHCP(gWIZNETINFO.ip);
    getGWfromDHCP(gWIZNETINFO.gw);
    getSNfromDHCP(gWIZNETINFO.sn);
    getDNSfromDHCP(gWIZNETINFO.dns);
    gWIZNETINFO.dhcp = NETINFO_DHCP;

    Net_Conf();
    display_Net_Info();
}

void wizchip_ipconflict(void)
{
    printf("CONFLICT IP from DHCP\r\n");
    while(1);
}

void DHCP_REG_FUNC(void)
{
    reg_dhcp_cbfunc(wizchip_ipassign, wizchip_ipassign, wizchip_ipconflict);
}

void Check_PHY(void)
{
    u8 tmp = 0x00;
    // PHY Check
    do {
        if(ctlwizchip(CW_GET_PHYLINK, (void*)&tmp) == -1) {
          printf("Unknown PHY Link status.\r\n");
        }
    } 
    while (tmp == PHY_LINK_OFF);
}


void Check_DHCP(void)
{
    switch(DHCP_run())
    {
        case DHCP_IP_ASSIGN:
                case DHCP_IP_CHANGED:
                    /* If this block empty, act with default_ip_assign & default_ip_update */
                    //
                    // This example calls my_ip_assign in the two case.
                    //
                    // Add to ...
                    //
                    break;
                case DHCP_IP_LEASED:
                    //
                    // TODO: insert user's code here
                    SetLedState(LED2, TRUE);
                    ET_INIT_FLAG = TRUE;
                    //
                    break;
                case DHCP_FAILED:
                    /* ===== Example pseudo code =====  */
                    // The below code can be replaced your code or omitted.
                    // if omitted, retry to process DHCP
                    gDHCP_RETRY++;
                    if(gDHCP_RETRY > MAX_DHCP_RETRY)
                    {
                        gWIZNETINFO.dhcp = NETINFO_STATIC;
                        DHCP_stop();      // if restart, recall DHCP_init()

                        printf(">> DHCP %d Failed\r\n", gDHCP_RETRY);
                        Net_Conf();
                        display_Net_Info();   // print out static netinfo to serial

                        gDHCP_RETRY = 0;
                    }
                    break;
                default:
                    break;
    }
}