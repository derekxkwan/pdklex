#Makefile for pdklex

lib.name = pdklex

# headers
dkrnd := src/dkrnd.c
dkmem:= src/dkmem.c
lutil:= src/lutil.c

dklmunge.class.sources = src/dklmunge.c $(dkmem) $(dkrnd) $(lutil)
dklgen.class.sources = src/dklgen.c $(dkmem) $(dkrnd) $(lutil)

datafiles =  pdklex-meta.pd \
	     dklmunge-help.pd \
	     dklgen-help.pd \
	     dklmunge-mungerlist.txt \
			 README.txt \
			 LICENSE.txt

suppress-wunused = yes

include Makefile.pdlibbuilder
