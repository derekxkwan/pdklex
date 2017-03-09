#Makefile for pdklex

lib.name = pdklex

# headers
dkrnd := src/dkrnd.c
dkmem:= src/dkmem.c

dklmunge.class.sources = src/dklmunge.c $(dkmem) $(dkrnd)

datafiles =  pdklex-meta.pd \
	     dklmunge-help.pd \
	     dklmunge-mungerlist.txt \
			 README.txt \
			 LICENSE.txt

suppress-wunused = yes

include Makefile.pdlibbuilder
