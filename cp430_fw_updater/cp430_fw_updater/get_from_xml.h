#ifndef __GET_FROM_XML_H__
#define __GET_FROM_XML_H__


//Acquire reset address from major and minor numbers by reading the XML file 
unsigned short get_reset_address_from_xml(unsigned char base_code_major, unsigned char base_code_minor, char* XMLFileFD);

#endif
