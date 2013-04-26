//*******************************************
//BANK0寄存器的定义：
//*******************************************
unsigned char	R0		@0X00:rpage 0;
unsigned char	TCC		@0X01:rpage 0;
unsigned char	PC		@0X02:rpage 0;
unsigned char	STATUS	@0X03:rpage 0;
//{
bit	C			@0X03@0:rpage 0;
bit DC			@0X03@1:rpage 0;
bit	Z			@0X03@2:rpage 0;
bit	P			@0X03@3:rpage 0;
bit T			@0X03@4:rpage 0;
bit	PS0			@0X03@5:rpage 0;
bit	PS1			@0X03@6:rpage 0;
//}
unsigned char	R4	@0X04:rpage 0;
unsigned char	PORT5	@0X05:rpage 0;
//{
bit SPI_SS		@0X05@0:rpage 0;
bit P51			@0X05@1:rpage 0;
bit P52			@0X05@2:rpage 0;
bit SPI_CLK		@0X05@3:rpage 0;
bit P54			@0X05@4:rpage 0;
bit P55			@0X05@5:rpage 0;
bit LEDC		@0X05@6:rpage 0;
bit RESET_N		@0X05@7:rpage 0;
//}
unsigned char	PORT6	@0X06:rpage 0;
unsigned char	PORT7	@0X07:rpage 0;
//{
bit	P73			@0X07@0X03:rpage 0;
bit PKT_FLAG	@0X07@0X07:rpage 0;
//}
unsigned char	PORT8	@0X08:rpage 0;
unsigned char	WUCR	@0X0A:rpage 0;
//{
bit SPIWE		@0X0A@0X03:rpage 0;
bit EXWE		@0X0A@0X04:rpage 0;
bit ICWE		@0X0A@0X06:rpage 0;
bit CMP2WE		@0X0A@0X07:rpage 0;
//}
unsigned char	EECR	@0X0B:rpage 0;
//{
bit EEPC		@0X0B@0X03:rpage 0;
bit EEDF		@0X0B@0X04:rpage 0;
bit EEWE		@0X0B@0X05:rpage 0;
bit WR			@0X0B@0X06:rpage 0;
bit RD			@0X0B@0X07:rpage 0;
//}
unsigned char	EEADDR	@0X0C:rpage 0;
unsigned char	EEDATA	@0X0D:rpage 0;
unsigned char	RMUST	@0X0E:rpage 0;
unsigned char	ISR1	@0X0F:rpage 0;
//{
bit TCIF		@0X0F@0X00:rpage 0;
bit ICIF		@0X0F@0X01:rpage 0;
bit EXIF		@0X0F@0X02:rpage 0;
bit PWMAIF		@0X0F@0X03:rpage 0;
bit PWMBIF		@0X0F@0X04:rpage 0;
bit SPIIF		@0X0F@0X05:rpage 0;
//}
//*******************************************
//BANK1寄存器的定义：
//*******************************************
unsigned char	TC1CR	@0X05:rpage 1;
//{
bit	TC1ES		@0X05@0X02:rpage 1;
bit TC1M		@0X05@0X03:rpage 1;
bit TC1CK0		@0X05@0X04:rpage 1;
bit TC1CK1		@0X05@0X05:rpage 1;
bit TC1S		@0X05@0X06:rpage 1;
bit TC1CAP		@0X05@0X07:rpage 1;
//}
unsigned char	TCR1DA	@0X06:rpage 1;
unsigned char	TCR1DB	@0X07:rpage	1;
unsigned char	TC2CR	@0X08:rpage 1;
//{
bit TC2CK0		@0X08@0X00:rpage 1;
bit TC2CK1		@0X08@0X01:rpage 1;
bit TC2CK2		@0X08@0X02:rpage 1;
bit TC2S		@0X08@0X03:rpage 1;
bit TC2M		@0X08@0X04:rpage 1;
bit TC2ES		@0X08@0X05:rpage 1;
//}
unsigned char	TC2DH	@0X09:rpage 1;
unsigned char	TC2DL	@0X0A:rpage 1;
unsigned char	SPIS	@0X0B:rpage 1;
//{
bit RBF			@0X0B@0X00:rpage 1;
bit OD4			@0X0B@0X02:rpage 1;
bit	OD3			@0X0B@0X03:rpage 1;
bit TD0			@0X0B@0X05:rpage 1;
bit TD1			@0X0B@0X06:rpage 1;
bit DORD		@0X0B@0X07:rpage 1;
//}
unsigned char	SPIC	@0X0C:rpage 1;
//{
bit	SBRS0		@0X0C@0X00:rpage 1;
bit	SBRS1		@0X0C@0X01:rpage 1;
bit SBRS2		@0X0C@0X02:rpage 1;
bit SDOC		@0X0C@0X03:rpage 1;
bit	SSE			@0X0C@0X04:rpage 1;
bit SRO			@0X0C@0X05:rpage 1;
bit SPIE		@0X0C@0X06:rpage 1;
bit CES			@0X0C@0X07:rpage 1;
//}
unsigned char	SPIRB	@0X0D:rpage 1;
unsigned char	SPIWB	@0X0E:rpage 1;
unsigned char	ISR2	@0X0F:rpage 1;
//*******************************************
//BANK2寄存器的定义：
//*******************************************
unsigned char	URC1	@0X0A:rpage 2;
//{
bit TXE			@0X0A@0X00:rpage 2;
bit UTBE		@0X0A@0X01:rpage 2;
bit BRATE0		@0X0A@0X02:rpage 2;
bit BRATE1		@0X0A@0X03:rpage 2;
bit BRATE2		@0X0A@0X04:rpage 2;
bit UMODE0		@0X0A@0X05:rpage 2;
bit UMODE1		@0X0A@0X06:rpage 2;
bit URTD8		@0X0A@0X07:rpage 2;
//}
unsigned char	URC2	@0X0B:rpage 2;
//{
bit	UINVEN		@0X0B@0X03:rpage 2;
bit	SBIM0		@0X0B@0X04:rpage 2;
bit SBIM1		@0X0B@0X05:rpage 2;
//}
unsigned char	URS		@0X0C:rpage 2;
//{
bit	RXE			@0X0C@0X00:rpage 2;
bit	URBF		@0X0C@0X01:rpage 2;
bit	FMERR		@0X0C@0X02:rpage 2;
bit	OVERR		@0X0C@0X03:rpage 2;
bit	PRERR		@0X0C@0X04:rpage 2;
bit	PRE			@0X0C@0X05:rpage 2;
bit	EVEN		@0X0C@0X06:rpage 2;
bit	URRD8		@0X0C@0X07:rpage 2;
//}
unsigned char	URRD		@0X0D:rpage 2;
unsigned char	URTD		@0X0E:rpage 2;
unsigned char	P7PH		@0X0F:rpage 2;
//{
bit	PH72		@0X0F@0X02:rpage 2;
bit	PH73		@0X0F@0X03:rpage 2;
bit	PH74		@0X0F@0X04:rpage 2;
bit	PH75		@0X0F@0X05:rpage 2;
bit	PH76		@0X0F@0X06:rpage 2;
bit	PH77		@0X0F@0X07:rpage 2;
//}
//*******************************************
//BANK3寄存器的定义：
//*******************************************
unsigned char	TMRCON		@0X05:rpage 3;
unsigned char	CMPCON		@0X07:rpage 3;
unsigned char	PWMCON		@0X08:rpage 3;
unsigned char	PRDAH		@0X09:rpage 3;
unsigned char	DTAH		@0X0A:rpage 3;
unsigned char	PRDBH		@0X0B:rpage 3;
unsigned char	DTBH		@0X0C:rpage 3;
unsigned char	TC3CR		@0X0D:rpage 3;
//{
bit TC3M0		@0X0D@0X00:rpage 3;
bit	TC3M1		@0X0D@0X01:rpage 3;
bit	TC3CK0		@0X0D@0X02:rpage 3;
bit	TC3CK1		@0X0D@0X03:rpage 3;
bit TC3CK2		@0X0D@0X04:rpage 3;
bit	TCS3S		@0X0D@0X05:rpage 3;
bit TC3FF0		@0X0D@0X06:rpage 3;
bit	TC3FF1		@0X0D@0X07:rpage 3;
//}
unsigned char	TCR3D		@0X0E:rpage 3;
unsigned char	P7PD		@0X0F:rpage 3;
//{
bit	PD72		@0X0F@0X02:rpage 3;
bit PD73		@0X0F@0X03:rpage 3;
bit PD74		@0X0F@0X04:rpage 3;
bit PD75		@0X0F@0X05:rpage 3;
bit PD76		@0X0F@0X06:rpage 3;
bit PD77		@0X0F@0X07:rpage 3;
//}
//*******************************************
//IO寄存器的定义：
//*******************************************
io	unsigned char	P5CR		@0X05:iopage 0;
io	unsigned char	P6CR		@0X06:iopage 0;
io 	unsigned char	P7CR		@0X07:iopage 0;
io	unsigned char	P8CR		@0X08:iopage 0;
io	unsigned char	WDTCR		@0X0A:iopage 0;
io	unsigned char	P56PD		@0X0B:iopage 0;
io	unsigned char	P6OD		@0X0C:iopage 0;
io	unsigned char	P6PH		@0X0D:iopage 0;
io	unsigned char	IMR2		@0X0E:iopage 0;
io  unsigned char	IMR1		@0X0F:iopage 0;

