/* Emircan Kirez - Ch3rry */
/* Last Update: 02/06/2022 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define MAX 255


/*
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|     |     |                             |
+ +-+ + +-+-+ +-+-+-+-+-+-+-+-+-+ +-+-+-+ +
|   |   |     |   |   |         |   |     |
+-+ + +-+ +-+-+ + + + + +-+-+-+ +-+ + +-+-+
|   | |   |     |   | |     | | |   |   | |
+ +-+ + +-+ +-+-+-+-+-+-+ +-+-+ +-+-+-+ +-+
| |   | | | |         | | |   |     | |   |
+ +-+-+ +-+ + +-+-+-+ + + + + +-+ + +-+-+ +
|     | |   |   |   | | |   |   | |       |
+ +-+ + + +-+-+ +-+ + +-+-+-+-+ + +-+-+-+-+
| |   | |     |   | |         | |       | |
+ + +-+ +-+-+-+-+ + +-+-+-+-+ + +-+ +-+ +-+
| | |             |         | |   | | |   |
+ + + +-+-+-+-+-+-+-+ +-+-+ + +-+ + + +-+ +
| |     |   |             | |   | |   |   |
+ +-+-+ + + + +-+ +-+-+ +-+-+ + + +-+ + +-+
| |     | | |   |     | |     | | | | |   |
+ + +-+-+ + +-+ + +-+-+-+ +-+-+-+ + + +-+ +
| | |     |   | |   |   |       | |   | | |
+ + + +-+-+-+-+ +-+ + +-+-+-+-+ + +-+-+-+ +
| |               | |   | | |   |     | | |
+ + +-+-+-+-+-+-+-+-+-+ +-+-+ +-+-+-+ +-+ +
| |     |   | |     | | | |         | |   |
+-+-+-+ + + + + +-+ +-+ +-+ +-+ +-+-+ + +-+
|   |   | | | | | |     | | |   |   | |   |
+ +-+ +-+ + + + +-+-+-+-+-+ + +-+ + + +-+-+
|       | | | |           | | |   | |     |
+-+-+-+ +-+ + +-+-+-+-+-+ + + +-+-+-+-+-+ +
|b          |           |   |         |  c|
+---+-+-+-+-+---+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/

//Linked list where I keep the lines that I read from the file
typedef struct lineNode{
	char line[100];
	struct lineNode *next;
}LINES; 

//The struct I created to keep the x and y indices together
typedef struct indices{
	int x, y;
}INDICES;

//Linked list where I keep the nodes I've visited 
typedef struct visitedIndices{
	INDICES* indices;
	struct visitedIndices *next;
}VISITED;

//The stack structure I use for DFS - Since I keep INDICES* in the linked list above, I keep it here as well for convenience
typedef struct {
	INDICES* item[MAX]; //0 .. MAX-1
	int top;
}STACK;

//Function Prototypes (18)
void initialize(STACK *s);
void push(STACK *s, INDICES* indices);
int isEmpty(STACK *s);
INDICES* pop(STACK *s);
INDICES* top(STACK *s);
int readFromFile(LINES **head, char *fileName, int *mazeHeight);
LINES *createLineNode(char *line);
void addLineNode(LINES **head, char *line);
int findWidth(char *line);
char **createMaze(int mazeHeight, int mazeWidth);
void initializeMaze(LINES *head, char **maze, int mazeHeight, int mazeWidth, int *startingX, int *startingY);
void findMazeExit(VISITED **visitedhead, int startingX, int startingY, STACK *s, char **maze, int mazeHeight, int mazeWidth);
void printMaze(char **maze, int mazeHeight, int mazeWidth, int score);
INDICES* createIndices(int x, int y);
VISITED* createVisitedIndices(INDICES *indices);
void addVisitedNode(VISITED **head, VISITED* newVisited);
void deleteVisitedNode(VISITED **head);
void deallocateMaze(char **maze, int mazeHeight, int mazeWidth);

int main(){
	srand(time(NULL));
	LINES *lineHead = NULL;
	VISITED *visitedHead = NULL;
	char fileName[20];
	char **maze; //the matrix where I keep the maze
	STACK s;
	int startingX, startingY, mazeWidth, mazeHeight;
	initialize(&s); //Initializes the STACK. If the stack is empty, our top value shows -1


	printf("Write the file you want to read the maze with its extension: ");
	scanf("%s", fileName);
	
	if(!readFromFile(&lineHead, fileName, &mazeHeight)){
		printf("The program was closed because the file could not be read.");
		return -1;
	}
	
	mazeWidth = findWidth(lineHead->line);
	maze = createMaze(mazeHeight, mazeWidth);
	initializeMaze(lineHead, maze, mazeHeight, mazeWidth, &startingX, &startingY);
	findMazeExit(&visitedHead, startingX, startingY, &s, maze, mazeHeight, mazeWidth);
	
	/* Performs deletions before closing the program */
	
	//Empties STACK
	while(!isEmpty(&s)){
		INDICES *tmp = pop(&s);
	}
	
	//Resets the heads of linked lists
	visitedHead = NULL;
	lineHead = NULL;
	
	//Deallocates the created maze
	deallocateMaze(maze, mazeHeight, mazeWidth);
	return 0;
}

/* STACK Functions */
void initialize(STACK *s){
	s->top = -1;
}

//I set the STACK large enough and didn't see if it was full
void push(STACK *s, INDICES* indices){ 
	s->item[++s->top] = indices; //increase one first, now the top value shows the new location, then add
}

int isEmpty(STACK *s){
	return s->top == -1 ? 1 : 0;
}

INDICES* pop(STACK *s){ //I didn't check if it's empty because in DFS, we pop as long as the stack is not empty
	return s->item[s->top--]; //return the value where top is pointing, then decrase the top value by one
}

INDICES* top(STACK *s){
	return s->item[s->top];
}

//It reads from the file line by line, sabes them in the LINES linked list and finds the height of the maze.
int readFromFile(LINES **head, char *fileName, int *mazeHeight){
	FILE *fPtr;
	char line[100];
	
	if((fPtr = fopen(fileName, "r")) == NULL){
		printf("The file could not opened!!\n");
		return 0;
	}
	

	while(fgets(line, 100, fPtr) != NULL){
		addLineNode(head, line);
		(*mazeHeight)++;
	}

	fclose(fPtr);
	return 1;
}

//Creates a node to store the lines we get when reading from the file line by line
LINES *createLineNode(char *line){
	LINES *newLineNode = (LINES *)malloc(sizeof(LINES));
	strcpy(newLineNode->line, line);
	newLineNode->next = NULL;
	return newLineNode;
}

//Creates a node that holds the rows and appends it to the linked list
void addLineNode(LINES **head, char *line){
	LINES *newLineNode = createLineNode(line);
	if(*head == NULL){
		*head = newLineNode;
	}else{
		LINES *tmp = *head;
		while(tmp->next != NULL){
			tmp = tmp->next;
		}
		tmp->next = newLineNode;
	}
}

//The width of the maze is determined by the length of a line
int findWidth(char *line){
	int i = 0;
	while(i < 100 && line[i] != '\n'){
		i++;
	}
	return i;
}

//Creates and returns a matrix based on the height and width of the maze
char **createMaze(int mazeHeight, int mazeWidth){
	char **maze;
	int i;
	maze = (char **)malloc(mazeHeight * sizeof(char *));
	for(i = 0; i < mazeHeight; i++){
		maze[i] = (char *)malloc(mazeWidth * sizeof(char));
	}
	return maze;
}

//Initalizes the matrix by taking the rows one by one from the LINES linked list that I had previously read from the file. It also finds the indices of the starting point
void initializeMaze(LINES *head, char **maze, int mazeHeight, int mazeWidth, int *startingX, int *startingY){
	int i, j, randomX, randomY;
	LINES *tmp = head;
	for(i = 0; i < mazeHeight; i++){
		for(j = 0; j < mazeWidth; j++){
			maze[i][j] = tmp->line[j];
			if(maze[i][j] == 'b'){
				*startingX = i;
				*startingY = j;
			}
		}
		tmp = tmp->next;
	}
	
	//I added 10 apples
	i = 0;
	while(i < 10){
		randomX = rand() % mazeHeight;
		randomY = rand() % mazeWidth;
		if(maze[randomX][randomY] == ' '){
			maze[randomX][randomY] = 'O';
			i++;
		} 
	}
}

void findMazeExit(VISITED **visitedHead, int startingX, int startingY, STACK *s, char **maze, int mazeHeight, int mazeWidth){
	//Controls left - down- right -up. In this way, up - right - down - left structure is set up in the stack. 
	int directions[][4] = {{0, 1, 0, -1}, {-1, 0, 1, 0}};  
	int i, x, y, counter, score = 0;
	INDICES *tmp = createIndices(startingX, startingY);
	INDICES *current;
	
	//I put the starting point into the VISITED linked list and push its neighbors onto the stack. After that I apply DFS
	addVisitedNode(visitedHead, createVisitedIndices(tmp));
	for(i = 0; i < 4; i++){
		x = startingX + directions[0][i];
		y = startingY + directions[1][i];
		
		if(maze[x][y] == ' ' || maze[x][y] == 'O'){ //If it's empty or an apple, push to the stack
			tmp = createIndices(x, y);
			addVisitedNode(visitedHead, createVisitedIndices(tmp));
			push(s, tmp);
		}
	}
	
	printMaze(maze, mazeHeight, mazeWidth, score);
	while(!isEmpty(s)){
		tmp = pop(s);
		if(maze[tmp->x][tmp->y] == 'O'){ //+10 points if it eats the apple
			score += 10;
		}
		maze[tmp->x][tmp->y] = '*';
		printMaze(maze, mazeHeight, mazeWidth, score);
		counter = 0;
		i = 0;
		while(i < 4){
			x = tmp->x + directions[0][i];
			y = tmp->y + directions[1][i];
			if(maze[x][y] == 'c'){
				printf("\nYour exit score from the maze: %d", score);
				current = createIndices(x, y);
				addVisitedNode(visitedHead, createVisitedIndices(current));
				return;
			}else if(maze[x][y] == ' ' || maze[x][y] == 'O'){ //If it's empty
				current = createIndices(x, y);
				addVisitedNode(visitedHead, createVisitedIndices(current));
				push(s, current);
				counter++;
			}
			i++;
		}
		

		if(counter == 0){ 
		//If counter == 0, it means that it has not gone anywhere. Which means it's a dead-end street. That's why I substract 5 from the score and remove the *'s up to the last value in the stack.
		//In this way, I am back on the way I went. For this, I use the VISITED linked list.
			score -= 5;
			current = top(s);
			while((*visitedHead)->indices->x != current->x || (*visitedHead)->indices->y != current->y){
				maze[(*visitedHead)->indices->x][(*visitedHead)->indices->y] = ' '; //I'm coming back
				printMaze(maze, mazeHeight, mazeWidth, score);
				deleteVisitedNode(visitedHead); //I'm trying to reach to the relevant point by deleting it from the linked list 		
			}
		}
	}
}

void printMaze(char **maze, int mazeHeight, int mazeWidth, int score){
	int i, j;
	printf("Your Current Score: %d\n", score);
	for(i = 0; i < mazeHeight; i++){
		for(j = 0; j < mazeWidth; j++){
			printf("%c", maze[i][j]);
		}
		printf("\n");
	}
	printf("\n");
	usleep(500000); //for better appearance
}

//Creates INDICES based on x and y values
INDICES* createIndices(int x, int y){
	INDICES* newIndices = (INDICES*)malloc(sizeof(INDICES));
	newIndices->x = x;
	newIndices->y = y;
	return newIndices;
}

//Creates a node containing the coordinate information of the points we visit
VISITED* createVisitedIndices(INDICES *indices){
	VISITED* newVisited = (VISITED*)malloc(sizeof(VISITED));
	newVisited->indices = indices;
	newVisited->next = NULL;
	return newVisited;
}

//It adds the relevant node to the linked list from the beginning, because if we're going to delete it, it makes more sense to add it to the beginning rather than going all the way to the end, since we will delete the last one we added
void addVisitedNode(VISITED **head, VISITED *newVisited){
	newVisited->next = *head;
	*head = newVisited;
}

//It deletes from the beginning
void deleteVisitedNode(VISITED **head){
	VISITED *tmp = *head;
	*head = (*head)->next;
	free(tmp);
}

//Deallocates dynamically created matrix
void deallocateMaze(char **maze, int mazeHeight, int mazeWidth){
	int i;
	for(i = 0; i < mazeHeight; i++){
		free(maze[i]);
	}
	free(maze);
}
