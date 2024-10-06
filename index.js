const addon = require('node-gyp-build')(__dirname);

module.exports = {
  i2cTransfer: addon.i2cTransfer,
};
