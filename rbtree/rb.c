#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum { RED, BLACK } nodeColor;
typedef struct rbnode {
    int data;
    nodeColor color;
    struct rbnode *left, *right, *parent;
} rbnode;

rbnode* createNode(int data);
void rotateLeft(rbnode **root, rbnode *x);
void rotateRight(rbnode **root, rbnode *y);
void insertFixup(rbnode **root, rbnode *x);
void deleteFixup(rbnode **root, rbnode *x);
void insert(rbnode **root, rbnode *x);
void delete(rbnode **root, rbnode *x);
rbnode* search(rbnode *root, int data);
void printInorder(rbnode *root);
void printLevelorder(rbnode *root);
rbnode* minimum(rbnode *node);
int height(rbnode *node);
void printCurrentLevel(rbnode *root, int level);

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s input.txt output.txt\n", argv[0]);
        return 1;
    }

    FILE *inputFile = fopen(argv[1], "r");
    FILE *outputFile = freopen(argv[2], "w", stdout);
    if (!inputFile || !outputFile) {
        fprintf(stderr, "Error opening file.\n");
        return 1;
    }

    rbnode *root = NULL;
    int value;
    int stage = 0; 

    while (fscanf(inputFile, "%d", &value) == 1) {
        if (value == -1) {
            stage = 1; 
            continue;
        }
        if (stage == 0) {
            if (search(root, value) == NULL) { 
                rbnode *newNode = createNode(value);
                insert(&root, newNode);
            }
        }
    }

    printInorder(root);
    printf("\n");
    printLevelorder(root);
    printf("\n");

    fseek(inputFile, 0, SEEK_SET);
    stage = 0; 

    while (fscanf(inputFile, "%d", &value) == 1) {
        if (value == -1) {
            stage = 1; 
            continue;
        }
        if (stage == 1) {
            rbnode *nodeToDelete = search(root, value);
            if (nodeToDelete != NULL) {
                delete(&root, nodeToDelete);
            }
        }
    }

    printInorder(root);
    printf("\n");
    printLevelorder(root);
    printf("\n");

    fclose(inputFile);
    fclose(outputFile);

    return 0;
}

rbnode* createNode(int data) {
    rbnode *newNode = (rbnode*)malloc(sizeof(rbnode));
    newNode->data = data;
    newNode->color = RED;
    newNode->left = newNode->right = newNode->parent = NULL;
    return newNode;
}

void rotateLeft(rbnode **root, rbnode *x) {
    rbnode *y = x->right;
    x->right = y->left;
    if (y->left != NULL) {
        y->left->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == NULL) {
        *root = y;
    } else if (x == x->parent->left) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }
    y->left = x;
    x->parent = y;
}

void rotateRight(rbnode **root, rbnode *y) {
    rbnode *x = y->left;
    y->left = x->right;
    if (x->right != NULL) {
        x->right->parent = y;
    }
    x->parent = y->parent;
    if (y->parent == NULL) {
        *root = x;
    } else if (y == y->parent->right) {
        y->parent->right = x;
    } else {
        y->parent->left = x;
    }
    x->right = y;
    y->parent = x;
}

void insertFixup(rbnode **root, rbnode *x) {
    while (x->parent && x->parent->color == RED) {
        if (x->parent == x->parent->parent->left) {
            rbnode *y = x->parent->parent->right;
            if (y && y->color == RED) {
                x->parent->color = BLACK;
                y->color = BLACK;
                x->parent->parent->color = RED;
                x = x->parent->parent;
            } else {
                if (x == x->parent->right) {
                    x = x->parent;
                    rotateLeft(root, x);
                }
                x->parent->color = BLACK;
                x->parent->parent->color = RED;
                rotateRight(root, x->parent->parent);
            }
        } else {
            rbnode *y = x->parent->parent->left;
            if (y && y->color == RED) {
                x->parent->color = BLACK;
                y->color = BLACK;
                x->parent->parent->color = RED;
                x = x->parent->parent;
            } else {
                if (x == x->parent->left) {
                    x = x->parent;
                    rotateRight(root, x);
                }
                x->parent->color = BLACK;
                x->parent->parent->color = RED;
                rotateLeft(root, x->parent->parent);
            }
        }
    }
    (*root)->color = BLACK;
}

void insert(rbnode **root, rbnode *x) {
    rbnode *y = NULL;
    rbnode *z = *root;
    while (z != NULL) {
        y = z;
        if (x->data < z->data) {
            z = z->left;
        } else {
            z = z->right;
        }
    }
    x->parent = y;
    if (y == NULL) {
        *root = x;
    } else if (x->data < y->data) {
        y->left = x;
    } else {
        y->right = x;
    }
    insertFixup(root, x);
}

rbnode* minimum(rbnode *node) {
    while (node->left != NULL) {
        node = node->left;
    }
    return node;
}

void deleteFixup(rbnode **root, rbnode *x) {
    while (x != *root && (!x || x->color == BLACK)) {
        if (x == x->parent->left) {
            rbnode *w = x->parent->right;
            if (w && w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                rotateLeft(root, x->parent);
                w = x->parent->right;
            }
            if ((!w->left || w->left->color == BLACK) && (!w->right || w->right->color == BLACK)) {
                w->color = RED;
                x = x->parent;
            } else {
                if (!w->right || w->right->color == BLACK) {
                    if (w->left) {
                        w->left->color = BLACK;
                    }
                    w->color = RED;
                    rotateRight(root, w);
                    w = x->parent->right;
                }
                w->color = x->parent->color;
                x->parent->color = BLACK;
                if (w->right) {
                    w->right->color = BLACK;
                }
                rotateLeft(root, x->parent);
                x = *root;
            }
        } else {
            rbnode *w = x->parent->left;
            if (w && w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                rotateRight(root, x->parent);
                w = x->parent->left;
            }
            if ((!w->right || w->right->color == BLACK) && (!w->left || w->left->color == BLACK)) {
                w->color = RED;
                x = x->parent;
            } else {
                if (!w->left || w->left->color == BLACK) {
                    if (w->right) {
                        w->right->color = BLACK;
                    }
                    w->color = RED;
                    rotateLeft(root, w);
                    w = x->parent->left;
                }
                w->color = x->parent->color;
                x->parent->color = BLACK;
                if (w->left) {
                    w->left->color = BLACK;
                }
                rotateRight(root, x->parent);
                x = *root;
            }
        }
    }
    if (x) {
        x->color = BLACK;
    }
}

void delete(rbnode **root, rbnode *z) {
    rbnode *y = z;
    rbnode *x;
    nodeColor yOriginalColor = y->color;
    if (z->left == NULL) {
        x = z->right;
        if (x) {
            x->parent = z->parent;
        }
        if (z->parent == NULL) {
            *root = x;
        } else if (z == z->parent->left) {
            z->parent->left = x;
        } else {
            z->parent->right = x;
        }
        if (x == NULL) {
            x = z->parent;
        }
    } else if (z->right == NULL) {
        x = z->left;
        if (x) {
            x->parent = z->parent;
        }
        if (z->parent == NULL) {
            *root = x;
        } else if (z == z->parent->left) {
            z->parent->left = x;
        } else {
            z->parent->right = x;
        }
        if (x == NULL) {
            x = z->parent;
        }
    } else {
        y = minimum(z->right);
        yOriginalColor = y->color;
        x = y->right;
        if (y->parent == z) {
            if (x) {
                x->parent = y;
            }
        } else {
            if (x) {
                x->parent = y->parent;
            }
            y->parent->left = x;
            y->right = z->right;
            if (z->right) {
                z->right->parent = y;
            }
        }
        if (z->parent == NULL) {
            *root = y;
        } else if (z == z->parent->left) {
            z->parent->left = y;
        } else {
            z->parent->right = y;
        }
        y->parent = z->parent;
        y->left = z->left;
        if (z->left) {
            z->left->parent = y;
        }
        y->color = z->color;
    }
    free(z);
    if (yOriginalColor == BLACK) {
        deleteFixup(root, x);
    }
}

rbnode* search(rbnode *root, int data) {
    if (root == NULL || root->data == data) {
        return root;
    }
    if (data < root->data) {
        return search(root->left, data);
    }
    return search(root->right, data);
}

void printInorder(rbnode *root) {
    if (root != NULL) {
        printInorder(root->left);
        printf("%d ", root->data);
        printInorder(root->right);
    }
}

void printLevelorder(rbnode *root) {
    if (root == NULL) {
        return;
    }
    int h = height(root);
    for (int i = 1; i <= h; i++) {
        printCurrentLevel(root, i);
    }
}

int height(rbnode *node) {
    if (node == NULL) {
        return 0;
    } else {
        int leftHeight = height(node->left);
        int rightHeight = height(node->right);
        if (leftHeight > rightHeight) {
            return (leftHeight + 1);
        } else {
            return (rightHeight + 1);
        }
    }
}

void printCurrentLevel(rbnode *root, int level) {
    if (root == NULL) {
        return;
    }
    if (level == 1) {
        printf("%d ", root->data);
    } else if (level > 1) {
        printCurrentLevel(root->left, level - 1);
        printCurrentLevel(root->right, level - 1);
    }
}
