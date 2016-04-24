#include <stdlib.h>
#include <stdio.h>
#include <string.h>
typedef struct nodeStr node;
typedef struct elementStr element;
typedef struct listStr list;

element* root;
node* fileSystem;

#define dir 0
#define file 1
#define maxFileSize 2000

char* showFile = "file, name = %s\n";
char* showDirect = "dir, name = %s\n";

struct listStr
{
	node* inf;
	list* next, *prev;
};

struct nodeStr
{
	node* parent;
	list* parentPos;
	list* childF, *childE;
	int type;

	char* name;
	char* buf;
};

struct elementStr
{
	node* parent;
	char* name;

	element *left, *right;
	element* treeParent;
};

void insert ( char* newName)
{
	int len;
	element* cur;

	if (root == NULL)
	{
		root = (element*)malloc(sizeof(element));
		root->parent = NULL;
		len = strlen(newName);
		root->name = (char*)malloc((len + 1)*sizeof(char));
		memcpy(root->name, newName, len);
		root->name[len] = 0;

		root->left = NULL;
		root->right = NULL;
		root->treeParent = NULL;
		return;
	}

	cur = root;

	while (1)
	{
		if (strcmp(cur->name, newName)< 0)
		{
			if (cur->right == NULL)
			{
				cur->right = (element*)malloc(sizeof(element));
				cur->right->parent = NULL;
				len = strlen(newName);
				cur->right->name = (char*)malloc((len + 1)*sizeof(char));
				memcpy(cur->right->name, newName, len);
				cur->right->name[len] = 0;

				cur->right->left = NULL;
				cur->right->right = NULL;
				cur->right->treeParent = cur;
				
				break;
			}

			cur = cur->right;
		} else
		{
			if (cur->left == NULL)
			{
				cur->left = (element*)malloc(sizeof(element));
				cur->left->parent = NULL;
				len = strlen(newName);
				cur->left->name = (char*)malloc((len + 1)*sizeof(char));
				memcpy(cur->left->name, newName, len);
				cur->left->name[len] = 0;

				cur->left->left = NULL;
				cur->left->right = NULL;
				cur->left->treeParent = cur;
				
				break;
			}
			cur = cur->left;
		}
	}
}

element* find(char* key)
{
	int res;
	element* cur = root;
	if (root == NULL)
		return NULL;

	while (1)
	{
		res = strcmp(cur->name, key);
		if (res == 0)
			return cur;
		else if (res < 0)
		{
			if (cur->right == NULL)
				return NULL;
			cur = cur->right;
		} else
		{
			if (cur->left == NULL)
				return NULL;
			cur = cur->left;
		}
	}
}

void removeT( char* key)
{
	int res;
	element* cur, *m, *min;
	element* par = NULL;

	if (root == NULL)
		return;

	cur = root;

	while (1)
	{
		res = strcmp(cur->name, key);
		if (res == 0)
		{
			if (cur->left == NULL && cur->right == NULL)
			{
				if (par->left == cur)
				{
					par->left = NULL;
					free(cur->name);
					free(cur);
				} else
				{
					par->right = NULL;
					free(cur->name);
					free(cur);
				}
			} else if ((cur->left != NULL && cur->right == NULL) || (cur->left == NULL && cur->right != NULL))
			{
				if (cur->left == NULL)
					m = cur->right;
				else
					m = cur->left;

				if (par->left == cur)
				{
					par->left = m;
					free(cur->name);
					free(cur);
				} else
				{
					par->right = m;
					free(cur->name);
					free(cur);
				}
			} else
			{
				if (par->left == cur)
				{
					if (cur->right->left == NULL)
					{
						free(cur->name);
						cur->right->left = cur->left;
						par->left = cur->right;
						free(cur);
					} else
					{
						min = cur->right;
						while (min->left != NULL)
							min = min->left;

						min->treeParent->left = min->right;

						free(cur->name);
						cur->name = min->name;
						free(min);
					}
				} else
				{
					if (cur->left->right == NULL)
					{
						free(cur->name);
						cur->left->right = cur->right;
						par->right = cur->left;
						free(cur);
					} else
					{
						min = cur->left;
						while (min->right != NULL)
							min = min->right;

						min->treeParent->right = min->left;

						free(cur->name);
						cur->name = min->name;
						free(min);
					}
				}
			}
			return;
		}
		else if (res < 0)
		{
			if (cur->right == NULL)
				return;
			par = cur;
			cur = cur->right;
		} else
		{
			if (cur->left == NULL)
				return;
			par = cur;
			cur = cur->left;
		}
	}
}



void printTree(FILE* f, element* cur)
{
	if (cur->right != NULL)
		printTree(f,cur->right);
	fprintf(f,"%s\n", cur->name);
	if (cur->left != NULL)
		printTree(f,cur->left);
}

void createSystem()
{
	fileSystem = (node*)malloc(sizeof(node));

	fileSystem->childF = NULL;
	fileSystem->childE = NULL;

	fileSystem->parent = NULL;
	fileSystem->buf = NULL;

	fileSystem->type = dir;
}

void insertNode(node* parent, node* child)
{
	if (parent->type == file)
	{
		printf("You've tried to insert node in file, it is imposible\n");
		free(child);
		return;
	}

	if (parent->childF == NULL)
	{
		parent->childF = (list*)malloc(sizeof(list));
		parent->childF->inf = child;
		parent->childF->next = NULL;
		parent->childF->prev = NULL;
		parent->childE = parent->childF;

		child->parent = parent;

		child->parentPos = parent->childE;
	} else
	{
		parent->childE->next = (list*)malloc(sizeof(list));
		parent->childE->next->prev = parent->childE;
		parent->childE = parent->childE->next;
		parent->childE->inf = child;
		parent->childE->next = NULL;

		child->parent = parent;
		child->parentPos = parent->childE;
	}
}

node* createFile(char* s)
{
	int l;
	node* temp = (node*)malloc(sizeof(node));

	temp->childE = NULL;
	temp->childF = NULL;

	temp->parent = NULL;

	temp->type = file;
	temp->buf = (char*)malloc(maxFileSize*sizeof(char));

	l = strlen(s);
	temp->name = (char*)malloc((l + 1)*sizeof(char));
	memcpy(temp->name, s, l);
	temp->name[l] = 0;

	return temp;
}

node* createDir(char* s)
{
	int l;
	node* temp = (node*)malloc(sizeof(node));

	temp->childE = NULL;
	temp->childF = NULL;

	temp->parent = NULL;

	temp->type = dir;
	temp->buf = NULL;

	l = strlen(s);
	temp->name = (char*)malloc((l + 1)*sizeof(char));
	memcpy(temp->name, s, l);
	temp->name[l] = 0;

	return temp;
}

void showDir(node* cur)
{
	list* temp;

	if (cur->type == file)
	{
		printf("It is file, not a directory\n");
		return;
	}

	temp = cur->childF;

	while (temp != NULL)
	{
		if (temp->inf->type == file)
			printf(showFile,temp->inf->name);
		else
			printf(showDirect,temp->inf->name);

		temp = temp->next;
	}
}

void deleteFromList(node*parent, list* l)
{
	if (parent->childE == parent->childF)
	{
		free(l);
		parent->childF = NULL;
		parent->childE = NULL;
	} else if (l == parent->childF)
	{
		parent->childF = parent->childF->next;
		parent->childF->prev = NULL;
		free(l);
	} else if (l == parent->childE)
	{
		parent->childE = parent->childE->prev;
		parent->childE->next = NULL;
		free(l);
	} else
	{
		l->next->prev = l->prev;
		l->prev->next = l->next;
		free(l);
	}
}

void deleteNode(node* cur)
{
	list* temp;
	if (cur == fileSystem)
	{
		printf("It is root directory, you can't delete it\n");
		return;
	}

	temp = cur->childF;

	while (temp != NULL)
	{
		deleteNode(temp->inf);
		//free(temp->inf);
		temp = cur->childF;
		if (temp == NULL)
			break;
		//temp = temp->next;
	}

	if (cur->buf != NULL)
		free(cur->buf);

	free(cur->name);
	deleteFromList(cur->parent,cur->parentPos);
	//
}

int main()
{
	FILE* f, *ou;
	char* temp;
	node* tmp;
	int len;
	root = NULL;

	f = fopen("in.txt", "r");
	ou = fopen("out.txt", "w");

	while (1)
	{
		temp = (char*)malloc(10*sizeof(char));
		fgets(temp,10,f);

		len = strlen(temp);
		if (temp[len - 1] == '\n')
			temp[len - 1] = 0;

		insert(temp);

		if (feof(f))
			break;
	}

	printTree(ou,root);

	/*printf("%s\n", find("1")->name);
	printf("%s\n", find("1")->name);
	printf("%s\n", find("1")->name);
	printf("%s\n", find("1")->name);
	printf("%s\n", find("2")->name);
	printf("%s\n", find("3")->name);
	printf("%s\n", find("4")->name);
	printf("%s\n", find("5")->name);

	removeT("9");
	removeT("6");
	removeT("5");
	removeT("3");
	removeT("0");
	removeT("2");
	removeT("4");
	removeT("1");

	fprintf(ou,"!!!!!!!!!!!\n");
	printTree(ou,root);*/
	
	fclose(f);
	fclose(ou);

	createSystem();

	insertNode(fileSystem,createFile("1"));
	insertNode(fileSystem,createFile("2"));
	insertNode(fileSystem,createFile("3"));
	insertNode(fileSystem,createFile("4"));
	insertNode(fileSystem,createFile("5"));

	insertNode(fileSystem,createDir("6"));
	insertNode(fileSystem,createDir("tmp"));

	showDir(fileSystem);

	tmp = fileSystem->childE->inf;

	insertNode(tmp,createFile("1"));
	insertNode(tmp,createFile("2"));
	insertNode(tmp,createFile("3"));
	insertNode(tmp,createFile("4"));
	insertNode(tmp,createFile("5"));

	insertNode(tmp,createDir("6"));
	insertNode(tmp,createDir("7"));

	insertNode(tmp->childE->inf,createFile("1"));
	insertNode(tmp->childE->inf,createFile("2"));
	insertNode(tmp->childE->inf,createFile("3"));
	insertNode(tmp->childE->inf,createFile("4"));
	insertNode(tmp->childE->inf,createFile("5"));

	insertNode(tmp->childE->inf,createDir("6"));
	insertNode(tmp->childE->inf,createDir("7"));

	printf("ASDASDASdA\n");
	showDir(tmp);

	deleteNode(tmp);
	printf("ASDASDASdA\n");
	showDir(tmp);

	printf("ASDASDASdA\n");
	showDir(fileSystem);

	getchar();
	return 0;
}