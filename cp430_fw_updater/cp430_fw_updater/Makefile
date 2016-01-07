APP 	:= cp430_fw_updater
OUTDIR 	:= build

CPP 	:= arm-none-linux-gnueabi-g++
CC 		:= arm-none-linux-gnueabi-gcc

INCLUDE := ./libxml2/include/libxml2
LIB		:= ./libxml2/lib

CFLAGS	:= -Wall -fpic

SRC		:= cp430_fw_updater.c get_from_xml.c
OBJECT	:= cp430_fw_updater.o get_from_xml.o


.PHONY: build clean

build: 
	rm -rf ${OUTDIR}
	mkdir ${OUTDIR}
	$(CC) $(CFLAGS) -I${INCLUDE} -c $(SRC)
	$(CC) -L${LIB} --static -o ${OUTDIR}/$(APP) $(OBJECT) -lxml2 -lm -lc

.PHONY: clean

clean:
	rm -rf ${OUTDIR}
	rm -f *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions

