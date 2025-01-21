#include "hw6.h"

int main(int argc, char *argv[]) {
    if(argc <= 6) {
        return MISSING_ARGUMENT;
    }

    extern char *optarg; extern int optind; int c; 
    char *search, *replace, *lines;
    int dup = 0, satR = 0, satS = 0, satL = 0, hasW = 0;

    while((c = getopt(argc, argv, "r:l:s:w")) != -1) {
        switch(c) {
            case 'r':
                if(satR == 0) {
                    satR++;

                    if(optarg[0] == '-') {
                        break;
                    }

                    satR++;
                    replace = optarg;
                } else {
                    dup = 1;
                }

                break;
            case 's':
                if(satS == 0) {
                    satS++;

                    if(optarg[0] == '-') {
                        break;
                    }

                    satS++;
                    search = optarg;
                } else {
                    dup = 1;
                }

                break;
            case 'l':
                if(satL == 0) {
                    satL++;

                    if(optarg[0] == '-') {
                        break;
                    }

                    satL++;
                    lines = optarg;
                } else {
                    dup = 1;
                }

                break;
            case 'w':
                if(hasW == 0) {
                    hasW = 1;
                } else {
                    dup = 1;
                }

                break;
            case '?':
                break;
        }

        if(dup == 1) {
            return DUPLICATE_ARGUMENT;
        }
    }

    FILE *infile;
    infile = fopen(argv[argc - 2], "r");
    if(infile == NULL) {
        return INPUT_FILE_MISSING;
    }

    FILE *outfile;
    outfile = fopen(argv[argc - 1], "w");
    if(outfile == NULL) {
        return OUTPUT_FILE_UNWRITABLE;
    }

    if(satS < 2) {
        return S_ARGUMENT_MISSING;
    }
    
    if(satR < 2) {
        return R_ARGUMENT_MISSING;
    }

    long start, end;
    if(satL == 1) {
        return L_ARGUMENT_INVALID;
    } else if(satL == 2) {
        char *numbers = strtok(lines, ",");

        if(numbers == NULL) {
            return L_ARGUMENT_INVALID;
        } else {
            char *ptr;
            start = strtol(numbers, &ptr, 10);
            if(ptr == numbers) {
                return L_ARGUMENT_INVALID;
            } else {
                numbers = strtok(NULL, ",");

                if(numbers == NULL) {
                    return L_ARGUMENT_INVALID;
                }

                end = strtol(numbers, &ptr, 10);
                if(ptr == numbers) {
                    return L_ARGUMENT_INVALID;
                }
            }

            if(start > end) {
                return L_ARGUMENT_INVALID;
            }
        }
    }

    if(hasW == 1) {
        if((search[0] == '*' && search[strlen(search) - 1] == '*') || (search[0] != '*' && search[strlen(search) - 1] != '*')) {
            return WILDCARD_INVALID;
        }
    }

    //First loop I get number of lines in the file
    int numLines = 0;
    char line[MAX_LINE];
    while(fgets(line, MAX_LINE, infile)  != NULL) {
        numLines++;
    }

    //I reset pointer so I can loop again
    rewind(infile);

    //This time loop to process each line into the fileLines
    char fileLines[numLines][MAX_LINE];
    int i=0;
    while(fgets(fileLines[i++], MAX_LINE, infile) != NULL) {}

    //Now loop the fileLines to process line by line for search words
    //Fix indexes 
    start--;
    end--;

    if(satL == 0) {
        start = 0;
        end = numLines - 1;
    }

    for(i=0; i<start; i++) {
        fprintf(outfile, "%s", fileLines[i]);
    }

    for(; i <= end && i <= numLines - 1; i++) {
        if(hasW == 0) {
            char *word = NULL;
            word = strstr(fileLines[i], search);
            int prevIdx = 0;
            int idx;

            while(word != NULL) {
                idx = word - fileLines[i];
                for(int j=prevIdx; j<idx; j++) {
                    fprintf(outfile, "%c", fileLines[i][j]);
                }

                prevIdx = idx + strlen(search);
                fprintf(outfile, "%s", replace);
                word = strstr(word + strlen(search), search);
            }

            for(int j=prevIdx; j<strlen(fileLines[i]); j++) {
                fprintf(outfile, "%c", fileLines[i][j]);
            }
        } else {
            int beginEnd = 0; //0 means substring at start, 1 means at end
            char *toSearch = malloc(strlen(search));
            toSearch[strlen(search) - 1] = 0;

            if(search[0] == '*') {
                strncpy(toSearch, search + 1, strlen(search) - 1);
                beginEnd = 1;
            } else {
                strncpy(toSearch, search, strlen(search) - 1);
            }

            int j = 0;            
            while(toSearch != NULL && j < strlen(fileLines[i])) {
                int startIdx = j;
                //Push grammar at beginning of suppposed word
                for(int k=j; k<strlen(fileLines[i]); k++) {
                    if(fileLines[i][k] == '\n') {
                        fprintf(outfile, "%c", fileLines[i][k]);
                        startIdx = strlen(fileLines[i]);
                        j = strlen(fileLines[i]);
                        break;
                    }

                    if(ispunct(fileLines[i][k]) || isspace(fileLines[i][k])) {
                        fprintf(outfile, "%c", fileLines[i][k]);
                    } else {
                        startIdx = k;
                        j=k;
                        break;
                    }
                }

                //Find last index of word
                int endIdx = startIdx;
                for(int k=startIdx; k<strlen(fileLines[i]); k++) {
                    if(i == numLines - 1 && k == strlen(fileLines[i]) - 1) {
                        endIdx = k;
                        break;
                    }

                    if(ispunct(fileLines[i][k]) || isspace(fileLines[i][k]) || fileLines[i][k] == '\n') {
                        endIdx = k - 1;
                        break;
                    }
                }     

                if(j < strlen(fileLines[i])) {
                    if(endIdx >= startIdx) {
                        char *words = (char *) malloc(endIdx - startIdx + 2);
                        words[endIdx - startIdx + 1] = 0;
                        strncpy(words, fileLines[i] + startIdx, endIdx - startIdx + 1); 

                        int match = 0;
                        char *p = strstr(words, toSearch);
                        if(p != NULL) {
                            if(beginEnd == 0 && p == words) {
                                match = 1;
                            } else if(beginEnd == 1 && p + strlen(toSearch) == words + strlen(words)) {
                                match = 1;
                            }
                        }

                        if(match == 1) {
                            fprintf(outfile, "%s", replace);
                        } else {
                            fprintf(outfile, "%s", words);
                        }

                        free(words);
                    }
                } 

                j = endIdx + 1;
            }

            free(toSearch);
        }
    }

    for(; i<numLines; i++) {
        fprintf(outfile, "%s", fileLines[i]);
    }

    fclose(infile);
    fclose(outfile);
    return 0;
}