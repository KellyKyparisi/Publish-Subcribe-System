/***************************************************************
 *
 * file: pss.h
 *
 * @Author  Kyriaki Kiouri Kyparisi
 * @Version 30-11-2022
 *
 * @e-mail
 *
 * @brief   Implementation of the "pss.h" header file for the Public Subscribe System,
 * function definitions
 *
 *
 ***************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "pss.h"

/**
 * @brief Optional function to initialize data structures that
 *        need initialization
 *
 * @param m Size of the hash table.
 * @param p Prime number for the universal hash functions.
 *
 * @return 0 on success
 *         1 on failure
 */
struct SubInfo **S = NULL;
struct Group G[MG];
int HashSize;
int prime;
int a,b;

int initialize(int m, int p){
    HashSize=m;
    prime=p;
    S=(SubInfo**)malloc(HashSize*sizeof(SubInfo*));
    int i;
    for (i = 0; i < HashSize; i++){
        S[i]=NULL;
    }
    for (i = 0; i < MG; i++)
    {
        G[i].gId = i;   /* declare unique id for the groups from 0 to MG-1*/
        G[i].gr= NULL;
        G[i].gsub = NULL;
    }
    srand(time(0));
    a=rand()%(prime-1)+1;
    b=rand()%(prime-1);
    return EXIT_SUCCESS;
}
/**
 * @brief Free resources
 *
 * @return 0 on success
 *         1 on failure
 */
int free_all(void){
    return EXIT_SUCCESS;
}

/**
 * @brief Insert info
 *
 * @param iTM Timestamp of arrival
 * @param iId Identifier of information
 * @param gids_arr Pointer to array containing the gids of the Event.
 * @param size_of_gids_arr Size of gids_arr including -1
 * @return 0 on success
 *          1 on failure
 */
int Insert_Info(int iTM,int iId,int* gids_arr,int size_of_gids_arr){
    if (iTM<0 ){
        printf("illegal argument\n");
        return EXIT_FAILURE;
    }
    int  i;
    for (i=0 ; i<(size_of_gids_arr-1) ;i++){
        if (gids_arr[i]<0 || gids_arr[i]>MG){
            printf("illegal argument\n");
            return EXIT_FAILURE;
        }
        Info *check; /* checks if id is unique*/
        check=BST_LookUp(iId,G[gids_arr[i]].gr);
        if(check!=NULL){
            printf("Id already exists.\n");
            return EXIT_SUCCESS;
        }
        G[gids_arr[i]].gr=BST_Insert(iTM,iId,G[gids_arr[i]].gr);
        Info *pointer=NULL;
        pointer=BST_LookUp(iId,G[gids_arr[i]].gr);
        pointer->igp[gids_arr[i]]=1;
    }
    for(i=0;i<(size_of_gids_arr-1);i++){
        printf("\tGROUPID = <%d>, ",gids_arr[i]);
        printf("INFOLIST==<");
        BST_Print(G[gids_arr[i]].gr);
        printf(">\n");
    }
    return EXIT_SUCCESS;
}
/**
 * @brief Subsriber Registration
 *
 * @param sTM Timestamp of arrival
 * @param sId Identifier of subscriber
 * @param gids_arr Pointer to array containing the gids of the Event.
 * @param size_of_gids_arr Size of gids_arr including -1
 * @return 0 on success
 *          1 on failure
 */
int Subscriber_Registration(int sTM,int sId,int* gids_arr,int size_of_gids_arr){
    int i;
    if (sTM<0 || size_of_gids_arr>MG){
        printf("illegal argument\n");
        return EXIT_SUCCESS;
    }

    for(i=0;i<size_of_gids_arr-1;i++){
        if(gids_arr[i]>=MG){
            printf("Illegal argument %d\n",gids_arr[i]);
            return EXIT_SUCCESS;
        }
        struct Subscription *lfound = L_LookUp(sId,G[gids_arr[i]].gsub); /* make sure every sId is unique*/
        if (lfound != NULL)
        { /* found that sId*/
            printf("That Id already exists.\n");
            return EXIT_SUCCESS;
        }
        G[gids_arr[i]].gsub=L_Insert(sId,G[gids_arr[i]].gsub);
    }
    HT_Insert(sTM,sId);
    SubInfo *ptr;
    ptr=HT_Lookup(sId);
    HT_Print(1);

    SubInfo *k=NULL;
    for(i=0;i<(size_of_gids_arr-1);i++){
        ptr->sgp[gids_arr[i]]=NULL;
        ptr->tgp[gids_arr[i]]=NULL;
        printf("\tGROUPID=<%d>, ",gids_arr[i]);
        L_Print(G[gids_arr[i]].gsub);
    }
    return EXIT_SUCCESS;
}
/**
 * @brief Prune Information from server and forward it to client
 *
 * @param tm Information timestamp of arrival
 * @return 0 on success
 *          1 on failure
 */
int Prune(int tm){
    int i;
    for(i=0;i<MG;i++){
        struct Subscription *ptr=G[i].gsub;
        while(ptr!=NULL){/*gia kathe sid tou group*/
            struct SubInfo *sub=HT_Lookup(ptr->sId);/*psaxno to hashtable*/
            sub->tgp[i]=PruneInsert(tm,G[i].gr,sub,i);
            ptr=ptr->snext;
        }
        /*deletes the info*/
        G[i].gr=PruneDelete(tm,G[i].gr);
        printf("\tGROUPID = <%d>, INFOLIST = <",i);
        BST_Print(G[i].gr);
        printf(">, ");
        L_Print(G[i].gsub);
    }
    HT_Print(0);

    return EXIT_SUCCESS;
}
/**
 * @brief Consume Information for subscriber
 *
 * @param sId Subscriber identifier
 * @return 0 on success
 *          1 on failure
 */
int Consume(int sId){
    SubInfo *check=HT_Lookup(sId);
    if(check==NULL){/*doesn't exist can't consume*/
        printf("That id doesn't exist\n");
        return EXIT_SUCCESS;
    }
    int i;
    for (i=0;i<MG;i++){
        TreeInfo *last=NULL;
        if(check->sgp[i]!=(TreeInfo *)1){ /*is interested*/
            printf("\tGROUPID =<%d>, ",i);
            if(check->sgp[i]==NULL){
                printf("INFOLIST = <");
                last=LO_BST_Print(check->tgp[i],-1);
                if(check->tgp[i]!=NULL){/*there is info in the tree*/
                    printf(">, NEWSGP = <%d>\n",last->tId);
                }else{
                    printf(">, NEWSGP=<>\n");
                }
            }else{/*exei ksana ginei consume dld*/
                TreeInfo *inf=check->tgp[i];
                if(inf!=NULL){
                    check->sgp[i]=LO_BST_LookUp(check->sgp[i]->ttm,check->sgp[i]);/*o sgp deixneise fyllo panta*/
                    printf("INFOLIST = <");
                    last=LO_BST_Print(check->sgp[i],-1);
                    printf(">, NEWSGP = <%d>\n",last->tId);
                }else{
                    printf("INFOLIST=<>, NEWSGP=<%d>\n",last->tId);
                }
            }
            check->sgp[i]=last;/*update sgp via LO*/
        }
    }

    return EXIT_SUCCESS;
}
/**
 * @brief Delete subscriber
 *
 * @param sId Subscriber identifier
 * @return 0 on success
 *          1 on failure
 */
int Delete_Subscriber(int sId){
    int oldg[MG]={0}; /* for groups sid was interested*/
    SubInfo *del= HT_Lookup(sId);
    if (del==NULL){
        printf("That id doesn't exist.\n");
        return EXIT_SUCCESS;
    }
    int i;
    for(i=0;i<MG;i++){
        if(del->sgp[i]!= (TreeInfo *)1){
            oldg[i]=1;
        }
    }
    HT_Delete(sId);
    HT_Print(1);
    for(i=0;i<MG;i++){
        if(oldg[i]==1){
            printf("\tGROUPID = <%d>",i);
            G[i].gsub = L_Delete(sId,G[i].gsub);
            L_Print(G[i].gsub);
        }
    }
    return EXIT_SUCCESS;
}
/**
 * @brief Print Data Structures of the system
 *
 * @return 0 on success
 *          1 on failure
 */
int Print_all(void){
    int i;
    int number_of_groups=0;
    for(i=0;i<MG;i++){
        printf("\tGROUPID = <%d>,INFOLIST = <",i);
        if(G[i].gr!=NULL){/*there is info in group*/
            number_of_groups++;
            BST_Print(G[i].gr);
        }
        printf(">");
        L_Print(G[i].gsub);
    }
    /*prints subscribers*/
    int number_of_subs=0;
    number_of_subs=HT_Print(1);
    /* prints SUBSCRIBERID and groupid */
    HT_Print(0);
    
    
    printf("\tNO_GROUPS = <%d>, ",number_of_groups);
    printf("NO_SUBSCRIBERS  = <%d> ",number_of_subs);
    printf("\n");
    return EXIT_SUCCESS;
}

/*my steps*/
/*Binary Search Tree code*/
int IsLeaf(struct Info *p){
    if(p->ilc==NULL && p->irc==NULL){
        return 1;
    }
    return 0;
}
struct Info *HasChild(struct Info *p){
    if(p->ilc!=NULL && p->irc==NULL){
        return p->ilc; /*has left child*/
    }else if(p->ilc==NULL && p->irc!=NULL){
        return p->irc;/*has right child*/
    }else{
        return NULL; /*has both children*/
    }
}
struct TreeInfo *PruneInsert(int tm,struct Info *root,struct SubInfo *sub,int gpos){

    if(root==NULL){
        return sub->tgp[gpos];
    }
    PruneInsert(tm,root->ilc,sub,gpos);
    PruneInsert(tm,root->irc,sub,gpos);
    if(root->itm<=tm){ /*postorder*/
        if (sub->tgp[gpos] != (struct TreeInfo *)1) {
            sub->tgp[gpos]=LO_BST_Insert(root->itm,root->iId,sub->tgp[gpos]);/*to kano insert sto leaf oriented*/
        }
    }
    return sub->tgp[gpos];
}
struct Info *PruneDelete(int tm,struct Info *root){
    if(root==NULL){
        return NULL;
    }
    PruneDelete(tm,root->ilc);
    PruneDelete(tm,root->irc);
    if (root->itm<=tm){
        struct Info *del=BST_LookUp(root->iId,root);
        BST_Delete(del->iId,del);
    }
    return root;
}
struct Info *BST_Insert(int itm,int iId,struct Info *I){
    int i;
    struct Info *node=(struct Info*) malloc(sizeof(struct Info));
    node->iId=iId;
    node->itm=itm;
    node->ilc=NULL;
    node->irc=NULL;
    node->ip=NULL;
    for(i=0;i<MG;i++){
        node->igp[i]=0;
    }
    if(I==NULL){
        I=node;
        return I;
    }
    struct Info *p=I;
    struct Info *prev=NULL;
    while(p!=NULL){
        prev=p;
        if(p->iId>iId){
            p=p->ilc;
        }else{
            p=p->irc;
        }
    }
    if(prev->iId>iId){
        if(prev->ilc==NULL){
            node->ip=prev;
            prev->ilc=node;
        }else{
            node->ilc=prev->ilc;
            prev->ilc->ip=node;
            node->ip=prev;
            prev->ilc=node;
        }
    }else{
        if(prev->irc==NULL){
            node->ip=prev;
            prev->irc=node;
        }else{
            node->irc=prev->irc;
            prev->irc->ip=node;
            node->ip=prev;
            prev->irc=node;
        }
    }
    return I; /*return root*/
}

struct Info *BST_LookUp(int iId,struct Info *I){
    struct Info *p=I;
    while( p!=NULL && iId!=p->iId ){
        if(iId>p->iId){
            p=p->irc;
        }else{
            p=p->ilc;
        }
    }
    return p;
}

struct Info *BST_Delete(int iId,struct Info *root){
    if(root==NULL){
        return NULL;
    }
    if(root->iId>iId){
        root->ilc=BST_Delete(iId,root->ilc);
    }
    else if(root->iId<iId){
        root->irc=BST_Delete(iId,root->irc);
    }else{
        if(IsLeaf(root)==1){
            if(root->ip!=NULL && root->ip->iId>root->iId){
                root->ip->ilc=NULL;
            }else if(root->ip!=NULL && root->ip->iId>root->iId){
                root->ip->irc=NULL;

            }
            return NULL;
        }else if(HasChild(root)!=NULL){ /* has 1 child */
            if(root->ip!=NULL && root->ip->iId>root->iId){
                root->ip->ilc=NULL;
            }else if(root->ip!=NULL && root->ip->iId>root->iId ){
                root->ip->irc=NULL;
            }
            return HasChild(root);
        }else{/*has both*/
            struct Info* temp =root->irc;
            while (temp && temp->ilc != NULL){
                temp = temp->ilc;
            }    
            root->iId = temp->iId;
            if(root->ip!=NULL){
                if(root->ip->iId>root->iId){
                    root->ip->ilc=NULL;
                }else{
                    root->ip->irc=NULL;
                }
            }
            root->irc = BST_Delete(temp->iId,root->irc);
        }
    }
    return root;
}

void BST_Print(struct Info *p){ /*φθινουσα πριντ*/
    if(p==NULL){return; }
    BST_Print(p->irc);
    printf("%d,",p->iId);
    BST_Print(p->ilc);
}

/*Subcription code*/
struct Subscription *L_LookUp(int psId,Subscription *H)
{
    struct Subscription *p;
    p = H;
    while (p != NULL)
    {
        if (p->sId != psId)
        {
            p = p->snext;
        }
        else if (p->sId == psId)
        {
            return p; /* found that sId and returns pointer*/
        }
    }

    return NULL; /* empty list or over boundaries*/
}
Subscription *L_Insert(int psId,struct Subscription *ggsub)
{
    struct Subscription *p = ggsub;
    struct Subscription *lfound = L_LookUp(psId,ggsub); /* make sure every sId is unique*/
    if (lfound == NULL)/* id is unique so do insert it*/
    {

        if (ggsub == NULL) /*list is empty*/
        {
            struct Subscription *tmp = malloc(sizeof(struct Subscription));
            ggsub = tmp;
            tmp->sId=psId;
            tmp->snext = NULL;
        }
        else
        { /*adds node in the beginning*/
            struct Subscription *tmp = malloc(sizeof(struct Subscription));
            tmp->sId=psId;
            tmp->snext=ggsub;
            ggsub = tmp;
        }
    }
    return ggsub;
}
Subscription  *L_Delete(int psId,Subscription  *H)
{
    /*check if id exists first*/
    struct Subscription *p = L_LookUp(psId,H);
    if (H == NULL)
    {
        printf("Can't delete anything, subscriber's list is already empty .\n");
        return EXIT_SUCCESS;
    }
    else if (p == NULL)
    {
        printf("That Id doesn't exist.\n");
        return EXIT_SUCCESS;
    }
    else if (p != NULL) /* id it exists*/
    {
        if (p == H)
        { /* if it's the first node the one i want to delete*/
            H= p->snext;
        }
        else
        {
            struct Subscription *temp = H; /* traverse 'till I found the previous one*/
            while (temp->snext != p)
            {
                temp = temp->snext;
            }                     /* found the prev*/
            if (p->snext == NULL) /* deletes the last node*/
            {
                temp->snext = NULL;
            }
            else
            {
                temp->snext = p->snext; /* deletes p node*/
            }
        }
    }
    return H;
}
void L_Print(struct Subscription *H){
    struct Subscription *p=H;
    printf(" SUBLIST=<");
    while (p != NULL)
    {
        printf("%d, ", p->sId);
        p=p->snext;
    }
    printf("> \n");

}
/*hash function code*/
int HashFunction(int sId){
    return (((a*sId)+b)%prime)%HashSize;
}
void HT_Insert(int stm,int sId){
    int pos=HashFunction(sId);
    S[pos]=SL_Insert(stm,sId,S[pos]);
}
void HT_Delete(int sId){
    int pos=HashFunction(sId);
    S[pos]=SL_Delete(sId,S[pos]);
}
int HT_Print(int k){
    if(k==1){ /*print for S event*/
        int i;
        int num=0;
        printf(" SUBSCRIBERLIST =<");
        for(i=0;i<HashSize;i++){
            if(S[i]!=NULL){
                num=num+SL_Print(S[i]);
            }
        }
        printf(">\n");
        return num;/*number of subscribers*/
    }else{ /*print for R event*/
        int i;
        for(i=0;i<HashSize;i++){
            if(S[i]!=NULL){
                printf("\tSUBSCRIBERID =<");
                SL_Print(S[i]);
                printf(">,GROUPLIST =\n");
                int j;
                for(j=0;j<MG;j++){
                    printf("\t\t<%d>,TREELIST =<",j);
                    if(S[i]->tgp[j]!=(TreeInfo *)1 && S[i]->tgp[j]!=NULL) {
                        LO_BST_Print(S[i]->tgp[j],j);
                    }
                    printf(">\n");
                }
            }
        }
        return 0;
    }
}
SubInfo *HT_Lookup(int sId){
    int pos=HashFunction(sId);
    SubInfo *ptr=S[pos];
    return SL_LookUp(ptr,sId);
}

/*SubInfo code*/
struct SubInfo *SL_Insert(int pstm, int psId,struct SubInfo *siList){
    struct SubInfo *node =NULL;
    int i;
    node=(struct SubInfo*) malloc(sizeof(struct SubInfo));
    node->sId=psId;
    node->stm=pstm;
    for(i=0;i<MG;i++){
        node->sgp[i]=(struct TreeInfo *)1;
        node->tgp[i]=(struct TreeInfo *)1;
    }
    node->snext=NULL;
    if(siList==NULL){
        siList=node;
        return siList;
    }
    struct SubInfo *ptr=siList;
    if(ptr->stm<=pstm){     /* first node */
        node->snext=ptr;
        siList=node;
        return siList;
    }else if(ptr->stm>pstm && ptr->snext==NULL){
        ptr->snext=node;
        return siList;
    }

    while(ptr!=NULL){
        if(ptr->snext==NULL){
            ptr->snext=node;
            break;
        }else if(ptr->snext->stm<=pstm){
            node->snext=ptr->snext;
            ptr->snext=node;
            break;
        }
        ptr=ptr->snext;
    }

    return siList;
}

struct SubInfo *SL_Delete(int psId,struct SubInfo *siList)
{
    /* check if id exists first*/
    struct SubInfo *p = SL_LookUp(S[HashFunction(psId)],psId);
    if (siList == NULL)
    {
        printf("Can't delete anything, subscriber's list is already empty .\n");
        return siList;
    }
    else if (p == NULL)
    {
        printf("That Id doesn't exist.\n");
        return siList;
    }
    else if (p != NULL)
    {
        if (p == siList)
        { /* if it's the first node the one i want to delete*/
            siList = p->snext;
        }
        else
        {
            struct SubInfo *temp = siList; /* traverse 'till I found the previous one*/
            while (temp->snext != p)
            {
                temp = temp->snext;
            }                     /* found the prev*/
            if (p->snext == NULL) /* deletes the last node*/
            {
                temp->snext = NULL;
            }
            else
            {
                temp->snext = p->snext;/* deletes p node*/
            }
        }
    }
    free(p);
    return siList;
}
struct SubInfo *SL_LookUp(SubInfo *l,int psId)
{
    int pos=HashFunction(psId);
    struct SubInfo *p;
    p = S[pos];
    while (p != NULL)
    {
        if (p->sId == psId)
        {
            return p; /* found that sId and returns pointer*/
        }
        else
        {
            p=p->snext;
        }
    }
    return NULL; /* empty list or over boundaries*/
}

int SL_Print(struct SubInfo *siList)
{
    struct SubInfo *p = siList;
    int num=0;
    while (p != NULL)
    {
        if(p->snext==NULL){
            num++;
            printf("%d,", p->sId);
        }
        p=p->snext;
    }
    return num;
}

/*leaf oriented code*/
int IsLeafTreeInfo(TreeInfo *T){
    if(T->trc==NULL && T->tlc==NULL){
        return 1;
    }
    return 0;
}
TreeInfo *LO_BST_LookUp(int ttm,TreeInfo *T){
    if(T==NULL || ((T->ttm==ttm) && (1==IsLeafTreeInfo(T)))){
        return T; /*found it*/
    }
    if(ttm<=T->ttm){
        return LO_BST_LookUp(ttm,T->tlc);
    }else{
        return LO_BST_LookUp(ttm,T->trc);
    }
}

TreeInfo *newNode(int ttm,int tId){
    struct TreeInfo *node=(struct TreeInfo*) malloc(sizeof(struct TreeInfo));
    node->ttm=ttm;
    node->tId=tId;
    node->tlc=NULL;
    node->trc=NULL;
    node->tp=NULL;
    node->next=NULL;
    node->prev=NULL;
    return node;
}

TreeInfo *LO_BST_Insert(int ttm,int tId,TreeInfo *T){
    TreeInfo *node;
    if(T==NULL){
        node=newNode(ttm,tId);
        return node;
    }
    TreeInfo *p=T;
    while(p->tlc!=NULL){p=p->tlc;} /*pio aristero leaf (plires dentro)*/
    while(p->ttm<ttm && p->next!=NULL){
        p=p->next;
    }

    if(p->ttm>ttm){
        p->tlc=newNode(ttm,tId);
        p->tlc->tp=p;
        p->trc=newNode(p->ttm,p->tId);
        p->trc->tp=p;
        p->ttm=ttm;
        p->tId=tId;
        if(p->prev!=NULL){
            p->prev->next=p->tlc;
            p->tlc->prev=p->prev;
            p->prev=NULL;
        }
        if (p->next!=NULL)
        {
            p->next->prev=p->trc;
            p->trc->next=p->next;
            p->next=NULL;
        }

    }else{ /*insert sto telos tis listas*/
        p->trc=newNode(ttm,tId);
        p->trc->tp=p;
        p->tlc=newNode(p->ttm,p->tId);
        p->tlc->tp=p;
        if (p->prev != NULL){
            p->prev->next=p->tlc;
        }
        p->tlc->prev=p->prev;
        p->prev=NULL;
        p->trc->next=NULL;/*telos listas*/
    }
    p->tlc->next=p->trc;
    p->trc->prev=p->tlc;
    return T;
}
TreeInfo *LO_BST_Print(TreeInfo *T,int j){
    TreeInfo *p=T;
    TreeInfo *last=NULL;
    if(p==NULL){return NULL;}
    if(j>-1){
        while(p->tlc!=NULL){p=p->tlc;}
        while(p!=NULL){
            last=p;
            printf("%d(%d),",p->ttm,j);
            p=p->next;
        }
        return last;
    }    
    else{
        while(p->tlc!=NULL){p=p->tlc;}
        while(p!=NULL){
            last=p;
            printf("%d,",p->ttm);
            p=p->next;
        }
        return last;
    }
}