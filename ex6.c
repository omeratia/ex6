#include "ex6.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

# define INT_BUFFER 128

// ================================================
// Basic struct definitions from ex6.h assumed:
//   PokemonData { int id; char *name; PokemonType TYPE; int hp; int attack; EvolutionStatus CAN_EVOLVE; }
//   PokemonNode { PokemonData* data; PokemonNode* left, *right; }
//   OwnerNode   { char* ownerName; PokemonNode* pokedexRoot; OwnerNode *next, *prev; }
//   OwnerNode* ownerHead;
//   const PokemonData pokedex[];
// ================================================

// --------------------------------------------------------------
// 1) Safe integer reading
// --------------------------------------------------------------

void trimWhitespace(char *str)
{
    // Remove leading spaces/tabs/\r
    int start = 0;
    while (str[start] == ' ' || str[start] == '\t' || str[start] == '\r')
        start++;

    if (start > 0)
    {
        int idx = 0;
        while (str[start])
            str[idx++] = str[start++];
        str[idx] = '\0';
    }

    // Remove trailing spaces/tabs/\r
    int len = (int)strlen(str);
    while (len > 0 && (str[len - 1] == ' ' || str[len - 1] == '\t' || str[len - 1] == '\r'))
    {
        str[--len] = '\0';
    }
}

char *myStrdup(const char *src)
{
    if (!src)
        return NULL;
    size_t len = strlen(src);
    char *dest = (char *)malloc(len + 1);
    if (!dest)
    {
        printf("Memory allocation failed in myStrdup.\n");
        return NULL;
    }
    strcpy(dest, src);
    return dest;
}

int readIntSafe(const char *prompt)
{
    char buffer[INT_BUFFER];
    int value;
    int success = 0;

    while (!success)
    {
        printf("%s", prompt);

        // If we fail to read, treat it as invalid
        if (!fgets(buffer, sizeof(buffer), stdin))
        {
            printf("Invalid input.\n");
            clearerr(stdin);
            continue;
        }

        // 1) Strip any trailing \r or \n
        //    so "123\r\n" becomes "123"
        size_t len = strlen(buffer);
        if (len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r'))
            buffer[--len] = '\0';
        if (len > 0 && (buffer[len - 1] == '\r' || buffer[len - 1] == '\n'))
            buffer[--len] = '\0';

        // 2) Check if empty after stripping
        if (len == 0)
        {
            printf("Invalid input.\n");
            continue;
        }

        // 3) Attempt to parse integer with strtol
        char *endptr;
        value = (int)strtol(buffer, &endptr, 10);

        // If endptr didn't point to the end => leftover chars => invalid
        // or if buffer was something non-numeric
        if (*endptr != '\0')
        {
            printf("Invalid input.\n");
        }
        else
        {
            // We got a valid integer
            success = 1;
        }
    }
    return value;
}

// --------------------------------------------------------------
// 2) Utility: Get type name from enum
// --------------------------------------------------------------
const char *getTypeName(PokemonType type)
{
    switch (type)
    {
    case GRASS:
        return "GRASS";
    case FIRE:
        return "FIRE";
    case WATER:
        return "WATER";
    case BUG:
        return "BUG";
    case NORMAL:
        return "NORMAL";
    case POISON:
        return "POISON";
    case ELECTRIC:
        return "ELECTRIC";
    case GROUND:
        return "GROUND";
    case FAIRY:
        return "FAIRY";
    case FIGHTING:
        return "FIGHTING";
    case PSYCHIC:
        return "PSYCHIC";
    case ROCK:
        return "ROCK";
    case GHOST:
        return "GHOST";
    case DRAGON:
        return "DRAGON";
    case ICE:
        return "ICE";
    default:
        return "UNKNOWN";
    }
}

// --------------------------------------------------------------
// Utility: getDynamicInput (for reading a line into malloc'd memory)
// --------------------------------------------------------------
char *getDynamicInput()
{
    char *input = NULL;
    size_t size = 0, capacity = 1;
    input = (char *)malloc(capacity);
    if (!input)
    {
        printf("Memory allocation failed.\n");
        return NULL;
    }

    int c;
    while ((c = getchar()) != '\n' && c != EOF)
    {
        if (size + 1 >= capacity)
        {
            capacity *= 2;
            char *temp = (char *)realloc(input, capacity);
            if (!temp)
            {
                printf("Memory reallocation failed.\n");
                free(input);
                return NULL;
            }
            input = temp;
        }
        input[size++] = (char)c;
    }
    input[size] = '\0';

    // Trim any leading/trailing whitespace or carriage returns
    trimWhitespace(input);

    return input;
}

// Function to print a single Pokemon node
void printPokemonNode(PokemonNode *node)
{
    if (!node)
        return;
    printf("ID: %d, Name: %s, Type: %s, HP: %d, Attack: %d, Can Evolve: %s\n",
           node->data->id,
           node->data->name,
           getTypeName(node->data->TYPE),
           node->data->hp,
           node->data->attack,
           (node->data->CAN_EVOLVE == CAN_EVOLVE) ? "Yes" : "No");
}

// --------------------------------------------------------------
// Display Menu
// --------------------------------------------------------------
void displayMenu(OwnerNode *owner)
{
    if (!owner->pokedexRoot)
    {
        printf("Pokedex is empty.\n");
        return;
    }

    printf("Display:\n");
    printf("1. BFS (Level-Order)\n");
    printf("2. Pre-Order\n");
    printf("3. In-Order\n");
    printf("4. Post-Order\n");
    printf("5. Alphabetical (by name)\n");

    int choice = readIntSafe("Your choice: ");

    switch (choice)
    {
    case 1:
        displayBFS(owner->pokedexRoot);
        break;
    case 2:
        preOrderTraversal(owner->pokedexRoot);
        break;
    case 3:
        inOrderTraversal(owner->pokedexRoot);
        break;
    case 4:
        postOrderTraversal(owner->pokedexRoot);
        break;
    case 5:
        displayAlphabetical(owner->pokedexRoot);
        break;
    default:
        printf("Invalid choice.\n");
    }
}

// --------------------------------------------------------------
// Sub-menu for existing Pokedex
// --------------------------------------------------------------
void enterExistingPokedexMenu()
{
    if (ownerHead == NULL){
        printf("No existing Pokedexes.\n");
        return;
    }
    // list owners
    printf("\nExisting Pokedexes:\n");
    int counter = 1;
    OwnerNode *iterator = ownerHead;
    do {
        printf("%d. %s\n",counter,iterator->ownerName);
        iterator = iterator->next;
        counter++;
    }
    while (iterator != ownerHead);
    int chosenPokedex = readIntSafe("Choose a Pokedex by number: ");
    OwnerNode *cur = ownerHead;
    counter = 0;
    chosenPokedex--; //adjust it because we are starting from the head
    //starting loop that will iterate through the list as the number of the index of desired owner
    while (counter<chosenPokedex){
        cur = cur->next;
        counter++;
    }

    printf("\nEntering %s's Pokedex...\n", cur->ownerName);

    int subChoice;
    do
    {
        printf("\n-- %s's Pokedex Menu --\n", cur->ownerName);
        printf("1. Add Pokemon\n");
        printf("2. Display Pokedex\n");
        printf("3. Release Pokemon (by ID)\n");
        printf("4. Pokemon Fight!\n");
        printf("5. Evolve Pokemon\n");
        printf("6. Back to Main\n");

        subChoice = readIntSafe("Your choice: ");

        switch (subChoice)
        {
        case 1:
            addPokemon(cur);
            break;
        case 2:
            displayMenu(cur);
            break;
        case 3:
            freePokemon(cur);
            break;
        case 4:
            pokemonFight(cur);
            break;
        case 5:
            evolvePokemon(cur);
            break;
        case 6:
            printf("Back to Main Menu.\n");
            break;
        default:
            printf("Invalid choice.\n");
        }
    } while (subChoice != 6);
}
//______evolve pokemon_____//
void evolvePokemon(OwnerNode *owner){
    //first condition
    if (owner->pokedexRoot == NULL){
        printf("Cannot evolve. Pokedex empty.\n");
        return;
    }
    //getting the desired id
    int idToEvolve = readIntSafe("Enter ID of Pokemon to evolve: ");
    //searching for it to determine if its already exists in the pokedex
    PokemonNode *pokemonToEvolve = searchPokemonBFS(owner->pokedexRoot,idToEvolve);
    if (pokemonToEvolve == NULL){
        printf("No Pokemon with ID %d found.\n", idToEvolve);
        return;
    }
    //getting 0 or 1 from the enum of the evolve condition
    int canEvolve = pokemonToEvolve->data->CAN_EVOLVE;
    if (!canEvolve){
        //returning if it cannot evolve
        printf("%s (ID %d) cannot evolve.",pokemonToEvolve->data->name,idToEvolve);
        return;
    }
    //if we got here it means our pokemon can evolve. just need to check if the evolved version is
    //already in our tree.
    int newId = idToEvolve+1;
    //checking if we have the evolved version
    PokemonNode* evolvedPokemon = searchPokemonBFS(owner->pokedexRoot,newId);
    //if we didnt get null it means the evolved version exists in the pokedex
    if (evolvedPokemon != NULL){
        printf("Evolution ID %d (%s) already in the Pokedex. ",newId,evolvedPokemon->data->name);
        printf("Releasing %s (ID %d).",pokemonToEvolve->data->name,idToEvolve);
        //removing the current one from the tree (the unevolved)
        owner->pokedexRoot = removePokemonByID(owner->pokedexRoot,idToEvolve);
        return;
    }
    else {
        //means evolved pokemon is null, we will create it
        evolvedPokemon = createPokemonNode(pokedex+idToEvolve);//place in array for the evolved version
        printf("Removing Pokemon %s (ID %d).\n",pokemonToEvolve->data->name,idToEvolve);
        printf("Pokemon evolved from %s (ID %d) to %s (ID %d).\n",pokemonToEvolve->data->name,idToEvolve,
        evolvedPokemon->data->name,newId);
        pokemonToEvolve->data = evolvedPokemon->data;//changing the data to the evolved version
        return;
    }

}

//______pokemon fight_____//
void pokemonFight(OwnerNode *owner){
    if (owner->pokedexRoot==NULL){
        printf("Pokedex is empty.\n");
        return;
    }
    int firstId, secondId;
    firstId = readIntSafe("Enter ID of the first Pokemon: ");
    secondId = readIntSafe("Enter ID of the second Pokemon: ");
    PokemonNode* firstPokemon =searchPokemonBFS(owner->pokedexRoot,firstId);
    PokemonNode* secondPokemon = searchPokemonBFS(owner->pokedexRoot,secondId);
    if (secondPokemon == NULL || firstPokemon == NULL){
        printf("One or both Pokemon IDs not found.\n");
        return;
    }
    float firstPokemonScore = ((firstPokemon->data->attack * 1.5) + (firstPokemon->data->hp * 1.2));
    float secondPokemonScore = ((secondPokemon->data->attack * 1.5) + (secondPokemon->data->hp * 1.2));
    printf("Pokemon 1: %s (Score = %.2f)\n",firstPokemon->data->name,firstPokemonScore);
    printf("Pokemon 2: %s (Score = %.2f)\n",secondPokemon->data->name,secondPokemonScore);
    if (firstPokemonScore > secondPokemonScore){
        printf("%s wins!\n",firstPokemon->data->name);
        return;
    }
    else if (secondPokemonScore>firstPokemonScore){
        printf("%s wins!\n",secondPokemon->data->name);
        return;
    }
    else {
        printf("It's a tie!\n");
        return;
    }

}

//__________deleting pokemon________//
PokemonNode *findParent(PokemonNode *root, PokemonNode *kid){
    //if we have invalid inputs or if the kid is actually the main root of the tree, we return null 
    if (root == NULL || kid==NULL || root == kid) return NULL;
    if (root->left == kid || root->right == kid) return root;
    if (kid->data->id > root->data->id){
        return findParent(root->right,kid);
    }
    else return findParent(root->left,kid);
}

PokemonNode *findMin(PokemonNode *root){
    while (root && root->left){
        root = root->left;
    }
    return root;
}

void freePokemon(OwnerNode *owner){
    if (owner->pokedexRoot == NULL){
        printf("No Pokemon to release.\n");
        return;
    }
    int chosenId = readIntSafe("Enter Pokemon ID to release: ");
    PokemonNode* nodeToDelete = searchPokemonBFS(owner->pokedexRoot,chosenId);
    if (!nodeToDelete){
        printf("No Pokemon with ID %d found.\n",chosenId);
        return;
    }
    char* nodeToDeleteName = nodeToDelete->data->name;
    owner->pokedexRoot = removePokemonByID(owner->pokedexRoot,chosenId);
    printf("Removing Pokemon %s (ID %d).\n",nodeToDeleteName,chosenId);
    return;
}

PokemonNode *removePokemonByID(PokemonNode *root, int id){
    //extracting the pointer to the node with the desired id
    PokemonNode* nodeToDelete = searchPokemonBFS(root,id);
    //if we recicved null that means no pokemon was found, returning the same root
    if (nodeToDelete == NULL){
        return root;
    }
    PokemonNode *parent = findParent(root,nodeToDelete);
    //no children:
    if (nodeToDelete->left == NULL && nodeToDelete->right == NULL){
        //checking if parent is null. if so it means its our root.
        if (parent == NULL){
            free(nodeToDelete);
            return NULL;
        }
        //checking if it was a left children to the parent
        if (parent->left == nodeToDelete){
            parent->left = NULL;
        }
        else {
            parent->right = NULL;
        }
        free(nodeToDelete);
        return root;
    }
    //has only left child
    if (nodeToDelete->left!=NULL && nodeToDelete->right == NULL){
        //if our node is the root
        if (parent == NULL){
            PokemonNode *newestRoot = nodeToDelete->left;
            free(nodeToDelete);
            return newestRoot;
        }
        //checking if nodeToDelete is a left or right child himself
        if (parent->left == nodeToDelete){
            parent->left = nodeToDelete->left;
        }
        else {
            parent->right = nodeToDelete->left;
        }
        free(nodeToDelete);
        return root;
    }
    //has only right child
    if (nodeToDelete->left==NULL && nodeToDelete->right != NULL){
        //if the node is the root we need to make his child the new root
        if (parent == NULL){
            PokemonNode *newestRoot = nodeToDelete->right;
            free(nodeToDelete);
            return newestRoot;
        }
        //checking if nodeToDelete is a left or right child himself
        if (parent->left == nodeToDelete){
            parent->left = nodeToDelete->right;
        }
        else {
            parent->right = nodeToDelete->right;
        }
        free(nodeToDelete);
        return root;
    }
    //has 2 children. find the minimum of the ids bigger than him, then recursively update the sub-tree
    if (nodeToDelete->left !=NULL && nodeToDelete->right!=NULL){
        //finding the successor, smallest one that bigger from our node
        PokemonNode *successor = findMin(nodeToDelete->right);
        //taking his data to replace the nodetoDelete data
        nodeToDelete->data = successor->data;
        //now recursivally we update the right subtree of the node to delete, in case our successor had a child
        nodeToDelete->right = removePokemonByID(nodeToDelete->right,successor->data->id);
        return root;
    }
    //got an warning but its okay we will never reach here
    return root;
}

// _________Queue functions________//
queueNode* createNode(PokemonNode* pokeNode){
   //function that creates queueNode with a givwn pokemon node to point at
   queueNode *node = (queueNode*)malloc(sizeof(queueNode));
   node->next = NULL;
   node->pokeNode = pokeNode;
   return node;
}

pokemonQueue* createPokemonQueue(){
   //creating queue of pokemon nodes pointers
   pokemonQueue* pokeQueue = (pokemonQueue*)malloc(sizeof(pokemonQueue));
   pokeQueue->front = NULL;
   pokeQueue->rear = NULL;
   return pokeQueue;
}

int isQueueEmpty(pokemonQueue* queue){
   //if our front is empty its certainly means all of our queue is empty
   return (queue->front == NULL);
}

void addToQueue(pokemonQueue *pokeQueue, PokemonNode* pokeNode){
   //adding new queueNode to the queue with a given pokeNode. first creating the node, pointing at the PokemonNode
   queueNode* node = createNode(pokeNode);
   //if the rear is null so the queue is empty. which means this current item will be the first and the last
   if (pokeQueue->rear == NULL){
      pokeQueue->front = node;
      pokeQueue->rear = node;
      return;
   }
   //if the queue isnt empty, the current 'last' item will be the second the last, and our new item will be the last
   pokeQueue->rear->next = node;
   pokeQueue->rear = node;
}

PokemonNode* removeFromQueue(pokemonQueue *queue){
   //extracting the current pokemon node "to be served", removing from the queue and freeing the pointer
   //if the queue is empty we have no pokemon node to extract
   if (isQueueEmpty(queue)){
      return NULL;
   }
   //creating a pointer to the current first in line item.
   queueNode *node = queue->front;
   //making the second item to be the first
   queue->front = queue->front->next;
   //if the second item doesnt exist, which means we had only one item in the queue, we are making the rear null
   //because now the queue is empty
   if (queue->front == NULL) queue->rear = NULL;
   //extracting the data AKA the pokemon node
   PokemonNode* pokeNode = node->pokeNode;
   //freeing the pointer that was created in the createNode function
   free(node);
   return pokeNode;
}

void BFSGeneric(PokemonNode *root, VisitNodeFunc visit){
    //based on queue, bfs generic visit every node in the tree.
   pokemonQueue* queue = createPokemonQueue();
   //first item in the queue is the root
   addToQueue(queue,root);
   //as long as we have items in the queue, AKA items to implement the visit function on
   while (!isQueueEmpty(queue)){
    //extracting the pokemon node first in the queue
      PokemonNode* current = removeFromQueue(queue);
      //implementing whatever visit function we need
      visit(current);
      //adding the left side of the last extracted node
      if (current->left != NULL){
         addToQueue(queue,current->left);
      }
      //adding the right side of the last extracted node
      if (current->right != NULL){
         addToQueue(queue,current->right);
      }
   }
   //clearly also freeing the queue itself
   free(queue);
}

//Displays

//Alphabetically

NodeArray *initNodeArray(){
   NodeArray* na = malloc(sizeof(NodeArray));
   if (na == NULL){
      exit(1);
   }
   na->size=0;
   na->capacity = 1;
   na->nodes = malloc(na->capacity*sizeof(PokemonNode*));
   if (na->nodes == NULL){
      exit(1);
   }
   return na;
}

void addNode(NodeArray *na, PokemonNode *node){
   //using capacity to decrement frequency of reallocing. if we recahed the capaicty we doubling it by 2
   //and reallocing
   if (na->size >= na->capacity){
      na->capacity*=2;
      na->nodes = realloc(na->nodes,sizeof(PokemonNode*)*na->capacity);
      if (!na->nodes){
         exit(1);
      }
   }
   //putting the pokemon node in the current place in the array
   na->nodes[na->size]=node;
   na->size++;
}

void collectAll(PokemonNode *root, NodeArray *na){
   if (root==NULL) return;
   addNode(na,root);
   collectAll(root->left,na);
   collectAll(root->right,na);
}

int compareByNameNode(const void *a, const void *b){
   //a and b are pointers to pointers to nodes. we must cast them 
   const PokemonNode *nodeA = *(const PokemonNode**)a;
   const PokemonNode *nodeB = *(const PokemonNode**)b;

   return strcmp(nodeA->data->name,nodeB->data->name);
}

void displayAlphabetical(PokemonNode *root){
   if (root==NULL){
      printf("Pokedex is empty.\n");
      return;
   }
   NodeArray* na = initNodeArray();
   collectAll(root,na);
   qsort(na->nodes,na->size,sizeof(PokemonNode*),compareByNameNode);
   for (int i=0; i<na->size;i++){
      printPokemonNode(na->nodes[i]);
   }
   free(na->nodes);
   free(na);
}

void displayBFS(PokemonNode *root){
    if (root==NULL){
      printf("Pokedex is empty.\n");
      return;
   }
    BFSGeneric(root, printPokemonNode);
}

void preOrderGeneric(PokemonNode *root, VisitNodeFunc visit){
    //applying visit in the preOrder way- root,left,right
    if (root == NULL) return;
    visit(root);
    preOrderGeneric(root->left,visit);
    preOrderGeneric(root->right,visit);
}

void preOrderTraversal(PokemonNode *root){
    if (root==NULL){
      printf("Pokedex is empty.\n");
      return;
   }
    //printing in preOrder
    preOrderGeneric(root,printPokemonNode);
}

void inOrderGeneric(PokemonNode *root, VisitNodeFunc visit){
    //applying visit in the inOrder way- Left-Root-Right
    if (root == NULL) return;
    inOrderGeneric(root->left,visit);
    visit(root);
    inOrderGeneric(root->right,visit);
}

void inOrderTraversal(PokemonNode *root){
    if (root==NULL){
      printf("Pokedex is empty.\n");
      return;
   }
    //printing in Order
    inOrderGeneric(root,printPokemonNode);
}

void postOrderGeneric(PokemonNode *root, VisitNodeFunc visit){
    //applying visit in the inOrder way- Left-Right-Root
    if (root == NULL) return;
    postOrderGeneric(root->left,visit);
    postOrderGeneric(root->right,visit);
    visit(root);
}


void postOrderTraversal(PokemonNode *root){
    if (root==NULL){
      printf("Pokedex is empty.\n");
      return;
   }
    postOrderGeneric(root,printPokemonNode);
}

void addPokemon(OwnerNode *owner){
    int chosenId = readIntSafe("Enter ID to add: ");
    if (searchPokemonBFS(owner->pokedexRoot,chosenId) != NULL){
        printf("Pokemon with ID %d is already in the Pokedex. No changes made.", chosenId);
        return;
    }
    PokemonNode *newPokemon = createPokemonNode(pokedex+(chosenId-1));
    if (owner->pokedexRoot==NULL){
        owner->pokedexRoot = newPokemon;
    }
    else insertPokemonNode(owner->pokedexRoot,newPokemon);
    printf("Pokemon %s (ID %d) added.\n",newPokemon->data->name,newPokemon->data->id);
}

PokemonNode *searchPokemonBFS(PokemonNode *root, int id){
    if (root == NULL) return NULL;
    if (root->data->id > id){
        return searchPokemonBFS(root->left,id);
    }
    if (root->data->id<id){
        return searchPokemonBFS(root->right,id);
    }
    return root;
}

void insertPokemonNode(PokemonNode *root, PokemonNode *newNode){
    if (root == NULL) return;
    if (root->data->id > newNode->data->id){
        if (root->left == NULL){
            root->left = newNode;
            return;
        }
        insertPokemonNode(root->left,newNode);
    }
    if (root->data->id < newNode->data->id){
        if (root->right == NULL){
            root->right = newNode;
            return;
        }
        insertPokemonNode(root->right,newNode);
    }
}

// --------------------------------------------------------------
// Main Menu
// --------------------------------------------------------------
void mainMenu()
{
    int choice;
    do
    {
        printf("\n=== Main Menu ===\n");
        printf("1. New Pokedex\n");
        printf("2. Existing Pokedex\n");
        printf("3. Delete a Pokedex\n");
        printf("4. Merge Pokedexes\n");
        printf("5. Sort Owners by Name\n");
        printf("6. Print Owners in a direction X times\n");
        printf("7. Exit\n");
        choice = readIntSafe("Your choice: ");

        switch (choice)
        {
        case 1:
            openPokedexMenu();
            break;
        case 2:
            enterExistingPokedexMenu();
            break;
        case 3:
            deletePokedex();
            break;
        case 4:
            mergePokedexMenu();
            break;
        case 5:
            sortOwners();
            break;
        case 6:
            printOwnersCircular();
            break;
        case 7:
            printf("Goodbye!\n");
            break;
        default:
            printf("Invalid.\n");
        }
    } while (choice != 7);
}

//_____free all owners____//

void freeOwnerNode(OwnerNode *owner){
    freePokemonTree(owner->pokedexRoot);
    free(owner->ownerName);
    free(owner);
    owner = NULL;
    return;
}
void freeAllOwners(){
    if (ownerHead==NULL){
        //can return we have nothing to delete
        return;
    }
    OwnerNode *iterator = ownerHead; //iterator to delete each time
    OwnerNode* nextToDelete; //next owner to be deleted
    
    do {
        nextToDelete = iterator->next; //getting the pointer of the next to be deleted
        freeOwnerNode(iterator); //deleting the current one
        iterator = nextToDelete; //moving forward to the next owner in the list
    }
    while (iterator!=ownerHead); //stopping if we reached the end

    ownerHead = NULL; //setting to null 
}

//__________print owners circular___//
void printOwnersCircular(){

    //check ownerHead isnt null
    if (ownerHead == NULL){
        printf("No owners.\n");
        return;
    }

    printf("Enter direction (F or B): ");
    char* choice = getDynamicInput();
    //using or operator to validate the we recived a valid input out of the 4 we recieved
    int comparationValue = (strcmp(choice,"F")==0|| strcmp(choice,"f")==0||
     strcmp(choice,"b")==0|| strcmp(choice,"B")==0);
    while (!comparationValue){
        free(choice);
        printf("Invalid direction, must be L or R.\n");
        printf("Enter direction (F or B):");
        choice = getDynamicInput();
        comparationValue = (strcmp(choice,"F")==0|| strcmp(choice,"f")==0||
        strcmp(choice,"b")==0|| strcmp(choice,"B")==0);
    }
    int printNum = readIntSafe("How many prints? ");
    OwnerNode* iterator = ownerHead;
    //printing forward
    if (strcmp(choice,"f")==0 || strcmp(choice,"F")==0){
        for (int i=0;i<printNum;i++){
            printf("[%d] %s\n",i+1,iterator->ownerName);
            iterator = iterator->next;
        }
    }
    else if (strcmp(choice,"b")==0 || strcmp(choice,"B")==0){
        for (int i=0;i<printNum;i++){
            printf("[%d] %s\n",i+1,iterator->ownerName);
            iterator = iterator->prev;
        }
    }
    free(choice);
    return;
}

//______merge owners______//
void mergePokedexMenu(){
    if (ownerHead == NULL || ownerHead->next == ownerHead){
        printf("Not enough owners to merge.\n");
        return;
    }
    printf("\n=== Merge Pokedexes ===\n");
    printf("Enter name of first owner: ");
    char *firstName = getDynamicInput();
    printf("Enter name of second owner: ");
    char *secondName = getDynamicInput();
    //getting the owner nodes of the desired owners
    OwnerNode *firstOwner = findOwnerByName(firstName);
    OwnerNode *secondOwner = findOwnerByName(secondName);
    if (firstOwner !=NULL && secondOwner != NULL){
        // printf("got the 2 pointers of %s and %s",firstOwner->ownerName,secondOwner->ownerName);
    }
    //printing message of merging start 
    printf("Merging %s and %s...\n",firstName,secondName);
    //taking the pokemon from the second owner to the first owner pokedex root
    BFSMerge(secondOwner->pokedexRoot,firstOwner->pokedexRoot);
    printf("Merge completed.\n");
    //using the func that deletes the owner, frees it used data from the heap and remove it from the linked list
    deletePokedexByAdress(secondOwner);
    printf("Owner '%s' has been removed after merging.\n",secondName);
    free(firstName);
    free(secondName);
    return;
}


void BFSMerge(PokemonNode *source, PokemonNode *dest){
    // printf("started the BFSMerge function\n");
    //based on queue, bfs generic visit every node in the tree.
   pokemonQueue* queue = createPokemonQueue();
//    printf("created queue\n");
   //first item in the queue is the root
   addToQueue(queue,source);
//    printf("adding the root to queue\n");
   //as long as we have items in the queue, AKA items to implement the visit function on
   while (!isQueueEmpty(queue)){
    //extracting the pokemon node first in the queue
      PokemonNode* current = removeFromQueue(queue);
    //   printf("removing %s from queue\n",current->data->name);
      //checking if the pokemon already exists in the destination owner. if not, we will insert it
      PokemonNode* isExist = searchPokemonBFS(dest,current->data->id);
      if (isExist == NULL){
        // printf("pokemon %s doesnt exist\n",current->data->name);
        PokemonNode* newPokemon = createPokemonNode(current->data);
        insertPokemonNode(dest,newPokemon);
        // printf("inserted %s to the dest root\n",current->data->name);
      }
      //adding the left side of the last extracted node
      if (current->left != NULL){
         addToQueue(queue,current->left);
      }
      //adding the right side of the last extracted node
      if (current->right != NULL){
         addToQueue(queue,current->right);
      }
   }
   //clearly also freeing the queue itself
   free(queue);
}

//_____sort owners_______//
void sortOwners(){
    //first checking if we have only 1 or 0 owners, if so we dont need to sort
    if (ownerHead == NULL || ownerHead->next == ownerHead){
        printf("0 or 1 owners only => no need to sort.\n");
        return;
    }
    //we will use bubble sort, first we will indicate the size of the linked list
    int numOfOwners=0;
    OwnerNode* iterator= ownerHead;//starting from the head
    do {
        numOfOwners+=1;//incremnting size by one
        iterator = iterator->next; //moving to the next one
    } while (iterator!=ownerHead); //stopping if we reached the head, aka we finished the list

    //sorting
    for (int i =0; i<numOfOwners-1;i++){
        iterator = ownerHead; //each outer iteration we start from the current owner head
        for (int j=0;j<numOfOwners-1-i;j++){
            OwnerNode* nextIterator = iterator->next; //getting the adjacent iterator 
            if (strcmp(iterator->ownerName,nextIterator->ownerName)>0){
                PokemonNode* iteratorRoot = iterator->pokedexRoot;
                char* iteratorName = iterator->ownerName;

                //taking the data of the iterator-> next into iterator
                iterator->ownerName = nextIterator->ownerName;
                iterator->pokedexRoot = nextIterator->pokedexRoot;

                //taking the data of the iterator from the temps into the iterator->next
                nextIterator->ownerName = iteratorName;
                nextIterator->pokedexRoot = iteratorRoot;
            }
            iterator = iterator->next; //moving to the next item in the list
        }
    }
    printf("Owners sorted by name.\n");
}

//_______Delete pokedex______//
void deletePokedex(){
    if (ownerHead == NULL){
        printf("No existing Pokedexes to delete.\n");
        return;
    }
    printf("\n=== Delete a Pokedex ===\n");
    int counter = 1;
    OwnerNode *iterator = ownerHead;
    do {
        printf("%d. %s\n",counter,iterator->ownerName);
        iterator = iterator->next;
        counter++;
    }
    while (iterator != ownerHead);
    int ownerIndexToDelete = readIntSafe("Choose a Pokedex to delete by number: ");
    OwnerNode *cur = ownerHead;
    counter = 0;
    ownerIndexToDelete--; //adjust it because we are starting from the head
    //starting loop that will iterate through the list as the number of the index of desired owner
    while (counter<ownerIndexToDelete){
        cur = cur->next;
        counter++;
    }
    printf("Deleting %s's entire Pokedex...\n",cur->ownerName);
    //first we will remove it from the circular list
    removeOwnerFromCircularList(cur);
    //now we will use the generic BST traversals to free all the pokemon nodes
    freePokemonTree(cur->pokedexRoot);
    free(cur->ownerName);
    free(cur);
    printf("Pokedex deleted.\n");
    return;
}

void deletePokedexByAdress(OwnerNode* owner){
    removeOwnerFromCircularList(owner);
    freePokemonTree(owner->pokedexRoot);
    free(owner->ownerName);
    free(owner);
    return;
}

void freePokemonTree(PokemonNode *root){
    //recieves the owner root and free all the pokemons
    postOrderGeneric(root,freePokemonNode);
    //now we need to free the root itself
    // free(root);
}

void freePokemonNode(PokemonNode *node){
    free(node);
}

void removeOwnerFromCircularList(OwnerNode *target){
    //checking if the traget is our only owner, if so we will adjust the owner head to be null
    if (target == ownerHead && ownerHead->next == ownerHead){
        ownerHead = NULL;
        return;
    }
    
    //the owners before and after the removed one will point at each other
    target->prev->next = target->next;
    target->next->prev = target->prev;
    
    
    //if our target was the ownerhead but it wasnt the only owner, we will update the head to next in line
    if (target == ownerHead){
        ownerHead = target->next;
    }
    }


OwnerNode *findOwnerByName(const char *name){
    if (ownerHead ==NULL) return NULL;
    //starting from the head of the owners list
    OwnerNode *current = ownerHead;
    
    do {
        if (strcmp(name,current->ownerName)==0) return current; //checking equivalnce in each node name and our input
        else current = current->next; //if we didnt find the name, going to the next node
    }
    while (current != ownerHead);
    return NULL; //if we reached here it means we didnt find the name.
}

PokemonNode *createPokemonNode(const PokemonData *data){
    PokemonNode *newPokemon = malloc(sizeof(PokemonNode));
    if (newPokemon == NULL) exit(1);
    
    newPokemon->data =(PokemonData*) data;
    newPokemon->left= NULL;
    newPokemon->right = NULL;
    return newPokemon;
}

OwnerNode *createOwner(char *ownerName, PokemonNode *starter){
    OwnerNode *newOwner = malloc(sizeof(OwnerNode));
    if (newOwner == NULL) exit(1);
    newOwner->ownerName = ownerName;
    newOwner->pokedexRoot = starter;
    newOwner->next = NULL;
    newOwner->prev = NULL;

    return newOwner;
}

void linkOwnerInCircularList(OwnerNode *newOwner){
    if (ownerHead == NULL){
        ownerHead = newOwner;
        newOwner->next = newOwner;
        newOwner->prev = newOwner;
        return;
    }
    else {OwnerNode *iterator = ownerHead;
    //moving down the least until we find the last one, hence the one pointing to the head
    while (iterator->next != ownerHead){
        iterator = iterator->next;
    }
    newOwner->next = ownerHead;
    newOwner->prev = iterator;
    iterator->next = newOwner;
    ownerHead->prev = newOwner;}
}

void openPokedexMenu(){
    printf("Your name: ");
    char *chosenName = getDynamicInput();
    if (findOwnerByName(chosenName)){
        printf("Owner '%s' already exists. Not creating a new Pokedex.\n",chosenName);
        free(chosenName);
        return;
    }
    int starter = readIntSafe("Choose Starter:\n1. Bulbasaur\n2. Charmander\n3. Squirtle\nYour choice: ");
    starter = (starter-1)*3;
    PokemonNode *newPokemon = createPokemonNode(pokedex+starter);
    OwnerNode *newOwner = createOwner(chosenName,newPokemon);
    linkOwnerInCircularList(newOwner);
    printf("New Pokedex created for %s with starter %s.\n",chosenName,newPokemon->data->name);
    free(chosenName);
}

int main()
{
    mainMenu();
    freeAllOwners();
    return 0;
}
