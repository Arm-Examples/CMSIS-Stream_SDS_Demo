#ifndef _GRAPH_CONTEXT_H_

#include "sds_rec.h"
#include "sds_play.h"

#ifdef   __cplusplus
extern "C" {
#endif


/*

Information used to communicate between the graph and the
main application.



*/
typedef struct {
   /* Track recorder errors */
   int recorderHasError;
   sdsRecId_t recorderWithError;
   /* Track player errors */
   int playerHasError;
   sdsPlayId_t playerWithError;
} graph_context;


#ifdef   __cplusplus
}
#endif

#endif