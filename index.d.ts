declare module '@transmission-dynamics/i2c-transfer' {
  /// <reference types="node" />

  /**
   * Performs an asynchronous I2C transfer by writing data to a device and reading the response.
   *
   * @param bus - The I2C bus device path (e.g., '/dev/i2c-1').
   * @param addr - The I2C slave device address (7-bit address).
   * @param writeBuffer - The data to write to the I2C device.
   * @param readLength - The number of bytes to read from the I2C device.
   * @returns A Promise that resolves with a Buffer containing the data read from the I2C device.
   * @throws An error if the I2C operation fails.
   */
  export function i2c_transfer(
    bus: string,
    addr: number,
    writeBuffer: Buffer | Uint8Array,
    readLength: number
  ): Promise<Buffer>;
}
