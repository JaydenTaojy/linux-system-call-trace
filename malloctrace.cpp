// This program is mostly based on the examples provided in the turtorial.
#include <stdio.h>
#include "pin.H"

#define MALLOC "malloc"
UINT64 _count=0;
UINT64 _size=0;
    
FILE * trace;

VOID ThreadStart(THREADID threadid, CONTEXT *ctxt, INT32 flags, VOID *v)
{
    fprintf(trace, "thread begin %d\n",threadid);
    fflush(trace);
}

VOID ThreadFini(THREADID threadid, const CONTEXT *ctxt, INT32 code, VOID *v)
{
    fprintf(trace, "thread end %d\n",threadid);
    fflush(trace);
}
 
void * MallocWrapper( CONTEXT * ctxt, AFUNPTR pf_malloc, size_t size) {
  void * res;
  _count++;
  _size+=size;
  PIN_CallApplicationFunction(ctxt, PIN_ThreadId(), CALLINGSTD_DEFAULT, pf_malloc, NULL,  PIN_PARG(void *), &res, PIN_PARG(size_t), size,  PIN_PARG_END());
  return res;
}

// This routine is executed for each image.
VOID ImageLoad(IMG img, VOID *)
{
    if (strstr(IMG_Name(img).c_str(), "libc.so") ||
        strstr(IMG_Name(img).c_str(), "MSVCR80") || strstr(IMG_Name(img).c_str(), "MSVCR90")){
    RTN rtn = RTN_FindByName(img, "malloc");
    
    if ( RTN_Valid( rtn ))
    {
        
        //wrapper
        PROTO protoMalloc = PROTO_Allocate( PIN_PARG(void *), CALLINGSTD_DEFAULT, "malloc", PIN_PARG(size_t), PIN_PARG_END());
        RTN_ReplaceSignature(rtn, AFUNPTR(MallocWrapper), IARG_PROTOTYPE, protoMalloc, IARG_CONST_CONTEXT, IARG_ORIG_FUNCPTR, IARG_FUNCARG_ENTRYPOINT_VALUE, 0, IARG_END);
    
    }}
}

// This routine is executed once at the end.
VOID Fini(INT32 code, VOID *v)
{
    fprintf(trace, "the total time of malloc call is %llu\n", _count);
    fprintf(trace, "the total size of malloc call is %llu\n", _size);
    fflush(trace);
    fclose(trace);
}

INT32 Usage()
{
    PIN_ERROR("This Pintool prints a trace of malloc calls in the guest application\n"
              + KNOB_BASE::StringKnobSummary() + "\n");
    return -1;
}

int main(INT32 argc, CHAR **argv)
{
    
    if (PIN_Init(argc, argv)) return Usage();
    PIN_InitSymbols();
    
    trace = fopen("malloctrace.log", "w");
    
    IMG_AddInstrumentFunction(ImageLoad, 0);

    PIN_AddThreadStartFunction(ThreadStart, 0);
    PIN_AddThreadFiniFunction(ThreadFini, 0);

    PIN_AddFiniFunction(Fini, 0);
    
    PIN_StartProgram();
    
    return 0;
}
