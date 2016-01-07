#ifndef __CP430_CORE_FW_UPDATE_UPPER_LEVEL_H__
#define __CP430_CORE_FW_UPDATE_UPPER_LEVEL_H__

#define COPY_FUNC_START_LOC		0x9400									// These locations (except ISR location) should be start positions of a segment (LOC%512==0)
#define MAIN_MEM_START_LOC	 	0x9800	
#define ISR_MEM_START_LOC	 	0xFFC0
#define EXT_MEM_START_LOC	 	0x13000

#define INFOD_START_ADDR		0x1000
#define INFOD_END_ADDR			0x1001
#define INFOC_START_ADDR		0x1040
#define INFOB_START_ADDR		0x1080
#define INFOB_END_ADDR			0x10A3

#define ADDR_MAJOR_NUM			0x109F
#define ADDR_MINOR_NUM			0x10A0

#define MAJOR_NUM_LOWEST		4
#define MINOR_NO_MAX			4
#define MAJOR_NUM_BASE_LIM		128

#define ADDR_REST_VEC_LOW		0xFFFE
#define ADDR_REST_VEC_HIGH		0xFFFF

#define NO_OF_WRITE_TRIES		5
#define NO_OF_CODE_PRTS			4
#define PART_COPY_MEM			0
#define PART_MAIN_MEM			1
#define PART_ISR_MEM			2
#define PART_EXT_MEM			3

#define COPY_FUNC_SIZE_MAX		0x0400
#define MAIN_MEM_SIZE_MAX		0x67BE
#define EXT_MEM_SIZE_MAX		0x31FF
#define ISR_MEM_SIZE_MAX	 	0x0040

#define SEGMENT_NO_POS			0
#define SEGMENT_NO_MIN			0
#define SEGMENT_NO_MAX			78

#define CODE_SIZE_MAX			0x9E00
#define ADDR_MAX_LN				5

#define BASE_CODE_RST_START		0x3100
#define BASE_CODE_RST_END		0x93FD

struct cp430_code_metadata{												//Has to support Little Endian format
	unsigned long 		copy_func_start_loc;
	unsigned short  	copy_func_code_size;
	unsigned char 		copy_func_code_checksum;
	unsigned long 		main_mem_start_loc;
	unsigned short  	main_mem_code_size;
	unsigned char 		main_mem_code_checksum;
	unsigned long 		isr_mem_start_loc;
	unsigned short  	isr_mem_code_size;
	unsigned char 		isr_mem_code_checksum;
	unsigned long 		ext_mem_start_loc;
	unsigned short  	ext_mem_code_size;
	unsigned char 		ext_mem_code_checksum;
	unsigned char 		fw_ver_major;
	unsigned char 		fw_ver_minor;
	unsigned short 		reset_address;
};



















#endif
