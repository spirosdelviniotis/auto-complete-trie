/* 
 * File:   Trie.h
 * Author: Spiros Delviniotis
 *
 * Created on 30 Απρίλιος 2014, 4:26 μμ
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "trie.h"


/* Trie Create */
int Trie_Create(triePtr* tree)
{
    /* Create new trie node */
    (*tree)->root = Trie_newNode('r',-1);
    if ((*tree)->root == NULL)
    {
        printf("Trie_Create ERROR: NULL newNode.\n");
        return (EXIT_FAILURE);
    }
    
    /* Initialize values */
    (*tree)->dictionary_counter = 0;
    (*tree)->root->father = NULL;
    
    printf("Trie_Create: Success.\n");
    
    return (EXIT_SUCCESS);
}


/* Trie newNode */
trie_nodePtr Trie_newNode(char character, int popularity)
{
    int i;  //counter
    trie_nodePtr temp_nodePtr = NULL;  //Temporary pointer to trie_node
    
    /* Allocate memory for node */
    temp_nodePtr = (trie_nodePtr)malloc(sizeof(trie_node_t));
    if (temp_nodePtr == NULL)
    {
        printf("Trie_newNode ERROR: Malloc at create node_trie.\n");
        return NULL;
    }
    
    /* Allocate memory for children */
    temp_nodePtr->children = (trie_nodePtr*)malloc(ALPHABET_SIZE*sizeof(trie_nodePtr));  //warning???
    if (temp_nodePtr->children == NULL)
    {
        printf("Trie_newNode ERROR: Malloc at create children of trie_node.\n");
        return NULL;
    }
    
    /* Allocate memory for N popular words */
    temp_nodePtr->popular_children = (trie_nodePtr*)malloc(N*sizeof(trie_nodePtr));
    if (temp_nodePtr->popular_children == NULL)
    {
        printf("Trie_newNode ERROR: Malloc at create popular children of trie_node.\n");
        return NULL;
    }
    
    /* Initialize */
    for (i=0; i<N; i++)
    {
        temp_nodePtr->popular_children[i] = NULL;
    }
    
    /* Initialize */
    for (i=0 ; i<ALPHABET_SIZE; i++)
    {
        temp_nodePtr->children[i] = NULL;
    }
    temp_nodePtr->character = character;
    temp_nodePtr->popularity = popularity;
    
    //printf("Trie_newNode: Success.\n");
    
    return temp_nodePtr;
    
}


/* Trie Insert */
int Trie_Insert(triePtr* tree , char word[])
{
    int depth;  //depth of word-subtree
    int i;  //counter
    int offset;  //cell of children table
    int word_lenth = strlen(word);  //lenth of insert word
    trie_nodePtr temp_nodePtr = (*tree)->root;  //initialized with root node
    trie_nodePtr leaf_nodePtr = NULL;  //leaf to best child
    
    for (depth=0; depth<word_lenth; depth++)
    {
        offset = word[depth]-96;  //update offset(cell))
        
        /* Check if Child exists */
        if (temp_nodePtr->children[offset] == NULL)
        {
            //printf("Trie_Insert: cell-> %d NULL.\n", word[depth]-96);  //PRINT CELL
            
            temp_nodePtr->children[offset] = Trie_newNode(word[depth],0);
            if (temp_nodePtr->children[offset] == NULL)
            {
                printf("Trie_Insert ERROR: NULL newNode.\n");
                return (EXIT_FAILURE);
            }
            
            temp_nodePtr->children[offset]->father = temp_nodePtr;
        }
        else
        {
            //printf("Trie_Insert: cell-> %d exists.\n", offset);  //PRINT CELL
        }
        
        temp_nodePtr = temp_nodePtr->children[offset];
    }
    
    /******* Insert last node with value = @ *******/
    if (temp_nodePtr->children[0] != NULL)
    {
        if (temp_nodePtr->children[0]->character == '@')
        {
            printf("Trie_Insert: \"%s\" -- EXISTS --\n",word);
            return (EXIT_SUCCESS);
        }
    }
    else
    {
        temp_nodePtr->children[0] = Trie_newNode('@',0);
        if (temp_nodePtr->children[0] == NULL)
        {
            printf("Trie_Insert ERROR: NULL Child.\n");
            return (EXIT_FAILURE);
        }
        
        /* initialize N best children */
        leaf_nodePtr = temp_nodePtr->children[0];  //initialize leaf
        temp_nodePtr->children[0]->father = temp_nodePtr;  //link father with children
        temp_nodePtr = (*tree)->root;  //initialized with root node
        
        for (depth=0; depth<word_lenth+1; depth++)  //loop: lenth of word + root node
        {
            offset = word[depth]-96;  //update offset(cell)
            
            for (i=0; i<N; i++)
            {
                if (temp_nodePtr->popular_children[i] == NULL)
                {
                    temp_nodePtr->popular_children[i] = leaf_nodePtr;
                    
                    //printf("Trie_Insert: NULL popular Child!\n");  //PRINT NULL POPULAR CHILD
                }
                else 
                {
                    //printf("Trie_Insert: BREAK!\n");  //I HAVE POPULAR CHILD INITIALIZED
                    break;
                }
            }
            
            temp_nodePtr = temp_nodePtr->children[offset];  //update temp node(down level)
        }
    }
    
    (*tree)->dictionary_counter++;  //increase dictionary_counter
    
    printf("Trie_Insert: \"%s\" Inserted.\n",word);
    
    return (EXIT_SUCCESS);
}


void Trie_PrintWord(triePtr* tree, char word[])  //OK
{ 
    int depth;  //depth of word-subtree
    int word_lenth = strlen(word);  //lenth of print word
    trie_nodePtr temp_nodePtr = (*tree)->root;  //initialized with root node
    
    printf("Trie_PrintWord: \"");
    for (depth=0; depth<word_lenth; depth++)
    {
        /* Check if Child exists */
        if (temp_nodePtr->children[word[depth]-96] == NULL)
        {
            printf("\" :: \"%s\" NOT exists.\n",word);
            return;
        }
        else 
        {
            printf("%c",temp_nodePtr->children[word[depth]-96]->character);
            temp_nodePtr = temp_nodePtr->children[word[depth]-96];
        }
    }
    
    printf("\" leaf-> %p\n",temp_nodePtr->children[0]);  //LEAF NODE ----> NULL?
    
    printf(".\n");

}


int Trie_PrintAutoComplete(triePtr* tree, char word[], char *choices) 
{
    int depth;  //depth of word-subtree
    int i,j;  //counter
    int word_lenth = strlen(word);  //lenth of insert word
    int offset;  //cell of children table
    char buffer[BUFFER_SIZE];  //buffer with word
    trie_nodePtr temp_nodePtr = (*tree)->root;  //initialized with root node
    trie_nodePtr current_nodePtr = NULL;  //
    
    printf("Trie_PrintAutoComplete: ");
    for (depth=0; depth<word_lenth; depth++)
    {
        offset = word[depth]-96;  //initialize offset
        
        /* Check if Child exists */
        if (temp_nodePtr->children[offset] == NULL)
        {
            printf("Trie_PrintAutoComplete ERROR: \"%s\" NOT exists.\n",word);
            return 2;  //not exists
        }
        else 
        {
            printf("%c",temp_nodePtr->children[offset]->character);
            temp_nodePtr = temp_nodePtr->children[offset];
        }
    }
    printf("\n");
    
    /* Temp node is the stop node to print N best children of node */
    current_nodePtr = temp_nodePtr;  //pointer to leaf's father(without '@')
    
    offset = 0;  //offset for buffer array
    choices[offset] = '#';
    offset++;
    for (i=0; i<N; i++)
    {
        /* print N popular words */
        temp_nodePtr = temp_nodePtr->popular_children[i];
        
        j=0;  //initialize counter
        printf("    [%d]. ",i+1);
        
        while ((temp_nodePtr != current_nodePtr) )
        {
            buffer[j] = temp_nodePtr->character;  //copy strating from [1]
            j++;
            
            temp_nodePtr = temp_nodePtr->father;
        }
        //buffer[j] = '\0';  //OPIONAL
        
        for (j=j; j>0; j--)
        {
            putchar(buffer[j]);
            choices[offset] = buffer[j];
            
            offset++;
        }
        
        choices[offset] = '#';
        offset++;

        printf("\n");
    }
    
    return (EXIT_SUCCESS);
}


int Trie_Update(triePtr* tree, char word[])
{
    trie_nodePtr leaf_node = NULL;  //pointer to leave of trie, return value from IncreasePopularity
    trie_nodePtr temp_node = NULL;  //temporary poinrer to have access in trie
    int compare_popularity = 0;  //popularity of word(in leave)
    int current_popularity = 0;  //popularity of best word (one of N best words in each node)
    int depth = 0;  //depth of word-tree
    int word_lenth = strlen(word);  //lenth of update word
    int i = 0;  //counter of N best nodes
    int flag = 0;  //exit_success, 2=exit for write word in file, 1=exit_failure
    
    if (Trie_Exists(tree,word) == 1) //not exists
    {
        if (Trie_Insert(tree,word) == 1)
        {
            printf("Trie_Update ERROR: Failed to insert \"%s\" in trie.\n", word);
            return (EXIT_FAILURE);
        }
        
        flag = 2;  //exit_write
    }
    
    /* Increase popularity */
    leaf_node = Trie_IncreasePopularity(tree,word);  //return leave of trie
    if (leaf_node == NULL)  //word not exists
    {
        printf("Trie_Update ERROR: Failed to Trie_IncreasePopularity.\n");
        return (EXIT_FAILURE);
    }
    
    /* initialize value of compare popularity */
    compare_popularity = leaf_node->popularity;
    
    /* Go one level up */
    temp_node = leaf_node->father;
    
    /* update popular children for each father node */
    for (depth=0; depth<word_lenth+1; depth++)  //word_lenth+1 -> ???
    {
        for (i=0; i<N; i++)
        {
            if (temp_node->popular_children[i] == NULL)
            {
                printf("Trie_Update ERROR: Node's Best child is NULL.\n");
                return (EXIT_FAILURE);
            }
            
            current_popularity = temp_node->popular_children[i]->popularity;  //popularity of one of the best words
            
            if (compare_popularity > current_popularity)
            {
                temp_node->popular_children[i] = leaf_node;
                
                //printf("Trie_Update: Best Nodes -> Updated.\n");  //UP TO DATE BEST NODES
                break;
            } 
        }
        
        temp_node = temp_node->father;
    }
    
    printf("Trie_Update: Success.\n");
    
    return flag;
}


int Trie_Exists(triePtr* tree, char word[])  //OK
{
    int depth;  //depth of word-subtree
    int word_lenth = strlen(word);  //lenth of insert word
    trie_nodePtr temp_nodePtr = (*tree)->root;  //initialized with root node
    
    for (depth=0; depth<word_lenth; depth++)
    {
        /* Check if Child exists */
        if (temp_nodePtr->children[word[depth]-96] == NULL)
        {
            printf("Trie_Exists ERROR: \"%s\" NOT exists.\n",word);
            return (EXIT_FAILURE);
        }
        else 
        {
            temp_nodePtr = temp_nodePtr->children[word[depth]-96];
        }
    }
    
    printf("        -> %c\n",temp_nodePtr->children[0]->character);
    
    printf("Trie_Exists: \"%s\" Exists.\n",word);
    
    return (EXIT_SUCCESS);
}


trie_nodePtr Trie_IncreasePopularity(triePtr* tree, char word[])  //OK
{
    int depth;  //depth of word-subtree
    int word_lenth = strlen(word);  //lenth of insert word
    trie_nodePtr temp_nodePtr = (*tree)->root;  //initialized with root node
    
    for (depth=0; depth<word_lenth; depth++)
    {
        //printf("Trie_IncreasePoupularity: [%d] popularity=%d\n",depth,temp_nodePtr->popularity);  //POPULARITY OF NODE
        
        /* Check if Child exists */
        if (temp_nodePtr->children[word[depth]-96] == NULL)
        {
            printf("Trie_IncreasePopularity ERROR: \"%s\" NOT exists.\n",word);
            return NULL;
        }
        
        temp_nodePtr = temp_nodePtr->children[word[depth]-96];
    }
    
    
    /*******************   NULL LEAF ???   ******************/
    /*printf("Trie_IncreasePopularity: temp_nodePtr->father->popularity=%d.\n",temp_nodePtr->father->popularity);
    printf("Trie_IncreasePopularity: temp_nodePtr->popularity=%d.\n",temp_nodePtr->popularity);
    printf("Trie_IncreasePopularity: temp_nodePtr->children[0]=%p.\n",temp_nodePtr->children[0]);
    printf("Trie_IncreasePopularity: temp_nodePtr->father->character=%c.\n",temp_nodePtr->father->character);
    printf("Trie_IncreasePopularity: temp_nodePtr->character=%c.\n",temp_nodePtr->character);*/
    
    
    /*if (temp_nodePtr->children[0] == NULL)
    {
        printf("Trie_IncreasePopularity ERROR: temp_nodePtr->children[0] = NULL\n");  //@ NOT EXISTS
    }*/
    
    /* Increase popularity of LAST node */
    temp_nodePtr->children[0]->popularity = temp_nodePtr->children[0]->popularity + 1;
    
    printf("Trie_IncreasePopularity: Success.\n");
    
    return temp_nodePtr->children[0];
}


int Trie_GetWord(trie_nodePtr nodePtr, trie_nodePtr nodePtr_current, char word[])  //OK
{
    int i = 0;  //cell in word
    trie_nodePtr temp_nodePtr = nodePtr;  //initialize to child
    
    if (temp_nodePtr == NULL)
    {
        printf("Trie_GetWord ERROR: NULL child.\n");
        return (EXIT_FAILURE);
    }
    
    /* Loop while node is not root */
    while ((temp_nodePtr->father != nodePtr_current) && (temp_nodePtr->father != NULL))
    {
        word[i] = temp_nodePtr->character;
        i++;
        
        temp_nodePtr = temp_nodePtr->father;
    }
    word[i] = '\0';
    
    printf("Trie_GetWord: %s .\n",word);
    
    return (EXIT_SUCCESS);
}


void Trie_Destroy(trie_nodePtr node)
{
    int i,j,k;
    
    if (NULL == node)
    {
        return;
    }
    
    
    /* free node */
    for (i=0; i<26; i++)
    {
        //free(kill_nodePtr->children[i]);
        Trie_Destroy(node->children[i]);
    }
    
    free(node);
    
    return;
    
    
    /*for (i=0; i<N ;i++)
    {
        free(kill_nodePtr->popular_children[i]);
    }*/
    
    /* go up */
    /*while (temp_nodePtr->father != NULL)
    {
        temp_nodePtr = temp_nodePtr->father;
    }
    
    while (temp_nodePtr->children[j] != NULL)
    {
        temp_nodePtr = temp_nodePtr->father;
    }*/
    
}

