const fs = require('node:fs');
const { execSync } = require('node:child_process');

fs.rmSync('./prebuilds', { recursive: true, force: true });

function build(platform) {
  const imageName = 'transmission-dynamics-i2c-transfer';
  const containerName = 'transmission-dynamics-i2c-transfer-container';
  execSync(`docker build . --platform=${platform} -t ${imageName}`, { stdio: 'inherit' });
  execSync(`docker container rm -f ${containerName}`, { stdio: 'ignore' });
  execSync(`docker create --name ${containerName} ${imageName}`, { stdio: 'inherit' });
  execSync(`docker cp ${containerName}:/workspace/prebuilds .`, { stdio: 'inherit' });
  execSync(`docker container rm -f ${containerName}`, { stdio: 'ignore' });
  execSync(`docker image rm -f ${imageName}`, { stdio: 'ignore' });
}

const platforms = [
  "linux/arm64/v8",
  "linux/arm/v7",
  "linux/amd64",
];

platforms.forEach(platform => build(platform));
