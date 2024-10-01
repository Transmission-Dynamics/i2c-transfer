const addon = require('bindings')('i2c_transfer');

module.exports = {
  i2c_transfer: addon.i2c_transfer,
};
