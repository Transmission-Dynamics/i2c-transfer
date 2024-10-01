# I2C Transfer Module

A simple Node.js module for performing I2C transfers using N-API.

## Installation

```bash
npm install @transmission-dynamics/i2c-transfer
```

## Usage

```js
const { i2c_transfer } = require('@transmission-dynamics/i2c-transfer');

(async () => {
  try {
    const bus = '/dev/i2c-1'; // I2C bus device path
    const addr = 0x54;        // I2C slave device address
    const writeBuffer = new Uint8Array([0x04]); // Data to write
    const readSize = 3;       // Number of bytes to read

    const readBuffer = await i2c_transfer(bus, addr, writeBuffer, readSize);

    console.log('Data read from I2C device:', readBuffer);
  } catch (err) {
    console.error('Error:', err.message);
  }
})();
```

## Description

The i2c_transfer function allows you to perform an I2C write followed by a read operation asynchronously. It communicates with I2C devices by writing data to a specified register or command and then reading the response.

### Parameters

- bus (string): The I2C bus device path (e.g., '/dev/i2c-1').
- addr (number): The I2C slave device address (7-bit address).
- writeBuffer (Buffer or Uint8Array): The data to write to the I2C device.
- readSize (number): The number of bytes to read from the I2C device.

### Returns

- A Promise that resolves with a Buffer containing the data read from the I2C device.

## Notes

- Permissions: Ensure your user has the necessary permissions to access the I2C bus device (e.g., /dev/i2c-1). You may need to run your script with elevated privileges or adjust the device permissions.
- Error Handling: The function throws errors if any I2C operation fails. Use try-catch blocks to handle exceptions.
- Asynchronous Operation: The i2c_transfer function is asynchronous and returns a Promise, allowing you to use async/await syntax.

## License

This project is licensed under the MIT License.