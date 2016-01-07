/*!
*********************************************************************************************************
*
*                          	   		Test Application for cp430_fw_update
*                                       (c) Copyright Zone24x7 Inc.
*
*
*********************************************************************************************************
*/

/*!
*********************************************************************************************************
*
* Filename      : cp430_fw_update_test.c
* Version       : 1.0.0
* Programmer(s) : AH
*
*********************************************************************************************************
* Note(s) : (1) Device Node - /dev/cp430_core
*
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <linux/ioctl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "cp430_ioctl.h"
#include "cp430_core_fw_update.h"
#include "cp430_core_fw_update_upper_level.h"
#include "get_from_xml.h"

#define CP_FW_UPDR_VER	1

#define WORD_SIZE		10
#define BASE_HEX		16
#define BASE_DECIMAL	10


#define TERMINATE_CHAR	'q'
#define ADDRESS_CHAR	'@'

#define SIMPLE_START	'a'
#define SIMPLE_END		'f'
#define CAPITAL_START	'A'
#define CAPITAL_END		'F'
#define NUMBER_START	'0'
#define NUMBER_END		'9'
#define INVALID_CHAR	0xFF
																		// Starting and ending addresses of the code parts. These arrays should be in the ascending order of the address values
																		// -> Copying Code, Main Mem, ISR Mem, Ext Mem
unsigned long start_locs [] = {(unsigned long) COPY_FUNC_START_LOC, (unsigned long) MAIN_MEM_START_LOC, (unsigned long) ISR_MEM_START_LOC, (unsigned long) EXT_MEM_START_LOC};
unsigned long end_locs [] 	= {((unsigned long) COPY_FUNC_START_LOC) + ((unsigned long) COPY_FUNC_SIZE_MAX) -1, ((unsigned long) MAIN_MEM_START_LOC) + ((unsigned long) MAIN_MEM_SIZE_MAX) -1, ((unsigned long) ISR_MEM_START_LOC) + ((unsigned long) ISR_MEM_SIZE_MAX) -1, ((unsigned long) EXT_MEM_START_LOC) + ((unsigned long) EXT_MEM_SIZE_MAX) -1};


unsigned char getCharacter(char character){								// Converts One ASCII character to Number
	if ((character >= SIMPLE_START) && (character <= SIMPLE_END)){
		return (character - SIMPLE_START + BASE_DECIMAL);
	}else if ((character >= CAPITAL_START) && (character <= CAPITAL_END)){
		return (character - CAPITAL_START + BASE_DECIMAL);
	}else if ((character >= NUMBER_START) && (character <= NUMBER_END)){
		return (character - NUMBER_START);
	}else{
		return INVALID_CHAR;
	}
}

unsigned long getAddress(char addrArray[]){								// Converts the string that contains an address to Numerical address
	unsigned long address=0;
	int i=0;
	unsigned char temp=0;
	
	for(i=0; (i < ADDR_MAX_LN); i++){									// String max length =  ADDR_MAX_LN
		temp = getCharacter(addrArray[i]);
		if(temp != INVALID_CHAR){
			address *= (unsigned long) BASE_HEX;
			address += (unsigned long) temp;
		}else{
			break;
		}
	}
	
	return address;														// Returns '0' if unsuccessful
}

char validateAddress(unsigned long address, unsigned long *current_pos, unsigned char *current_part, unsigned long *current_index, struct cp430_code_metadata *meta_data){
	char valid_addr = 0, starting_location = 0;
	unsigned int i=0;
	
	
	if(!address){
		printf("Unable to read address\n");
		return 0;
	}
	
	for(i=0; i<NO_OF_CODE_PRTS; i++){									// Check weather the address is within range
		if((address >= start_locs[i]) && (address <= end_locs[i])){
			valid_addr = 1;
			break;
		}
	}
	
	if(!valid_addr){
		printf("Outside the address space %lu \n", address);
		return 0;
	}
	
	if(*current_part != i){												// Update starting addresses in the meta data structure & current position when jumping into a new part
		starting_location = 1;
		switch (i) 
		{
			case PART_COPY_MEM:
				meta_data->copy_func_start_loc = address;
				*current_pos = address;
				break;
			case PART_MAIN_MEM:
				meta_data->main_mem_start_loc = address;
				*current_pos = address;
				break;
			case PART_ISR_MEM:
				meta_data->isr_mem_start_loc = address;
				*current_pos = address;
				break;
			case PART_EXT_MEM:
				meta_data->ext_mem_start_loc = address;
				*current_pos = address;
				break;
			default:
				break;
		}
		*current_part = i;
	}
	
	
	if((address > *current_pos) && (!starting_location)){				// Keep 0xFF if there are missing values inside code parts (Not in between code parts). Increase current_index according to the number of missing values
		*current_index += (address - *current_pos);						// Update code size in the meta data structure & current position accordingly
		
		switch (*current_part) 
		{
			case PART_COPY_MEM:
				meta_data->copy_func_code_size += (address - *current_pos);
				break;
			case PART_MAIN_MEM:
				meta_data->main_mem_code_size += (address - *current_pos);
				break;
			case PART_ISR_MEM:
				meta_data->isr_mem_code_size += (address - *current_pos);
				break;
			case PART_EXT_MEM:
				meta_data->ext_mem_code_size += (address - *current_pos);
				break;
			default:
				break;
		}
		
		*current_pos = address;
	}
	
	return 1;
}

char getValue(char valueArray[],unsigned char* value){					// Reads strings that contains values and convert them to numbers
	int i=0;
	unsigned char temp=0;
	
	*value = 0;
	for(i=0; (i < 2); i++){
		temp = getCharacter(valueArray[i]);
		if(temp != INVALID_CHAR){
			(*value) *= (unsigned long) BASE_HEX;
			(*value) += (unsigned long) temp;
		}else{
			return 0;													// Return '0' if unsucessful
		}
	}
	
	return 1;
}

char validateInfoPosition(unsigned char info_seg_val, unsigned long *info_seg_pos, char * reading_info_seg, struct cp430_code_metadata *meta_data){
	char valid_addr=0;
	
	if(!(*info_seg_pos)){
		printf("Unable to read address\n");
		return 0;
	}
																		// Values in info segs are within range
	if((((*info_seg_pos) >= INFOB_START_ADDR) && ((*info_seg_pos) <= INFOB_END_ADDR)) || ((*info_seg_pos) == INFOC_START_ADDR) || (((*info_seg_pos) >= INFOD_START_ADDR) && ((*info_seg_pos) <= INFOD_END_ADDR))){
		valid_addr =1;
	}
	
	if(!valid_addr){
		printf("Outside the info seg address space %lu \n", (*info_seg_pos));
		return 0;
	}
	
	switch (*info_seg_pos)												// Get required values from info segs
	{
		case ADDR_MAJOR_NUM:
			meta_data->fw_ver_major = info_seg_val;
			break;
		case ADDR_MINOR_NUM:
			meta_data->fw_ver_minor = info_seg_val;
			break;
		case INFOB_END_ADDR:
			*reading_info_seg = 0;
			break;
		default:
			break;
	}
	
	(*info_seg_pos)++;
		
	return 1;
}

char validatePosition(unsigned long current_pos, unsigned char *current_part, struct cp430_code_metadata *meta_data){
	unsigned int i=0;
	char valid_addr=0;
	
	if(current_pos > end_locs[*current_part]){							// When jumping into a new part
		for(i=0; i<NO_OF_CODE_PRTS; i++){								// Check weather space is enough to accommodate code
			if((current_pos >= start_locs[i]) && (current_pos <= end_locs[i])){
				valid_addr = 1;
				break;
			}
		}
		
		if(!valid_addr){
			printf("Space not enough %lu \n", current_pos);
			return 0;
		}
		
		switch (i) 														// Update starting addresses in the meta data structure
		{
			case PART_COPY_MEM:
				meta_data->copy_func_start_loc = current_pos;
				break;
			case PART_MAIN_MEM:
				meta_data->main_mem_start_loc = current_pos;
				break;
			case PART_ISR_MEM:
				meta_data->isr_mem_start_loc = current_pos;
				break;
			case PART_EXT_MEM:
				meta_data->ext_mem_start_loc = current_pos;
				break;
			default:
				break;
		}
			
		*current_part = i;

	}
	
	switch (*current_part) 												// Update code size when a new value is read
	{
		case PART_COPY_MEM:
			meta_data->copy_func_code_size++;
			break;
		case PART_MAIN_MEM:
			meta_data->main_mem_code_size++;
			break;
		case PART_ISR_MEM:
			meta_data->isr_mem_code_size++;
			break;
		case PART_EXT_MEM:
			meta_data->ext_mem_code_size++;
			break;
		default:
			break;
	}
		
	return 1;
}

																		// Acquire reset address from code & replace it with base code address
void getSetResetAddrs(unsigned char *code_val, unsigned long current_pos, unsigned char current_part, struct cp430_code_metadata *meta_data, unsigned short base_code_reset_address){
	if(current_part == PART_ISR_MEM){
		if(current_pos == ADDR_REST_VEC_HIGH){							// High byte
			meta_data->reset_address |= ((*code_val) << 8);
			if (base_code_reset_address){
				(*code_val) = (base_code_reset_address >> 8);
			}
		}else if(current_pos == ADDR_REST_VEC_LOW){						// Low byte
			meta_data->reset_address |= (*code_val);
			if (base_code_reset_address){
				(*code_val) = base_code_reset_address;
			}
		}
	}
}

																		// Generates checksums and update meta data structure - Do after reading the whole file 
void getChecksums(unsigned char code[], struct cp430_code_metadata *meta_data){
	unsigned long lengths [NO_OF_CODE_PRTS];
	unsigned char chksms [NO_OF_CODE_PRTS];
	unsigned char checksum;
	
	int i,j;
	unsigned long k;
	
	lengths[PART_COPY_MEM] = meta_data->copy_func_code_size;
	lengths[PART_MAIN_MEM] = meta_data->main_mem_code_size;
	lengths[PART_ISR_MEM] = meta_data->isr_mem_code_size;
	lengths[PART_EXT_MEM] = meta_data->ext_mem_code_size;
	
	k=0;
	for (i = 0; i < NO_OF_CODE_PRTS; i++)
	{
		checksum = 0;
		for (j = 0; j < lengths[i]; j++)
		{
		  checksum += code[k];
		  k++;
		}
		chksms[i] = checksum;
	}
	
	meta_data->copy_func_code_checksum = chksms[PART_COPY_MEM];
	meta_data->main_mem_code_checksum = chksms[PART_MAIN_MEM];
	meta_data->isr_mem_code_checksum = chksms[PART_ISR_MEM];
	meta_data->ext_mem_code_checksum = chksms[PART_EXT_MEM];
}

																		// Do after reading the whole file
char validateMetadata(struct cp430_code_metadata *meta_data){
																		// Copy mem should start from COPY_FUNC_START_LOC 
	if((meta_data->copy_func_start_loc != ((unsigned long) COPY_FUNC_START_LOC))){
		printf("Copy mem should start from %lu not %lu\n", ((unsigned long) COPY_FUNC_START_LOC), (meta_data->copy_func_start_loc));
		return 0;
	}
	
	if((meta_data->main_mem_start_loc % MSP430_SEG_SZ)){				// Verifies weather Main Mem, Ext Mem starts from a segment starting address
		printf("Main mem should start from a segment starting position - %lu\n", (meta_data->main_mem_start_loc));
		return 0;
	}
	
	if((meta_data->ext_mem_start_loc % MSP430_SEG_SZ)){
		printf(" Ext mem should start from a segment starting position - %lu\n", (meta_data->ext_mem_start_loc));
		return 0;
	}
	
																		//Major number within range
	if(((meta_data->fw_ver_major) < MAJOR_NUM_LOWEST) || ((meta_data->fw_ver_major) >= MAJOR_NUM_BASE_LIM)){
		printf("Invalid Major Number - %d\n", (meta_data->fw_ver_major));
		return 0;
	}
	
	if((meta_data->fw_ver_minor) > MINOR_NO_MAX){						// Minor number within range
		printf("Invalid Minor Number - %d\n", (meta_data->fw_ver_minor));
		return 0;
	}
	
																		//Reset Address out of range
	if(!(((meta_data->reset_address) >= start_locs[PART_MAIN_MEM]) && ((meta_data->reset_address) <= end_locs[PART_MAIN_MEM]))){
		printf("Reset Address out of range - 0x%.4X\n", (meta_data->reset_address));
		return 0;
	}
	
	return 1;
}

																		// Breaks the code into MSP430_SEG_SZ size segments and transport through SPI - Do after reading the whole file 
char sendSegData(int fd, unsigned long code_ln, unsigned char code[]){
	int i=0, j=0, ret=0, tried=1;
	unsigned long k=0;
	unsigned char seg_data[SEGMENT_DATA_SIZE];
	
	for(i=SEGMENT_NO_MIN; i <= SEGMENT_NO_MAX; ){
		memset(seg_data, 0xFF, sizeof(seg_data));
		seg_data[SEGMENT_NO_POS] = i;
		
		for(j=SEGMENT_NO_POS+1; j<SEGMENT_DATA_SIZE; j++){
			seg_data[j] = code[k];
			k++;
			
			if(k >= code_ln){
				break;
			}
		}
		
		ret = ioctl(fd, CP430_CORE_SEND_SEGMENT, seg_data);
		if(ret >= 0){
			printf("Segment %d sent\r\n",seg_data[0]);
			i++;
			tried=1;
			
			if(k >= code_ln){
				break;
			}
		}
		else if(tried < NO_OF_WRITE_TRIES){								// Retry max of NO_OF_WRITE_TRIES times - Might not transfer successfully at the first attempt
			printf("CP430_CORE_SEND_SEGMENT (no - %d) failed : %d\r\n", seg_data[0], ret);
			k -= MSP430_SEG_SZ;
			tried++;
		}else{
			printf("SPI SEGDATA Write Failed\n");
			return 0;
		}
	}
	
	return 1;
}

																		// Sends meta data structure 
char sendMetadata(int fd, struct cp430_code_metadata *meta_data){
	int ret=0, tried=1;
	
	for(tried=1; tried <= NO_OF_WRITE_TRIES; tried++){
		ret = ioctl(fd, CP430_CORE_SEND_METADATA, meta_data);
		if(ret < 0) {
			printf("CP430_CORE_SEND_METADATA failed - %d \r\n", ret);
		}
		else {
			printf("METADATA sent\r\n");
			break;
		}
	}
	
	if(tried > NO_OF_WRITE_TRIES) {
		printf("SPI METADATA Write Failed\n");
		return 0;
	}
	
	return 1;
}

unsigned short getResetAddressFromBaseCodeVersion (int fd, char* XMLFIleFD){
	unsigned char base_code_major, base_code_minor;
	unsigned short reset_address;
	char buffer[WORD_SIZE];
	int ret = 0;
	
	memset(buffer, 0, sizeof(buffer));
	ret = ioctl(fd, CP430_CORE_GET_STATUS, buffer);						// Check weather OTA is suopprted
	if(ret < 0) {
		printf("CP430_CORE_GET_STATUS ioctl failed : %d\r\n", ret);
		return 0;
	}else if(buffer[0] < MAJOR_NUM_LOWEST){
		printf("Firmware versions below v%d are not OTA capable\r\n", MAJOR_NUM_LOWEST);
		return 0;
	}
	
	memset(buffer, 0, sizeof(buffer));
	ret = ioctl(fd, CP430_CORE_GET_ALL_FW_VERSIONS, buffer);			// Issue CP430_CORE_GET_ALL_FW_VERSIONS ioctl to read base code version
	if(ret < 0) {
		printf("CP430_CORE_GET_ALL_FW_VERSIONS ioctl failed : %d\r\n", ret);
		return 0;
	}
	
	if(buffer[0] >= MAJOR_NUM_BASE_LIM){								// Get base code version
		base_code_major = buffer[0];
		base_code_minor = buffer[1];
	}else if(buffer[2] >= MAJOR_NUM_BASE_LIM){
		base_code_major = buffer[2];
		base_code_minor = buffer[3];
	}else{
		printf("Base code not idetified - Available major numbers - %d , %d\n", buffer[0],buffer[2]);
		return 0;
	}
	
	reset_address = get_reset_address_from_xml(base_code_major,base_code_minor, XMLFIleFD);
	
																		// Base code reset address within limits
	if((reset_address<BASE_CODE_RST_START) || (reset_address>BASE_CODE_RST_END)){
		printf("Acquired reset address out of range - 0x%.4X \n", reset_address);
		return 0;
	}
	
	printf("Idetified Base Code - v%d.%d - Reset Address - 0x%.4X \n", base_code_major, base_code_minor, reset_address);
	return reset_address;
}

char verifyArgs(int argc, char *argv[]){
	char strCPMEM[7];
	char* extCPMEM;
	char strXML[7];
	char* extXML;
	
	strcpy(strCPMEM, ".cpmem");
	strcpy(strXML, ".xml");
	
	if( argc != 3) {
		printf("Argument order should be - First arg :- code file path, Second arg :- xml file path\n");
		return 0;
    }
    
    extCPMEM = strstr(argv[1],strCPMEM);
    if(extCPMEM == NULL){
		printf("First arg should be code file path with .cpmem extension\n");
		return 0;
	}
	
	extXML = strstr(argv[2],strXML);
    if(extXML == NULL){
		printf("Second arg should be XML file path with .xml extension\n");
		return 0;
	}
	
	return 1;
}

void writeToFile(unsigned char code[],unsigned long current_index){		// Debugging function
	FILE* outfilefd = NULL;
	unsigned long i,j;
	
	outfilefd = fopen("codeRes.txt","w");
	
	if(NULL == outfilefd)
    {
        printf("OUT File open error\n");
        return;
    }
	
	for(i=0; i<current_index; ){
		for (j=0; j<16; j++){
			fprintf(outfilefd, "%02X ", code[i]);
			i++;
		}
		printf("\n");
	}
	
	fclose(outfilefd);
}


int main(int argc, char *argv[])
{
	int fd     = 0;
	FILE* hexfilefd = NULL;
	char buffer[WORD_SIZE], read_issue=0;
	unsigned char code[CODE_SIZE_MAX];
	struct cp430_code_metadata meta_data;
	unsigned short base_code_reset_address = 0;
	
	unsigned long current_pos=0, current_index=0, info_seg_pos=0;
	unsigned char current_part=-1;
	
	unsigned long temp_ulong;
	
	char reading_info_seg = 1;
	unsigned char info_seg_val = 0;
	
	printf("CP430 firmware updater version : v%d\n\n", CP_FW_UPDR_VER);
	
	if(!(verifyArgs(argc, argv))){
		return -1;
	}
	
	fd = open("/dev/cp430_core", O_RDWR);

	if (fd < 0)
	{
		printf("Could not open device - /dev/cp430_core \r\n");
		return -1;
	}
	
	base_code_reset_address = getResetAddressFromBaseCodeVersion(fd, argv[2]);
	if(!base_code_reset_address){
		return -1;
	}
	
	hexfilefd = fopen(argv[1],"r");

    if(NULL == hexfilefd)
    {
        printf("HEX file %s open error\n", argv[1]);
        close(fd);
        return -1;
    }
    
    memset(code, 0xFF, sizeof(code));									// Fill the code with 0xFF so that the missing values will not be changed in the flash
    memset(( void *) (&meta_data), 0, sizeof(meta_data));
	
	while(1){
		memset(buffer, 0, sizeof(buffer));
		if(!(fscanf(hexfilefd, "%s", buffer))){
			printf("Termination character not met\n");
			read_issue=1;
			break;
		}

		if(reading_info_seg){											// Read values in the info segments
			if(buffer[0] == ADDRESS_CHAR){
				info_seg_pos = getAddress(&buffer[1]);
			}else if(buffer[0] == TERMINATE_CHAR){
				read_issue=1;
				break;
			}else{
				if(getValue(buffer,&info_seg_val)){
					if(!(validateInfoPosition(info_seg_val, &info_seg_pos, &reading_info_seg, &meta_data))){
						read_issue=1;
						break;
					}
				}
			}
		}else{
			if(buffer[0] == ADDRESS_CHAR){
				temp_ulong = getAddress(&buffer[1]);
				if (!(validateAddress(temp_ulong, &current_pos, &current_part, &current_index, &meta_data))){
					read_issue=1;
					break;
				}
			}else if(buffer[0] == TERMINATE_CHAR){
				break;
			}else{
				if(getValue(buffer,&code[current_index])){				// Read value and place it in the code array
					getSetResetAddrs(&code[current_index], current_pos, current_part, &meta_data, base_code_reset_address);
					current_index++;									// If the value is readable increase the index
					if(validatePosition(current_pos, &current_part, &meta_data)){
						current_pos++;									// If the read value fits to a valid position increase the current position		
					}else{
						read_issue=1;
						break;
					}
				}
			}
		}
	}
	
	if(read_issue){
		close(fd);
		fclose(hexfilefd);
		return -1;
	}
	
	if(!(validateMetadata(&meta_data))){
		close(fd);
		fclose(hexfilefd);
		return -1;
	}
	
	getChecksums(code, &meta_data);
	
	if(!(sendSegData(fd, current_index,code))){
		close(fd);
		fclose(hexfilefd);
		return -1;
	}
	
	printf("Copy Func - %lu, %d, %d\n", meta_data.copy_func_start_loc, meta_data.copy_func_code_size, meta_data.copy_func_code_checksum);
	printf("Main Mem  - %lu, %d, %d\n", meta_data.main_mem_start_loc, meta_data.main_mem_code_size, meta_data.main_mem_code_checksum);
	printf(" ISR Mem  - %lu, %d, %d\n", meta_data.isr_mem_start_loc, meta_data.isr_mem_code_size, meta_data.isr_mem_code_checksum);
	printf(" Ext Mem  - %lu, %d, %d\n", meta_data.ext_mem_start_loc, meta_data.ext_mem_code_size, meta_data.ext_mem_code_checksum);
	
	if(!(sendMetadata(fd, &meta_data))){
		close(fd);
		fclose(hexfilefd);
		return -1;
	}
	
	//writeToFile(code, current_index);
	
	close(fd);
	fclose(hexfilefd);
	return 0;
}
