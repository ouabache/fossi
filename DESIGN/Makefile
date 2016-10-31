SHELL=/bin/sh
MAKE=make
CUR_DIR=$(shell pwd)
home=$(CUR_DIR)
design=socgen
project=fpgas
vendor=opencores.org




.PHONY check_sims:
check_sims:   
	@for COMP in `ls $(CUR_DIR)/work`; do \
	echo "****************************************";\
	echo " number of $$COMP sims run";\
	find ./work/$$COMP  | grep test_define | grep -v target | grep -v children| grep -v cov | wc -l;\
	echo " number of sims that finished";\
	find ./work/$$COMP  | grep _sim.log | xargs grep PASSED $1    | wc -l    ;\
	echo " number of warnings";\
	find ./work/$$COMP  | grep _sim.log | xargs grep WARNING $1   | wc -l ;\
	echo " number of errors";\
	find ./work/$$COMP  | grep _sim.log | xargs grep ERROR $1     | wc -l ;\
	echo " Elaboration Errors";\
	find ./work/$$COMP  | grep _elab.log | xargs cat $1  ;\
	echo " Code Coverage";\
	echo " number of warnings";\
	find ./work/$$COMP  | grep _cov.log | xargs grep WARNING $1  ;\
	echo " number of errors";\
	find ./work/$$COMP  | grep _cov.log | xargs grep ERROR $1    ;\
	echo " Lint Coverage";\
	echo " number of errors";\
	find ./work/$$COMP  | grep lint.log | xargs grep Error $1  ;\
	done;\




.PHONY check_fpgas:
check_fpgas: 
	(\
	cd ${home}/work  ;\
	echo " number of xilinx fpgas";\
	find . | grep syn/ise |grep Yst |  wc -l   ;\
	echo " number that finished";\
	find . | grep Board_Design_jtag.bit | wc -l ;\
	echo " number of lattice fpgas";\
	find . | grep syn/ara |grep Bits.bin |  wc -l   ;\
	echo " number that finished";\
	find . | grep Bits.bin | wc -l ;\
	)

