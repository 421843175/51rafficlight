#include<REGX52.H> 

#define RED 14
#define GREEN 12

// P3->K P1->D(灯) P20->D9 P21->D10 P22->(74HC138)A P23->B P24->C P0->(动态数码管)A-DP 
sbit LSA=P2^2;  //端口P22
sbit LSB=P2^3;
sbit LSC=P2^4;

sbit D1=P1^0;
sbit D2=P1^1;
sbit D3=P1^2;
sbit D4=P1^3;
sbit D5=P1^4;
sbit D6=P1^5;
sbit D7=P1^6;
sbit D8=P1^7;
sbit D9=P2^0;
sbit D10=P2^1;

unsigned char numbermode[17]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71};
void Delayms(unsigned int delayms)		//@12.000MHz
{
	unsigned char i, j;
	while(delayms--){
		i = 2;
		j = 239;
		do
		{
			while (--j);
		} while (--i);
	}
}

void Timer0Init(void)		//1毫秒@12.000MHz
{
	TMOD &= 0xF0;		//设置定时器模式
	TMOD |= 0x01;		//设置定时器模式
	TL0 = 0x18;		//设置定时初值
	TH0 = 0xFC;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	//配置中断 根据电路图 打开ET0开关
	ET0=1;
	EA=1;
	PT0=0; //这个双路开关拨到下面
}

void Timer1Init(void)		//1毫秒@12.000MHz
{
	TMOD &= 0x0F;		//设置定时器模式
	TMOD |= 0x10;		//设置定时器模式
	TL1 = 0x18;		//设置定时初值
	TH1 = 0xFC;		//设置定时初值
	TF1 = 0;		//清除TF1标志
	TR1 = 0;		//定时器1不开始计时
		//配置中断 根据电路图 打开ET0开关
	ET1=1;
	EA=1;
	PT1=0; //这个双路开关拨到下面
}

void controllerLight(unsigned char location,number){
	switch(location){
		case 1:
			LSC=1;LSB=1;LSA=1;  //根据电路图走Y7 最后一个数字亮
			break;
		case 2:
			LSC=1;LSB=1;LSA=0;  
			break;
		case 3:
			LSC=1;LSB=0;LSA=1;  
			break;
		case 4:
			LSC=1;LSB=0;LSA=0;  
			break;
		case 5:
			LSC=0;LSB=1;LSA=1;
			break;
		case 6:
			LSC=0;LSB=1;LSA=0; 
			break;
		case 7:
			LSC=0;LSB=0;LSA=1;  
			break;
		case 8:
			LSC=0;LSB=0;LSA=0;  
			break;
	}
	P0=numbermode[number];
	
	
	//优化一下 防止位选 段选   位选 中显示数据篡位问题
	Delayms(1);
	P0=0x00;

}


void subNumber(int number){
	int ge;
	int location=1;
	if(number==0){
	controllerLight(1,0);
	return;
	}
	
	while(number){
		ge=number%10;	
		number=number/10;
		controllerLight(location++,ge);
	
		}

}
int redTime=RED;
int greenTime=-GREEN;
int redTime2=RED;
int greenTime2=-GREEN;
int guoren;
void main(){
	int k=1;


	while(1){
				if(P3_0==0){
					
			Delayms(20);  
			while(P3_0==0);
			Delayms(20);
					
			Timer0Init();
			k=1;
		
					
			//打开定时器0
			TR0 = 1;	
					
					//灯清空
				P1=0xFF;
				D9=1;
				D10=1;
					
		
					
						
				D5=0;
				D6=0;
				D9=0;
				D2=0;
					
				while(P3_1!=0 && k==1){

					if(redTime>=0){
						subNumber(redTime);
					}else {
					subNumber(greenTime);
					}
					if(P3_1==0){
						k=2;
						break;
					}
						
				}
				
				
				
				
			}
		
		if(P3_1==0){
				Timer1Init();
			
			Delayms(20);  
			while(P3_1==0);
			Delayms(20);
			
			k=2;
			
			TR0 = 0;		//定时器0关闭计时
			TR1=1;  //定时器1开始计时
			  
			P1=0xFF;
			D9=1;
			D10=1;
			
			D2=0;
			D6=0;
			
			while(P3_0!=0 && k==2){

				
					if(redTime2>=0 && guoren){
						subNumber(redTime2);
					}else if(redTime2<=0 && greenTime2>=0 && guoren) {
					subNumber(greenTime2);
					}
					
						if(P3_1==0){
							Delayms(20);  
							while(P3_1==0); 
							Delayms(20);
							
							redTime2=RED;
							guoren=1;
						}
						
					if(P3_0==0){
						k=1;
						break;
					}
			}
		}
}

	
}

/************************************************/
void Time0_DaoL() interrupt 1{ 
	static int T0count=0;
	static int greentored=0;
	static int redflag=1;
//每次中断初始化
	TL0 = 0x18;		//设置定时初值
	TH0 = 0xFC;		//设置定时初值
	
	
	T0count++;
	if(T0count>=1000){  //到达1s
		T0count=0;
		if(redTime>=0){
		redTime--;
		}else {
			greenTime--;
		}
		
	}
	if(redTime<=6 && redTime>3){
			greentored++;
			if(greentored%500==0){
				greentored=0;
				D6=~D6;
			}
	}else if(redTime>=0 && redTime<=3){
		if(D7!=0){
		D6=1;
		D7=0;
		}
			greentored++;
			if(greentored%500==0){
				greentored=0;
				D9=~D9;
			}
	
	}else if(redTime<0){
		if(greenTime<=0 && redflag){
			greenTime=GREEN;
			D3=0;
			D5=1;
			D7=1;
			D8=0;
			
			D1=0;
			D10=0;
			D2=1;
			D9=1;
			
			redflag=0;
		
			
		}
			
	}
	
	//greenTime<=0 redlight
	if(greenTime>3 && greenTime<=6){
			greentored++;
			if(greentored%500==0){
				greentored=0;
				D3=~D3;
			}
	}else if(greenTime>=0 && greenTime<=3){
		if(D4!=0){	
			D4=0;
		D3=1;
		}
	
		greentored++;
			if(greentored%500==0){
				greentored=0;
				D1=~D1;
			}
	}
	else if(greenTime<0){
			if(redTime<=0 && !redflag){
			D4=1;
			D5=0;
			D6=0;
			D8=1;
			D7=1;
				
			D1=1;
			D2=0;
			D9=0;
			D10=1;
			redTime=RED;
			redflag=1;
		}

	}

}









/*************************************/
void Time1_DaoL() interrupt 3{

static int T1count=0;
	static int t1greentored=0;
//每次中断初始化
	TL1 = 0x18;		//设置定时初值
	TH1 = 0xFC;		//设置定时初值
	
	
	T1count++;
	if(T1count>=1000){  //到达1s
		T1count=0;
		if(redTime2>=0){
		redTime2--;
		}else {
			greenTime2--;
		}
		
	}
	if(redTime2<=6 && redTime2>3){
			t1greentored++;
			if(t1greentored%500==0){
				t1greentored=0;
				D6=~D6;
			}
	}else if(redTime2>=0 && redTime2<=3){
		if(D7!=0){
		D6=1;
		D7=0;
		}
	
	
	}else if(redTime2<0){
		if(guoren==1){
		greenTime2=GREEN;
			D7=1;
			D8=0;
			D6=1;
			D1=0;
			D2=1;	
			guoren=2;
		}
			
	}
	
	//greenTime<=0 redlight
	if(greenTime2>=0 && greenTime2<=3){	
		
	
		t1greentored++;
			if(t1greentored%500==0){
				t1greentored=0;
				D1=~D1;
			}
	}
	else if(greenTime2<0){
			if(redTime2<=0&&guoren==2){
	
			D6=0;
			D8=1;
			D7=1;
				
			D1=1;
			D2=0;
			guoren=0;
		}

	}
	

	

}
