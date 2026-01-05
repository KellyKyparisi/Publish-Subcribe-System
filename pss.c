/***************************************************************
 *
 * file: pss.h
 *
 * @Author Kyriaki Kiouri Kyparisi
 * @Version 20-10-2020
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
#include "pss.h"

/**
 * @brief Optional function to initialize data structures that
 *        need initialization
 *
 * @return 0 on success
 *         1 on failure
 */
struct SubInfo *S = NULL;
struct Group G[MG];

int initialize()
{
    int i;
    for (i = 0; i < MG; i++)
    {
        G[i].gId = i; /* declare unique id for the groups from 0 to MG-1*/
        G[i].gfirst = NULL;
        G[i].glast = NULL;
        G[i].ggsub = NULL;
    }
    return EXIT_SUCCESS;
}

/**
 * @brief Free resources
 *
 * @return 0 on success
 *         1 on failure
 */
int free_all(void)
{
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
int Insert_Info(int iTM, int iId, int *gids_arr, int size_of_gids_arr){
    if (iTM<0 ){
        printf("illegal argument\n");
        return EXIT_SUCCESS;
    }
    int  i;
    for(i=0;i<(size_of_gids_arr-1);i++){
        if (gids_arr[i]<0 || gids_arr[i]>MG){
            printf("illegal argument\n");
            return EXIT_SUCCESS;
        }
    }
    for (i=0 ; i<(size_of_gids_arr-1) ;i++){
        Info *check; /* checks if id is unique*/
        check=DL_LookUp(iId,G[gids_arr[i]].gfirst);
        if(check!=NULL){
            printf("Id already exists.\n");
            return EXIT_SUCCESS;
        }
        G[gids_arr[i]].gfirst=DL_Insert(iId,iTM,G[gids_arr[i]].gfirst);
        G[gids_arr[i]].glast=End_List(G[gids_arr[i]].gfirst);
        Info *pointer=NULL;
        pointer=DL_LookUp(iId,G[gids_arr[i]].gfirst);
        pointer->igp[gids_arr[i]]=1;
    }
    for(i=0;i<(size_of_gids_arr-1);i++){
        printf("GROUPID = <%d>, ",gids_arr[i]);
        DL_Print(G[gids_arr[i]].gfirst);
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
int Subscriber_Registration(int sTM, int sId, int *gids_arr, int size_of_gids_arr)
{
    int i;
    if (sTM<0 || size_of_gids_arr>MG){
        printf("illegal argument\n");
        return EXIT_SUCCESS;
    }

    for(i=0;i<size_of_gids_arr-1;i++){
        if(gids_arr[i]>=MG){
            printf("Illegal argument\n");
            return EXIT_SUCCESS;
        }
    }
    SubInfo *check;
    check=SL_LookUp(sId,S);
    if(check!=NULL){
        printf("That id exists.\n");
        return EXIT_SUCCESS;
    }
    SubInfo *k=NULL;
    S=SL_Insert(sTM,sId,S);
    k=SL_LookUp(sId,S);
    for(i=0;i<size_of_gids_arr-1;i++){
        G[gids_arr[i]].ggsub=L_Insert(sId,G[gids_arr[i]].ggsub);
        k->sgp[gids_arr[i]]=G[gids_arr[i]].glast;
    }
    SL_Print(S);
    for(i=0;i<size_of_gids_arr-1;i++){
        printf("GROUPID=<%d>, ",gids_arr[i]);
        L_Print(G[gids_arr[i]].ggsub);
    }

    return EXIT_SUCCESS;
}
/**
 * @brief Consume Information for subscriber
 *
 * @param sId Subscriber identifier
 * @return 0 on success
 *          1 on failure
 */
int Consume(int sId)
{
    SubInfo *check;
    check=SL_LookUp(sId,S);
    if(check==NULL){/*doesn't exist can't consume*/
        printf("That id doesn't exist\n");
        return EXIT_SUCCESS;
    }
    SubInfo *ptr;
    ptr=SL_LookUp(sId,S);
    int i;
    for (i=0;i<MG;i++){
        if(ptr->sgp[i]!=(Info *)1){
            printf("GROUPID =<%d>, ",i);
            if(ptr->sgp[i]==NULL){
                if(G[i].gfirst!=NULL){/*there is info in group*/
                    Info *inf=G[i].glast;
                    printf("INFOLIST = <");
                    while(inf!=NULL){
                        printf("%d, ",inf->iId);
                        inf=inf->iprev;
                    }
                    printf(">, NEWSGP = <%d>\n",G[i].gfirst->iId);
                }else{
                    printf("INFOLIST=<>, NEWSGP=<>\n");
                }
            }else{

                Info *inf=ptr->sgp[i]->iprev;
                if(inf!=NULL){
                    printf("INFOLIST = <");
                    while(inf!=NULL){
                        printf("%d, ",inf->iId);
                        inf=inf->iprev;
                    }
                    printf(">, NEWSGP = <%d>\n",G[i].gfirst->iId);
                }else{
                    printf("INFOLIST=<>, NEWSGP=<%d>\n",ptr->sgp[i]->iId);
                }
            }
            ptr->sgp[i]=G[i].gfirst;
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
int Delete_Subscriber(int sId)
{
    int oldg[MG]={0}; /* for the groups sid was interested*/
    SubInfo *del;
    del=SL_LookUp(sId,S);
    if (del==NULL){ /*checks if it exists*/
        printf("That id doesn't exist.\n");
        return EXIT_SUCCESS;
    }
    int i;
    for(i=0;i<MG;i++){
        if(del->sgp[i]!= (Info *)1){
            oldg[i]=1;
        }
    }
    S=SL_Delete(sId,S);
    SL_Print(S);
    for(i=0;i<MG;i++){
        if(oldg[i]==1){
            printf("GROUPID = <%d>",i);
            G[i].ggsub = L_Delete(sId,G[i].ggsub);
            L_Print(G[i].ggsub);
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
int Print_all(void)
{
    /* prints groups' ids and Info list */
    int i,j;
    for(i=0;i<MG;i++){
        printf("GROUPID = <%d>, ",i);
        if(G[i].gfirst!=NULL){/*there is info in group*/
            Info *iprint;
            iprint=G[i].glast;
            printf("INFOLIST = <");
            while(iprint!=NULL){
                if(iprint->iprev==NULL){
                    printf("%d",iprint->iId);
                }else{
                    printf("%d, ",iprint->iId);
                }
                iprint=iprint->iprev;
            }
        }else {
            printf("INFOLIST= <");
        }
        printf(">\n");
    }
    /*prints subscribers list*/
    SL_Print(S);

    /*prints subscriber id, group id */
    int gsum[MG]={0};
    int ssum=0;
    SubInfo *sprint;
    sprint=S;
    while(sprint!=NULL){
        ssum++; /*for number of subscribers*/
        printf("SUBSCRIBERID = <%d>, GROUPLIST = <",sprint->sId);
        for (i=0;i<MG;i++){
            if(sprint->sgp[i]!=(Info *)1){ /*the groups sub is interested in*/
                printf("%d, ",i);
                gsum[i]=gsum[i]+1;
            }
        }
        sprint=sprint->snext;
        printf(">\n");
    }
    /*number of groups and subscribers*/
    int number_of_groups=0;
    for(i=0;i<MG;i++){
        if(gsum[i]!=0){
            number_of_groups++;
        }
    }
    printf("NO_GROUPS = <%d>, ",number_of_groups);
    printf("NO_SUBSCRIBERS  = <%d> ",ssum);
    printf("\n");
    return EXIT_SUCCESS;
}


/*step 1*/

struct SubInfo *SL_LookUp(int psId,struct SubInfo *S)
{
    struct SubInfo *p;
    p = S;
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

struct SubInfo *SL_Insert(int pstm, int psId,struct SubInfo *S){
    /*SubInfo *check=SL_LookUp(psId,S);
    if (check->sId==psId){ /*it already exists
        return NULL;
    }*/

    struct SubInfo *node =NULL;
    int i;
    node=(struct SubInfo*) malloc(sizeof(struct SubInfo));
    node->sId=psId;
    node->stm=pstm;
    for(i=0;i<MG;i++){
        node->sgp[i]=(struct Info *)1;
    }
    node->snext=NULL;
    if(S==NULL){
        S=node;
        return S;
    }
    struct SubInfo *ptr=S;
    if(ptr->stm<=pstm){     /* first node */
        node->snext=ptr;
        S=node;
        return S;
    }else if(ptr->stm>pstm && ptr->snext==NULL){
        ptr->snext=node;
        return S;
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

    return S;
}

struct SubInfo *SL_Delete( int psId,struct SubInfo *S)
{
    /* check if id exists first*/
    struct SubInfo *p = SL_LookUp(psId,S);
    if (S == NULL)
    {
        printf("Can't delete anything, subscriber's list is already empty .\n");
        return S;
    }
    else if (p == NULL)
    {
        printf("That Id doesn't exist.\n");
        return S;
    }
    else if (p != NULL)
    {
        if (p == S)
        { /* if it's the first node the one i want to delete*/
            S = p->snext;
        }
        else
        {
            struct SubInfo *temp = S; /* traverse 'till I found the previous one*/
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
    return S;
}

void SL_Print(struct SubInfo *S)
{

    struct SubInfo *p = S;
    printf("SUBSCRIBERLIST =<");
    while (p != NULL)
    {
        if(p->snext==NULL){
            printf("%d", p->sId);
        }else{
            printf("%d, ", p->sId);
        }
        p=p->snext;
    }
    printf(">\n");
}


/* step 2*/

struct Info *DL_LookUp(int piId,Info * I)
{ /* sorted double linked list*/
    struct Info *p;
    p = I;
    while (p != NULL)
    {
        if (p->iId != piId)
        {
            p = p->inext;
        }
        else if (p->iId == piId)
        {
            return p; /* found that iId and returns pointer*/
        }
    }
    return NULL; /* empty list or over boundaries*/
}

Info* DL_Insert(int iId, int itm,Info* I)
{
    /*Info *check=DL_LookUp(iId,I);
    if (check->iId==iId){ /*it already exists s
        return NULL;
    }*/
    Info *node = (Info*) malloc(sizeof(Info));
    node->itm=itm;
    node->iId=iId;
    int i;
    for(i=0;i<MG;i++){
        node->igp[i]=0;
    }
    node->iprev=NULL;
    node->inext=NULL;
    if(I==NULL){
        I=node;
        return I;
    }
    Info* p=I;
    while(p!=NULL){
        if(p->itm<=itm){
            if(p->iprev!=NULL){
                node->iprev=p->iprev;
                p->iprev->inext=node;
                p->iprev=node;
                node->inext=p;
                break;
            }else{
                node->inext=p;
                p->iprev=node;
                I=node;
                break;
            }
        }else if(p->inext==NULL){
            p->inext=node;
            node->iprev=p;
            break;
        }
        p=p->inext;
    }
    return I;
}

Info* DL_Delete(int piId,Info *I)
{
    struct Info *p = DL_LookUp(piId,I);
    if (I == NULL)
    {
        printf("Can't delete anything, Info's list is already empty .\n");
        return EXIT_SUCCESS;
    }
    else if (p == NULL)
    {
        printf("That Id doesn't exist.\n");
        return EXIT_SUCCESS;
    }
    else if (p != NULL) /* the id exists, so we can delete it*/
    {

        if (p == I)
        { /* if it's the first node the one i want to delete*/
            I = p->inext;
            p->inext->iprev = NULL;
        }
        else
        {
            struct Info *temp = I;
            if (p->inext == NULL)
            {
                p->iprev->inext = NULL; /* deletes the last node*/
            }
            else
            {
                p->iprev->inext = p->inext; /* deletes the p node*/
            }
        }
    }
    return I;
}
struct Info *End_List(struct Info *I){
    struct Info *p = I;
    while (p->inext != NULL)
    {
        p = p->inext;
    }
    return p;
}
void DL_Print(struct Info *I)
{
    struct Info *p = I;
    printf("INFOLIST==<");
    while (p != NULL)
    {
        printf("%d, ", p->iId);
        p = p->inext;
    }
    printf(">\n");
}

/* step4 */

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

    if (lfound != NULL)
    { /* found that sId*/
        printf("That Id already exists.\n");
        return EXIT_SUCCESS;
    }
    else /* id is unique so do insert it*/
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
    printf("SUBLIST=< ");
    while (p != NULL)
    {
        printf("%d, ", p->sId);
        p=p->snext;
    }
    printf("> \n");

}

