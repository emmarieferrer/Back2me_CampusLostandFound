/*
 * ================================================
 *   Back2me: Campus Lost & Found System
 * ================================================
 *
 * DATA STRUCTURES USED:
 *   1. Hash Table  - stores FOUND items (fast O(1) lookup by ID)
 *   2. Linked List - stores LOST items  (dynamic insertion at head)
 *   3. Queue       - claim processing   (FIFO order)
 *   4. Graph       - campus map         (adjacency matrix for Dijkstra)
 *
 * ALGORITHMS USED:
 *   1. Searching   - keyword + ID search across Hash Table and Linked List
 *   2. Dijkstra    - shortest path between campus locations
 *
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

/* 
   CONSTANTS
    */
#define TABLE_SIZE 10   // Hash table size
#define MAX        10   // Max queue capacity
#define LOC         8   // Number of campus locations

/* 
   DATA STRUCTURE 1: LINKED LIST NODE
   Used to store LOST items
 */
typedef struct Item {
    int  id;
    char desc[100];
    int  location;
    struct Item* next;   // pointer to next node in linked list
} Item;

/* 
   DATA STRUCTURE 1: LINKED LIST
   Head pointer for the Lost Items linked list
   */
Item* lostHead = NULL;

/*
   DATA STRUCTURE 2: HASH TABLE
   Array of linked list chains - stores FOUND items
   Hash function: index = id % TABLE_SIZE
    */
Item* foundTable[TABLE_SIZE] = {NULL};

/* 
   DATA STRUCTURE 3: QUEUE
   Used for claim processing (FIFO)
   front = index of first claim to process
   rear  = index of last claim added
    */
typedef struct {
    int data[MAX];
    int front, rear;
} Queue;

Queue q;

/* 
   DATA STRUCTURE 4: GRAPH
   Adjacency matrix representing the campus map
   graph[i][j] = distance between location i and j
   0 means no direct connection
   Used by Dijkstra shortest path algorithm
   */

// Location index labels
char* locations[LOC] = {
    "Cultural Hall",     // 0
    "Library",           // 1
    "Food Court",        // 2
    "IT Building",       // 3
    "Registrar Office",  // 4
    "Gymnasium",         // 5
    "OSAS Office",       // 6
    "Gate"               // 7
};

// Adjacency matrix (symmetric - undirected graph)
int graph[LOC][LOC] = {
    /* 0  1  2  3  4  5  6  7 */
    {  0, 2, 4, 0, 0, 0, 0, 7 },  // 0 Cultural Hall
    {  2, 0, 1, 3, 0, 0, 0, 0 },  // 1 Library
    {  4, 1, 0, 2, 5, 0, 0, 0 },  // 2 Food Court
    {  0, 3, 2, 0, 3, 6, 0, 0 },  // 3 IT Building
    {  0, 0, 5, 3, 0, 2, 4, 0 },  // 4 Registrar Office
    {  0, 0, 0, 6, 2, 0, 1, 3 },  // 5 Gymnasium
    {  0, 0, 0, 0, 4, 1, 0, 2 },  // 6 OSAS Office
    {  7, 0, 0, 0, 0, 3, 2, 0 }   // 7 Gate
};

/* ------------------------------------------------
   FUNCTION PROTOTYPES
   ------------------------------------------------ */
int  getInt();
void showLocations();
int  hash(int key);
void addLost();
void viewLost();
void addFound();
void viewFound();
void searchItem();
void enqueue(int id);
void markClaimed();
void processClaim();
int  minDist(int dist[], int inN[]);
void dijkstra(int src);
void cleanup();

/* ------------------------------------------------
   UTILITY: Safe integer input
   ------------------------------------------------ */
int getInt() {
    int x;
    while (scanf("%d", &x) != 1) {
        printf("Invalid! Enter a number: ");
        while (getchar() != '\n');
    }
    return x;
}

/* ------------------------------------------------
   UTILITY: Display all campus locations
   ------------------------------------------------ */
void showLocations() {
    int i;
    printf("\n--- CAMPUS LOCATIONS ---\n");
    for (i = 0; i < LOC; i++)
        printf("  %d. %s\n", i, locations[i]);
    printf("------------------------\n");
}

/* ================================================
   DATA STRUCTURE 2: HASH TABLE
   Hash Function - modulo division
   Maps item ID to a table index
   ================================================ */
int hash(int key) {
    return key % TABLE_SIZE;
}

/* ================================================
   DATA STRUCTURE 1: LINKED LIST
   Add a new LOST item (insert at head)
   ================================================ */
void addLost() {
    Item* newItem = malloc(sizeof(Item));
    if (!newItem) {
        printf("Memory allocation failed!\n");
        return;
    }

    printf("\n--- ADD LOST ITEM ---\n");
    printf("Enter Lost Item ID: ");
    newItem->id = getInt();

    while (getchar() != '\n');

    printf("Item Name: ");
    fgets(newItem->desc, 100, stdin);
    newItem->desc[strcspn(newItem->desc, "\n")] = 0;

    showLocations();
    printf("Enter location number (0-7): ");
    newItem->location = getInt();

    if (newItem->location < 0 || newItem->location >= LOC) {
        printf("Invalid location! Setting to Cultural Hall (0).\n");
        newItem->location = 0;
    }

    // Insert at head of linked list
    newItem->next = lostHead;
    lostHead = newItem;

    printf("Lost item recorded successfully!\n");
}

/* ================================================
   DATA STRUCTURE 1: LINKED LIST
   Traverse and display all LOST items
   ================================================ */
void viewLost() {
    Item* temp;
    int count = 1;

    printf("\n--- LOST ITEMS (Linked List) ---\n");
    temp = lostHead;

    if (!temp) {
        printf("No lost items in the system.\n");
        return;
    }

    while (temp) {
        printf("\n[%d] ID:          %d\n",   count, temp->id);
        printf("     Item Name: %s\n",     temp->desc);
        printf("     Lost at:     %s\n",     locations[temp->location]);
        printf("     ---\n");
        temp = temp->next;
        count++;
    }
}

/* ================================================
   DATA STRUCTURE 2: HASH TABLE
   Add a new FOUND item
   Uses hash function to find the correct bucket
   Handles collisions via chaining (linked list)
   ================================================ */
void addFound() {
    Item* newItem = malloc(sizeof(Item));
    int index;

    if (!newItem) {
        printf("Memory allocation failed!\n");
        return;
    }

    printf("\n--- ADD FOUND ITEM ---\n");
    printf("Enter Found Item ID: ");
    newItem->id = getInt();

    while (getchar() != '\n');

    printf("Item Name: ");
    fgets(newItem->desc, 100, stdin);
    newItem->desc[strcspn(newItem->desc, "\n")] = 0;

    showLocations();
    printf("Enter location number (0-7): ");
    newItem->location = getInt();

    if (newItem->location < 0 || newItem->location >= LOC) {
        printf("Invalid location! Setting to Cultural Hall (0).\n");
        newItem->location = 0;
    }

    // Hash the ID to find the bucket, insert at chain head
    index = hash(newItem->id);
    newItem->next = foundTable[index];
    foundTable[index] = newItem;

    printf("Found item added to hash table (index %d).\n", index);
}

/* ================================================
   DATA STRUCTURE 2: HASH TABLE
   Iterate through all buckets and display FOUND items
   ================================================ */
void viewFound() {
    int i;
    int empty = 1;
    Item* temp;

    printf("\n--- FOUND ITEMS (Hash Table) ---\n");
    for (i = 0; i < TABLE_SIZE; i++) {
        temp = foundTable[i];
        while (temp) {
            printf("\nID:          %d\n",   temp->id);
            printf("Item Name: %s\n",     temp->desc);
            printf("Found at:    %s\n",     locations[temp->location]);
            printf("Hash Index:  %d\n",     i);
            printf("---\n");
            temp = temp->next;
            empty = 0;
        }
    }
    if (empty) printf("No found items in the system.\n");
}

/* ================================================
   ALGORITHM 1: SEARCHING (Keyword / Description Search)
   Searches item name or description in both:
   - Hash Table  (FOUND items): scans all buckets O(n)
   - Linked List (LOST items):  linear traversal   O(n)
   Match is case-insensitive substring search.
   ================================================ */
void searchItem() {
    char keyword[100];
    char descLower[100], kwLower[100];
    int  index, found, j;
    Item* temp;

    printf("\n--- SEARCH ITEM ---\n");
    printf("Enter item name or description keyword: ");
    while (getchar() != '\n');   // clear input buffer
    fgets(keyword, 100, stdin);
    keyword[strcspn(keyword, "\n")] = 0;

    // Convert keyword to lowercase for case-insensitive match
    for (j = 0; keyword[j]; j++)
        kwLower[j] = (char)(keyword[j] >= 'A' && keyword[j] <= 'Z'
                      ? keyword[j] + 32 : keyword[j]);
    kwLower[j] = '\0';

    found = 0;

    // --- Search Hash Table (FOUND items) ---
    printf("\n--- Results in FOUND items ---\n");
    for (index = 0; index < TABLE_SIZE; index++) {
        temp = foundTable[index];
        while (temp) {
            for (j = 0; temp->desc[j]; j++)
                descLower[j] = (char)(temp->desc[j] >= 'A' && temp->desc[j] <= 'Z'
                               ? temp->desc[j] + 32 : temp->desc[j]);
            descLower[j] = '\0';

            if (strstr(descLower, kwLower)) {
                printf("\n  ID:          %d\n", temp->id);
                printf("  Description: %s\n",   temp->desc);
                printf("  Location:    %s\n",   locations[temp->location]);
                printf("  Status:      FOUND - Ready for claiming\n");
                found = 1;
            }
            temp = temp->next;
        }
    }
    if (!found)
        printf("  No matching found items.\n");

    // --- Search Linked List (LOST items) ---
    found = 0;
    printf("\n--- Results in LOST items ---\n");
    temp = lostHead;
    while (temp) {
        for (j = 0; temp->desc[j]; j++)
            descLower[j] = (char)(temp->desc[j] >= 'A' && temp->desc[j] <= 'Z'
                           ? temp->desc[j] + 32 : temp->desc[j]);
        descLower[j] = '\0';

        if (strstr(descLower, kwLower)) {
            printf("\n  ID:          %d\n", temp->id);
            printf("  Description: %s\n",   temp->desc);
            printf("  Lost at:     %s\n",   locations[temp->location]);
            printf("  Status:      LOST - Not yet found\n");
            found = 1;
        }
        temp = temp->next;
    }
    if (!found)
        printf("  No matching lost items.\n");
}

/* ================================================
   DATA STRUCTURE 3: QUEUE
   Enqueue - add a claim to the rear of the queue (FIFO)
   Queue position display starts at index 0
   ================================================ */
void enqueue(int id) {
    if (q.rear == MAX - 1) {
        printf("Claim queue is full! Cannot add more claims.\n");
        return;
    }
    if (q.front == -1) q.front = 0;
    q.rear++;
    q.data[q.rear] = id;
    printf("Item ID %d added to claim queue (Queue index: %d).\n", id, q.rear);
}

/* ================================================
   DATA STRUCTURE 3: QUEUE
   Mark an item for claiming - adds ID to queue rear
   User chooses whether to claim from LOST or FOUND list.
   - FOUND: removes from hash table when processed
   - LOST:  moves from linked list to found/claimed state
   Queue index display starts at 0.
   ================================================ */
void markClaimed() {
    int  choice, id, index, found;
    Item* temp;
    Item* prev;

    printf("\n--- MARK ITEM FOR CLAIM ---\n");
    printf("Claim from:\n");
    printf("  1. Found Items\n");
    printf("  2. Lost Items\n");
    printf("Enter choice: ");
    choice = getInt();

    printf("Enter Item ID to mark as claimed: ");
    id = getInt();
    found = 0;

    if (choice == 1) {
        // Verify item exists in hash table (FOUND items)
        index = hash(id);
        temp  = foundTable[index];
        while (temp) {
            if (temp->id == id) { found = 1; break; }
            temp = temp->next;
        }
        if (!found) {
            printf("Item ID %d not found in FOUND items.\n", id);
            return;
        }
        enqueue(id);

    } else if (choice == 2) {
        // Verify item exists in linked list (LOST items)
        // and move it to the found hash table
        temp = lostHead;
        prev = NULL;
        while (temp) {
            if (temp->id == id) { found = 1; break; }
            prev = temp;
            temp = temp->next;
        }
        if (!found) {
            printf("Item ID %d not found in LOST items.\n", id);
            return;
        }

        // Remove from lost linked list
        if (prev)
            prev->next = temp->next;
        else
            lostHead = temp->next;

        // Move to found hash table so it can be processed
        index       = hash(temp->id);
        temp->next  = foundTable[index];
        foundTable[index] = temp;
        printf("Item ID %d moved from LOST to FOUND list.\n", id);

        enqueue(id);

    } else {
        printf("Invalid choice.\n");
        return;
    }
}

/* ================================================
   DATA STRUCTURE 3: QUEUE
   Process Claim - dequeue from front (FIFO)
   Removes the item from the hash table once claimed
   ================================================ */
void processClaim() {
    int id, index;
    Item* temp;
    Item* prev;

    printf("\n--- PROCESS CLAIM (Queue - FIFO) ---\n");

    // Check if queue is empty
    if (q.front == -1 || q.front > q.rear) {
        printf("No claims to process at this time.\n");
        return;
    }

    // Dequeue from front (FIFO)
    id = q.data[q.front];
    q.front++;
    if (q.front > q.rear) {
        q.front = -1;
        q.rear  = -1;
    }

    // Remove item from hash table
    index = hash(id);
    temp  = foundTable[index];
    prev  = NULL;

    while (temp) {
        if (temp->id == id) {
            if (prev)
                prev->next = temp->next;
            else
                foundTable[index] = temp->next;

            printf("\nCLAIM PROCESSED SUCCESSFULLY!\n");
            printf("  Item ID:     %d\n", temp->id);
            printf("  Item Name: %s\n", temp->desc);
            printf("  Found at:    %s\n", locations[temp->location]);
            printf("  Status:      Item returned to owner\n");
            free(temp);
            return;
        }
        prev = temp;
        temp = temp->next;
    }

    printf("Error: Item ID %d not found. Claim cannot be processed.\n", id);
}

/* ================================================
   ALGORITHM 2: DIJKSTRA'S SHORTEST PATH
   Helper - Step 7: find node NOT in N with smallest label
   Returns -1 if no labelled nodes remain outside N
   ================================================ */
int minDist(int dist[], int inN[]) {
    int min = INT_MAX, idx = -1, i;
    for (i = 0; i < LOC; i++) {
        if (!inN[i] && dist[i] < min) {
            min = dist[i];
            idx = i;
        }
    }
    return idx;
}

/* ================================================
   ALGORITHM 2: DIJKSTRA'S SHORTEST PATH
   Uses the campus Graph (adjacency matrix) to find
   shortest paths from a source to all other locations
   ================================================ */
void dijkstra(int src) {
    int dist[LOC], inN[LOC];
    int i, u, v, newLabel;

    if (src < 0 || src >= LOC) {
        printf("Invalid source location!\n");
        return;
    }

    // STEP 2 & 3: Initialize - all nodes unlabelled, N is empty
    for (i = 0; i < LOC; i++) {
        dist[i] = INT_MAX;  // no label yet
        inN[i]  = 0;        // not in N
    }

    // STEP 3: Label source with 0, insert into N
    dist[src] = 0;
    inN[src]  = 1;
    u = src;

    printf("\n--- SHORTEST PATH FROM %s ---\n", locations[src]);

    // STEP 4: Repeat until no more labelled nodes outside N
    while (1) {

        // STEP 5: Consider each neighbour of u NOT in N
        for (v = 0; v < LOC; v++) {
            if (!inN[v] && graph[u][v] > 0) {
                newLabel = dist[u] + graph[u][v];

                if (dist[v] == INT_MAX) {
                    // STEP 6a: No label yet - assign label
                    dist[v] = newLabel;

                } else if (newLabel < dist[v]) {
                    // STEP 6b: Already labelled - take minimum
                    dist[v] = newLabel;
                }
            }
        }

        // STEP 7: Pick node NOT in N with smallest label
        u = minDist(dist, inN);

        // STEP 4 stop condition: no labelled nodes outside N
        if (u == -1) break;

        inN[u] = 1;
    }

    // Display final results
    printf("\n=== SHORTEST PATHS FROM: %s ===\n", locations[src]);
    printf("%-22s -> %-22s %s\n", "FROM", "TO", "DISTANCE");
    printf("--------------------------------------------------\n");
    for (i = 0; i < LOC; i++) {
        if (i == src) continue;
        if (dist[i] == INT_MAX)
            printf("%-22s -> %-22s UNREACHABLE\n", locations[src], locations[i]);
        else
            printf("%-22s -> %-22s %d\n",          locations[src], locations[i], dist[i]);
    }
    printf("==================================================\n");
}

/* ------------------------------------------------
   CLEANUP: Free all dynamically allocated memory
   ------------------------------------------------ */
void cleanup() {
    int i, lostCount = 0, foundCount = 0;
    Item *current, *next;

    printf("\n--- CLEANING UP MEMORY ---\n");

    // Free Linked List (lost items)
    current = lostHead;
    while (current) {
        next = current->next;
        free(current);
        current = next;
        lostCount++;
    }
    lostHead = NULL;

    // Free Hash Table chains (found items)
    for (i = 0; i < TABLE_SIZE; i++) {
        current = foundTable[i];
        while (current) {
            next = current->next;
            free(current);
            current = next;
            foundCount++;
        }
        foundTable[i] = NULL;
    }

    printf("Freed: %d lost item(s), %d found item(s).\n", lostCount, foundCount);
}

/* ------------------------------------------------
   MAIN FUNCTION
   ------------------------------------------------ */
int main() {
    int choice, src;

    // Initialize queue
    q.front = -1;
    q.rear  = -1;

    printf("\n");
    printf("========================================\n");
    printf("   WELCOME TO FINDME\n");
    printf("   Campus Lost & Found System\n");
    printf("========================================\n");

    while (1) {
        printf("\n+------------------------------------+\n");
        printf("|           MAIN MENU                |\n");
        printf("|------------------------------------|\n");
        printf("| 1. Add Lost Item                   |\n");
        printf("| 2. Add Found Item                  |\n");
        printf("| 3. View Lost Items                 |\n");
        printf("| 4. View Found Items                |\n");
        printf("| 5. Search Item                     |\n");
        printf("| 6. Mark Item as Claimed            |\n");
        printf("| 7. Process Claim Queue             |\n");
        printf("| 8. Find Shortest Route (Dijkstra)  |\n");
        printf("| 9. Exit                            |\n");
        printf("+------------------------------------+\n");
        printf("Enter your choice: ");

        choice = getInt();

        switch (choice) {
            case 1: addLost();      
			break;
            case 2: addFound();     
			break;
            case 3: viewLost();     
			break;
            case 4: viewFound();    
			break;
            case 5: searchItem();   
			break;
            case 6: markClaimed();  
			break;
            case 7: processClaim(); 
			break;
            case 8:
                showLocations();
                printf("Enter source location number (0-7): ");
                src = getInt();
                dijkstra(src);
                break;
            case 9:
                printf("\nThank you for using FINDME!\n");
                cleanup();
                printf("Goodbye!\n");
                exit(0);
            default:
                printf("\nInvalid choice! Please enter 1-9.\n");
        }
    }

    return 0;
}
