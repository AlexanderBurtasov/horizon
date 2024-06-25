// std
#include <chrono>
#include <cmath>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
// esp32
#include "driver/adc.h"
#include "driver/i2c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_vfs.h"
#include "esp_spiffs.h"
// prj
#include "types/GuideVector.h"
#include "utils/TimeMeter.h"
#include "HorizonWidget.h"
#include "I2cJoystick.h"
#include "I2cMasterHelper.h"
#include "st7789.h"
#include "VgaFont.h"

using namespace std;

void DoColorTest(TFT_t *dev, const std::vector<uint16_t> &colors, uint16_t width, uint16_t height);
void DoPlainView(TFT_t *dev, GuideVector &rGuide, mutex &rMutex);
void DoFontTest(TFT_t *dev);
void DoI2cTest();
void DoI2cTest2();
void DoMlxTest();
void DoJoystickTest();

void DoRcControlTest();
void DoRcControlTest2();
void DoStupid();
void DoAdcChannelTest(adc1_channel_t adcEnum);

void DoTftTest();
void DoLineTest();
void DoSinTest();
void DoReceiverTest();
void DoTransmitterTest();
void DoServoTest();

void DoAdcJoystickTest();
void DoAdcJoystickTftTest();
void DoAdcJoystickLn298Test();

void DoJoystickTftTest();

void DoReceiverTest();
void DoTransmitterTest();

extern "C"
{

void app_main() 
{
  //DoJoystickTftTest();
  //DoAdcJoystickTest();
  //DoAdcJoystickTftTest();
  //DoAdcJoystickLn298Test();

  //DoReceiverTest();
  DoTransmitterTest();
}

}

void DoJoystickTest()
{
  I2cMasterHelper i2cMaster{I2cMasterHelper::I2cPort::cNumber_0, 21, 22, 100000};
  I2cJoystick i2cJoystick{0x9, i2cMaster, 10};

  while (true)
  {
    auto status = i2cJoystick.Update();
    if (0x0 != status)
    {
      cout << "x: " << static_cast<int16_t>(i2cJoystick.GetX())
        << "; y: " << static_cast<int16_t>(i2cJoystick.GetY()) << endl;
    }

    this_thread::sleep_for(chrono::milliseconds(100));
  }
}

void DoI2cTest2()
{
    I2cMasterHelper i2cMaster{I2cMasterHelper::I2cPort::cNumber_0, 21, 22, 100000};
    int8_t rx_data[2] = {0};
    int8_t x = 0;
    int8_t y = 0;
    bool updated = false;

    while (true)
    {
        {
            //TimeMeter meter{"reading duration: "};
            i2cMaster.ReadBytes(0x9, 0x12, reinterpret_cast<uint8_t *>(rx_data), 1);
            i2cMaster.ReadBytes(0x9, 0x13, reinterpret_cast<uint8_t *>(rx_data+1), 1);
        }

        if (rx_data[0] != x)
        {
            x = rx_data[0];
            updated = true;
        }
        if (rx_data[1] != y)
        {
            y = rx_data[1];
            updated = true;
        }
        if (updated)
        {
            cout << "x: " << static_cast<int16_t>(x) << "; y: " << static_cast<int16_t>(y) << endl;
            updated = false;
        }
        this_thread::sleep_for(chrono::milliseconds(100));
    }
}

static esp_err_t i2c_master_read_slave_reg(i2c_port_t i2c_num, uint8_t i2c_addr, uint8_t i2c_reg, uint8_t* data_rd, size_t size);

void DoI2cTest()
{
    #define I2C_SLAVE_ADDR0x9
    #define TIMEOUT_MS 1000
    #define DELAY_MS 1000
    #define I2C_MASTER_TX_BUF_DISABLE 0                           /*!< I2C master doesn't need buffer */
    #define I2C_MASTER_RX_BUF_DISABLE 0                           /*!< I2C master doesn't need buffer */

	int8_t rx_data[0x2];
	i2c_config_t conf = {
		.mode = I2C_MODE_MASTER,
		.sda_io_num = 21,
		.scl_io_num = 22,
		.sda_pullup_en = GPIO_PULLUP_ENABLE,
		.scl_pullup_en = GPIO_PULLUP_ENABLE
	};
    conf.master.clk_speed = 100000;
    conf.clk_flags = 0x0;

    int8_t x = 0;
    int8_t y = 0;
    bool updated = false;
	i2c_param_config(I2C_NUM_0, &conf);
	ESP_ERROR_CHECK(i2c_driver_install(I2C_NUM_0, conf.mode
        , I2C_MASTER_RX_BUF_DISABLE
        , I2C_MASTER_TX_BUF_DISABLE
        , 0));

	while (true)
    {
//		i2c_master_read_from_device(I2C_NUM_0, I2C_SLAVE_ADDR, reinterpret_cast<uint8_t *>(rx_data), 0x13, TIMEOUT_MS/portTICK_RATE_MS);
//		ESP_LOG_BUFFER_HEX(TAG, rx_data, 5);
        //i2c_xxx_read(0x9, 0x12, reinterpret_cast<uint8_t *>(rx_data), 2);
        {
            TimeMeter meter{"reading duration"};
            i2c_master_read_slave_reg(I2C_NUM_0, 0x9, 0x12, reinterpret_cast<uint8_t *>(rx_data), 1);
            i2c_master_read_slave_reg(I2C_NUM_0, 0x9, 0x13, reinterpret_cast<uint8_t *>(rx_data+1), 1);
        }
		//vTaskDelay(DELAY_MS/portTICK_RATE_MS);

        if (rx_data[0] != x)
        {
            x = rx_data[0];
            updated = true;
        }
        if (rx_data[1] != y)
        {
            y = rx_data[1];
            updated = true;
        }
        if (updated)
        {
            cout << "x: " << static_cast<int16_t>(x) << "; y: " << static_cast<int16_t>(y) << endl;
            updated = false;
        }
        this_thread::sleep_for(chrono::milliseconds(100));
	}
}

static esp_err_t i2c_master_read_slave_reg(i2c_port_t i2c_num, uint8_t i2c_addr, uint8_t i2c_reg, uint8_t* data_rd, size_t size)
{
    if (size == 0)
    {
        return ESP_OK;
    }
    #define READ_BIT I2C_MASTER_READ
    #define ACK_VAL 0x0
    #define NACK_VAL 0x1
    constexpr bool ACK_CHECK_EN = true;

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    // first, send device address (indicating write) & register to be read
    i2c_master_write_byte(cmd, ( i2c_addr << 1 ), ACK_CHECK_EN);
    // send register we want
    i2c_master_write_byte(cmd, i2c_reg, ACK_CHECK_EN);
    // Send repeated start
    i2c_master_start(cmd);
    // now send device address (indicating read) & read data
    i2c_master_write_byte(cmd, ( i2c_addr << 1 ) | READ_BIT, ACK_CHECK_EN);
    // if (size > 1)
    // {
    //     i2c_master_read(cmd, data_rd, size - 1, static_cast<i2c_ack_type_t>(ACK_VAL));
    // }
    // i2c_master_read_byte(cmd, data_rd + size - 1, static_cast<i2c_ack_type_t>(NACK_VAL));
    i2c_master_read(cmd, data_rd, size, static_cast<i2c_ack_type_t>(ACK_VAL));
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}
