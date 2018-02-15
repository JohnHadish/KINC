
# Coding for KINC on Ubuntu 16.04
To begin development work in KINC on Ubuntu 16.04 the following steps can be used to setup your coding environment:

## Step 1: Install Dependencies
This document assumes you already have the g++ compiler installed.

### OpenCL
First we need to instal the OpenCL development libraries.  

```bash
sudo apt install ocl-icd-libopencl1
sudo apt install opencl-headers
sudo apt install clinfo
sudo apt install ocl-icd-opencl-dev
```
For Intel or NVidia drivers see the respective drivers.

### OpenMPI
Next, we need to install the OpenMPI library

```bash
sudo apt-get install openmpi-bin libopenmpi-dev openmpi-common
```

### QT 5.7 or greater
First identify the most recent version of QT at http://download.qt.io/official_releases/qt and adjust the commands below accordingly:

```bash
wget http://download.qt.io/official_releases/qt/5.10/5.10.0/qt-opensource-linux-x64-5.10.0.run
./qt-opensource-linux-x64-5.7.0.run


## Step 2: Download ACE and KINC
Use Git to clone the ACE and ACE repositories and switch to the develop branch

```bash
git clone git@github.com:SystemsGenetics/ACE.git
cd ACE
git checkout develop 

cd ../
git clone git@github.com:SystemsGenetics/KINC.git
cd KINC
git checkout develop
```
### Step 3: Compile ACE and KINC on the command-line
Compile ACE according to its own instructions.  If you install ACE locally you'll have to add the following to your LD_LIBRARY_PATH environment variable.

```bash
export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:${HOME}/software/lib"
```

To Compile KINC run these command inside the build directory:

```bash
qmake ../src/KINC.pro
make
```
You can now run KINC.

### Step 4: Use QTCreator for Coding
First add ACE. Click **File** > **Open File or Project** and then navigate in the file browser to the ACE directory and click the ACE.pro file. Navigate through configure setup.  Do the same for KINC.