
#ifndef MTR_CLNT_H_
#define MTR_CLNT_H_


mtrStatus_t SendCommand(void *req, mtrID_t mtrID, cmdID_t cmd, uint32_t *data);

#endif
