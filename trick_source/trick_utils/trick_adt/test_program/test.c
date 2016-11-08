
#include <stdio.h>
#include <stdlib.h>
#include "../bst.h"
#include <string.h>

typedef struct
{
   int key;
   char* data;
}ITEM;


int compare(void* left, void* right)
{
   ITEM* pitem1=(ITEM*)left;
   ITEM* pitem2=(ITEM*)right;
   if(pitem1->key-pitem2->key < 0)
      return -1;
   else if(pitem1->key-pitem2->key > 0)
      return 1;
   else
      return 0;
}


void main()
{
   short done=0;
   int newnum;
   char ch;
   char buffer[256];
   ITEM* newitem;
   BST bst;
   bstNode* node;
   ITEM tempitem;
   bstInit(&bst);
   bstSetCompareFunc(&bst,compare);
   while(!done)
   {
      fprintf(stderr,"(1) Add Item\n(2) Lookup\n(3) Remove Item\n");
      ch=getchar();
      fflush(stdin);
      switch(ch)
      {
         case '1':
           newitem=(ITEM*)malloc(sizeof(ITEM));
           printf("\nEnter a string\n");
           scanf("%s",buffer);
           newitem->data=strdup(buffer);
           printf("Enter key\n");
           scanf("%d",&newnum);
           newitem->key=newnum;
           printf("inserting at %d\n",newnum);
           bstInsert(newitem,&bst);
           break;
         case '2':
           printf("Enter key\n");

           scanf("%d",&newnum);
           printf("Looking for %d\n",newnum);
           tempitem.key=newnum;
           node=bstFind(&tempitem,&bst);
           if(node != NULL)
           {
              newitem=(ITEM*)bstGetInfo(node);
              printf("found %s\n",newitem->data);
           }
           else
           {
              printf("Item not found\n");
           }
           break;
         case '3':
           printf("Enter key\n");
           scanf("%d",&newnum);
           tempitem.key=newnum;
           node=bstFind(&tempitem,&bst);
           if(node != NULL)
           {
              newitem=(ITEM*)bstGetInfo(node);
              printf("removing %s\n",newitem->data);
              bstDelete(node,&bst);
           }
           else
           {
              printf("Item not found\n");
           }
           break;
         default:done=1;
      }
      fflush(stdin);
   }
}









