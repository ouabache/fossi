This directory contains symbol libs for gschem drawings.

type:


make install_geda


to install geda ,socgen libs and config scripts

edit scripts in ~/.gEDA to customize gschem's operations

Try this:
gschem -p -o OUTPUTFILE -s SCHEMEFILE INPUTFILE

A good starting point for SCHEMEFILE is /usr/share/gEDA/scheme/image.scm.  You can change it to suit your needs.
