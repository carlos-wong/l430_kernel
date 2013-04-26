#include <asm/jzsoc.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/delay.h>
//#define TEST

#define  EM_PKT_W     (32 * 3 + 26)
#define  EM_FIFO_W    (32 * 3 + 27)
#define  EM_RESET     (32 * 4 + 19)

#define EM_SPI_CLK   (32*5 + 10) 
#define EM_SPI_DIN   (32*5 + 12)
#define EM_SPI_DOUT  (32*5 + 11)
#define EM_SPI_CS    (32*4 + 18) 

unsigned char spi_rw_char(unsigned char wr_data);


#if 0
static void spi_loop_test()
{
	int i;
	int data;
	
	printk("spi loop test start!\n");

#if 0
	for(i=0; i<256; i++)
	{
	   data = spi_rw_char(i);
	  
	   if(data != i)
	   printk("spi test error,i = %d\n",i);
	}
#endif
	while(1)
		spi_rw_char(0x28);
	
	printk("spi loop test end!\n");

}

unsigned char spi_rw_char(unsigned char wr_data)
{	
	unsigned char rd_data;

	__ssi_flush_rxfifo();
	__ssi_flush_txfifo();

	while(__ssi_txfifo_full()) ;

	__ssi_transmit_data(wr_data);
	
	while(__ssi_rxfifo_empty());
	
	rd_data = (unsigned char)(__ssi_receive_data());

	return rd_data;
}

#endif

unsigned char spi_rw_char(unsigned char wr_data)
{	
	unsigned char rd_data;

	__ssi_flush_rxfifo();
	__ssi_flush_txfifo();

	while(__ssi_txfifo_full()) ;

	__ssi_transmit_data(wr_data);
	
	while(__ssi_rxfifo_empty());
	
	rd_data = (unsigned char)(__ssi_receive_data());

	return rd_data;
}



void smsspibus_xfer(void *context, unsigned char *txbuf,
		unsigned long txbuf_phy_addr, unsigned char *rxbuf,
		unsigned long rxbuf_phy_addr, int len)
{
	int i;
	//if (txbuf == NULL && rxbuf == NULL) return ;
	if ( rxbuf == NULL)
	{
		PERROR("rxbuf==NULL\n");
		return;
	}

	if (txbuf == NULL)
	{
		for(i=0; i<len; i++)
		{
			rxbuf[i] = spi_rw_char(0xff);
		}	
	}
	else
	{
		for(i=0; i<len; i++)
		{
			rxbuf[i] = spi_rw_char(txbuf[i]);
		}	
	}

	return;
}

void 2d4g_init()
{

}

void *2d4g_init(void *context, void (*smsspi_interruptHandler) (void *),
		void *intr_context)
{
	int ret;

	printk("smsspiphy_init..!\n");	

	//power off module
	__gpio_as_output(SIANO_CMMB_PWREN);
	__gpio_clear_pin(SIANO_CMMB_PWREN);
	mdelay(100);
	//__gpio_set_pin(SIANO_CMMB_PWREN);
	
	
	//init gpio
	
	__gpio_as_ssi();

	//config spi
	//__cpm_ssiclk_select_pllout(); // pllout: 360 MHz 
	__cpm_ssiclk_select_exclk(); // pllout: 360 MHz 
	__cpm_set_ssidiv(1);	// ssi source clk = 360 /(n+1) = 180 MHz 
	REG_SSI_GR = 11;	// clock = (180)/(2*(n+1)) MHz   // 7.5  MHz
	__cpm_start_ssi();

	__ssi_disable();
	__ssi_disable_tx_intr();
	__ssi_disable_rx_intr();
	__ssi_enable_receive();
	__ssi_flush_fifo();
	__ssi_clear_errors();

	__ssi_set_spi_clock_phase(0);     //PHA = 0
	__ssi_set_spi_clock_polarity(0);  //POL = 0
	__ssi_set_msb();
	__ssi_set_frame_length(8);
	__ssi_disable_loopback(); // just for test 
	//__ssi_enable_loopback(); // just for test 
	__ssi_select_ce();

	__ssi_set_tx_trigger(112); //n(128 - DMA_BURST_SIZE), total 128 entries 
	__ssi_set_rx_trigger(16);  //n(DMA_BURST_SIZE), total 128 entries 
	__ssi_set_msb();
	__ssi_spi_format();

	__ssi_enable();

	__gpio_as_irq_fall_edge(SIANO_CMMB_IRQ);		
	__gpio_unmask_irq(SIANO_CMMB_IRQ);

	//init irq
	if( ret = request_irq(IRQ_GPIO_0+SIANO_CMMB_IRQ, smsspi_interruptHandler, IRQF_DISABLED, "siano_cmmb_irq", NULL) )
	{
		printk("request siano cmmb irq error!\n");
		return -1;
	}
#ifdef TEST
//	SPIBus_Test_1();
//	test_clk();
//	spi_loop_test();
#endif

	return dummy_device;
}

void smsspiphy_deinit(void *context)
{
	//power off module
	
	free_irq(IRQ_GPIO_0+SIANO_CMMB_IRQ, NULL);
	__ssi_disable();
	__cpm_stop_ssi();
	__gpio_as_output(SIANO_CMMB_PWREN);
	__gpio_set_pin(SIANO_CMMB_PWREN);
	printk("smsspiphy_deinit!\n");
}


