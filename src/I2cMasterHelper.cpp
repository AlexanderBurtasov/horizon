#include "I2cMasterHelper.h"
// std
#include <iostream>
// espidf
#include "driver/i2c.h"

using std::cout;
using std::endl;

I2cMasterHelper::I2cMasterHelper(I2cPort portNum, int sdaNum, int sclNum, uint32_t clockSpeed)
  : m_portNum{portNum}
{
	i2c_config_t config;
  config.mode = I2C_MODE_MASTER;
  config.sda_io_num = sdaNum;
  config.scl_io_num = sclNum;
  config.sda_pullup_en = GPIO_PULLUP_ENABLE;
  config.scl_pullup_en = GPIO_PULLUP_ENABLE;
  config.master.clk_speed = clockSpeed;
  config.clk_flags = 0x0;

  {
    const auto callResult = ::i2c_param_config(static_cast<i2c_port_t>(portNum), &config);
    if (ESP_OK != callResult)
    {
      cout << "fail configuring i2c param" << endl;
    }
  }

  {
    const auto callResult = ::i2c_driver_install(static_cast<i2c_port_t>(portNum), config.mode
      , 0x0 //I2C_MASTER_RX_BUF_DISABLE
      , 0x0 //I2C_MASTER_TX_BUF_DISABLE
      , 0);
    if (ESP_OK != callResult)
    {
      cout << "fail installing i2c driver" << endl;
    }
  }
}

bool I2cMasterHelper::ReadBytes(int deviceAddress, int regAddress, uint8_t *dest, size_t dataLength)
{
    constexpr uint8_t cReadBit = I2C_MASTER_READ;
    #define ACK_VAL 0x0
    #define NACK_VAL 0x1
    constexpr bool ACK_CHECK_EN = true;

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    // first, send device address (indicating write) & register to be read
    i2c_master_write_byte(cmd, (deviceAddress << 1), ACK_CHECK_EN);
    // send register we want
    i2c_master_write_byte(cmd, regAddress, ACK_CHECK_EN);
    // Send repeated start
    i2c_master_start(cmd);
    // now send device address (indicating read) & read data
    i2c_master_write_byte(cmd, (deviceAddress << 1) | cReadBit, ACK_CHECK_EN);
    // if (size > 1)
    // {
    //     i2c_master_read(cmd, data_rd, size - 1, static_cast<i2c_ack_type_t>(ACK_VAL));
    // }
    // i2c_master_read_byte(cmd, data_rd + size - 1, static_cast<i2c_ack_type_t>(NACK_VAL));
    const auto t1 = i2c_master_read(cmd, dest, dataLength, static_cast<i2c_ack_type_t>(ACK_VAL));
    if (ESP_OK != t1)
    {
      cout << "i2c_master_read: " << static_cast<int>(t1) << endl;
    }
    i2c_master_stop(cmd);
  
    const auto t2 = i2c_master_cmd_begin(static_cast<i2c_port_t>(m_portNum), cmd, 4 * 1000 / portTICK_PERIOD_MS);
    if (ESP_OK != t2)
    {
//      cout << "i2c_master_cmd_begin: " << static_cast<int>(t2) << endl;
    }
    i2c_cmd_link_delete(cmd);
    return t2 == ESP_OK;
}
