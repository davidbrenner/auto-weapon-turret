#ifndef __BLOBTRACK_H__
#define __BLOBTRACK_H__

#ifdef __cplusplus
int blobtrack_init(void);
void blobtrack_cleanup(void);
extern "C" {
#endif
    int adp_blobtrack_init(void);
    void adp_blobtrack_cleanup(void);
#ifdef __cplusplus
}
#endif

#endif
