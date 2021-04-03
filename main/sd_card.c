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

#ifdef CONFIG_IDF_TARGET_ESP32
#include "driver/sdmmc_host.h"
#endif

#include "esp_vfs.h"
#include "esp_vfs_fat.h"
static const char *TAG = "example";

#define USE_SPI_MODE

#ifndef SPI_DMA_CHAN
#define SPI_DMA_CHAN    1
#endif //SPI_DMA_CHAN
#ifdef USE_SPI_MODE
// Pin mapping when using SPI mode.
// With this mapping, SD card can be used both in SPI and 1-line SD mode.
// Note that a pull-up on CS line is required in SD mode.
#define PIN_NUM_MISO 2
#define PIN_NUM_MOSI 15
#define PIN_NUM_CLK  14
#define PIN_NUM_CS   13
#endif //USE_SPI_MODE


#define PIN_SD_CLK 14
#define PIN_SD_CS   13
#define PIN_SD_MISO   2
#define PIN_SD_MOSI    15


#ifdef ARD_SDIO    
#include "stm32_adafruit_sd.h"
#endif
uint8_t      SD_BUFF[512];
#define SD_TIMEOUT 30 * 1000

sdmmc_card_t* card;
const char mount_point[] = MOUNT_POINT;
extern int bNoDisk;
void initSDCard()
{
#ifdef ARD_SDIO    
#include "stm32_adafruit_sd.h"
    setPinsSDIO(PIN_SD_CS,PIN_SD_CLK,PIN_SD_MISO,PIN_SD_MOSI);
  SD_CardInfo CardInfo;
  uint8_t res = BSP_SD_Init();
  //if(res==MSD_OK)
  {
  BSP_SD_GetCardInfo(&CardInfo);
  printf("hsd.CardCapacity =%lld\n",CardInfo.CardCapacity);
  //printf("hsd.SdCard.BlockNbr=%d\n",hsd.SdCard.BlockNbr);
  //printf("hsd.SdCard.BlockSize=%d\n",hsd.SdCard.BlockSize);
  //printf("hsd.SdCard.CardType=%d\n",hsd.SdCard.CardType);
  //printf("hsd.SdCard.CardVersion=%d\n",hsd.SdCard.CardVersion);
  //printf("hsd.SdCard.Class=%d\n",hsd.SdCard.Class);
  printf("hsd.CardBlockSize =%d\n",CardInfo.CardBlockSize);
  printf("hsd.SdCard.LogBlockNbr=%d\n",CardInfo.LogBlockNbr);
  printf("hsd.SdCard.LogBlockSize=%d\n",CardInfo.LogBlockSize);
  uint64_t size_k = CardInfo.LogBlockNbr;
  size_k = size_k*CardInfo.LogBlockSize;

  printf("CardInfo size %d MB\n",(int)(size_k/1024/1024));
	  
  {
	  int32_t tk = HAL_GetTick();
	  int stat = 0;
	  int kk;
	  for(kk=0;(kk<2*64)&&!stat;kk++)
	  {
  			stat = BSP_SD_ReadBlocks(&SD_BUFF[0],kk,1, SD_TIMEOUT);
	  }
	  tk = HAL_GetTick()-tk;
	  printf("Read %d blocks (bytes = %d) in %d ms st = %d\n",kk,kk*512,tk,stat);
	//  paint();
  }
  }
 #else
  esp_err_t ret;
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
#ifdef CONFIG_EXAMPLE_FORMAT_IF_MOUNT_FAILED
        .format_if_mount_failed = true,
#else
        .format_if_mount_failed = false,
#endif // EXAMPLE_FORMAT_IF_MOUNT_FAILED
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    };
    ESP_LOGI(TAG, "Initializing SD card");

        ESP_LOGI(TAG, "Using SPI peripheral");

    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
//    host.max_freq_khz = 10000;
    spi_bus_config_t bus_cfg = {
        .mosi_io_num = PIN_NUM_MOSI,
        .miso_io_num = PIN_NUM_MISO,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4000,
    };
    ret = spi_bus_initialize(host.slot, &bus_cfg, SPI_DMA_CHAN);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize bus.");
        return;
    }

    // This initializes the slot without card detect (CD) and write protect (WP) signals.
    // Modify slot_config.gpio_cd and slot_config.gpio_wp if your board has these signals.
    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = PIN_NUM_CS;
    slot_config.host_id = host.slot;

    int flagOk = 1;
    //ret = ESP_FAIL;
    ret = esp_vfs_fat_sdspi_mount(mount_point, &host, &slot_config, &mount_config, &card);
    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount filesystem. "
                "If you want the card to be formatted, set the EXAMPLE_FORMAT_IF_MOUNT_FAILED menuconfig option.");
        } else {
            ESP_LOGE(TAG, "Failed to initialize the card (%s). "
                "Make sure SD card lines have pull-up resistors in place.", esp_err_to_name(ret));
        }
	flagOk = 0;
        //return;
    }
    else
    {
	flagOk = 1;    
    }
    if(flagOk)
    {
	    // Card has been initialized, print its properties
	    sdmmc_card_print_info(stdout, card);
	  {
		  int32_t tk = HAL_GetTick();
		  esp_err_t stat = ESP_OK;
		  int kk;
		  for(kk=0;(kk<2*64)&&(stat==ESP_OK);kk++)
		  {
			 // stat = BSP_SD_ReadBlocks(&SD_BUFF[0],kk,1, SD_TIMEOUT);
			  stat = sdmmc_read_sectors(card,&SD_BUFF[0],kk,1);
		  }
		  tk = HAL_GetTick()-tk;
		  printf("Read %d blocks (bytes = %d) in %d ms st = %d\n",kk,kk*512,tk,stat);
		//  paint();
	  }
		int numFiles = readDirIntoList("");
		printf("numFiles = %d\n\n",numFiles);
		numFiles = readDirIntoList("/Z48");
		printf("numFiles = %d\n\n",numFiles);
		numFiles = readDirIntoList("MP3");
		printf("numFiles = %d\n\n",numFiles);
       //
	bNoDisk = 0;
   }
#endif	
}
void closeCard()
{
	esp_vfs_fat_sdcard_unmount(mount_point, card);
}
