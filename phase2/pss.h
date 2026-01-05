/***************************************************************
 *
 * file: pss.h
 *
 * @Authors  Kyriaki Kiouri Kyparisi
 * @Version 30-11-2022
 *
 *
 * @brief   Header file for the Public Subscribe System,
 * with the structures and function prototypes
 *
 ***************************************************************
 */

#ifndef pss_h
#define pss_h
#define MG 64

typedef struct Info {
    int iId;
    int itm;
    int igp[MG];
    struct Info *ilc;
    struct Info *irc;
    struct Info *ip;
}Info;

typedef struct Subscription {
    int sId;
    struct Subscription *snext;
}Subscription;

typedef struct Group {
    int gId;
    struct Subscription *gsub;
    struct Info *gr;
}Group;

typedef struct SubInfo {
    int sId;
    int stm;
    struct TreeInfo *tgp[MG];
    struct TreeInfo *sgp[MG];
    struct SubInfo *snext;
}SubInfo;

typedef struct TreeInfo {
    int tId;
    int ttm;
    struct TreeInfo *tlc;
    struct TreeInfo *trc;
    struct TreeInfo *tp;
    struct TreeInfo *prev;
    struct TreeInfo *next;
}TreeInfo;

int IsLeaf(struct Info *p);
struct Info *HasChild(struct Info *p);
struct Info *BST_Insert(int itm,int iId,struct Info *I);
struct TreeInfo *PruneInsert(int itm,struct Info *I,struct SubInfo *sub,int gpos);
struct Info *PruneDelete(int tm,struct Info *root);
//void InsertLO(struct Info *root,struct Subscription *gsub,int gpos);
struct Info *BST_LookUp(int iId,Info *I);
struct Info *BST_Delete(int iId,struct Info *I);
void BST_Print(struct Info *p);
struct Subscription *L_LookUp(int psId,Subscription *H);
Subscription *L_Insert(int psId,struct Subscription *gsub);
Subscription  *L_Delete(int psId,Subscription  *H);
void L_Print(struct Subscription *H);
int HashFunction(int sId);
void HT_Insert(int stm,int sId);
void HT_Delete(int sId);
int HT_Print(int k);
struct SubInfo *HT_Lookup(int sId);
struct SubInfo *SL_Insert(int pstm, int psId,struct SubInfo *S);
struct SubInfo *SL_LookUp(struct SubInfo *S,int psId);
struct SubInfo *SL_Delete( int psId,struct SubInfo *siList);
int SL_Print(struct SubInfo *S);
struct TreeInfo *LO_BST_Print(TreeInfo *T,int j);
TreeInfo *LO_BST_Insert(int ttm,int tId,TreeInfo *T);
TreeInfo *LO_BST_LookUp(int ttm,TreeInfo *T);
int IsLeafTreeInfo(TreeInfo *T);
TreeInfo *newNode(int ttm,int tIm);
/**
 * @brief Optional function to initialize data structures that
 *        need initialization
 *
 * @param m Size of hash table
 * @param p Prime number for the universal hash function
 *
 * @return 0 on success
 *         1 on failure
 */
int initialize(int m, int p);

/**
 * @brief Free resources
 *
 * @return 0 on success
 *         1 on failure
 */
int free_all(void);

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
int Insert_Info(int iTM,int iId,int* gids_arr,int size_of_gids_arr);

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
int Subscriber_Registration(int sTM,int sId,int* gids_arr,int size_of_gids_arr);

/**
 * @brief Prune Information from server and forward it to client
 *
 * @param tm Information timestamp of arrival
 * @return 0 on success
 *          1 on failure
 */
int Prune(int tm);

/**
 * @brief Consume Information for subscriber
 *
 * @param sId Subscriber identifier
 * @return 0 on success
 *          1 on failure
 */
int Consume(int sId);

/**
 * @brief Delete subscriber
 *
 * @param sId Subscriber identifier
 * @return 0 on success
 *          1 on failure
 */
int Delete_Subscriber(int sId);

/**
 * @brief Print Data Structures of the system
 *
 * @return 0 on success
 *          1 on failure
 */
int Print_all(void);

#endif /* pss_h */

