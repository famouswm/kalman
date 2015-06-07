#include "hmc.h"

HMC5883::HMC5883(PinName sda, PinName scl) : hmc_i2c(sda, scl)
{

}

bool HMC5883::init(int frequency, 
         int num_averaging_samples, 
         int data_output_rate, 
         int bias_mode,
         int gain,
         int measurement_mode)
{
    hmc_i2c.frequency(frequency);

    writeByte(HMC_CONFIGURATION_REGISTER_A, measurement_mode | (data_output_rate << 2) | (num_averaging_samples << 5));
    writeByte(HMC_CONFIGURATION_REGISTER_B, gain << 5);
    writeByte(HMC_MODE_REGISTER, measurement_mode);

    // TODO: Self-test

    // Test connection (verify id readback)
    char id[3] = { 0, 0, 0 };
    readBytes(HMC_ID_REGISTER_A, id, 3);
    return ((int)id[0] == HMC_ID_A_DEFAULT) &&
           ((int)id[1] == HMC_ID_B_DEFAULT) &&
           ((int)id[2] == HMC_ID_C_DEFAULT);
}

void HMC5883::getXYZ(int16_t *x, int16_t *y, int16_t *z)
{
    int16_t xzy[3] = { 0, 0, 0 }; // Note the order here!
    readBytes(HMC_DATA_OUTPUT_X_MSB, (char*)&xzy[0], 6);
    swapBytes(&xzy[0]);
    swapBytes(&xzy[1]);
    swapBytes(&xzy[2]);
    *x = xzy[0];
    *z = xzy[1];
    *y = xzy[2];
}

int HMC5883::writeByte(int where, int what)
{
    char data[2] = { (char)where, (char)what };
    int nack = hmc_i2c.write(HMC_WRITE_ADDRESS, data, 2);
    return !nack;

    // int ack = 0;
    // hmc_i2c.start();
    // ack |= hmc_i2c.write(HMC_WRITE_ADDRESS);
    // ack |= hmc_i2c.write(where);
    // ack |= hmc_i2c.write(what);
    // hmc_i2c.stop();
    // return ack;
}

int HMC5883::readBytes(int where, char *data, int length)
{
    int nack = 0;
    nack |= hmc_i2c.write(HMC_WRITE_ADDRESS, (const char*)&where, 1, true);
    nack |= hmc_i2c.read(HMC_READ_ADDRESS, data, length);
    return !nack;

    // int ack = 0;
    // hmc_i2c.start();
    // ack |= hmc_i2c.write(HMC_WRITE_ADDRESS);
    // ack |= hmc_i2c.write(where);
    // hmc_i2c.stop();

    // int nack = hmc_i2c.read(HMC_READ_ADDRESS, data, length);
    // return !nack && ack;
}

void HMC5883::swapBytes(int16_t *v)
{
    *v = ((*v & 0x00ff) << 8) | ((*v & 0xff00) >> 8);
}