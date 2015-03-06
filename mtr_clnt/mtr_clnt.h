#ifndef TLM_CLNT_H_
#define TLM_CLNT_H_


//Attach to mtrSrv via shared mem
//In 1: pointer to be alloc'd to the shared memory
//In 2: key for that shared memory space
//ret: 0 if OK, else an error occured
int MtrClntShmat(mtrCmdIf_t** self, key_t key);

//In 1: shm ptr
//In 2: motor to run command on
//In 3: command to run
//In 4: data to send/recv
//In 5: timeout in ms before returning failure
//ret:  0=OK, -1 = Timeout
int MtrClntSendCmd(mtrCmdIf_t* self, mtrID_t mtr, cmdID_t cmd, uint32_t *data, const uint32_t timeout); 

#endif
