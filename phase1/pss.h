/***************************************************************
 *
 * file: pss.h
 *
 * @Author  Kyriaki Kiouri Kyparisi
 * @Version 20-10-2020
 *
 * @e-mail       hy240-list@csd.uoc.gr
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
    struct Info *iprev;
    struct Info *inext;
}Info;
typedef struct Subscription {
    int sId;
    struct Subscription *snext;
}Subscription;
typedef struct Group {
    int gId;
    struct Subscription *ggsub;
    struct Info *gfirst;
    struct Info *glast;
}Group;
typedef struct SubInfo {
    int sId;
    int stm;
    struct Info *sgp[MG];
    struct SubInfo *snext;
}SubInfo;

/**
 * @brief Optional function to initialize data structures that
 *        need initialization
 *
 * @return 0 on success
 *         1 on failure
 */
int initialize(void);

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

/*step 1 */
struct SubInfo *SL_LookUp(int psId,struct SubInfo *S);
struct SubInfo *SL_Insert(int pstm, int psId,struct SubInfo *S);
struct SubInfo *SL_Delete( int psId,struct SubInfo *S);
void SL_Print(struct SubInfo *S);

/*step 2 */
struct Info *DL_LookUp(int piId,Info * I);
Info* DL_Insert(int iId, int itm,Info* I);
Info* DL_Delete(int piId,Info *I);
struct Info *End_List(struct Info *I);
void DL_Print(struct Info *I);

/*step 4 */
struct Subscription *L_LookUp(int psId,Subscription *H);
Subscription *L_Insert(int psId,struct Subscription *ggsub);
Subscription  *L_Delete(int psId,Subscription  *H);
void L_Print(struct Subscription *H);

#endif /* pss_h */

