APP 	:= getAllFWVersions
OUTDIR 	:= build

CPP 	:= arm-none-linux-gnueabi-g++
CC 	:= arm-none-linux-gnueabi-gcc

INCLUDE := 

CFLAGS	:= -Wall --static

SRC	:= getAllFWVersions.c


.PHONY: build clean

build: 
	rm -rf ${OUTDIR}
	mkdir ${OUTDIR}
	$(CC) $(CFLAGS) -o ${OUTDIR}/$(APP) $(SRC)

.PHONY: clean

clean:
	rm -rf ${OUTDIR}
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions

