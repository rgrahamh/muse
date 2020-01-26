#include "shared.h"

char* escapeApostrophe(char* str){
	//New string will be, at maximum, the strlen of the old string * 2 +1
	char* new_str = (char*)calloc(1, (strlen(str) * 2) + 1);
	int apost_count = 0;
	int i = 0;
	for(; str[i] != '\0'; i++){
		if(str[i] == '\''){
			new_str[i+(apost_count++)] = '\'';
			new_str[i+apost_count] = '\'';
		}
		else{
			new_str[i+apost_count] = str[i];
		}
	}
	return new_str;
}

//A dummy function
void dummy() {
  return;
}
