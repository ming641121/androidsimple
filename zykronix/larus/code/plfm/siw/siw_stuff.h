
#ifndef _SIW_STUFF_H_
#define _SIW_STUFF_H_

#define SAR_BUF_SIZE      2048
#define SAR_BUF_HEADER    sizeof(MGMBFD_t) - sizeof(MGMDATA_t)
#define SIW_HEADER_SIZE   12

/**** this temporary define here, it needs to include from SAR *****/
typedef struct {
 char   data[SAR_BUF_SIZE];
} MGMDATA_t;

struct mgmbfd_t {
 struct mgmbfd_t  *next;
 U32_t      length;
 U32_t      offset;
 U8_t       siwHeader[SIW_HEADER_SIZE];
 MGMDATA_t  data;
};

typedef struct mgmbfd_t MGMBFD_t;

#endif  /* _SIW_STUFF_H_ */
