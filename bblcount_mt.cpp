// This program is mostly based on the examples provided in the turtorial.
#include <iostream>
#include <fstream>
#include "pin.H"
using std::ostream;
using std::cout;
using std::cerr;
using std::string;
using std::endl;

FILE * trace;

INT32 numThreads = 0;
const INT32 MaxNumThreads = 10000;

#define PADSIZE 56  // 64byte linesize : 64 - 8
struct THREAD_DATA
{
    UINT64 _count;
    UINT8 _pad[PADSIZE];
}icount[MaxNumThreads];

VOID PIN_FAST_ANALYSIS_CALL docount(THREADID tid)
{
    icount[tid]._count ++;
}

VOID ThreadStart(THREADID threadid, CONTEXT *ctxt, INT32 flags, VOID *v)
{
    numThreads++;
    ASSERT(numThreads <= MaxNumThreads, "Maximum number of threads exceeded\n");
}

VOID Trace(TRACE trace, VOID *v)
{
    // Visit every basic block  in the trace
    for (BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl = BBL_Next(bbl))
    {
        BBL_InsertCall(bbl, IPOINT_ANYWHERE, (AFUNPTR)docount, IARG_FAST_ANALYSIS_CALL, IARG_THREAD_ID, IARG_END);
    }
}


VOID Fini(INT32 code, VOID *v)
{
    for (INT32 t=0; t<numThreads; t++){
     fprintf(trace,"Basic block Count of thread#%d=: %llu \n", t,icount[t]._count);
        fclose(trace);
    }
    fclose(trace);
}


INT32 Usage()
{
    cerr << "This Pintool counts the number of dynamic instructions executed" << endl;
    cerr << endl << KNOB_BASE::StringKnobSummary() << endl;
    return 1;
}


int main(int argc, char * argv[])
{
    // Initialize pin
    if (PIN_Init(argc, argv)) return Usage();

    trace = fopen("bblcount_mt.log", "w");
    
    PIN_AddThreadStartFunction(ThreadStart, NULL);
    for (INT32 t=0; t<MaxNumThreads; t++) {
        icount[t]._count = 0;}
    PIN_AddFiniFunction(Fini, NULL);

    TRACE_AddInstrumentFunction(Trace, NULL);

    PIN_StartProgram();
    
    return 1;
}
