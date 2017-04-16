/* 
 * File:   trie.h
 * Author: Spiros Delviniotis
 *
 * Created on 30 Απρίλιος 2014, 4:26 μμ
 */

#define N 5
#define ALPHABET_SIZE 26
#define BUFFER_SIZE 30

typedef struct trie_node* trie_nodePtr;  /* Pointer to trie node */
typedef struct trie_node trie_node_t;  /* Type trie node */
typedef struct trie* triePtr;  /* Pointer to Trie */
typedef struct trie trie_t;  /* Type trie */


/* Trie node */
struct trie_node
{
    char character;
    int popularity;
    trie_nodePtr father;  /* Pointer to father node */
    trie_nodePtr* children;  /* Table of trie_nodePtr for children */
    trie_nodePtr* popular_children;  /* pointer to popular leaves */
};  

/* Trie tree */
struct trie
{
    trie_nodePtr root;  /* Root node */
    int dictionary_counter;  /* Leaves counter */
};


/* Trie functions */
trie_nodePtr Trie_newNode(char,int);  /* Allocates memory for new node */
void Trie_Destroy(trie_nodePtr);  /* Destroy of trie */
void Trie_PrintWord(triePtr*, char *);  /* Search and print word if exists */
int Trie_PrintAutoComplete(triePtr*, char *,char *);  /* Search and print possible words */
trie_nodePtr Trie_IncreasePopularity(triePtr*, char *);  /* Increase popularity of word, returns leave node */
int Trie_Update(triePtr*, char *);  /* Updates the popularity of word and insert if not exists */  
int Trie_Exists(triePtr*, char *);  /* Check if word exists (0) in trie */
int Trie_GetWord(trie_nodePtr, trie_nodePtr, char *);  /* Get word starting from leave */
int Trie_Insert(triePtr*, char *);  /* Insert node at trie */
int Trie_Create(triePtr*);  /* Creates an empty trie */

