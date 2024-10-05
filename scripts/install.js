const { execSync } = require('child_process');

if (process.platform === 'linux') {
  console.log('Linux platform detected. Running node-gyp rebuild...');
  execSync('node-gyp rebuild', { stdio: 'inherit' });
} else {
  console.log('Non-Linux platform detected. Skipping build.');
}
