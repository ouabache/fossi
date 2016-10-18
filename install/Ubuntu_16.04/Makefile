SHELL=/bin/sh
MAKE=make
CUR_DIR=$(shell pwd)
home=$(CUR_DIR)/..

all:   install


.PHONY install:  socgen optimsoc icestorm
install:
	(\
        sudo apt-get  install -y emacs;\
        sudo apt-get  install -y cvs;\
        sudo apt-get  install -y subversion;\
        sudo apt-get  install -y git;\
        sudo apt-get  install -y qt5-default;\
        sudo apt-get  install -y libqt5xmlpatterns5-dev;\
        sudo apt-get  install -y qttools5-private-dev;\
        sudo apt-get  install -y qttools5-dev-tools;\
        sudo apt-get  install -y python-pip;\
        sudo apt-get  install -y build-essential;\
        sudo apt-get  install -y make;\
        sudo apt-get  install -y gcc;\
        sudo apt-get  install -y g++;\
        sudo apt-get  install -y flex;\
        sudo apt-get  install -y bison;\
        sudo apt-get  install -y patch;\
        sudo apt-get  install -y texinfo;\
        sudo apt-get  install -y libncurses5-dev;\
        sudo apt-get  install -y libmpfr-dev;\
        sudo apt-get  install -y libgmp3-dev;\
        sudo apt-get  install -y libmpc-dev;\
        sudo apt-get  install -y libzip-dev;\
        sudo apt-get  install -y python-dev;\
        sudo apt-get  install -y libexpat1-dev;\
        sudo apt-get  install -y libtool;\
        sudo apt-get  install -y autoconf;\
        sudo apt-get  install -y libelf-dev;\
	sudo apt-get  install -y geda-gschem;\
)



.PHONY socgen:
socgen:
	(\
        sudo apt-get  install -y xutils-dev;\
        sudo apt-get  install -y tkcvs;\
        sudo apt-get  install -y libxml-libxml-perl;\
        sudo apt-get  install -y libberkeleydb-perl;\
        sudo apt-get  install -y libscalar-util-numeric-perl;\
        sudo apt-get  install -y libparallel-forkmanager-perl;\
        sudo apt-get  install -y libverilog-perl;\
        sudo apt-get  install -y gtkwave;\
        sudo apt-get  install -y verilator;\
        sudo apt-get  install -y covered;\
        sudo apt-get  install -y crasm;\
	)


.PHONY optimsoc:
optimsoc:
	(\
        sudo apt-get  install -y git;\
        sudo apt-get  install -y build;\
        sudo apt-get  install -y libusb-1.0-0-dev;\
        sudo apt-get  install -y libboost-dev;\
        sudo apt-get  install -y swig;\
        sudo apt-get  install -y python3;\
        sudo apt-get  install -y python3-yaml;\
        sudo apt-get  install -y gtkwave;\
	)



.PHONY icestorm:
icestorm:
	(\
        sudo apt-get install -y build-essential;\
        sudo apt-get install -y clang;\
        sudo apt-get install -y bison;\
        sudo apt-get install -y flex;\
        sudo apt-get install -y libreadline-dev;\
        sudo apt-get install -y gawk;\
        sudo apt-get install -y tcl-dev;\
        sudo apt-get install -y libffi-dev;\
        sudo apt-get install -y mercurial;\
        sudo apt-get install -y graphviz;\
        sudo apt-get install -y xdot;\
        sudo apt-get install -y pkg-config;\
        sudo apt-get install -y python;\
        sudo apt-get install -y python3;\
        sudo apt-get install -y libftdi-dev;\
	)
