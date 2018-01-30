/*
*Charlie Vorbach
*Linear Unit Converter
*Type in unit and it will convert to any other using data from unitdata.data
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define STR_MAX 1024
#define MAX_UNITS 1024
#define BACKWARDS 'b'
#define FILENAME "unitdata.data"
#define PLUS '+'
#define MINUS '-'
#define TIMES '*'

struct Unit {
     float value;
     char* name;
     char* conversion;
     char* type;
} Unit;

int loadUnit(struct Unit* ptr) {
     //set up to read file
     FILE* udata = fopen(FILENAME, "r");
     char line[STR_MAX];   
     //set up to process data
     int nlength = sizeof(char) * strlen(ptr->name);
     ptr->type = malloc(sizeof(char) * STR_MAX);
     ptr->conversion = malloc(sizeof(char) * STR_MAX);;
     
     //While EOF isn't reached read file
     if (udata != NULL){
          while (fgets(line, STR_MAX, udata)) {
               if(line[0] == '#') {
               
               }               
               else if (strncmp(ptr->name, line, nlength) == 0){     
                    int i = 0;
                    int j = 0;
                    //read name
                    while(!isspace(line[i])) {
                         i++;
                    }
                    i++;
                    //read conversion
                    while(!isspace(line[i])) {
                         ptr->conversion[j] = line[i];
                         j++;
                         i++;
                    }
                    j = 0;
                    i++;
                    //read first type
                    while(!isspace(line[i])) {
                         ptr->type[j] = line[i];
                         j++;
                         i++;
                    }
                    return 0;
               }
          }
          //error unit not found
         	printf("Unit not found. Try adding it to unitdata.data.\n");
         	return 1;
     }
     else {
     	//error opening file
     	printf("Couldn't open unitdata.data.\n");
     	return 2;
     }
}

int getType(char* kind, struct Unit* list) {
     //prep file
     FILE* unitdata = fopen(FILENAME, "r");
     char line[STR_MAX];
     //prep to read
     int klength = strlen(kind);
     int num = 0;
     
     //Read line
     if(unitdata != NULL) {

     	while(fgets(line, STR_MAX, unitdata)) {
     		if (line[0] != '#') {
     			//if type is same as input 
				int llength = strlen(line);
				int j = 0;
				for (int i = 0; i < llength; i++){
					if (line[i] == kind[j] || j == klength) {
						if(j == klength) {
							//go to begining of line and prep to store
							list[num].name = malloc(sizeof(char) * STR_MAX);
							list[num].conversion = malloc(sizeof(char) * STR_MAX);
							int k = 0;
							int l = 0;
							//get name, remember malloc
							while(!isspace(line[k])) {
								list[num].name[k] = line[k];
								k++;
							}
							k++;
							//get conversion
							while(!isspace(line[k])) {
								list[num].conversion[l] = line[k];
								k++;
								l++;
							}						
							//interate num
							num++;
							j = 0;
						} 
						else {
							j++;
						}
					}
					else {
						j = 0;
					}
				}
     		}
     	}
     	
     	return num;
     }
}

//convert inputed unit to standard
int parse(struct Unit* ptr, char* equ, float amount, char mode) {
     int length = strlen(equ);
     int i = 0;  

     //get name of standard
     while(isalpha(equ[i])) {
          i++;
     }
     
     //get symbol and perform operation
     char* sk = malloc(sizeof(char) * length);
     int j = 0;
     int sign = 1;
     float k;
     if (equ[i] == TIMES) {
     	i++;
     	//slope
     	if (equ[i] == MINUS) {
     		sign = -1;
     		i++;
     	}
     	while(isdigit(equ[i]) || equ[i] == '.') {
     		sk[j] = equ[i];
     		j++;
     		i++;
     	}
     	sk[j] = '\0';
     	k = atof(sk);
     	k = sign*k;
     	
     	//y-intercept
     	char* sc = malloc(sizeof(char) * length);
     	float c;
     	j = 0;
     	
     	//get sing
     	if (equ[i] == MINUS) {
     		sign = -1;
     		i++;
     	}
     	else {
     		sign = 1;
     		i++;
     	}
     	
     	//get value
     	while(isdigit(equ[i]) || equ[i] == '.') {
     		sc[j] = equ[i];
     		i++;
     		j++;
     	}
     	sc[j] = '\0';
     	c = atof(sc);
     	c = sign *c;
     	
     	if (mode == BACKWARDS) {
     		ptr->value = (amount - c) / k;
     	}
     	else {
     		ptr->value = k * amount + c;
     	}
     	free(sk);
     	free(sc);
     	return 0;
     }
     else {
     	printf("Error in reading unit");
     	return 1;
     }
}

//convert from stadard to other units and print out
int convert(struct Unit* list, int num) {
	for (int i = 0; i < num; i++) {
	     parse(&list[i], list[i].conversion, list[0].value, 'f');
	     printf("%f %s\n", list[i].value, list[i].name);
	}
		return 0;
}

//Free memory
int cleanup (struct Unit* in_ptr, struct Unit* out_ptr, int num) {
	//free input
	free(in_ptr->conversion);
	free(in_ptr->type);
	
	//free output array
	for (int i = 0; i <= num; i++) {
		free(out_ptr[i].name);
		free(out_ptr[i].conversion);
		free(out_ptr[i].type);
	}
	
	return 0;
}

//Get args
int main(int argc, char** argv) {
     //check input
     if (argc != 3 && argc != 4) {
          printf("Syntax: name value unit [optional: unit to output]\n");
          return -1;
     }
     for (int i = 0, len = strlen(argv[1]); i < len; i++) {
     	if (!isdigit(argv[1][i]) && argv[1][i] != '.') {
     		printf("Invalid Input Value\n");
     		return -2;
     	}
     }
     
     //store input
     struct Unit input;
     input.value = atof(argv[1]);
     input.name = argv[2];
     
     //look up unit
     if (loadUnit(&input) != 0 ) {
          return -6;
     }
     //if converting to specific unit
     if (argc == 4) {
     	struct Unit speci;
     	struct Unit stand;
     	speci.name = argv[3];
     	//load unit
     	if (loadUnit(&speci) !=0) {
     		return -7;
     	}
     	//check units are compatible
     	if (strcmp(speci.type, input.type) != 0) {
     		printf("Units measure different quanities. Can't convert.\n");
     		return -13;
     	}
     	//parse input backwards
     	parse(&stand, input.conversion, input.value, BACKWARDS);
     	
     	//parse output
     	parse(&speci, speci.conversion, stand.value, 'f');
     	printf("%f %s\n", speci.value, speci.name);
     	
     	//clean up memory
     	free(input.type);
     	free(input.conversion);
     	free(speci.type);
     	free(speci.conversion);
     	//exit successfully
     	return 0;
     }
     //else get all units of that type
     struct Unit output[MAX_UNITS];
     int unitnumber = getType(input.type, output);
     if (unitnumber < 0) {
     	return -2;
     }
     
     //parse unit conversion
     parse(&output[0], input.conversion, input.value, BACKWARDS);
     
     //convert from standard to all other units of type and print out
     convert(output, unitnumber);
     
     //cleanup memory
     cleanup(&input, output, unitnumber);
     return 0;
}

