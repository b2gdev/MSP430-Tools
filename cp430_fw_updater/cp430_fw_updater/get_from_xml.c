#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libxml/tree.h>
#include <libxml/parser.h>

#include "get_from_xml.h"

#define BASE_DECIMAL	10
#define MAX_LN			3
#define MAX_LN_RST_ADDR	5
#define NUMBER_START	'0'

char arrayMajor[MAX_LN+1];
char arrayMinor[MAX_LN+1];

char* convert_to_string(unsigned char number, char array[]){
	/* Convert major and minor numbers to string */
	int i;
	
	array[MAX_LN] = '\0';
	
	for(i= MAX_LN-1; i>=0; i--){
		array[i]= (number % BASE_DECIMAL) + NUMBER_START;
		number /= BASE_DECIMAL;
	}
	
	for(i=0; i<MAX_LN; i++){
		if(array[i]!=NUMBER_START){
			return &array[i];
		}
	}
	
	return &array[MAX_LN-1];
}

unsigned short convert_to_number(char array[]){
	int i, len=strlen(array);
	unsigned short temp=0;
	
	/* Convert the reset address string to number */
	for(i=0; i<len; i++){
		temp *= BASE_DECIMAL;
		temp += (array[i] - NUMBER_START);
	}
	
	return temp;
}

xmlNodePtr find_major_node(xmlNodePtr root_node,const char* major_num){
	xmlNodePtr major_node = root_node->xmlChildrenNode;
	xmlChar *key;

	/* Read the children of the root node and get the matching major number */ 
	while (major_node != NULL) {
		if(major_node->type == XML_ELEMENT_NODE) {
			key = xmlGetProp(major_node, (const xmlChar *)"major_num");
			if ((!xmlStrcmp(key, (const xmlChar *) major_num))){
				return major_node;
			}
		}
		major_node = major_node->next;
	}
	return NULL;
}

unsigned short find_reset_addr(xmlNodePtr major_node,const char* minor_num){
	xmlNodePtr minor_node = major_node->xmlChildrenNode;
	xmlChar *key;
	
	/* Read the children of the major node and get the matching minor number and return the reset address*/ 
	while (minor_node != NULL) {
		if(minor_node->type == XML_ELEMENT_NODE) {
			key = xmlGetProp(minor_node, (const xmlChar *)"minor_num");
			if ((!xmlStrcmp(key, (const xmlChar *) minor_num))){
				key = xmlGetProp(minor_node, (const xmlChar *)"reset_addr");
				return convert_to_number((char *) key);
			}
		}
		minor_node = minor_node->next;
	}
	
	return 0;
}


unsigned short get_reset_address_from_xml(unsigned char base_code_major, unsigned char base_code_minor, char* XMLFileFD){
	xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;
    xmlNode *major_element = NULL;
    const char* major_no = convert_to_string(base_code_major, arrayMajor);
    const char* minor_no = convert_to_string(base_code_minor, arrayMinor);
    unsigned short temp;

	/* Init libxml */
	xmlInitParser();
	LIBXML_TEST_VERSION

	/* Load XML document */
    doc = xmlParseFile(XMLFileFD);
    if (doc == NULL) {
		xmlCleanupParser();
		printf("Unable to parse XML file \"%s\"\n", XMLFileFD);
		return 0;
    }
    
    /* Get the root element node */
    root_element = xmlDocGetRootElement(doc);
    if (root_element == NULL) {
		xmlFreeDoc(doc);
		xmlCleanupParser();
		printf("Unable to read root element\n");
		return 0;
    }
    
    /* Get the major element node */
    major_element = find_major_node(root_element, major_no);
    if (major_element == NULL) {
		xmlFreeDoc(doc);
		xmlCleanupParser();
		printf("Unable to find major no %d\n", base_code_major);
		return 0;
    }
    
    /* Get reset address */
    temp = find_reset_addr(major_element, minor_no);
    if(!temp){
		printf("Unable to find reset address for v%d:%d from list\n", base_code_major,base_code_minor);
	}
    
    
    xmlFreeDoc(doc);
	xmlCleanupParser();

	return temp;
}
