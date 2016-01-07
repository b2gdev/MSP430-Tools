
.PHONY: build clean

build:
	make -C getMSP430Version/
	make -C getAllFWVersions/
	make -C cp430_fw_updater/cp430_fw_updater/

.PHONY: clean

clean:
	make -C getMSP430Version/ clean
	make -C getAllFWVersions/ clean
	make -C cp430_fw_updater/cp430_fw_updater/ clean


