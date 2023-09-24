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

#define CONFIG_WIDTH  240
#define CONFIG_HEIGHT 280
#define CONFIG_MOSI_GPIO 13
//#define CONFIG_SCLK_GPIO 14
#define CONFIG_SCLK_GPIO 23
#define CONFIG_CS_GPIO 16
#define CONFIG_DC_GPIO 19
#define CONFIG_RESET_GPIO 4
#define CONFIG_BL_GPIO 22

#define CONFIG_OFFSETX 0
#define CONFIG_OFFSETY 20

using namespace std;


void DoColorTest(TFT_t *dev, const std::vector<uint16_t> &colors, uint16_t width, uint16_t height);
void DoPlainView(TFT_t *dev, GuideVector &rGuide, mutex &rMutex);
void DoFontTest(TFT_t *dev);
void DoI2cTest();
void DoI2cTest2();
void DoJoystickTest();

void DoRcControlTest();
void DoRcControlTest2();
void DoStupid();
void DoAdc(adc1_channel_t adcEnum);

extern "C"
{

void app_main() 
{
    cout << "Hells Bells!_1" << endl;

    //DoAdc(ADC1_CHANNEL_7); // pin35
    //DoAdc(ADC1_CHANNEL_6); // pin34
    //DoAdc(ADC1_CHANNEL_3); // pin39
    //DoAdc(ADC1_CHANNEL_0); // pin36
    //DoRcControlTest();
    DoRcControlTest2();

    //DoStupid();
    //DoRcControlTest();
//    DoJoystickTest();
//    DoI2cTest2();
    // TFT_t dev;
    // spi_master_init(&dev, CONFIG_MOSI_GPIO, CONFIG_SCLK_GPIO, CONFIG_CS_GPIO, CONFIG_DC_GPIO, CONFIG_RESET_GPIO, CONFIG_BL_GPIO);
    // lcdInit(&dev, CONFIG_WIDTH, CONFIG_HEIGHT, CONFIG_OFFSETX, CONFIG_OFFSETY);
    // lcdDrawFillRect(&dev, 0, 0, 239, 279, CYAN);

    // GuideVector guide;
    // mutex guideMutex;

    // auto callFunc = [&guide, &guideMutex]()
    // {
    //     uint32_t tickCount = 0;
    //     for (;;)
    //     {
    //         guide.x0 = 10 * sin(2*3.14 * tickCount / 100);
    //         guide.y0 = -5 * sin(2*3.14 * tickCount / 25);

    //         guide.x1 = 10 * cos(2*3.14 * tickCount / 50);
    //         guide.y1 = 8 * sin(2*3.14 * tickCount / 100);

    //         ++tickCount;
    //         this_thread::sleep_for(chrono::milliseconds(100));
    //     }
    // };

    // thread th{callFunc};

    // DoPlainView(&dev, guide, guideMutex);
    // th.join();

    // {
    //     TimeMeter meter("Clear Screen old: ");
    //     ::lcdDrawFillRect(&dev, 0, 0, 239, 279, BLUE);
    // }
    // {
    //     TimeMeter meter("Clear Screen yellow: ");
    //     ::DrawFillRect(&dev, 0, 0, 240, 280, YELLOW);
    // }

    // std::vector<uint16_t> colors;
    // const uint16_t cDelta = 8;

    // ::DrawFillRect(&dev, 15, 15, 210, 250, GREEN);

    // for (int i = 192; i > 0; i -= cDelta)
    // {
    //     colors.push_back(::rgb565_conv(16, i, 192));
    // }
    // {
    //     TimeMeter meter("Color Test: ");
    //     DoColorTest(&dev, colors, 240, 4);
    // }

    // {
    //     TimeMeter meter("Blue line test: ");
    //     for (uint16_t j = 10; j < 280; ++j)
    //     {
    //         ::lcdDrawLine(&dev, 10, 220, 230, 280-j, BLUE);
    //     }
    // }

    // std::this_thread::sleep_for(chrono::milliseconds(1500));

    // {
    //     TimeMeter meter("Clear Screen blue: ");
    //     ::DrawFillRect(&dev, 0, 128, 128, 64, BLUE);
    // }

/*    HorizonWidget widget{&dev, 240, 280};
    {
        TimeMeter meter("Horizon: ");
        widget.paintEvent();
    }
    std::chrono::milliseconds ms{1};

    Vga8x8 vga8x8{&dev};
    Vga8x16 vga8x16{&dev};
    Vga16x32 vga16x32{&dev};
    */
/*
    {
        TimeMeter meter("Fonts: ");
        for (uint8_t i = 0x20; i < 0x30; ++i)
        {
            vga8x8.DisplayChar(i, 10 + (i - 0x20) * 10, 20);
            vga8x16.DisplayChar(i, 10 + (i - 0x20) * 10, 50);
        }
    }
*/
    // for (size_t i = 0; i < 90; ++i)
    // {
    //     TimeMeter meter{("pitch angle: " + std::to_string(i) + "; update time: ").c_str()};
    //     widget.setPitch(i);
    //     widget.setRoll(i);
    //     widget.paintEvent();
    // }
    // for (int i = 90; i > -90; --i)
    // {
    //     TimeMeter meter{("pitch angle: " + std::to_string(i) + "; update time: ").c_str()};
    //     widget.setPitch(i);
    //     widget.setRoll(i);
    //     widget.paintEvent();
    // }
}

}

void DoAdc(adc1_channel_t adcChannel)
{
    gpio_num_t aPin; 
    auto r = adc1_pad_get_io_num(adcChannel, &aPin); // gpio_12
    assert(r == ESP_OK);

    r = adc1_config_channel_atten(adcChannel, ADC_ATTEN_DB_11);
    assert(r == ESP_OK);

    int vX;
    vX = ::adc1_get_raw(adcChannel);

    cout << "first: " << vX << endl;

    for (;;)
    {
        int value = ::adc1_get_raw(adcChannel);
        if (vX != value)
        {
            cout << "value: " << value << endl;
        }
        this_thread::sleep_for(chrono::milliseconds(100));
    }

    // r = adc2_pad_get_io_num(ADC2_CHANNEL_6, &bPin);      // gpio 14
    // assert(r == ESP_OK);

    // r = adc2_config_channel_atten(ADC2_CHANNEL_6, ADC_ATTEN_DB_11);
    // assert(r == ESP_OK);                                                                                                                                                                                                                                                                                                                                                                                                                               

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
    #define I2C_SLAVE_ADDR	0x9
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

void DoPlainView(TFT_t *dev, GuideVector &rGuide, mutex &rMutex)
{
    Vga16x32 vga16x32{dev};
    vga16x32.DisplayText("x0: ", 20, 40, WHITE, BLACK);
    vga16x32.DisplayText("y0: ", 20, 70, WHITE, BLACK);
    vga16x32.DisplayText("x1: ", 20, 110, WHITE, BLACK);
    vga16x32.DisplayText("y1: ", 20, 140, WHITE, BLACK);

    auto callLambda = [dev, &rGuide, &rMutex, &vga16x32]()
    {
    vector<bool> updateItems;
    updateItems.resize(4);
    GuideVector current{-127, -127, -127, -127};

    for (;;)
    {
        {
            lock_guard<mutex> lock{rMutex};
            if (current.x0 != rGuide.x0)
            {
                updateItems[0] = true;
                current.x0 = rGuide.x0;
            }
            else
            {
                updateItems[0] = false;
            }

            if (current.y0 != rGuide.y0)
            {
                updateItems[1] = true;
                current.y0 = rGuide.y0;
            }
            else
            {
                updateItems[1] = false;
            }

            if (current.x1 != rGuide.x1)
            {
                updateItems[2] = true;
                current.x1 = rGuide.x1;
            }
            else
            {
                updateItems[2] = false;
            }

            if (current.y1 != rGuide.y1)
            {
                updateItems[3] = true;
                current.y1 = rGuide.y1;
            }
            else
            {
                updateItems[3] = false;
            }
        }

        if (updateItems[0])
        {
            vga16x32.DisplayText(std::to_string(current.x0).c_str(), 70, 40, WHITE, YELLOW);
        }
        if (updateItems[1])
        {
            vga16x32.DisplayText(std::to_string(current.y0).c_str(), 70, 70, WHITE, YELLOW);
        }
        if (updateItems[2])
        {
            vga16x32.DisplayText(std::to_string(current.x1).c_str(), 70, 110, WHITE, YELLOW);
        }
        if (updateItems[3])
        {
            vga16x32.DisplayText(std::to_string(current.y1).c_str(), 70, 140, WHITE, YELLOW);
        }
        this_thread::sleep_for(chrono::milliseconds(100));
    }
    };

    thread th{callLambda};
    th.join();
}

void DoFontTest(TFT_t *dev)
{
    Vga8x16 vga8x16{dev};

    TimeMeter meter("Font 8x16: ");
    uint16_t left = 15;
    uint16_t top = 20;
    uint16_t pos = 0;
    for (uint16_t sym = 0x0; sym <= 0xff; ++sym)
    {
        vga8x16.DisplayChar(sym, left + pos * 10, top, BLACK, YELLOW);
        ++pos;
        if (pos == 20)
        {
            pos = 0;
            top += 20;
        }
        if (top >= 260)
        {
            top = 20;
        }
    }
}

void DoColorTest(TFT_t *dev, const std::vector<uint16_t> &colors, uint16_t width, uint16_t height)
{
    uint16_t y = 0;
    uint16_t x = 0;
    for (const auto color : colors)
    {
        ::DrawFillRect(dev, x, y, width, height, color);
        y += height + 1;
    }
}
