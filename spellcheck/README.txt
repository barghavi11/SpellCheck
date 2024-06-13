---------------------------------------SPELL CHECKER-----------------------------------------------
                                       - - - - - - - 
                           name: Barghavi Gopinath and Upasana Patel 
                                   netID: bg586 and up62

OVERVIEW 
---------
spchk.c is a program that takes in a dictionary file and a series of entries. These entries can be 
either a .txt file OR a directory. If an entry is a directory, spell check will first find all .txt
files within the directory. For every .txt file in the directory and command line, spell check will 
grab every word from the file and search for the word in the dictionary. If the word does not exist,
the program will return the misspelt word to the user, along with the name of the file, line number,
and column number that the word exists in. For my program to run most efficiently, the dictionary 
should be sorted lexicographically.

RULES
-----
Certain rules and exceptions exist when checking for words:

1. All words in the dictionary will accept its complete uppercase form 
    - hello will accept "HELLO" "Hello" and "hello" as spelt correctly
2. Words capitalized in the dictionary will not take its first letter lowercase
    - Hello will only accept "HELLO" and "Hello" as spelt correctly 
3. Words with uppercase letters in the middle of the word follow this rule: 
    - MacDonald will only accept "MacDonald" and "MACDONALD"
    - helloWorld will accept "helloWorld", "HelloWorld", and "HELLOWORLD"
4. non-alphabetical characters at the beginning and end of words are ignored 
    - 28.'hello?! will check the spelling of "hello"
5. hyphenated words will check the spelling of its individual subwords
    - "sub-par" will check the spelling of "sub" and "par"
    - if one of the subwords are misspelt, the entire word is said to be misspelt                        

TESTING
-------
To check my code for thoroughness and robustness, I performed a series of tests, as follows:

tests for directory traversal:
- - - - - - - - - - - - - - - 

1. Test text files in the same directory against the given dictionary - "callmemaybe.txt"
2. Test directory against the given dictionary - "dir1" and "dir3"
3. Test combinations of directories and text files against the given dictionary 
4. Test combinations of directories and text files against my own dictionary - "mydict.txt"
5. Included a .c file to ensure that the program does not try to parse words, and a file beginning 
with "." (this is not allowed).

tests for spell checking: 
- - - - - - - - - - - - - 
1. Have words with trailing punctuation - "321-no" and "--00-no" and "$12hello!.-bye-#he"
2. Have words that start in the middle of a line "          the"
3. Have phrases that are not words - "{}}]**)#"
4. Have words with the first letter capitalized - "Hello" 
5. Test words such as "maCDonald" "macDonald" "macdonald" and "MACDONALD" against dictionary 
word "MacDonald"

TO COMPILE:
----------
in terminal, write out commands in this order to compile and run spell check:
make
./spchk <dictionary_file> <file_or_dictionary_1> <file_or_dictionary_2> .... <file_or_dictionary_n>