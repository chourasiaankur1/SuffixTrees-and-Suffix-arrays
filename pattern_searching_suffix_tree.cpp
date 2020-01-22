#include<bits/stdc++.h>
#define MAX_CHAR 256
using namespace std;
struct SuffixTreeNode{

    struct SuffixTreeNode* children[MAX_CHAR];
    struct SuffixTreeNode* suffixLink;
    int start;
    int *end;
    int suffixIndex;
};

typedef struct SuffixTreeNode Node;
string text;
Node* root = NULL;

Node* lastNewNode = NULL;
Node* activeNode = NULL;
int activeEdge = -1;
int activeLength = 0;

int remainingSuffixCount = 0;
int leafEnd = -1;
int *rootEnd = NULL;
int * splitEnd = NULL;
int size = -1;

Node* newNode(int start,int *end)
{
    Node *node = new Node;
    int i;
    for(i=0;i<MAX_CHAR;i++)
        node->children[i]=NULL;
    node->suffixLink = root;
    node->start = start;
    node->end = end;
    node->suffixIndex = -1;
    return node;
}

int edgeLength(Node* n){
    return *(n->end) - (n->start) +1;
}

int walkDown(Node* currNode)
{
    if(activeLength>=edgeLength(currNode))
    {
        activeEdge+= edgeLength(currNode);
        activeLength-= edgeLength(currNode);
        activeNode = currNode;
        return 1;
    }
    return 0;
}

void extendedSuffixTree(int pos)
{
    leafEnd = pos;
    remainingSuffixCount++;

    lastNewNode = NULL;

    while(remainingSuffixCount>0)
    {
        if(activeLength==0)
            activeEdge=pos;

        if(activeNode->children[text[activeEdge]]==NULL)
        {
            activeNode->children[text[activeEdge]] = newNode(pos,&leafEnd);

            if(lastNewNode!=NULL)
            {
                lastNewNode->suffixLink = activeNode;
                lastNewNode = NULL;
            }
        }
        else
        {
            Node* next = activeNode->children[text[activeEdge]];
            if(walkDown(next))
            {
                continue;
            }
            if(text[next->start+activeLength]==text[pos])
            {
                if(lastNewNode!=NULL && activeNode!=root)
                {
                    lastNewNode->suffixLink = activeNode;
                    lastNewNode=NULL;
                }

                activeLength++;
                break;
            }

            splitEnd = new int;
            *splitEnd = next->start+activeLength-1;

            //new Internal node

            Node* split = newNode(next->start,splitEnd);
            activeNode->children[text[activeEdge]]=split;

            //new leaf coming out of the new internal node
            split->children[text[next->start]]=newNode(pos,&leafEnd);
            next->start+= activeLength;
            split->children[text[next->start]]=next;

            if(lastNewNode!=NULL)
            {
                lastNewNode->suffixLink = split;
            }

            lastNewNode=split;
        }

        remainingSuffixCount--;

        if(activeNode==root && activeLength>0)
        {
            activeLength--;
            activeEdge = pos-remainingSuffixCount+1;
        }
        else if(activeNode!=root)
        {
            activeNode = activeNode->suffixLink;
        }
    }
}

void print(int i,int j)
{
    int k;
    for(k=i;k<=j;k++)
        cout<<text[k];
}

void setSuffixIndexByDFS(Node *n,int labelHeight)
{
    if(n==NULL)
        return;

    //if(n->start!=-1)
      //  print(n->start,*(n->end));

    int leaf = 1;
    int i;
    for(i=0;i<MAX_CHAR;i++)
    {
        if(n->children[i]!=NULL)
        {
            //if(leaf==1 && n->start!=-1)
                //cout<<"["<<n->suffixIndex<<"]"<<"\n";
            leaf = 0;
            setSuffixIndexByDFS(n->children[i],labelHeight+
                                                edgeLength(n->children[i]));
        }
    }
    if(leaf==1)
    {
        n->suffixIndex = size - labelHeight;
        //cout<<"["<<n->suffixIndex<<"]"<<"\n";
    }
}
// function to free the dynamically allocated suffix Tree
void freeSuffixTreeByPostOrder(Node *n)
{
    if(n==NULL)
        return;

    int i;
    for(i=0;i<MAX_CHAR;i++)
    {
        if(n->children[i]!=NULL)
            freeSuffixTreeByPostOrder(n->children[i]);
    }

    if(n->suffixIndex==-1)
        free(n->end);
    free(n);
}
void buildSuffixTree()
{
    size = text.length();
    int i;
    rootEnd = new int;
    *rootEnd = -1;

    root = newNode(-1,rootEnd);

    activeNode = root;
    for(i=0;i<size;i++)
        extendedSuffixTree(i);

    int labelHeight = 0;
    setSuffixIndexByDFS(root,labelHeight);
    //freeSuffixTreeByPostOrder(root);
}
int traverseEdge(string str, int idx, int start, int end)
{
    int k = 0;
    for(k=start; k<=end && str[idx] != '\0'; k++, idx++)
    {
        if(text[k] != str[idx])
            return -1;  // mo match
    }
    if(str[idx] == '\0')
        return 1;  // match
    return 0;  // more characters yet to match
}

int doTraversal(Node *n, string str, int idx)
{
    if(n == NULL)
    {
        return -1; // no match
    }
    int res = -1;
    //If node n is not root node, then traverse edge
    //from node n's parent to node n.
    if(n->start != -1)
    {
        res = traverseEdge(str, idx, n->start, *(n->end));
        if(res != 0)
            return res;  // match (res = 1) or no match (res = -1)
    }
    //Get the character index to search
    idx = idx + edgeLength(n);

    //If there is an edge from node n going out
    //with current character str[idx], traverse that edge

    if(n->children[str[idx]] != NULL)
        return doTraversal(n->children[str[idx]], str, idx);
    else
        return -1;  // no match
}

void checkForSubString(string str)
{
    int res = doTraversal(root, str, -1);
    if(res == 1)
        cout<<str<<" is present"<<"\n";
    else
        cout<<str<<" is not present"<<"\n";
}

int main()
{
    cin>>text;
    buildSuffixTree();
    checkForSubString("abc");
    return 0;
}
