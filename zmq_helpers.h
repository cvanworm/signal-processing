#include<string.h>
#include<stdlib.h>
#include<stdio.h>
#include<time.h>
#include<math.h>
#include<signal.h>
#include<unistd.h>
#include<wait.h>
#include<assert.h>

struct workers {
    void *work;
    void *hb;
    char *host;
};

static int addNums(int num1, int num2){ return num1 + num2; }
static int subNums(int num1, int num2){ return num1 - num2; }
static int mulNums(int num1, int num2){ return num1 * num2; }
static int divNums(int num1, int num2){ return num1 / num2; }
static int funtask(int (*func)(),int,int);

int findMax(int *arr, const long length){
    int max = 0;
    for(int i = 0; i<length; i++){
        if(arr[i] > max){
            max = arr[i];
        }
    }

    return max;
}

float findAvg(int *arr, const long length){
    float tmp = 0;
    for(int i=0; i<length; i++){
        tmp += arr[i];
    }
    return tmp/length;
}

//Adds two arrays of same length
int* arrAdd(int* arr, int* arr2, int length){
    for(int i = 0; i<length; i++){
        arr[i] = arr[i]+arr2[i];
    }
    return arr;
}

//Subtracts two arrays of same length
int* arrSub(int* arr, int* arr2, int length){
    for(int i = 0; i<length; i++){
        arr[i] = arr[i]-arr2[i];
    }
    return arr;
}

//Multiplies two arrays of same length
int* arrMul(int* arr, int* arr2, int length){
    for(int i = 0; i<length; i++){
        arr[i] = arr[i]*arr2[i];
    }
    return arr;
}

//Divides two arrays of same length
int* arrDiv(int* arr, int* arr2, int length){
    for(int i = 0; i<length; i++){
        arr[i] = arr[i]/arr2[i];
    }
    return arr;
}

int* arrFunTask(int* (*func)(int*,int*,int), int* c, int* a, int l){
    int* result = func(c,a,l);
    return result;
}

//a task that takes a task id, function as input, as well as two inputs
int funtask(int (*func)(int,int),int a ,int b){
    int result=0;
    result = func(a,b);
    return result;
}//end funtask

int* stringToArr(const char* input, long* length){
    // Find the length of the string
    //printf("input: %s\n",input);
    *length = 0;
    const char* temp = input;
    while (*temp) {
        if (*temp == ',')
            (*length)++;
        temp++;
    }
    (*length)++;

    // Allocate memory for the new integer array
    int* arr = (int*)malloc(*length * sizeof(int));

    // Tokenize the string and convert each token to an integer
    char* token = strtok((char*)input, "[],");
    
    //printf("Token: ");
    int index = 0;
    while (token != NULL && index < *length) {
        //printf("%s,",token);
        arr[index] = atoi(token);
        token = strtok(NULL, "[],");
        index++;
    }

    return arr;
}

char* arrToString(const int* arr, int length) {
    // Allocate memory for the string buffer
    char* str = (char*)malloc((length+1) * sizeof(char));
    int strIndex = 0;

    // Convert each integer element to a character and append it to the string buffer
    for (int i = 0; i < length; i++) {
        char buffer[20];  // Assuming the maximum number of digits in an integer is 20
        if(i == length-1){
            snprintf(buffer, sizeof(buffer), "%d", arr[i]);
        }else{
            snprintf(buffer, sizeof(buffer), "%d,", arr[i]);
        }
        
        int bufferLength = strlen(buffer);
        strncpy(str + strIndex, buffer, bufferLength);
        strIndex += bufferLength;
    }
    str[strIndex] = '\0';  // Add null terminator at the end of the string

    return str;
}

char* openFile(char *fname, long* numbytes){
    FILE    *infile;
    char    *buffer;
    long     tmpbytes;

    //printf("Opening file.\n");

    infile = fopen(fname, "r");

    if(!infile){
        printf("File not opened.\n");
    }

    fseek(infile, 0L, SEEK_END);
    tmpbytes = ftell(infile);

    //printf("Found numbytes.\n");

    fseek(infile, 0L, SEEK_SET);

    buffer = (char*)calloc(tmpbytes, sizeof(char));

    //printf("Created buffer.\n");

    fread(buffer, sizeof(char), tmpbytes, infile);

    //printf("Read file.\n");

    fclose(infile);

    *numbytes = tmpbytes;

    return buffer;
}


void appendStringToFile(const char* filename, const char* string) {
    FILE* file = fopen(filename, "a");
    if (file == NULL) {
        printf("Failed to open file: %s\n", filename);
        return;
    }

    fprintf(file, "%s\n", string);
    fclose(file);
}

char* formatString(char* str, long numbytes){
    char *tmp = (char*)calloc(numbytes+1, sizeof(char));
    int length = strlen(str);
    int i;
    for(i = 0; i<length; i++){
        if(str[i]=='\n'){
            str[i] = ';';
        }
    }
    strcat(tmp, str);
    return tmp;
}

#define MAX_TOKENS 100
#define MAX_TOKEN_LENGTH 1000

char** splitStringOnSpaces(const char* input, int* numTokens) {
    //printf("Input: %s\n", input);
    // Allocate memory for the array of strings
    char** tokens = (char**)malloc(MAX_TOKENS * sizeof(char*));
    if (tokens == NULL) {
        fprintf(stderr, "Failed to allocate memory for tokens.\n");
        exit(1);
    }

    // Copy the input string to a temporary buffer for tokenization
    char buffer[MAX_TOKEN_LENGTH];
    strncpy(buffer, input, sizeof(buffer));

    //printf("Buffer: %s\n", buffer);
    // Tokenize the input string on spaces
    char* token = strtok(buffer, " ");
    *numTokens = 0;

    while (token != NULL && *numTokens < MAX_TOKENS) {
        tokens[*numTokens] = strdup(token);
        (*numTokens)++;
        token = strtok(NULL, " ");
    }

    // Null-terminate the array of strings
    tokens[*numTokens] = NULL;

    return tokens;
}

char** splitStringOnSemiColons(const char* input, int* numTokens) {
    //printf("Input: %s\n", input);
    // Allocate memory for the array of strings
    char** tokens = (char**)malloc(MAX_TOKENS * sizeof(char*));
    if (tokens == NULL) {
        fprintf(stderr, "Failed to allocate memory for tokens.\n");
        exit(1);
    }

    // Copy the input string to a temporary buffer for tokenization
    char buffer[MAX_TOKEN_LENGTH];
    strncpy(buffer, input, sizeof(buffer));

    //printf("Buffer: %s\n", buffer);
    // Tokenize the input string on spaces
    char* token = strtok(buffer, ";");
    *numTokens = 0;

    while (token != NULL && *numTokens < MAX_TOKENS) {
        tokens[*numTokens] = strdup(token);
        (*numTokens)++;
        token = strtok(NULL, ";");
    }

    // Null-terminate the array of strings
    tokens[*numTokens] = NULL;

    return tokens;
}

// int getNumberOfElements(char** array) {
//     int count = 0;
//     while (array[count] != NULL) {
//         count++;
//     }
//     return count;
// }

int getNumberElements(struct workers* array) {
    int count = 0;
    while (array[count].host != NULL) {
        count++;
    }
    return count;
}


typedef int (*function_type)(int,int);
function_type funNameToFun(const char* name){
	if(strcmp(name, "add") ==0){
		return addNums;
	}
	if(strcmp(name, "sub") ==0){
                return subNums;
        }
	if(strcmp(name, "mul") ==0){
                return mulNums;
        }
	if(strcmp(name, "div") ==0){
                return divNums;
        }
	return addNums;

}

typedef int* (*arr_function_type)(int*,int*,int);
arr_function_type arrNameToFun(const char* name){
	if(strcmp(name, "add") ==0){
		return arrAdd;
	}
	if(strcmp(name, "sub") ==0){
                return arrSub;
        }
	if(strcmp(name, "mul") ==0){
                return arrMul;
        }
	if(strcmp(name, "div") ==0){
                return arrDiv;
        }
	return arrAdd;

}

