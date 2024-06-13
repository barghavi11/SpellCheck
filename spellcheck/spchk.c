#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include "arraylist.h"

void findFiles(char* name);
void openFile(char *file);
void read_words(int fd, int isDictionary, char *file);
int binarySearch(char *word);
char *removePunctuation(char *word);
int checkWord(char *word, char *file, int line, int column);
int linearsearch(char *word);


arraylist_t *dict_words;
int countStart;

#ifndef DEBUG
#define DEBUG 0
#endif

#ifndef TEST
#define TEST 0
#endif


#ifndef BUFLENGTH
#define BUFLENGTH 10
#endif


int main(int argc, char** argv){
    char *dict_file = argv[1];
    int fd = open(dict_file, O_RDONLY);
    if (DEBUG) printf("%i\n", fd);

    // initialize arraylist
    dict_words = malloc(sizeof(arraylist_t));
    al_init(dict_words, 10);

    if (TEST) printf("dict_words arraylist has a length of %d and a capacity %d\n",
     dict_words->length, dict_words->capacity);

    read_words(fd, 1, dict_file); // read the words of dictionary file and add to arraylist
   
    
    // for each file or directory, call findFiles() to find txt files in directory or to check 
    // if the file is a .txt file
    for (int i = 2; i < argc; i++){
        if (DEBUG) printf("here\n");
        // check if the name of the given argument is a txt file itself
        char* ptr = argv[i];
        if (DEBUG) printf("ptr holds %s\n", ptr);
        // check if the string ends in ".txt"
        if((strlen(ptr) >= 4) && (strcmp((ptr+strlen(ptr) - 4), ".txt") == 0)){
            if (DEBUG) printf("file to be opened is %s\n", argv[i]);
            openFile(argv[i]);
        }
        else{
            // else it is a directory, call recursive method to find all the files
            findFiles(argv[i]);
        }
        
    }
    al_destroy(dict_words);
    return 0;

}

void findFiles(char* name){
    struct stat buf;
    DIR *dir; 
    struct dirent *entry;

    // open the directory
    dir = opendir(name);

    if (dir == NULL){
        perror("error opening directory\n");
    }
    else{
        while((entry = readdir(dir)) != NULL){
            char* potentialFile = entry->d_name;
            if (potentialFile[0] == '.') continue;
            char *temp = malloc((strlen(name) + strlen(entry->d_name) + 2) * sizeof(char));
            strcpy(temp, name);
            strcat(temp, "/");
            strcat(temp, potentialFile);

            int r = stat(temp, &buf);
            if (r == -1) printf("error retrieving file attributes\n");
    
            // if .txt file
            if ((S_ISREG(buf.st_mode)) 
            && (strlen(potentialFile) >=4)
            && (strcmp(potentialFile + strlen(potentialFile) - 4, ".txt") == 0)){
                openFile(temp);
            }
            // if not, recurse through directory and find all .txt files
            else if(S_ISDIR(buf.st_mode)){
                findFiles(temp);
                free(temp);
                continue;
            }
            free(temp);

        }
        closedir(dir);
    }
}


void openFile(char *file){
    // open() to open the file inputted
    int fd = open(file, O_RDONLY);
    if (fd == -1) printf("Could not open file: %s\n", file);
    read_words(fd, 0, file);
    close(fd);
}



int compare_strings(char *s1, char *s2){
    int len1 = 0; 
    int len2 = 0;
    int apostrophe1 = 0;
    int apostrophe2 = 0;
    // checking for apostrophes to account for the way the dictionary is ordered
    while(s1[len1] != '\0' && s2[len2] != '\0'){
        if(s1[len1] == '\'' && s2[len2] == '\'') {
            len1++;
            len2++;
            apostrophe1++;
            apostrophe2++;
        }
        else if (s1[len1] == '\'') {
            len1++;
            apostrophe1++;
        }
        else if (s2[len2] == '\'') {
            len2++;
            apostrophe2++;
        }
        else{
            // if any of the characters are not the same, do direct comparison
            if(s1[len1] < s2[len2]) return -1;
            else if (s1[len1] > s2[len2]) return 1;
            len1++;
            len2++;
        }
    }
    if (strcmp(s1, s2) == 0) return 0;
    else{
        // if the lengths are the same without the apostrophes
        if ((strlen(s1)-apostrophe1) == (strlen(s2) - apostrophe2)){
            int l1 = 0;
            int l2 = 0;
            // iterate through, the first word that has its "lone" apostrophe goes first 
            while(s1[l1] != '\0' && s2[l2] != '\0'){
                if (s1[l1] == '\'' && s2[l2] != '\''){
                    return -1; // s1 comes before s2
                }
                else if(s2[l2] == '\'' && s1[l1] != '\''){
                    return 1; // s1 comes after s2
                }
                l1++;
                l2++;
            }
        }
        else if (strlen(s1) < strlen(s2)) return -1; 
        else if (strlen(s1) > strlen(s2)) return 1;
    }
    return EXIT_FAILURE;

}

int checkWord(char *word, char *file, int line, int column){
    char *subword;
    int lasthyphen = 0;
    //remove punctuation for the word
    char *wordwithoutpunc = removePunctuation(word);
    for (int i = 0; i <strlen(wordwithoutpunc); i++){
        if (*(wordwithoutpunc+i) == '-'){
            subword = (char*) malloc((i+1 - lasthyphen) * sizeof(char));
            strncpy(subword, wordwithoutpunc + lasthyphen , i-lasthyphen);
            subword[i-lasthyphen] = '\0';

            lasthyphen = i+1;
                
                if (*subword != '\0'){
                    //call binary search, while ensuring the word is not a null character
                    if (binarySearch(subword) == -1) {
                        printf("%s (%d,%d): %s\n", file, line, column + countStart, wordwithoutpunc);
                        free(subword);
                        free(wordwithoutpunc);
                        
                        return -1;
                    }              
                }
                free(subword);
        }
    }
    

    char *remainingsubword = wordwithoutpunc+lasthyphen;
    
    if (*remainingsubword == '\0') {
        free(wordwithoutpunc);
        return 0;
    }
    // binary search 
    if (binarySearch(remainingsubword) == -1) {
        printf("%s (%d,%d): %s\n", file, line, column + countStart, wordwithoutpunc);
        free(wordwithoutpunc);
        
        return -1;
        }   

    free(wordwithoutpunc);

    return 0;

}

char *removePunctuation(char *word){
    int countBeginning = 0;
    int countEnd = 0;
    // count number of open parenthesis and open quotations in the beginnning
    for (int i = 0; i < strlen(word); i++){
        if(*(word+i) == '(' || *(word+i) == '{' || *(word+i) == '[' ||*(word+i) == '\'' ||*(word+i) == '"'){
            
            countBeginning++;
        }
        else break;
    }
    // count number of trailing punctuation
    if(countBeginning != strlen(word)){
        for (int i = strlen(word) - 1; i >= countBeginning; i--) {
        if(!isalpha((word[i]))){
            countEnd++;
        }
        else break;
    }
    }

    

    // create a new word without the beginning and ending characters to be removed
    char *wordwithoutpunc = (char*) malloc(sizeof(char) * (strlen(word) - countBeginning - countEnd + 1));
    
    strncpy(wordwithoutpunc, word+countBeginning, strlen(word+countBeginning) - countEnd );
    if (TEST) printf("%ld\n",strlen(word) - countBeginning - countEnd);

    wordwithoutpunc[strlen(word+countBeginning) - countEnd] = '\0';

    
    countStart = countBeginning;

    
    return wordwithoutpunc;

}

int linearsearch(char *word){
    int numUpper = 0;
    int allUppercase = 0; 
    int numApostrophe = 0;
    for (int i = 0; i < strlen(word); i++){
        if(((isalpha(*(word+i)) && (isupper(*(word+i)))) || *(word+i) == '\'')) numUpper++;
        // if(*(word+i) == '\'') numApostrophe++;
    }
    
    //if every character is uppercase 
    if (numUpper == (strlen(word - numApostrophe))) allUppercase = 1;

    for (int i = 0; i < dict_words->length; i++){
        // if the word's first letter is lowercase
        if(!isalpha(*word) || islower(*word)){
            if (strcmp(dict_words->data[i], word) == 0){
                return 1;
            }
        }
        // if the word is entirely uppercase
        else if (allUppercase){ 
            // if all uppercase, compare the uppercase version of every word 
            if (strcasecmp(al_get(dict_words, i), word) == 0){
                return 1;
            }
            continue;
        }
        else if(isupper(*(word))){
        // otherwise, if the first letter of the dictionary word is lowercase:
            if (islower(dict_words->data[i][0])){
                *word = tolower(*word);
                if(compare_strings(dict_words->data[i], word) == 0){
                // if the strings are equal now
                // restore uppercase first letter
                *word = toupper(*word);
                return 1; 
                    }
                *word = toupper(*word);
            }

        }
        
    }
    // if it goes through every word in the dictionary, return -1
    return -1; 

}


int binarySearch(char *word){
    int hi = dict_words->length - 1;
    al_get(dict_words, 1);
    int lo = 0;
    int mid;
    
    while (lo <= hi){
        mid = lo + (hi - lo)/2;
        int cmp = compare_strings(al_get(dict_words, mid), word);
        if(TEST) printf("cmp:%d\n", cmp);
        if (cmp == 0) {
            return mid;
        }
        else if (cmp > 0) hi = mid - 1;
        else if (cmp < 0) lo = mid + 1;
    }
    // just in case the dictionary was not sorted, run linear search to do a check 
    // or, if the first letter was uppercase/ word is entirely uppercase
    return linearsearch(word);
}

void read_words(int fd, int isDictionary, char *file)
{
    int buflength = BUFLENGTH;
    char *buf = malloc(BUFLENGTH);

    int pos = 0;
    int bytes;
    int word_start;
    int line = 1;
    int column = 1;

    while ((bytes = read(fd, buf + pos, buflength - pos)) > 0) {
	if (DEBUG) printf("read %d bytes; pos=%d\n", bytes, pos);

	word_start = 0;
	int bufend = pos + bytes;

	while (pos < bufend) {
	    if (DEBUG) printf("start %d, pos %d, char '%c'\n", word_start, pos, buf[pos]);
        // if blank space, then store the word and check for it
	    if (buf[pos] == ' ') {
            buf[pos] = '\0';
            int wordcolumn = column - strlen(buf + word_start);
            if (DEBUG) printf("%s at line %d and column %d\n",buf+word_start, line, wordcolumn);
            char *word = buf + word_start;
            // if the call is on a dictionary, push to arraylist
            // else, check the word (call is on .txt file)
            if (isDictionary) {
                if (TEST) printf("pushing %s to arraylist\n", word);
                al_push(dict_words, word);
                }
            else if (!isDictionary && *word != '\0' && *word != ' ') {
                if (TEST) printf("calling search on %s\n", word);
                checkWord(word, file, line, wordcolumn);
            }
		    // we found a word, starting at word_start ending before pos

		    word_start = pos + 1;
	    }
        else if(buf[pos] == '\n'){
            buf[pos] = '\0';
            int wordcolumn = column - strlen(buf + word_start);
            if (DEBUG) printf("%s at line %d and column %ld\n",buf+word_start, line, column - strlen(buf + word_start));
            char *word = buf + word_start;
            if (word == 0) continue;
            // if the call is on a dictionary, push to arraylist
            // else, check the word (call is on .txt file)
            if (isDictionary) {
                if (TEST) printf("pushing %s to arraylist\n", word);
                al_push(dict_words, word);
                }
            else if (!isDictionary && *word != '\0' && *word != ' ') {

                checkWord(word, file, line, wordcolumn);
            }
            line++;
            column = 0;

            word_start = pos + 1;
        }
	    pos++;
        column++;
	}

	// no partial line
	if (word_start == pos) {
	    pos = 0;
	
	// partial line
	// move segment to start of buffer and fill rest of buffer using new read() call 
	} else if (word_start > 0) {
	    int segment_length = pos - word_start;
	    memmove(buf, buf + word_start, segment_length);
	    pos = segment_length;

	    if (DEBUG) printf("move %d bytes to buffer start\n", segment_length);
	
	// partial line that takes up the whole buffer
	} else if (bufend == buflength) {
	    buflength *= 2;
	    buf = realloc(buf, buflength);

	    if (DEBUG) printf("increase buffer to %d bytes\n", buflength);
	}
    }
    
    // partial line after end of file reached
    if (pos > 0) {
        word_start = 0;
	    if (pos == buflength) {
	        buf = realloc(buf, buflength + 1);
	    }
	    buf[pos] = '\0';
        int wordcolumn = column - strlen(buf + word_start);
        char *word = buf + word_start;
        // if the call is on a dictionary, push to arraylist
        // else, check the word (call is on .txt file)
        if (isDictionary) {
            if (TEST) printf("pushing %s to arraylist\n", word);
            al_push(dict_words, word);
            }
        else if (!isDictionary && *word != '\0' && *word != ' ') {
            if (TEST) printf("calling binary search on %s\n", word);
            checkWord(word, file, line, wordcolumn);
            }

        if (DEBUG) printf("buf: %s , word_start: %d\n", buf, word_start);
        if (DEBUG) printf("%s at line %d and column %ld\n",buf+word_start, line, column - strlen(buf + word_start));
    }

    free(buf);
    
}
