#include "644.H"
#define uchar unsigned char
#define Read_Flag	0x80
#define	RF_CHANNEL	23
#define	SOF	0X7F
bit Bit_TX;	//标识为TX端，1有效
extern int IntVecIdx; //occupied 0x10:rpage 0
void	Reg_Initial();
void	Clr_All_Ram();
void	Chk_Up_Down_Board();
void	Delay_1ms(unsigned char i);
void	EM198850_Initial();
void	RF_CHANNEL_SET(uchar RF_Channel);
void	RF_Enter_TX_NoAck_NoCRC_Payload5();
void	RF_Enter_RX_NoAck_NoCRC_Payload5();
void	Write_FIFO(uchar R_DATA_CNT);
void	Wait_PKT_High();
void	Read_FIFO(uchar R_DATA_CNT);
void main()
{
	Reg_Initial();
	Clr_All_Ram();
	Chk_Up_Down_Board();
	EM198850_Initial();
	RF_CHANNEL_SET(RF_CHANNEL);
	if(Bit_TX==1)
	{
		RF_Enter_TX_NoAck_NoCRC_Payload5();
		while(1)
		{
			Write_FIFO(5);
			Wait_PKT_High();
		}
		
	}
	else
	{
		RF_Enter_RX_NoAck_NoCRC_Payload5();
		while(1)
		{
			Wait_PKT_High();
			Read_FIFO(5);
		}
	}
}

void _intcall ALLint(void) @ int 
{
  switch(IntVecIdx)
  {
    case 0x4:
    break;
    
    case 0x7:
    break;
    
    case 0xA:
    break;

    case 0x13:
    break;
    
    case 0x16:
    break;
    
    case 0x19:
    break;
    
    case 0x1C:
    break;
    
    case 0x1F:
    break;    

    case 0x22:
    break;  

    case 0x25:
    break;   
    
    case 0x28:
    break;    
    
    case 0x2B:
    break;        
    
    case 0x2E:
    break;      
  } 	
  
}

void _intcall ext_interrupt_l(void) @ 0x03:low_int 0
{
 _asm{MOV A,0x2};
}

void _intcall port6pinchange_l(void) @ 0x06:low_int 1
{
 _asm{MOV A,0x2};
}


void _intcall tccint_l(void) @ 0x09:low_int 2
{
 _asm{MOV A,0x2};
}

void _intcall spi_l(void) @ 0x12:low_int 3
{
 _asm{MOV A,0x2};
}


void _intcall Comparat_l(void) @ 0x15:low_int 4
{
 _asm{MOV A,0x2};
}

void _intcall TC1_l(void) @ 0x18:low_int 5
{
 _asm{MOV A,0x2};
}

void _intcall UARTT_l(void) @ 0x1B:low_int 6
{
 _asm{MOV A,0x2};
}

void _intcall UARTR_l(void) @ 0x1E:low_int 7
{
 _asm{MOV A,0x2};
}

void _intcall UARTRE_l(void) @ 0x21:low_int 8
{
 _asm{MOV A,0x2};
}

void _intcall TC2_l(void) @ 0x24:low_int 9
{
 _asm{MOV A,0x2};
}

void _intcall TC3_l(void) @ 0x27:low_int 10
{
 _asm{MOV A,0x2};
}

void _intcall PWMA_l(void) @ 0x2A:low_int 11
{
 _asm{MOV A,0x2};
}

void _intcall PWMB_l(void) @ 0x2D:low_int 12
{
 _asm{MOV A,0x2};
}
void	Reg_Initial()
{
	PORT5=0X11;
	PORT6=0X00;
	PORT7=0X00;
	PORT8=0X00;
	WUCR=0X40;
	RMUST=0X60;
	ISR1=0X00;
	TC1CR=0X00;
	TC2CR=0X00;
	SPIS=0X00;
	SPIC=0XCA;
	SPIRB=0X00;
	SPIWB=0X00;
	ISR2=0x00;
	URC1=0X00;
	URC2=0X10;
	URS=0X00;
	P7PH=0X87;
	CMPCON=0X00;
	TC3CR=0X00;
	P7PD=0XFF;
	_asm{
			mov		A,@0X80
			CONTW
		}
	P5CR=0X14;
	P6CR=0X70;
	P7CR=0XF8;
	P8CR=0X00;
	WDTCR=0X00;
	P56PD=0XFF;
	P6OD=0X00;
	P6PH=0X9F;
	IMR2=0X00;
	IMR1=0X00;
}
void	Clr_All_Ram()
{
	R4=0X20;
	while(1)
	 {
	 	R0=0;
	 	R4+=1;
	 	if(R4==0)	break;
	 	if((R4&0X3F)==0)	R4|=0X20;
	 }
	 _asm{nop}
}
void	Chk_Up_Down_Board()
{
	Bit_TX=0;
	Delay_1ms(1);
	if(P73==0)	Bit_TX=1;
	
}
void	Delay_1ms(unsigned char i)
{
	unsigned char	R_TEMP;
	R_TEMP=i;
	while(R_TEMP!=0)
	 {
	 	R_TEMP--;
	 	_asm{
	 		  MOV	A,@6
	 		TD:
	 		  NOP
	 		  NOP
	 		  NOP
	 		  NOP
	 		  ADD	A,@1
	 		  JBS	0X03,2
	 		  JMP	TD
	 		}
	 }
}
void	Delay_1us(uchar i)
{
	uchar R_TEMP;
	while(R_TEMP!=0)
	{
		R_TEMP++;
		R_TEMP--;
		R_TEMP--;
	}
}
const uchar FrameTable[]={0X4E,0X02,0X4D,0X01,0X42,0X98,0X43,0XC4,0X44,0X06,
						 0X45,0X10,0X46,0X09,0X47,0X31,0X48,0X01,0X49,0X8A,
						 0X4A,0X27,0X4B,0X00,0X4C,0X06,0X50,0X00,0X51,0X11,
						 0X52,0X22,0X53,0X33,0X54,0X44,0X55,0X55,0X56,0X66,
						 0X57,0X77,0X58,0X08,0X00,0XE5,0X01,0X84,0X02,0X00,
						 0X03,0XC6,0X04,0X00,0X05,0X40,0X06,0X5D,0X07,0X18,
						 0X08,0X40,0X09,0X18,0X0A,0X47,0X0B,0X0B,0X0C,0XE0,
						 0X0D,0X4F,0X0E,0X11,0X0F,0X1C,0X20,0XAD,0X21,0X64,
						 0X22,0X00,0X23,0XC3,0X24,0XBD,0X25,0XA2,0X26,0X1A,
						 0X27,0X09,0X28,0X00,0X29,0XB8,0X2A,0X71,0X2B,0X06,
						 0X2C,0X80,0X2D,0X1A,0X2E,0X03,0X2F,0X64,0X30,0XC0,
						 0X31,0X00,0X32,0X40,0X33,0X3B,0X00,0XA7,0X32,0X4A,
						 0X00,0XE5,0X0E,0X91,0X40,0X51,0X41,0X81,0X0C,0XC0,
						 0X02,0X80,0X04,0X4A,0X05,0XDA,0X05,0XFA,0XFF};
const uchar FrameTable2[]={0X05,0X40,0X02,0X00,0X0C,0XE0,0XFF};
const uchar Test_Table[]={0X07,0X58,0X0E,0X11,0X2E,0X23,0X0E,0X91,0XFF};
const uchar Resume_Table[]={0x07,0x18,0x0E,0X11,0X2E,0X03,0X0E,0X91,0XFF};
const uchar CHK_FrameTable[]={0X4E,0X02,0X43,0XC4,0X44,0X06,0X45,0X10,
							 0X48,0X01,0X4C,0X06,0X50,0X00,0X51,0X11,
							 0X52,0X22,0X53,0X33,0X54,0X44,0X55,0X55,
							 0X56,0X66,0X57,0X77,0X58,0X08,0XFF};
void	RF_SPI_WRITE_READ()
{
	uchar R_TEMP;
	R_TEMP=SPIRB;
	SPIRB=0;
	SPIS=0;
	SPIC=0XCA;
	SSE=1;
	while(SSE==1);
	SPIWB=0;
}

void	EM198850_REG_WRITE(uchar R_Addr,uchar R_DATA)
{
	SPI_SS=0;
	SPIWB=R_Addr;
	RF_SPI_WRITE_READ();
	SPIWB=R_DATA;
	RF_SPI_WRITE_READ();
	SPIE=0;
	URC2=0;
	SPI_CLK=0;
	SPI_SS=1;
	URC2=0X10;
	SPIE=1;
}				  
uchar	EM198850_REG_READ(uchar R_Addr)
{
	SPI_SS=0;
	SPIWB=R_Addr|Read_Flag;
	RF_SPI_WRITE_READ();
	RF_SPI_WRITE_READ();
	SPI_SS=1;
	return(SPIRB);
}

void	EM198850_REG_INITIAL1()
{
	uchar	R_Index,R_Addr,R_DATA;
	R_Index=0;
	while(FrameTable[R_Index]!=0xFF)
	  {
	  	R_Addr=FrameTable[R_Index];
	  	R_Index++;
	  	R_DATA=FrameTable[R_Index];
	  	R_Index++;
	  	EM198850_REG_WRITE(R_Addr,R_DATA);
	  }

}

void	EM198850_REG_INITIAL2()
{
	uchar	R_Index,R_Addr,R_DATA;
	R_Index=0;
	while(FrameTable2[R_Index]!=0xFF)
	  {
	  	R_Addr=FrameTable2[R_Index];
	  	R_Index++;
	  	R_DATA=FrameTable2[R_Index];
	  	R_Index++;
	  	EM198850_REG_WRITE(R_Addr,R_DATA);
	  }
}

void	RSSI_INITIAL()
{
	uchar R_bigest,R_Index,R_DATA;
	R_bigest=0;
	R_Index=0;
	while(R_Index!=5)
	{
		R_Index++;
		R_DATA=EM198850_REG_READ(0x4B);
		if(R_bigest<R_DATA)
		{
			R_bigest=R_DATA;
		}
	}
	EM198850_REG_WRITE(0X4A,R_bigest);
}
uchar	CHK_PLLD()
{
	uchar R_Index,R_Addr,R_DATA,R_Ret_Value=0;
	R_Index=0;
	while(Test_Table[R_Index]!=0xFF)
	{
		R_Addr=Test_Table[R_Index];
		R_Index++;
		R_DATA=Test_Table[R_Index];
		R_Index++;
		EM198850_REG_WRITE(R_Addr,R_DATA);
	}
	Delay_1us(200);
	Delay_1us(200);
	R_DATA=EM198850_REG_READ(0X4B);
	R_DATA&=0X80;
	if(R_DATA!=0X00)
	{
		R_Ret_Value=1;
	}
	
	R_Index=0;
	while(Resume_Table[R_Index]!=0xFF)
	{
		R_Addr=Resume_Table[R_Index];
		R_Index++;
		R_DATA=Resume_Table[R_Index];
		R_Index++;
		EM198850_REG_WRITE(R_Addr,R_DATA);
	}
	return(R_Ret_Value);
}
uchar	EM198850_REG_Test()
{
	uchar R_Index,R_Addr,R_DATA,R_Ret_Value=1;
	R_Index=0;
	while(CHK_FrameTable[R_Index]!=0xFF)
	{
		R_Addr=CHK_FrameTable[R_Index];
		R_Index++;
		R_DATA=EM198850_REG_READ(R_Addr);
		if(R_DATA!=CHK_FrameTable[R_Index])
		{
			R_Ret_Value=0;
			break;
		}
		R_Index++;
	}
	return(R_Ret_Value);
}
void	EM198850_Initial()
{
	uchar R_Right;
RF_Initial_Again:
	RESET_N=0;
	Delay_1ms(2);
	RESET_N=1;
	Delay_1ms(1);
	SPI_SS=1;
	EM198850_REG_INITIAL1();
	Delay_1us(250);
	RSSI_INITIAL();
	EM198850_REG_INITIAL2();
//	R_Right=CHK_PLLD();
//	if(R_Right==0)
//	{
//		goto RF_Initial_Again;
//	}
	R_Right=EM198850_REG_Test();
	if(R_Right==0)
	{
		goto RF_Initial_Again;
	}
}
void	RF_CHANNEL_SET(uchar RF_Channel)
{
	EM198850_REG_WRITE(0x02,RF_Channel);
}
void	RF_Enter_TX_NoAck_NoCRC_Payload5()
{	
	EM198850_REG_WRITE(0x40,0X52);	//TX NO ACK
	EM198850_REG_WRITE(0x41,0X80);
	EM198850_REG_WRITE(0x43,0X84); //CRC=0
	EM198850_REG_WRITE(0X44,0X05); //PAYLOAD=5
	EM198850_REG_WRITE(0x45,0X10); //PACKER CNT=1
	EM198850_REG_WRITE(0x47,0X01); //RTX CNT==0
	
}
void	RF_Enter_RX_NoAck_NoCRC_Payload5()
{
	EM198850_REG_WRITE(0x40,0X51);	//RX NO ACK
	EM198850_REG_WRITE(0x41,0X81);
	EM198850_REG_WRITE(0x43,0X84); //CRC=0
	EM198850_REG_WRITE(0X44,0X05); //PAYLOAD=5
	EM198850_REG_WRITE(0x45,0X10); //PACKER CNT=1
	EM198850_REG_WRITE(0x47,0X01); //RTX CNT==0
}
void	Write_FIFO(uchar R_DATA_CNT)
{
	SPI_SS=0;
	SPIWB=SOF;
	RF_SPI_WRITE_READ();
	while(R_DATA_CNT!=0)
	{
		R_DATA_CNT--;
		SPIWB=0X55;
		RF_SPI_WRITE_READ();
	}
	SPI_SS=1;
}
void	Wait_PKT_High()
{
	while(PKT_FLAG==0);
}
void	Read_FIFO(uchar R_DATA_CNT)
{
	uchar FIFO_DATA[5];
	SPI_SS=0;
	SPIWB=SOF|0x80;
	RF_SPI_WRITE_READ();
	while(R_DATA_CNT!=0)
	{
		R_DATA_CNT--;
		RF_SPI_WRITE_READ();
		FIFO_DATA[5-R_DATA_CNT]=SPIRB;
	}
	SPI_SS=1;
}