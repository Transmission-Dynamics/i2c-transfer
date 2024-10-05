const addon = require('bindings')('i2cTransfer');

module.exports = {
  i2cTransfer: addon.i2cTransfer,
};
