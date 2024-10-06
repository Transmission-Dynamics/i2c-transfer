const { execSync } = require('child_process');

if (process.platform === 'linux') {
  console.log('Linux platform detected. Running node-gyp-build...');
  execSync('node-gyp-build', { stdio: 'inherit' });
} else {
  console.log('Non-Linux platform detected. Skipping build.');
}
