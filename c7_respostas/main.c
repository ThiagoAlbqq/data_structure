#include <stdio.h>
#include <stdlib.h>

typedef struct NodeBinaryTree {
  int value;
  struct NodeBinaryTree *left;
  struct NodeBinaryTree *right;
} NodeBinaryTree;

NodeBinaryTree *root;

NodeBinaryTree *search(NodeBinaryTree *node, NodeBinaryTree *parent,
                       int value) {
  if (node == NULL) {
    return parent;
  }
  if (value == node->value) {
    return node;
  } else if (value < node->value) {
    return search(node->left, node, value);
  } else {
    return search(node->right, node, value);
  }
}

void add(NodeBinaryTree **root, int value) {
  NodeBinaryTree *newNode = malloc(sizeof(NodeBinaryTree));
  newNode->value = value;
  newNode->left = NULL;
  newNode->right = NULL;
  if (*root == NULL) {
    *root = newNode;
  } else {
    NodeBinaryTree *parent_or_children = search(*root, NULL, value);
    if (parent_or_children->value == value) {
      printf("O valor já está inserido!\n");
      free(newNode);
    } else {
      if (value < parent_or_children->value) {
        parent_or_children->left = newNode;
      } else {
        parent_or_children->right = newNode;
      }
    }
  }
}

NodeBinaryTree *minor_number(NodeBinaryTree *node) {
  if (node->right == NULL)
    return node;
  minor_number(node->right);
}

NodeBinaryTree *bigger_number(NodeBinaryTree *node) {
  if (node->left == NULL)
    return node;
  bigger_number(node->left);
}

NodeBinaryTree *removed(NodeBinaryTree *root, int value) {
  if (root == NULL) {
    return root;
  }

  if (value < root->value) {
    root->left = removed(root->left, value);
  } else if (value > root->value) {
    root->right = removed(root->right, value);
  } else {
    if (root->left == NULL) {
      NodeBinaryTree *temp = root->right;
      free(root);
      return temp;
    } else if (root->right == NULL) {
      NodeBinaryTree *temp = root->left;
      free(root);
      return temp;
    }
    NodeBinaryTree *temp = minor_number(root->right);

    root->value = temp->value;

    root->right = removed(root->right, temp->value);
  }
  return root;
}

void printTree(NodeBinaryTree *root) {
  if (root == NULL) {
    return;
  }
  printTree(root->left);
  printf("%d ", root->value);
  printTree(root->right);
}

int main() {
  add(&root, 9);
  add(&root, 4);
  add(&root, 10);
  add(&root, 16);
  add(&root, 20);
  add(&root, 9);

  printTree(root);
  printf("\n");

  removed(root, 10);

  printTree(root);

  return 0;
}