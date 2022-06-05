# 3DSync

Homebrew for Nintendo 3DS/2DS console family that allows synchronization of saves and files to a cloud, to another console or to a PC

Configurator has built-in support for Checkpoint and JKSM folders, but you can add custom one if you like

**currently in early stage, supports only Dropbox to upload/download files**


## Usage

1. Follow steps on the [configurator page](https://concreted.github.io/3DSync/) to obtain the configuration file
2. Place the configurator file in the following folder of the console SD card: `/3ds/3DSync/3DSync.ini`
3. Download and install .cia file **or** run the .3dsx from the homebrew launcher

### Checkpoint Citra save downloading 

The application will attempt to find and download Citra saves in Dropbox to the 3DS's Checkpoint directory. To enable, place your entire Citra `sdmc` directory in your Dropbox's `Apps/3DSyncU` directory. The final directory structure in Dropbox should look something like:

```
Apps
|_3DSyncU
  |_sdmc
    |_Nintendo 3DS
      |_...
```

## Development

Follow the steps below to install build dependencies and compile on Ubuntu:

```bash
sudo apt install zip unzip -y

# Install devkitpro with 3DS support
sudo apt-get install gdebi-core
wget https://github.com/devkitPro/pacman/releases/download/v1.0.2/devkitpro-pacman.amd64.deb
sudo gdebi devkitpro-pacman.amd64.deb
sudo dkp-pacman -S 3ds-dev

source /etc/profile.d/devkit-env.sh

# Install other 3DS build dependencies
sudo dkp-pacman -S 3ds-curl 3ds-mbedtls 3ds-libjson-c

wget https://github.com/Steveice10/bannertool/releases/download/1.2.0/bannertool.zip
unzip bannertool.zip -d bannertool
sudo cp bannertool/linux-x86_64/bannertool /usr/local/bin

wget https://github.com/3DSGuy/Project_CTR/releases/download/makerom-v0.18.3/makerom-v0.18.3-ubuntu_x86_64.zip
unzip makerom-v0.18.3-ubuntu_x86_64.zip -d makerom
sudo cp makerom/makerom /usr/local/bin
sudo chmod +x /usr/local/bin/makerom

# Build 3DS binaries
mkdir -p ~/src
cd ~/src
git clone git@github.com:concreted/3DSync.git
cd 3DSync
make
```

A testing binary can be built to test functionality locally:

```bash
# Build
## Before building, you may have to comment out line 8 in source/utils/curl.cpp (referring to VERSION_STRING)
g++ -o test test.cpp source/modules/dropbox.cpp source/utils/curl.cpp -lcurl -ljson-c

# Run the test binary
## Set your Dropbox refresh token as env var
export DROPBOX_REFRESH_TOKEN=<your Dropbox refresh token>
./test
```
