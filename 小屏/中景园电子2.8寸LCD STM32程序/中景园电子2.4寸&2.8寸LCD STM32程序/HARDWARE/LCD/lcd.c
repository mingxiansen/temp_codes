//#include "bmp.h"
#include "lcd.h"
#include "lcdfont.h"  	 
#include "delay.h"



u8 ref=0;//刷新显示
u16 vx=15542,vy=11165;  //比例因子，此值除以1000之后表示多少个AD值代表一个像素点
u16 chx=140,chy=146;//默认像素点坐标为0时的AD起始值

//用于SPI初始化
void SPI_init(void)
{
  SPI_InitTypeDef  SPI_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  //开启相应IO端口的时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_GPIOB,ENABLE);
 //使能SPI1时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
  //配置 SPI_NRF_SPI的 SCK,MISO,MOSI引脚，GPIOA^5,GPIOA^6,GPIOA^7 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用功能
	GPIO_Init(GPIOA, &GPIO_InitStructure);  
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);  
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
		   
  SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx; //双线全双工
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;	 					//主模式
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;	 				//数据大小8位
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;		 				//时钟极性，空闲时为低
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;						//第1个边沿有效，上升沿为采样时刻
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		   					//NSS信号由软件产生
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;  //8分频，9MHz
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;  				//高位在前
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI1, &SPI_InitStructure);
  // Enable SPI1  
  SPI_Cmd(SPI1, ENABLE);
}

//void DMA_Configuration(void)
//{
//	DMA_InitTypeDef DMA_InitStructure;

//	RCC->AHB1ENR|=RCC_AHB1Periph_DMA2;

//	DMA_InitStructure.DMA_Channel = DMA_Channel_3;  //通道选择SPI1TX
//	DMA_InitStructure.DMA_PeripheralBaseAddr =(uint32_t)&(SPI1->DR);//DMA外设地址
//	//DMA_InitStructure.DMA_Memory0BaseAddr = mar;//DMA 存储器0地址
//	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;//存储器到外设模式
//	//DMA_InitStructure.DMA_BufferSize = 7;//数据传输量 
//	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设非增量模式
//	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable ;//DMA_MemoryInc_Enable;//存储器增量模式
//	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//外设数据长度:16位
//	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//存储器数据长度:16位
//	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;// 使用普通模式 
//	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;//中等优先级
//	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
//	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
//	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//存储器突发单次传输
//	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//外设突发单次传输

//	DMA_Init(DMA2_Stream5,&DMA_InitStructure);//初始化DMA Stream
//	//DMA_ClearITPendingBit(DMA2_Stream5,DMA_IT_TCIF7);
//	//DMA_ITConfig(DMA2_Stream7,DMA_IT_TC,ENABLE); 
//}

//void SPI_DMA_SendByte(uint16_t Byte ,uint32_t Length)
//{
//	DMA_Cmd(DMA2_Stream5,DISABLE);
//	while(DMA_GetCmdStatus(DMA2_Stream5)==ENABLE);
//	DMA2_Stream5->M0AR=(uint32_t)&Byte;
//	DMA2_Stream5->NDTR=Length;
//	SPI_DMACmd(SPI1,SPI_I2S_DMAReq_Tx,ENABLE);
//	DMA_Cmd(DMA2_Stream5,ENABLE);
//	while(DMA_GetFlagStatus(DMA2_Stream5,DMA_FLAG_TCIF5)==RESET);
//	DMA_ClearFlag(DMA2_Stream5,DMA_FLAG_TCIF5);
//} 



void SPI_write_cmd(u8 data)
{
		GPIO_ResetBits(GPIOB,GPIO_Pin_0);
		DelayUs(5);
		while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(SPI1,data);
}

void SPI_write_data(u8 data)
{
		GPIO_SetBits(GPIOB,GPIO_Pin_0);
		DelayUs(5);
		while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(SPI1,data);
}
 void SPI_write_data_u16(u16 data)
{	
		GPIO_SetBits(GPIOB,GPIO_Pin_0);
		DelayUs(5);
		while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(SPI1,data>>8);
		while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(SPI1,data);
}	  
 void SPI_write_cmd_data(u8 cmd,u16 data)
{	
    SPI_write_cmd(cmd);
		SPI_write_data_u16(data);
}
void xianshi()//显示信息
{   
	BACK_COLOR=WHITE;
	POINT_COLOR=RED;	
	showhanzi(10,0,0);  //晶
	showhanzi(45,0,1);  //耀
  LCD_ShowString(10,35,"2.4 TFT SPI 240*320");
	LCD_ShowString(10,55,"LCD_W:");	LCD_ShowNum(70,55,LCD_W,3);
	LCD_ShowString(110,55,"LCD_H:");LCD_ShowNum(160,55,LCD_H,3);	
}

void showimage() //显示40*40图片
{
  	int i,j,k; 

	for(k=3;k<8;k++)
	{
	   	for(j=0;j<6;j++)
		{	
			Address_set(40*j,40*k,40*j+39,40*k+39);		//坐标设置
		    for(i=0;i<1600;i++)
			 { 	
				 		
			  	 SPI_write_data(image[i*2+1]);	 
				   SPI_write_data(image[i*2]);				
			 }	
		 }
	}
	ref=0;				
}

u16 BACK_COLOR, POINT_COLOR;   //背景色，画笔色

void Address_set(unsigned int x1,unsigned int y1,unsigned int x2,unsigned int y2)
{ 
	SPI_write_cmd(0x2a);
   SPI_write_data(x1>>8);
   SPI_write_data(x1);
   SPI_write_data(x2>>8);
   SPI_write_data(x2);
  
   SPI_write_cmd(0x2b);
   SPI_write_data(y1>>8);
   SPI_write_data(y1);
   SPI_write_data(y2>>8);
   SPI_write_data(y2);

   SPI_write_cmd(0x2C);					 						 
}


void Lcd_Init(void)
{
	  //打开片选使能
	 OLED_RST_Clr();
	delay_ms(20);
	OLED_RST_Set();
	delay_ms(20);
	
//************* Start Initial Sequence **********// 
SPI_write_cmd(0xCF);  
SPI_write_data(0x00); 
SPI_write_data(0xD9); 
SPI_write_data(0X30); 
 
SPI_write_cmd(0xED);  
SPI_write_data(0x64); 
SPI_write_data(0x03); 
SPI_write_data(0X12); 
SPI_write_data(0X81); 
 
SPI_write_cmd(0xE8);  
SPI_write_data(0x85); 
SPI_write_data(0x10); 
SPI_write_data(0x78); 
 
SPI_write_cmd(0xCB);  
SPI_write_data(0x39); 
SPI_write_data(0x2C); 
SPI_write_data(0x00); 
SPI_write_data(0x34); 
SPI_write_data(0x02); 
 
SPI_write_cmd(0xF7);  
SPI_write_data(0x20); 
 
SPI_write_cmd(0xEA);  
SPI_write_data(0x00); 
SPI_write_data(0x00); 
 
SPI_write_cmd(0xC0);    //Power control 
SPI_write_data(0x21);   //VRH[5:0] 
 
SPI_write_cmd(0xC1);    //Power control 
SPI_write_data(0x12);   //SAP[2:0];BT[3:0] 
 
SPI_write_cmd(0xC5);    //VCM control 
SPI_write_data(0x32); 
SPI_write_data(0x3C); 
 
SPI_write_cmd(0xC7);    //VCM control2 
SPI_write_data(0XC1); 
 
SPI_write_cmd(0x36);    // Memory Access Control 
SPI_write_data(0x08); 
 
SPI_write_cmd(0x3A);   
SPI_write_data(0x55); 

SPI_write_cmd(0xB1);   
SPI_write_data(0x00);   
SPI_write_data(0x18); 
 
SPI_write_cmd(0xB6);    // Display Function Control 
SPI_write_data(0x0A); 
SPI_write_data(0xA2); 

 
 
SPI_write_cmd(0xF2);    // 3Gamma Function Disable 
SPI_write_data(0x00); 
 
SPI_write_cmd(0x26);    //Gamma curve selected 
SPI_write_data(0x01); 
 
SPI_write_cmd(0xE0);    //Set Gamma 
SPI_write_data(0x0F); 
SPI_write_data(0x20); 
SPI_write_data(0x1E); 
SPI_write_data(0x09); 
SPI_write_data(0x12); 
SPI_write_data(0x0B); 
SPI_write_data(0x50); 
SPI_write_data(0XBA); 
SPI_write_data(0x44); 
SPI_write_data(0x09); 
SPI_write_data(0x14); 
SPI_write_data(0x05); 
SPI_write_data(0x23); 
SPI_write_data(0x21); 
SPI_write_data(0x00); 
 
SPI_write_cmd(0XE1);    //Set Gamma 
SPI_write_data(0x00); 
SPI_write_data(0x19); 
SPI_write_data(0x19); 
SPI_write_data(0x00); 
SPI_write_data(0x12); 
SPI_write_data(0x07); 
SPI_write_data(0x2D); 
SPI_write_data(0x28); 
SPI_write_data(0x3F); 
SPI_write_data(0x02); 
SPI_write_data(0x0A); 
SPI_write_data(0x08); 
SPI_write_data(0x25); 
SPI_write_data(0x2D); 
SPI_write_data(0x0F); 
 
SPI_write_cmd(0x11);    //Exit Sleep 
delay_ms(120); 
SPI_write_cmd(0x29);    //Display on 
 
} 

//清屏函数
//Color:要清屏的填充色
void LCD_Clear(u16 Color)
{
	u16 i,j;  	
	Address_set(0,0,LCD_W-1,LCD_H-1);
    for(i=0;i<LCD_W;i++)
	 {
	  for (j=0;j<LCD_H;j++)
	   	{
        	SPI_write_data(Color);	 			 
	    }

	  }
}



//在指定位置显示一个汉字(32*33大小)
//dcolor为内容颜色，gbcolor为背静颜色
void showhanzi(unsigned int x,unsigned int y,unsigned char index)	
{  
	unsigned char i,j;
	unsigned char *temp=hanzi;    
    Address_set(x,y,x+31,y+31); //设置区域      
	temp+=index*128;	
	for(j=0;j<128;j++)
	{
		for(i=0;i<8;i++)
		{ 		     
		 	if((*temp&(1<<i))!=0)
			{
				SPI_write_data(POINT_COLOR);
			} 
			else
			{
				SPI_write_data(BACK_COLOR);
			}   
		}
		temp++;
	 }
}
//画点
//POINT_COLOR:此点的颜色
void LCD_DrawPoint(u16 x,u16 y)
{
	Address_set(x,y,x,y);//设置光标位置 
	SPI_write_data(POINT_COLOR); 	    
} 	 
//画一个大点
//POINT_COLOR:此点的颜色
void LCD_DrawPoint_big(u16 x,u16 y)
{
	LCD_Fill(x-1,y-1,x+1,y+1,POINT_COLOR);
} 
//在指定区域内填充指定颜色
//区域大小:
//  (xend-xsta)*(yend-ysta)
void LCD_Fill(u16 xsta,u16 ysta,u16 xend,u16 yend,u16 color)
{          
	u16 i,j; 
	Address_set(xsta,ysta,xend,yend);      //设置光标位置 
	for(i=ysta;i<=yend;i++)
	{													   	 	
		for(j=xsta;j<=xend;j++)SPI_write_data(color);//设置光标位置 	    
	} 					  	    
}  
//画线
//x1,y1:起点坐标
//x2,y2:终点坐标  
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 

	delta_x=x2-x1; //计算坐标增量 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //设置单步方向 
	else if(delta_x==0)incx=0;//垂直线 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//水平线 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//画线输出 
	{  
		LCD_DrawPoint(uRow,uCol);//画点 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
}    
//画矩形
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2)
{
	LCD_DrawLine(x1,y1,x2,y1);
	LCD_DrawLine(x1,y1,x1,y2);
	LCD_DrawLine(x1,y2,x2,y2);
	LCD_DrawLine(x2,y1,x2,y2);
}
//在指定位置画一个指定大小的圆
//(x,y):中心点
//r    :半径
void Draw_Circle(u16 x0,u16 y0,u8 r)
{
	int a,b;
	int di;
	a=0;b=r;	  
	di=3-(r<<1);             //判断下个点位置的标志
	while(a<=b)
	{
		LCD_DrawPoint(x0-b,y0-a);             //3           
		LCD_DrawPoint(x0+b,y0-a);             //0           
		LCD_DrawPoint(x0-a,y0+b);             //1       
		LCD_DrawPoint(x0-b,y0-a);             //7           
		LCD_DrawPoint(x0-a,y0-b);             //2             
		LCD_DrawPoint(x0+b,y0+a);             //4               
		LCD_DrawPoint(x0+a,y0-b);             //5
		LCD_DrawPoint(x0+a,y0+b);             //6 
		LCD_DrawPoint(x0-b,y0+a);             
		a++;
		//使用Bresenham算法画圆     
		if(di<0)di +=4*a+6;	  
		else
		{
			di+=10+4*(a-b);   
			b--;
		} 
		LCD_DrawPoint(x0+a,y0+b);
	}
} 
//在指定位置显示一个字符

//num:要显示的字符:" "--->"~"
//mode:叠加方式(1)还是非叠加方式(0)
//在指定位置显示一个字符

//num:要显示的字符:" "--->"~"

//mode:叠加方式(1)还是非叠加方式(0)
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 mode)
{
    u8 temp;
    u8 pos,t;
	u16 x0=x;
	u16 colortemp=POINT_COLOR;      
    if(x>LCD_W-16||y>LCD_H-16)return;	    
	//设置窗口		   
	num=num-' ';//得到偏移后的值
	Address_set(x,y,x+8-1,y+16-1);      //设置光标位置 
	if(!mode) //非叠加方式
	{
		for(pos=0;pos<16;pos++)
		{ 
			temp=asc2_1608[(u16)num*16+pos];		 //调用1608字体
			for(t=0;t<8;t++)
		    {                 
		        if(temp&0x01)POINT_COLOR=colortemp;
				else POINT_COLOR=BACK_COLOR;
				SPI_write_data(POINT_COLOR);	
				temp>>=1; 
				x++;
		    }
			x=x0;
			y++;
		}	
	}else//叠加方式
	{
		for(pos=0;pos<16;pos++)
		{
		    temp=asc2_1608[(u16)num*16+pos];		 //调用1608字体
			for(t=0;t<8;t++)
		    {                 
		        if(temp&0x01)LCD_DrawPoint(x+t,y+pos);//画一个点     
		        temp>>=1; 
		    }
		}
	}
	POINT_COLOR=colortemp;	    	   	 	  
}   
//m^n函数
u32 mypow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}			 
//显示2个数字
//x,y :起点坐标	 
//len :数字的位数
//color:颜色
//num:数值(0~4294967295);	
void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len)
{         	
	u8 t,temp;
	u8 enshow=0;
	num=(u16)num;
	for(t=0;t<len;t++)
	{
		temp=(num/mypow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				LCD_ShowChar(x+8*t,y,' ',0);
				continue;
			}else enshow=1; 
		 	 
		}
	 	LCD_ShowChar(x+8*t,y,temp+48,0); 
	}
} 
//显示2个数字
//x,y:起点坐标
//num:数值(0~99);	 
void LCD_Show2Num(u16 x,u16 y,u16 num,u8 len)
{         	
	u8 t,temp;						   
	for(t=0;t<len;t++)
	{
		temp=(num/mypow(10,len-t-1))%10;
	 	LCD_ShowChar(x+8*t,y,temp+'0',0); 
	}
} 
//显示字符串
//x,y:起点坐标  
//*p:字符串起始地址
//用16字体
void LCD_ShowString(u16 x,u16 y,const u8 *p)
{         
    while(*p!='\0')
    {       
        if(x>LCD_W-16){x=0;y+=16;}
        if(y>LCD_H-16){y=x=0;LCD_Clear(RED);}
        LCD_ShowChar(x,y,*p,0);
        x+=8;
        p++;
    }  
}

