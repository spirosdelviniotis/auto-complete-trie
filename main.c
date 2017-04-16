/* 
 * File:   main.c
 * Author: Spiros Delviniotis
 *
 * Created on 30 Απρίλιος 2014, 4:22 μμ
 */

#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include "trie.h"

#define LINESIZE 128
#define BACKSPACE 0x7f


int getchar_silent()
{
    int ch;
    struct termios oldt, newt;

    /* Retrieve old terminal settings */
    tcgetattr(STDIN_FILENO, &oldt);

    /* Disable canonical input mode, and input character echoing. */
    newt = oldt;
    newt.c_lflag &= ~( ICANON | ECHO );

    /* Set new terminal settings */
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    /* Read next character, and then switch to old terminal settings. */
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    return ch;
}

int main(int argc, char** argv) {
    trie_t tree;
    trie_t* treePtr=&tree;
    
    FILE *ptr_file;  //pointer to file
    char read[100];
    char choice[5];  //choice to complet
    char buffer[40];  //reading buffer
    char *word = NULL;  //word to insert at trie
    int i;  //counter for loop
    int j;  //choice
    int status;  //insert status
    int next;  //charachter
    int number_choice = 0;  //number of choice
    struct winsize w;
    
    /* Read file to train the trie */
    printf("----- Training Trie -----\n");
    
    /***********************  Read file from input  ***************************/
    char *home = getenv("HOME");  //home = $HOME

    char path[100];  //path of read file
    
    strcat(home, "/.dict");

    if (argc < 3)
    {
        strcpy(path, home);
    }
    else if (argc == 3)
    {
        strcpy(path, argv[2]);
    }
    
    /* open file */
    ptr_file = fopen(path,"r");
    //ptr_file = fopen("file_dictionary_small.txt","r");
    //ptr_file = fopen("file_dictionary.txt","r");
    //ptr_file = fopen("dictionary.txt","r");
    if (!ptr_file)
    {
        printf("Main ERROR: File not exists.\n");
        return (EXIT_FAILURE);
    }
    
    /* Trie Creation */
    status = Trie_Create(&treePtr);
    if (status == 1)
    {
        printf("Main ERROR: Trie_Insert.\n");
        return (EXIT_FAILURE);
    }
    
    word = (char*)malloc(sizeof(char)*(strlen(buffer)));
    if (word == NULL)
    {
        printf("Main ERROR: Allocation memory.\n");
        return (EXIT_FAILURE);
    }
    
    while (fgets(buffer,40, ptr_file) != NULL)
    {
        /* Copy string from buffer without junk data(\n) */
        for (i=0; i<(strlen(buffer)-1);i++)
        {
            /* Convert characters A->a */
            if (buffer[i]>64 && buffer[i]<91)
            {
                word[i]=buffer[i] + 32;
            }
            else if (buffer[i]>96 && buffer[i]<123)
            {
                word[i]=buffer[i];
            }
            else
            {
                printf("Main: Error, unknown character.\n");
            }
        }
        word[i+1]='\0';  //finalize word
        
        /* Insert word in Trie */
        status = Trie_Insert(&treePtr,word);
        if (status == 1)
        {
            printf("Main ERROR: Trie_Insert.\n");
            return (EXIT_FAILURE);
        }
        
        /**************** WORD EXISTS AT TRIE ****************/
        //Trie_PrintWord(&treePtr,word);
    }
    
    printf("Main: Tree counter -> %d\n",treePtr->dictionary_counter);
    
    free(word);  //free malloced pointer -> memory map
    fclose(ptr_file);  //close file
    printf("----- END TRAINNING OF TRIE -----\n");

    //initialize
    i=0;  //read
    j=0;  //choice
    /* Keep reading one character at a time */
    while ((next = getchar_silent()) != EOF)
    {
        /* Print normal characters */
        if (isalpha(next))
        {
            putchar(next);
            
            /* Check character IN [A..Z][a..z] */
            if (next>64 && next<91)
            {
                read[i]=next + 32;
                i++;
            }
            else if (next>96 && next<123)
            {
                read[i]=next;
                i++;
            }
            else
            {
                printf("\n->Main: Error, unknown character.\n");
            }
        }
        else if (next == ' ')  //update TRIE
        {
            if (i==0)  //no word in read buffer
            {
                continue;
            }
            else
            {
                read[i]='\0';  //finalize word
            
                status = Trie_Update(&treePtr,read);
                if (status == 1)
                {
                    printf("Main ERROR: Trie_Update.\n");
                    return (EXIT_FAILURE);
                }
                else if (status == 2)
                {
                    /** IF NOT EXIST'S WRITE TO FILE **/
                    printf("Main: New word! Give me one moment to insert at file..\nOK!!!\n");

                    /* Open and write word to file */
                    ptr_file = fopen(path,"a");  //open file to append
                    fprintf(ptr_file , "\n%s\n" , read);  //write word at end of file
                    fclose(ptr_file);  //close file
                }

                i=0;  //re-initialize i
            }
        }
        else if (next == '\t')  //auto complete
        {
            read[i]='\0';  //finalize word
            char *choice_table;
            
            choice_table = (char*)malloc(sizeof(char)*N*30);
            if (choice_table == NULL)
            {
                printf("Main ERROR: Malloc choice_table.\n");
                return (EXIT_FAILURE);
            }
            
            status = Trie_PrintAutoComplete(&treePtr, read, choice_table);
            if (status == 2)  //word not exists
            {
                continue;  //ignore char
            }
            else  //word exists
            {
                i=0;  //initialize i
            
                /** CHOICE IF WORD EXISTS **/
                while (1)
                {
                    printf("Please press a number.\n");

                    j=0;  //re-initialize j
                    next = getchar_silent();
                    putchar(next);

                    while (next>47 && next<58)  //get number of choice
                    {
                        choice[j]=next;
                        j++;

                        next = getchar_silent();  //read next character
                        putchar(next);
                    }
                    choice[j]='\0';  //finalize choice
                    
                    if (j == 0)  //no valid answer
                    {
                        printf("Please press a number.\n");
                    }
                    number_choice = atoi(choice);
                    if (number_choice <= N)
                    {
                        /* Find choice */
                        int k=0;  //counter  
                        int flag = 1;  //flag to stop counting
                        while (flag  == 1)
                        {
                            if (choice_table[k] == '#')
                            {
                                if (k == number_choice-1)
                                {
                                    flag = 0;  //stop loop to print word
                                }
                                k++;  //increase k '#'
                            }

                            choice_table++;  //move pointer
                        }

                        putchar('>');
                        while (choice_table[k] != '#')
                        {
                            printf("%c",choice_table[k]);
                            k++;
                        }
                        putchar('\n');

                        break;
                    }
                    else
                    {
                        printf("Main: Error, Please insert valid value in your choice. Valid choices are numbers: 1...%d.\n",N);
                    }
                }
            }
        }
        else if (next == VEOF)  //exit
        {
            printf("\n\nExiting. Bye...");
            goto THE_END;
        }
        else if (next == BACKSPACE)
        {
            printf("\b \b");
            break;
        }

        
    }
THE_END:
    putchar('\n');
    return EXIT_SUCCESS;
}

