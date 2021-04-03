/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
   https://github.com/rossumur/esp_8_bit
*/
#include <stdio.h>
#include <stdlib.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/portmacro.h"
#include "esp_types.h"
#include "esp_heap_caps.h"
#include "esp_attr.h"
#include "esp_intr_alloc.h"
#include "esp_err.h"
#include "soc/gpio_reg.h"
#include "soc/rtc.h"
#include "soc/soc.h"
#include "soc/i2s_struct.h"
#include "soc/i2s_reg.h"
#include "soc/ledc_struct.h"
#include "soc/rtc_io_reg.h"
#include "soc/io_mux_reg.h"
#include "driver/ledc.h"
#include "driver/periph_ctrl.h"
#include "driver/dac.h"
#include "driver/gpio.h"
#include "driver/i2s.h"
#include "hal/timer_ll.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/periph_ctrl.h"
#include "driver/timer.h"
#include "driver/gpio.h"
#include "soc/soc.h"
#include "soc/periph_defs.h"
#include "esp_log.h"
#include "main.h"
//#include "rom/gpio.h"
//#include "rom/lldesc.h"
#include "driver/periph_ctrl.h"
#include "driver/dac.h"
#include "driver/gpio.h"
#include "driver/i2s.h"
#include "esp_attr.h"
#include "esp32/rom/gpio.h"
#include "esp32/rom/lldesc.h"
#include "soc/dport_access.h"
#include "soc/dport_reg.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>

#include "esp_vfs_fat.h"
#include "driver/sdspi_host.h"
#include "driver/spi_common.h"
#include "sdmmc_cmd.h"
#include <sys/time.h>
#include "usb_host.h"
#include "esp_vfs.h"
#include "esp_vfs_fat.h"



int64_t get_system_time_us() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000000LL + (tv.tv_usec ));
}
uint8_t hash(int kk)
{
	return kk^(kk>>8);
}
uint32_t hash32(uint32_t kk)
{
	return kk^(kk>>8);
}

void IRAM_ATTR timer_group0_isr(void *para)
{
	timer_group_clr_intr_status_in_isr(TIMER_GROUP_0, TIMER_0);
	//taskENTER_CRITICAL();
	usb_process();
	//taskEXIT_CRITICAL();
	timer_group_enable_alarm_in_isr(TIMER_GROUP_0, TIMER_0);
}
#define BLINK_GPIO  22
#define DP_P  16
#define DM_P  17
#define DP1_P  18
#define DM1_P  19

#define PIXEL_CLOCK (17.734476)
//4.43361875
#define PFR  (PIXEL_CLOCK*0.25) 

#define LW (((int)(64*PIXEL_CLOCK+1.5))&~1)
//#define LW (int)(64*PIXEL_CLOCK+0.5)

#define TD   			1
#define SCLK 			PIXEL_CLOCK	
#define N_SIZE      		LW
#define HALF_TICKS    	(N_SIZE/2)
#define TL_us        		(N_SIZE/2-TN)

#define T10_4us      	(((int)(10.4*SCLK/TD+0.5)))
#define TS_us        		(((int)((2.7)*SCLK/TD+0.5)))
#define TN_us        		(((int)((4.7)*SCLK/TD+0.5)))

#define IRE(_x)          ((uint32_t)(((_x)+40)*255/3.3/147.5) << 8)   // 3.3V DAC
//#define IRE(_x)          ((uint32_t)(((_x)+40)*255*2/3.3/147.5) << 8)   // 3.3V DAC
#define SYNC_LEVELR      IRE(-40)
#define BLANKING_LEVEL   IRE(0)
#define BLACK_LEVEL      IRE(7.5)
#define GRAY_LEVEL       IRE(50)
#define WHITE_LEVEL      IRE(100)

#define   SYNC_LEVEL SYNC_LEVELR
#define   BASE_LEVEL BLANKING_LEVEL
#define   TAMP            (WHITE_LEVEL-BLACK_LEVEL)*0.4

uint16_t BUFF0_R[N_SIZE];
uint16_t BUFF0_S[N_SIZE];

uint16_t BUFF0_RE[N_SIZE];
uint16_t BUFF0_SE[N_SIZE];


#define PNTK  pnt[k^1]
inline void  funcDef(uint16_t *pnt)
{
	for(int k=0;k<TN_us;k++)
	{
		PNTK = SYNC_LEVEL;
	}
	for(int k=TN_us;k<N_SIZE;k++)
	{
		PNTK = BASE_LEVEL;
	}
}
inline void  funcDefT(uint16_t *pnt)
{
	funcDef(pnt);
#if 0
	uint32_t* pnt32 = pnt;
	for(int k=0;k<TN_us/2;k++)
	{
		pnt32[k] = ((SYNC_LEVEL<<16) | SYNC_LEVEL);
	}
#endif
}

// LL
void funcL1(uint16_t *pnt)
{
	for(int k=0;k<N_SIZE;k++)
	{
		PNTK = BASE_LEVEL ;
	}
	for(int k=0;k<HALF_TICKS-TN_us;k++)
	{
		PNTK = SYNC_LEVEL ;
	}
	for(int k=HALF_TICKS;k<N_SIZE-TN_us;k++)
	{
		PNTK = SYNC_LEVEL ;
	}
}

void funcL3(uint16_t *pnt)
{
	for(int k=0;k<N_SIZE;k++)
	{
		PNTK = BASE_LEVEL ;
	}
	for(int k=0;k<HALF_TICKS-TN_us;k++)
	{
		PNTK = SYNC_LEVEL ;
	}
	for(int k=HALF_TICKS;k<HALF_TICKS+TS_us;k++)
	{
		PNTK = SYNC_LEVEL ;
	}
}

void funcL313(uint16_t *pnt)
{
	for(int k=0;k<N_SIZE;k++)
	{
		PNTK = BASE_LEVEL ;
	}
	for(int k=0;k<TS_us;k++)
	{
		PNTK = SYNC_LEVEL ;
	}
	for(int k=HALF_TICKS;k<N_SIZE-TN_us;k++)
	{
		PNTK = SYNC_LEVEL ;
	}
}

void funcL318(uint16_t *pnt)
{
	for(int k=0;k<N_SIZE;k++)
	{
		PNTK = BASE_LEVEL ;
	}
	for(int k=0;k<TS_us;k++)
	{
		PNTK = SYNC_LEVEL ;
	}
}
void funcL623(uint16_t *pnt)
{
	for(int k=0;k<N_SIZE;k++)
	{
		PNTK = BASE_LEVEL ;
	}
	for(int k=0;k<TN_us;k++)
	{
		PNTK = SYNC_LEVEL ;
	}
	for(int k=HALF_TICKS;k<HALF_TICKS+TS_us;k++)
	{
		PNTK = SYNC_LEVEL ;
	}
}
void funcL624(uint16_t *pnt)
{
	for(int k=0;k<N_SIZE;k++)
	{
		PNTK = BASE_LEVEL ;
	}
	for(int k=0;k<TS_us;k++)
	{
		PNTK = SYNC_LEVEL ;
	}
	for(int k=HALF_TICKS;k<HALF_TICKS+TS_us;k++)
	{
		PNTK = SYNC_LEVEL ;
	}
}

#include <math.h>
 
enum lineType {LinWD,lineND,line1,line3,line313,line318,line623,line624};
#define PAL       626
#define NTSC    526
#define LINES   PAL
uint8_t policyT[PAL+1];

volatile int lineN = 0;
int 		 vBaseLine = 0;
//~ float sinTable[1024];
float pSin(float arg)
{
	return sin(arg);
	//return sinTable[((int)(arg*1024/2/M_PI)+1024)%1024];
}
float pCos(float arg)
{
	return cos(arg);
	//return sinTable[((int)(arg*1024/2/M_PI)+256)%1024];
}

// odd/even //9bit RGB 
uint16_t colorTableMain[2][256][1<<2];
#include "lcd.h"
int WWWLINES = 625;
#define SCREEN_W  SW
#define SCREEN_H   SH
#define BORDER_H   (WWWLINES/2-SCREEN_H)/2

uint8_t   screen[SCREEN_H][SCREEN_W];
int         startScreenLine;
int 	      W_SCALE  = 3;
uint8_t makeColor(int R,int G,int B);
void RecalcStartLine()
{
    float tnF = (4.7+5.7)   * PIXEL_CLOCK;
    float tnL = (64.0-1.65) * PIXEL_CLOCK;
    startScreenLine = ((int)(((tnL-tnF)-SCREEN_W*W_SCALE)/2+tnF))&~1;
}
int initTables()
{
	int WW= 0;
	lineN = 0;
	//~ for(int k=0;k<1024;k++)
	//~ {
		//~ sinTable[k] = sin(k*2*M_PI/1024);
	//~ }
    for(int k=0;k<PAL;k++)
    {
    	policyT[k] = LinWD;
    }

    if(LINES==PAL)
    {
    	policyT[623]   = line623;
    	policyT[624]   = line624;
    	policyT[625]   = line624;
    	policyT[1]     = line1;
    	policyT[2]     = line1;
    	policyT[3]     = line3;
    	policyT[4]     = line624;
    	policyT[5] 	   = line624;
    	policyT[311]   = line624;
    	policyT[312]   = line624;
    	policyT[313]   = line313;
    	policyT[314]   = line1;
    	policyT[315]   = line1;
    	policyT[316]   = line624;
    	policyT[317]   = line624;
    	policyT[318]   = line318;
    	int DH = 20;
    	vBaseLine = 24+DH;
    	for(int k=0;k<vBaseLine;k++)
    	{
    		if(policyT[k]==LinWD)
    			policyT[k] = lineND;
    		if(policyT[k+LINES/2]==LinWD)
    			policyT[k+LINES/2] = lineND;
    	}
    	for(int k=LINES/2-DH;k<LINES/2;k++)
    	{
    		if(policyT[k]==LinWD)
    			policyT[k] = lineND;
    		if(policyT[k+LINES/2]==LinWD)
    			policyT[k+LINES/2] = lineND;
    	}
    }
    else
    {
    	policyT[1]   = line624;
    	policyT[2]   = line624;
    	policyT[3]   = line624;
    	policyT[4]   = line1;
    	policyT[5]   = line1;
    	policyT[6]   = line1;
    	policyT[7]   = line624;
    	policyT[8]   = line624;
    	policyT[9]   = line624;

    	policyT[263]   = line623;
    	policyT[264]   = line624;
    	policyT[265]   = line624;
    	policyT[266]   = line313;
    	policyT[267]   = line1;
    	policyT[268]   = line1;
    	policyT[269]   = line3;
    	policyT[270]   = line1;
    	policyT[271]   = line1;
    	policyT[272]   = line318;
    	vBaseLine = 23;
    	for(int k=0;k<23;k++)
    	{
    		if(policyT[k]==LinWD)
    			policyT[k] = lineND;
    		if(policyT[k+LINES/2]==LinWD)
    			policyT[k+LINES/2] = lineND;
    	}
    }
    for(int k=0;k<LINES;k++)
    {
    	if(policyT[k] == LinWD) WW++;
    }
//#define FS2PI (4.43361875*2*M_PI)
    #define FS2PI (PIXEL_CLOCK*0.25*2*M_PI)
    //float phiT[4] =  {0,1*M_PI/2,2*M_PI/2,3*M_PI/2};
    float phiT[4] ;//=  {0,1,2,3}*M_PI*0.5;
    for(int i=0;i<4;i++)
    {
	    phiT[i] = (i+0.5)*M_PI/2;
    }
    //t0 = (k)/PIXEL_CLOCK;
    float tnF = (4.7+5.7)   * PIXEL_CLOCK;
    float tnL = (64.0-1.65) * PIXEL_CLOCK;
    printf("tnF=%f ,tnL=%f ,d =%f \n",tnF,tnL,tnL-tnF);
    
    startScreenLine = ((int)(((tnL-tnF)-SCREEN_W*W_SCALE)/2+tnF))&~1;
    int center = (tnL+tnF)/2;
	 #define CSC  400.0   
	 float ctable[] = {0,0.24,0.5,0.9};
	 int smin =0xffff;
	 int smax =0;
    for(int p=0;p<2;p++)
    {
	    
	    for(int c =0;c<64;c++)
	    {
		float Rl = ctable[(c>>4)&3];//(((c>>4)&0x3))*255.0f/4/CSC;   
		float Gl = ctable[(c>>2)&3];//(((c>>2)&0x3))*255.0f/4/CSC;   
		float Bl = ctable[(c>>0)&3];//((c&0x3))*255.0f/4/CSC;   
		    
		float Y=  0.299*Rl+0.587*Gl+0.114*Bl;
		float U = 0.95*0.492*(Bl-Y);
		float V = 0.95*0.877*(Rl-Y);
		 //  U = 0; 
		 //  V = 0; 
		for(int t=0;t<4;t++)
		{
			float phi  = phiT[t&0x3];	
			//if(flg) phi  = phiT[(t+2)&0x3];
			float val;
			if(p==0)
			{	
				val = (Y+U*pSin(phi)-V*pCos(phi))*TAMP+BLACK_LEVEL;
			}
			else if(p==1)
			{
				val = (Y+U*pSin(phi)+V*pCos(phi))*TAMP+BLACK_LEVEL;

			}
			//if(val>255) val = 255;
			//if(val<0)     val = 0;
			if(val<smin) smin = val;
			if(val>smax) smax = val;
			colorTableMain[p][c][t] = val;//*TAMP+SYNC_LEVEL;
		}	
	    }
    }
	    printf("smin =%x  smax = %x\n",smin,smax);
    for(int  y = 0;y<SCREEN_H;y++)
    {
	    for(int  x = 0;x<SCREEN_W;x++)
	    {
		int n = 255*x/SCREEN_W;
		if(y<SCREEN_H/3)    
		{
			screen[y][x] = makeColor(0,0,n);//(x/8+y/8)%256;
		}
		else if(y<SCREEN_H*2/3)    
		{
			screen[y][x] = makeColor(0,n,0);//(x/8+y/8)%256;
		}
		else
		{
			screen[y][x] = makeColor(n,0,0);//(x/8+y/8)%256;
		}
			
	    }
    }
    for(int tt=0;tt<2;tt++)
    {
		for(int p=0;p<2;p++)
		{
			uint8_t prev = 0;
			int tta = (((int)((4.7)*PIXEL_CLOCK))&~1)+1;
			for(int k=0;k<N_SIZE;k++)
			{
				float  t0  = (k)/PIXEL_CLOCK; //uS
				//float phi  = t0*FS2PI;
				float  phi  = phiT[k&0x3];
				//if(flg) phi  = phiT[(k+2)&0x3];
				float val  = SYNC_LEVELR;
				if(k<tta)
				{
					val = SYNC_LEVELR;
				}
				else if(t0 < (4.7+0.9))
				//else if(k < tt )
				{
					val = BLANKING_LEVEL;
				}
				else if(t0< (4.7+0.9+2.2) )
				{
#define SHIFT (M_PI)
					//phi  = phiT[k&0x3];
					if(p==0)
					{
						//val = pSin(phi+(135*M_PI/180))*BLANKING_LEVEL/1.5+BLANKING_LEVEL;
						val = pSin(phi+(3*M_PI/4))*BLANKING_LEVEL/1.5+BLANKING_LEVEL;
					}
					else
					{
						val = pSin(phi-(3*M_PI/4))*BLANKING_LEVEL/1.5+BLANKING_LEVEL;
						//val = pSin(phi+(225*M_PI/180))*BLANKING_LEVEL/1.5+BLANKING_LEVEL;
					}
				}
				else if(t0< (4.7+5.7) )
				{
					val =BLANKING_LEVEL;
				}
				else if(t0 < (64.0-1.65) )
				{
					 float Rl = 0xCD/CSC;//(1.0-t1)*0.9;
					 float Gl = 0xCD/CSC;
					 float Bl = 0xCD/CSC;
						Rl = 0 ;
						Gl = 0 ;
						Bl = 0 ;
					if(abs(k-center)>(SCREEN_W+BORDER_W*2)*W_SCALE*0.5)
					{
						Rl = 0 ;
						Gl = 0 ;
						Bl = 0 ;
					}
					if(tt==0)
					{
						Rl = 0 ;
						Gl = 0 ;
						Bl = 0 ;
					}
					float Y=  0.299*Rl+0.587*Gl+0.114*Bl;
					float U = 0.492*(Bl-Y);
					float V = 0.877*(Rl-Y);
					if(p==0)
					{
						val = (Y+U*pSin(phi)+V*pCos(phi))*TAMP+BLACK_LEVEL;
						//~ if(k>400&&k<48)
						//~ {
							//~ printf("pSin(phi)=%f pCos(phi)=%f\n",pSin(phi),pCos(phi));
						//~ }
					}
					else
					{
						val = (Y+U*pSin(phi)-V*pCos(phi))*TAMP+BLACK_LEVEL;
					}
				}
				else
				{
					val = BLANKING_LEVEL;
				}

				int BH = val;///*TAMP+SYNC_LEVEL;
				uint16_t* pnt = BUFF0_SE;
				if(tt==0)
				{
					if(p==0)
					{
							pnt= BUFF0_RE;
					}
					else
					{
							pnt=BUFF0_SE;
					}
				}
				else
				{
					if(p==0)
					{
							pnt=BUFF0_R;
					}
					else
					{
							pnt=BUFF0_S;
					}
				}
				PNTK=BH;
			}

		}
    }
    WWWLINES = WW;
    return WW;
}



// PAL init & most of video code from here https://github.com/rossumur/esp_8_bit
// see https://github.com/rossumur/esp_8_bit/blob/master/README.md
// credits to https://bitluni.net/esp32-color-pal

#define VIDEO_PIN   26
#define AUDIO_PIN   LEDC_HS_CH0_GPIO   // can be any pin


lldesc_t _dma_desc[4] = {0};
intr_handle_t _isr_handle;
int _pal_ = 1;

//extern "C"
void IRAM_ATTR video_isr(volatile void* buf);

// simple isr
void IRAM_ATTR i2s_intr_handler_video(void *arg)
{
    if (I2S0.int_st.out_eof)
        video_isr(((lldesc_t*)I2S0.out_eof_des_addr)->buf); // get the next line of video
    I2S0.int_clr.val = I2S0.int_st.val;                     // reset the interrupt
}

static esp_err_t start_dma(int line_width,int samples_per_cc, int ch)
{
    periph_module_enable(PERIPH_I2S0_MODULE);

    // setup interrupt
    if (esp_intr_alloc(ETS_I2S0_INTR_SOURCE, ESP_INTR_FLAG_LEVEL1 | ESP_INTR_FLAG_IRAM,
        i2s_intr_handler_video, 0, &_isr_handle) != ESP_OK)
        return -1;

    // reset conf
    I2S0.conf.val = 1;
    I2S0.conf.val = 0;
    I2S0.conf.tx_right_first = 1;
    I2S0.conf.tx_mono = (ch == 2 ? 0 : 1);

    I2S0.conf2.lcd_en = 1;
    I2S0.fifo_conf.tx_fifo_mod_force_en = 1;
    I2S0.sample_rate_conf.tx_bits_mod = 16;
    I2S0.conf_chan.tx_chan_mod = (ch == 2) ? 0 : 1;

    // Create TX DMA buffers
    for (int i = 0; i < 2; i++) {
        int n = line_width*2*ch;
        if (n >= 4092) {
            printf("DMA chunk too big:%d\n",n);
            return -1;
        }
        _dma_desc[i].buf = (uint8_t*)heap_caps_calloc(1, n, MALLOC_CAP_DMA);
        if (!_dma_desc[i].buf)
            return -1;
        
        _dma_desc[i].owner = 1;
        _dma_desc[i].eof = 1;
        _dma_desc[i].length = n;
        _dma_desc[i].size = n;
        _dma_desc[i].empty = (uint32_t)(i == 1 ? _dma_desc : _dma_desc+1);
    }
    I2S0.out_link.addr = (uint32_t)_dma_desc;

    //  Setup up the apll: See ref 3.2.7 Audio PLL
    //  f_xtal = (int)rtc_clk_xtal_freq_get() * 1000000;
    //  f_out = xtal_freq * (4 + sdm2 + sdm1/256 + sdm0/65536); // 250 < f_out < 500
    //  apll_freq = f_out/((o_div + 2) * 2)
    //  operating range of the f_out is 250 MHz ~ 500 MHz
    //  operating range of the apll_freq is 16 ~ 128 MHz.
    //  select sdm0,sdm1,sdm2 to produce nice multiples of colorburst frequencies

    //  see calc_freq() for math: (4+a)*10/((2 + b)*2) mhz
    //  up to 20mhz seems to work ok:
    //  rtc_clk_apll_enable(1,0x00,0x00,0x4,0);   // 20mhz for fancy DDS

    if (!_pal_) {
        switch (samples_per_cc) {
            case 3: rtc_clk_apll_enable(1,0x46,0x97,0x4,2);   break;    // 10.7386363636 3x NTSC (10.7386398315mhz)
            case 4: rtc_clk_apll_enable(1,0x46,0x97,0x4,1);   break;    // 14.3181818182 4x NTSC (14.3181864421mhz)
        }
    } else {
        rtc_clk_apll_enable(1,0x04,0xA4,0x6,1);     // 17.734476mhz ~4x PAL
    }
    double sdm0 = 0x4;
    double sdm1 = 0xa4;
    double sdm2 = 0x6;
    double o_div = 1;
    double xtal_freq = 10;
    double apll_freq = xtal_freq * (4 + sdm2 + sdm1/256 + sdm0/65536)/((o_div + 2) * 2);
    printf("apll_freq = %f\n",(float)apll_freq);
    I2S0.clkm_conf.clkm_div_num = 1;            // I2S clock divider’s integral value.
    I2S0.clkm_conf.clkm_div_b = 0;              // Fractional clock divider’s numerator value.
    I2S0.clkm_conf.clkm_div_a = 1;              // Fractional clock divider’s denominator value
    I2S0.sample_rate_conf.tx_bck_div_num = 1;
    I2S0.clkm_conf.clka_en = 1;                 // Set this bit to enable clk_apll.
    I2S0.fifo_conf.tx_fifo_mod = (ch == 2) ? 0 : 1; // 32-bit dual or 16-bit single channel data

    dac_output_enable(DAC_CHANNEL_1);           // DAC, video on GPIO25
    dac_i2s_enable();                           // start DAC!

    I2S0.conf.tx_start = 1;                     // start DMA!
    I2S0.int_clr.val = 0xFFFFFFFF;
    I2S0.int_ena.out_eof = 1;
    I2S0.out_link.start = 1;
    return esp_intr_enable(_isr_handle);        // start interruprs!
}
#define NTSC_COLOR_CLOCKS_PER_SCANLINE 228       // really 227.5 for NTSC but want to avoid half phase fiddling for now
#define NTSC_FREQUENCY (315000000.0/88)
#define NTSC_LINES 262

//#define PAL_COLOR_CLOCKS_PER_SCANLINE 284        // really 283.75 ?
//#define PAL_FREQUENCY 4433618.75
//#define PAL_LINES 312
#define LEDC_HS_TIMER          LEDC_TIMER_1
#define LEDC_HS_MODE           LEDC_HIGH_SPEED_MODE
#define LEDC_HS_CH0_GPIO       (27)
#define LEDC_HS_CH0_CHANNEL    LEDC_CHANNEL_0
#define LEDC_TEST_CH_NUM       (1)
      ledc_channel_config_t ch_conf =    
      {
            .channel    = LEDC_HS_CH0_CHANNEL,
            .duty           = 64,
            .gpio_num   = LEDC_HS_CH0_GPIO,
            .speed_mode = LEDC_HS_MODE,
            .hpoint          = 0,
            .timer_sel      = LEDC_HS_TIMER
        };

void video_init_hw(int line_width, int samples_per_cc)
{
    // setup apll 4x NTSC or PAL colorburst rate
	
    // Now ideally we would like to use the decoupled left DAC channel to produce audio
    // But when using the APLL there appears to be some clock domain conflict that causes
    // nasty digitial spikes and dropouts. You are also limited to a single audio channel.
    // So it is back to PWM/PDM and a 1 bit DAC for us. Good news is that we can do stereo
    // if we want to and have lots of different ways of doing nice noise shaping etc.

    // PWM audio out of pin 18 -> can be anything
    // lots of other ways, PDM by hand over I2S1, spi circular buffer etc
    // but if you would like stereo the led pwm seems like a fine choice
    // needs a simple rc filter (1k->1.2k resistor & 10nf->15nf cap work fine)

    // 18 ----/\/\/\/----|------- a out
    //          1k       |
    //                  ---
    //                  --- 10nf
    //                   |
    //                   v gnd
	
    ledc_timer_config_t ledc_timer = {
        .duty_resolution = LEDC_TIMER_8_BIT, // resolution of PWM duty
        .freq_hz =  15600,                      // frequency of PWM signal
        .speed_mode = LEDC_HS_MODE,           // timer mode
        .timer_num = LEDC_HS_TIMER,            // timer index
        .clk_cfg = LEDC_AUTO_CLK,              // Auto select the source clock
    };
      ledc_timer_config(&ledc_timer);
      
      //~ ledc_channel_config_t ch_conf =    
      //~ {
            //~ .channel    = LEDC_HS_CH0_CHANNEL,
            //~ .duty           = 64,
            //~ .gpio_num   = LEDC_HS_CH0_GPIO,
            //~ .speed_mode = LEDC_HS_MODE,
            //~ .hpoint          = 0,
            //~ .timer_sel      = LEDC_HS_TIMER
        //~ };
	
	ledc_channel_config(&ch_conf);
	
    start_dma(line_width,samples_per_cc,1);
    
#if 0
    ledcSetup(0,2000000,7);    // 625000 khz is as fast as we go w 7 bits
    ledcAttachPin(AUDIO_PIN, 0);
    ledcWrite(0,0);
#endif
    //  IR input if used
#ifdef IR_PIN
    //pinMode(IR_PIN,INPUT);
#endif
}
// send an audio sample every scanline (15720hz for ntsc, 15600hz for PAL)
inline void IRAM_ATTR audio_sample(uint8_t s)
{
        ledc_set_duty(ch_conf.speed_mode, ch_conf.channel, s);
        ledc_update_duty(ch_conf.speed_mode, ch_conf.channel);
	
#if 0	
    auto& reg = LEDC.channel_group[0].channel[0];
    reg.duty.duty = s << 4; // 25 bit (21.4)
    reg.conf0.sig_out_en = 1; // This is the output enable control bit for channel
    reg.conf1.duty_start = 1; // When duty_num duty_cycle and duty_scale has been configured. these register won't take effect until set duty_start. this bit is automatically cleared by hardware
    reg.conf0.clk_en = 1;
#endif	
}
/*
void* MALLOC32(int x, const char* label)
{
    printf("MALLOC32 %d free, %d biggest, allocating %s:%d\n",
      heap_caps_get_free_size(MALLOC_CAP_32BIT),heap_caps_get_largest_free_block(MALLOC_CAP_32BIT),label,x);
    void * r = heap_caps_malloc(x,MALLOC_CAP_32BIT);
    if (!r) {
        printf("MALLOC32 FAILED allocation of %s:%d!!!!####################\n",label,x);
        esp_restart();
    }
    else
        printf("MALLOC32 allocation of %s:%d %p\n",label,x,r);
    return r;
}
*/

// sound
#define RS_SIZE_BITS (11)
#define RS_SIZE (1<<RS_SIZE_BITS)

uint16_t VoiceBuff0T[RS_SIZE];
volatile int sound_head = 0;
volatile int sound_tail = 0;

int sound_size()
{
	return (sound_head+RS_SIZE-sound_tail)&(RS_SIZE-1);
}

int waitFor()
{
	int cntWait = 0;
	//TimMasterHandle.Instance = TIM2;
	while(((sound_head+RS_SIZE-sound_tail)&(RS_SIZE-1))>RS_SIZE/2)
	{
		//procKeyb();
		vTaskDelay(2/portTICK_PERIOD_MS);
		cntWait++;
	}
	return cntWait;
}
void push_pair(uint16_t Left,uint16_t Right)
{
	waitFor();
	VoiceBuff0T[sound_head] = Left;
	sound_head = (sound_head+1)&(RS_SIZE-1);
}
void cleanAudioBuffer()
{
	waitFor();
	for(int k=0;k<RS_SIZE;k++)
	{
		push_pair(MAX_VOLUME/2,MAX_VOLUME/2);
	}
}

#define ISR_BEGIN()
#define ISR_END()
volatile int prevFull0;
volatile int prevFull1;
uint32_t frameCounterL = 0;
int      LineB  = 0;
#include <string.h>
//3*3 bits color table;+1 /odd even  2/ pixels
uint32_t timeInFrame =0;
int cnttt = 0;
void HAL_DMACallback(uint16_t * pnt)
{
	
			//	int64_t start_DMA_us = get_system_time_us();
				//if(flagEnd!=1) return;
				//flagEnd = 0;
				cnttt ++;
				//if(!(cnttt&1)) pnt0++;
				lineN++;
				if(lineN==LINES)
				{
					  lineN = 1;
					  frameCounterL++;
				 }

			  int pol = policyT[lineN];
			  //pol = LinWD;
			  //~ if((prevFull0!=pol) || (prevFull1!=pol))
			  {
				  switch(pol)
				  {
					  case  line1: funcL1(pnt);break;
					  case  line3: funcL3(pnt);break;
					  case  line313: funcL313(pnt);break;
					  case  line318: funcL318(pnt);break;
					  case  line623: funcL623(pnt);break;
					  case  line624: funcL624(pnt);break;
					//  case  LinWD:   funcDefT(pnt0);break;
					//  case  lineND:   funcDef(pnt0);break;
				  }
			  }
		          prevFull0 = prevFull1;
			  prevFull1 = pol;
			    //float tnF = (4.7+5.7)   * PIXEL_CLOCK;
			    //float tnL = (64.0-1.65) * PIXEL_CLOCK;
			    //startScreenLine = ((int)(((tnL-tnF)-SCREEN_W*W_SCALE)/2+tnF))&~1;

///*
			  if(pol==LinWD)
			  {
				  int rLine = ((lineN-1) % (LINES/2))-(vBaseLine-1);
				  int start = startScreenLine;
				  {
					  int flg = cnttt&1;//(frameCounter&1)==(lineN&1);
					  int y = rLine - BORDER_H;
					  if( y>=0&&y < SCREEN_H)
					  {
						
						  if(flg)
						  {
							//memcpy(pnt1,BUFF0_R,N_SIZE*2);
							  
							memcpy(pnt,BUFF0_R,start*2);
							memcpy(pnt+N_SIZE-start,BUFF0_R+N_SIZE-start,start*2);
						  }
						  else
						  {
							  //memcpy(pnt1,BUFF0_S,N_SIZE*2);
							  
							  
							  memcpy(pnt,BUFF0_S,start*2);
							  memcpy(pnt+N_SIZE-start,BUFF0_S+N_SIZE-start,start*2);
						  }
						  uint16_t *arpf = colorTableMain[flg][0];
						 if(W_SCALE == 2) 
						 {
							for(int x = 0;x< SCREEN_W ;x++)
							{
								int N =   screen[y][x];
								uint16_t *arpfp = &arpf[N*4];			
								{
								int k = x*2+start;
								PNTK = arpfp[(k)&0x3];
								k++;
								PNTK = arpfp[(k)&0x3];
								}
							 }
						 }
						else if(W_SCALE == 3)
						{
							for(int x = 0;x< SCREEN_W ;x++)
							{
								int N =   screen[y][x];
								uint16_t *arpfp = &arpf[N*4];			
								{
								int k = x*3+start;
								PNTK = arpfp[(k)&0x3];
								k++;
								PNTK = arpfp[(k)&0x3];
								k++;
								PNTK = arpfp[(k)&0x3];
								}
							 }
						}
						else 
						{							
							for(int x = 0;x< SCREEN_W ;x++)
							{
								int N =   screen[y][x];
								uint16_t *arpfp = &arpf[N*4];			
								for(int t=0;t<W_SCALE;t++)
								{
									int k = x*W_SCALE+t+start;
									uint16_t color = colorTableMain[flg][N][(k)&0x3];
									PNTK = color;
								}
							}
						}
						//memcpy(TEMP_LINE,pnt0,N_SIZE*2);
						//~ uint32_t box = TEMP_LINE[start-2]+TEMP_LINE[start-1]+TEMP_LINE[start]; 
						//~ for(int r=start;r<start+SCREEN_W*W_SCALE+1;r++)
						//~ {
							//~ box+= TEMP_LINE[r+1]-TEMP_LINE[r-2];
							//~ pnt0[r] = box/3;
						//~ }
						//pnt0[0] = TEMP_LINE[0];
					  }
					  else
					  {
						  if(flg)
						  {
							  //memcpy(pnt0,BUFF0_R,N_SIZE*2);
							 // memcpy(pnt0,BUFF0_R,start*2);
							  memcpy(pnt,BUFF0_R,N_SIZE*2);
							  //memcpy(pnt0,sflag?BUFF1_RE:BUFF0_RE,N_SIZE*2);
						  }
						  else
						  {
							  //memcpy(pnt0,BUFF0_S,N_SIZE*2);
							  //memcpy(pnt0,BUFF0_S,start*2);
							  //memcpy(pnt0,sflag?BUFF1_SE:BUFF0_SE,N_SIZE*2);
							  memcpy(pnt,BUFF0_S,N_SIZE*2);
						  }
					  }
				  }
			  }
			  if(pol==lineND)
			  {
				if(cnttt&1)	
				{
					memcpy(pnt,BUFF0_RE,N_SIZE*2);
				}
				else
				{
					memcpy(pnt,BUFF0_SE,N_SIZE*2);
				}
			  }
			  //~ if(flagTime)
				  //~ timeInFrame =get_system_time_us() -  start_DMA_us;
//*/
}
void IRAM_ATTR video_isr(volatile void* vbuf)
{
    ISR_BEGIN();
	uint16_t* bb = (uint16_t *)(vbuf);
	
	HAL_DMACallback(bb);
	audio_sample(VoiceBuff0T[sound_tail]);
	sound_tail = (sound_tail+1)&(RS_SIZE-1);
	
	//funcDef(bb);
	/*
	for(int k=0;k<LW;k++)	
	{
		bb[k] = line[k];
	}
	*/
    ISR_END();
}

 
void led(int on_fff)
{
			gpio_set_level(BLINK_GPIO, !on_fff);
}

#include "hid_keyb.h"

uint32_t  HAL_GetTick()
{
	uint64_t  ttt =  get_system_time_us();
	return ttt/1000;
}
void HAL_Delay(int ms)
{
	 vTaskDelay(ms / portTICK_PERIOD_MS);
}
void Delay(int ms)
{
	HAL_Delay(ms);
}

void dTest();

static xQueueHandle usb_mess_Que = NULL;

volatile uint16_t keyMatrix  = 0;
uint16_t scanJSK()
{
	return keyMatrix;
}
void clearKey()
{
	keyMatrix = 0;
}

MY_HID_KEYBD_Info_TypeDef prev_state_s;

struct USBMessage
{
	uint8_t src;
	uint8_t len;
	uint8_t  data[0x8];
};

uint8_t * RAMCOMMON[8];

void initRam()
{
    printf("Free heap: %d\n", esp_get_free_heap_size());
    for(int j=0;j<8;j++)
    {
	    RAMCOMMON[j] = malloc(0x4000);
	    if(RAMCOMMON[j])
	    {
		    for(int k=0;k<0x4000;k++)
			RAMCOMMON[j][k] = k;
		    printf("Free heap: %d j=%d\n", esp_get_free_heap_size(),j);
	    }
    }
}
uint8_t capsFl = 0;

void kscan0()
{
	struct USBMessage msg;
        if(xQueueReceive(usb_mess_Que, &msg, 0)) 
	{
	    printf("%02x %02x ",msg.src,msg.len);
		int unum= msg.src / 4;
            for(int k=0;k<msg.len;k++)
	   {
		   printf("%02x ",msg.data[k]);
		   if(k==0)
		   {
			 prev_state_s.lshift =   msg.data[k]&0x2?1:0;
			 prev_state_s.rshift =   msg.data[k]&0x20?1:0;
			 prev_state_s.lctrl  =   msg.data[k]&0x1?1:0;
			 prev_state_s.rctrl  =   msg.data[k]&0x10?1:0;
			 prev_state_s.lalt   =   msg.data[k]&0x4?1:0;
			 prev_state_s.ralt   =   msg.data[k]&0x40?1:0;
		   }
		   if(k>=2)
		   {
			   prev_state_s.keys[k-2] = msg.data[k];
			   
			   if(msg.data[k]==KEY_CAPS_LOCK)
			   {
				uint8_t flg = usbGetFlags(unum);
				   if(flg!=0)
				   {
					  usbSetFlags(unum,0x0) ;
				   }
				   else
				   {
					 usbSetFlags(unum,0x7) ;  
				   }
			   }
			switch(msg.data[k])
			{
				case  KEY_RIGHTARROW: 	keyMatrix = K_RIGHT;break;
				case  KEY_LEFTARROW: 	keyMatrix = K_LEFT;break;
				case  KEY_DOWNARROW: 	keyMatrix = K_DOWN;break;
				case  KEY_UPARROW: 		keyMatrix = K_UP;break;
				case  KEY_F10: 			setTapeSpeed(0);break;
				case  KEY_F11: 			setTapeSpeed(1);break;
				case  KEY_F12: 			setTapeSpeed(2);break;

				//case  0x0:  			keyMatrix = 0;break;
				case  KEY_ENTER:  		keyMatrix = K_FIRE;break;//enterbreak;
				case  KEY_SPACEBAR: 	keyMatrix = K_SPACE;break;
				case  KEY_ESCAPE:		keyMatrix = K_ESC;break;
				case  KEY_F4:			W_SCALE =2; RecalcStartLine();break;
				case  KEY_F5:			W_SCALE =3; RecalcStartLine();break;
				case  KEY_F7:  			setScreenOffset(0);break;
				case  KEY_F8:  			setScreenOffset(1);break;
				case  KEY_F9:  			setScreenOffset(2);break;
				case  KEY_F6:
								initRamSpec();
								ay_reset();
								z80_reset(1);
								SKEY_QUE_init();
								setScreenOffset(0);
							break;
			};
				if(msg.data[k]==KEY_CAPS_LOCK)
				{
					if(capsFl&2)
					{
						capsFl&=~2;
					}
					else
					{
						capsFl|=2;
					}
				}
				if(msg.data[k]==KEY_KEYPAD_NUM_LOCK_AND_CLEAR)
				{
					if(capsFl&1)
					{
						capsFl&=~1;
					}
					else
					{
						capsFl|=1;
					}
				}
				if(msg.data[k]==KEY_SCROLL_LOCK)
				{
					if(capsFl&4)
					{
						capsFl&=~4;
					}
					else
					{
						capsFl|=4;
					}
				}
			   
			   
		   }
	    }	
	    if(keyMatrix)
	    {
		    //~ printf("keyMatrix = %x\n",keyMatrix);
	    }
	    printf("\n");
        }
}



int dummyFunction(struct SYS_EVENT* ev)
{
	printf("dummy mess   = %x\r\n",ev->message);
	printf("dummy param1 = %x\r\n",ev->param1);
	printf("dummy param2 = %x\r\n",ev->param2);
	return 0;
}
int backLight = 10;
int getBacklight()
{
	return backLight;
}
void setBacklight(int bl)
{
	backLight = bl;
}

dispathFunction currFunc = &dummyFunction;
#define stackFuncSize 16
dispathFunction stackFunc[stackFuncSize];


int stackFuncP = 0;

void pushMenuFunc(void*fnk)
{
	if(stackFuncP<stackFuncSize-2)
	{
		stackFunc[stackFuncP] = fnk;
		stackFuncP++;
	}
}

dispathFunction popMenuFunc()
{
	if(stackFuncP>0)
	{
		stackFuncP--;
	}
	currFunc = stackFunc[stackFuncP];
	return currFunc;
}

void setCurrentFunc(void*fnk)
{
	pushMenuFunc(currFunc);
	currFunc = fnk;
}

void usbMess(uint8_t src,uint8_t len,uint8_t *data)
{
	struct  USBMessage msg;
	msg.src = src;
	msg.len = len<0x8?len:0x8;
	for(int k=0;k<msg.len;k++)
	{
		msg.data[k] = data[k];
	}
	xQueueSend(usb_mess_Que, ( void * ) &msg,(TickType_t)0);
}

uint8_t * getRamPage(int k)
{
	return &RAMCOMMON[k+4][0];
}
uint8_t * getRamBuffer0x4000(int k)
{
	return RAMCOMMON[k];
}



int readDirIntoList(char* currentDir)
{
	//~ FILINFO 	MyFileInfo;
	//~ DIR 		MyDirectory;
	//~ FRESULT 	res;
//	taskENTER_CRITICAL();
	DIR *	MyDirectory;
	//root
	char fullDir[0x30];
	strcpy(fullDir,MOUNT_POINT);
	strcpy(fullDir+strlen(fullDir),"/");
	strcpy(fullDir+strlen(fullDir),currentDir);
	MyDirectory = opendir(fullDir);
	int files = 0;
	initStrList();
	if(MyDirectory)
	{
		printf("open dir res ok %s\n",fullDir);
		struct dirent *d;
		while ((d = readdir(MyDirectory)) != NULL)
		{
			if(d->d_type==DT_DIR)
			{
				printf("DIR:%s\n",d->d_name);
			}
			else if(d->d_type==DT_REG)
			{
				 int ft = getFileType(d->d_name);
				 if(ft)
				 {
				    		  //putStr(MyFileInfo.fattrib,MyFileInfo.fname);
					  putStr(ft,d->d_name);
				}
				files++;
			}
		}
		closedir(MyDirectory);
	}
	else
	{
		printf("open dir err %s\n",fullDir);
	}

    return files;
}

//#include "esp_task_wdt.h"

int bNoDisk = 1;

void timer_task(void *pvParameter)
{
	    timer_config_t config = {
        .divider = TIMER_DIVIDER,
        .counter_dir = TIMER_COUNT_UP,
        .counter_en = TIMER_PAUSE,
        .alarm_en = TIMER_ALARM_EN,
        .auto_reload = 1,
    }; // default clock source is APB
    

  
    usb_mess_Que  = xQueueCreate(10,sizeof(struct USBMessage));
    
    initStates(DP_P,DM_P,DP1_P,DM1_P,-1,-1,-1,-1);
    gpio_pad_select_gpio(BLINK_GPIO);
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);


    int timer_idx = TIMER_0;
    double timer_interval_sec = TIMER_INTERVAL0_SEC;

     timer_init(TIMER_GROUP_0, timer_idx, &config);
    timer_set_counter_value(TIMER_GROUP_0, timer_idx, 0x00000000ULL);
    timer_set_alarm_value(TIMER_GROUP_0, timer_idx, timer_interval_sec * TIMER_SCALE);
    timer_enable_intr(TIMER_GROUP_0, timer_idx);
    timer_isr_register(TIMER_GROUP_0, timer_idx, timer_group0_isr,(void *) timer_idx, ESP_INTR_FLAG_IRAM, NULL);
    timer_start(TIMER_GROUP_0, timer_idx);
    initRam();
    initSDCard();
	volatile struct SYS_EVENT event;
	event.message = MESS_OPEN;
	event.param1  = 0x1234;
	event.param2  = 0x5678;
	//~ if(bFlagCPM)
	//~ {
	  //~ init_cpm();
	  //~ z80_reset(1);
	//~ }
	//~ else
	{
		init_Sinclair();
		if(bNoDisk)
		{
				clearFullScreen();
				setModeFile(1);
				setCurrentFunc(&z48_z128_dispatch);
		}
		else
		{
				setCurrentFunc(&menu_dispatch);
		}
	}
	//  uint32_t prevtk =  HAL_GetTick()+20;
	while(1)
	{
		  if(keyMatrix&&(event.message==MESS_IDLE))
		  {
			event.param1 = keyMatrix;
			event.param2 = HAL_GetTick();
			event.message = MESS_KEYBOARD;
			//~ printf("keyMatrix = %x\n",keyMatrix);
		  }
		  else
		  {
			//  HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
			  //~ if(keyMatrix)
			//~ printf("event.message = %x %p\n",event.message,currFunc);
		  }
		  (*currFunc)(&event);
		  if(event.message==MESS_CLOSE)
		  {
			event.message = MESS_REPAINT;
			printf("stackFuncP B =%d\r\n",stackFuncP);
			popMenuFunc();
			printf("stackFuncP A b=%d\r\n",stackFuncP);
		  }
		  kscan0();
		 // taskYIELD ();
		  //vTaskDelay(0);
		  //esp_task_wdt_reset();
		  //vTaskDelay(10/portTICK_PERIOD_MS);
	}
    
}


void blink_task(void *pvParameter)
{
    int WW= initTables();
	sound_head = 0;
	sound_tail = 0;
    printf("N_SIZE = %d H=%d %d\n",N_SIZE,WW/2,WW);
    video_init_hw(LW,4);
     printf("init ok\n")	;
//    dTest();
    while(1)
    {
	vTaskDelay(10 / portTICK_PERIOD_MS);
	printState();
    }
}

void app_main(void)
{
    printf("\n\n\nProgram start!\n");
    {
	printf("portTICK_PERIOD_MS = %d\n",portTICK_PERIOD_MS);
    }
//	perfTest();
    /* Print chip information */
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    printf("This is %s chip with %d CPU cores, WiFi%s%s, ",
            CONFIG_IDF_TARGET,
            chip_info.cores,
            (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
            (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

    printf("silicon revision %d, ", chip_info.revision);

    printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
            (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    printf("Free heap: %d\n", esp_get_free_heap_size());

    
    
    for(int k = 0;k<RS_SIZE;k++)
	VoiceBuff0T[k] = sin(k*M_PI*2*16/RS_SIZE)*120+130;
    xTaskCreatePinnedToCore(&timer_task, "timer_task", 4096, "task 0", 5, NULL,1);
    xTaskCreatePinnedToCore(&blink_task, "blink_task", 4096, "task 1", 5, NULL,0);
   printf("Hello world end %f!\n",64*PIXEL_CLOCK);	
}
