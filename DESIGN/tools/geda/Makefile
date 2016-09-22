SHELL=/bin/sh
MAKE=make
CUR_DIR=$(shell pwd)
home=$(CUR_DIR)/..



.PHONY	install_geda:
install_geda:  
	cp -r dot_gEDA  ~/.gEDA;\
	sudo apt-get  install -y geda;\
	) 





.PHONY install:
install:
	(\
	sudo apt-get  install -y autoconf;\
	sudo apt-get  install -y libtool;\
	sudo apt-get  install -y autopoint;\
	sudo apt-get  install -y guile-1.8-dev;\
	sudo apt-get  install -y libglib2.0-dev;\
	sudo apt-get  install -y libgtk2.0-dev;\
	)

.PHONY	b_geda:
b_geda:  
	cp -r dot_gEDA  ~/.gEDA;\
	cp geda-gaf-1.9.0.tar.gz ~;\
	cd  ~;\
	tar -xf geda-gaf-1.9.0.tar.gz ;\
	cd  geda-gaf-1.9.0 ;\
	./configure --prefix=/home/johne/geda;\
	make;\
	make install ;\
	) 

