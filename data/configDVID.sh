# config DVID
#
# gcc 4.8 and go 1.6 recommended
# to build gcc, gmp, mpfr, mpc are required

DVIDWORKSPACE=$1
GOPACKAGEDIR=$2

#
export BUILDEM_DIR=$DVIDWORKSPACE
export PATH=$BUILDEM_DIR/bin:$PATH
export LD_LIBRARY_PATH=$BUILDEM_DIR/lib:$LD_LIBRARY_PATH

# build go
tar -C $BUILDEM_DIR -xzf go1.6.3.linux-amd64.tar.gz
git clone https://go.googlesource.com/go
git checkout go1.6.3

export GOPATH=$GOPACKAGEDIR
export GOROOT=$BUILDEM_DIR/go
export PATH=${PATH}:${GOROOT//://bin:}/bin:

export GOROOT_BOOTSTRAP=$GOROOT

cd $GOROOT/src
./all.bash

# build dvid
export DVIDSRC=$GOPATH/src/github.com/janelia-flyem/dvid

if [ $BUILDDVID = "-b" ]
then

mkdir -p $DVIDSRC
cd $DVIDSRC
git clone https://github.com/janelia-flyem/dvid .

mkdir build
cd build
cmake -D BUILDEM_DIR=$BUILDEM_DIR .. 
make
make dvid
make test

fi
