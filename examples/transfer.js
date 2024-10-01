const { i2c_transfer } = require('../index.js');

(async() => {
  try {
    const bus = '/dev/i2c-1';
    const addr = 0x54;
    const writeBuffer = new Uint8Array([0x04]);
    const readSize = 3;
    const readBuffer = await i2c_transfer(bus, addr, writeBuffer, readSize);
    console.log('Data read from I2C device:', readBuffer);
  } catch (err) {
    console.error('Error:', err.message);
  }
})();
