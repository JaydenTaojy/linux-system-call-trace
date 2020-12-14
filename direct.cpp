// This program is mostly based on the examples provided in the turtorial with the addition of direct and indirect control flow check
#include <iostream>
#include <fstream>
#include "pin.H"
using std::cerr;
using std::ofstream;
using std::ios;
using std::string;
using std::endl;


FILE * direct;

static UINT64 bcount = 0;
static UINT64 bcount2 = 0;

VOID PIN_FAST_ANALYSIS_CALL docount() { bcount ++; }
VOID PIN_FAST_ANALYSIS_CALL docount2() { bcount2 ++; }

VOID Trace(TRACE trace, VOID *v)
{
    // Visit every basic block  in the trace
    for (BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl = BBL_Next(bbl))
    {
    
        for (INS ins = BBL_InsHead(bbl); INS_Valid(ins); ins = INS_Next(ins))
        {
           
           if(INS_IsDirectControlFlow(ins)){
              INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(docount), IARG_FAST_ANALYSIS_CALL,
                              IARG_END);
           }
         else if(INS_IsIndirectControlFlow(ins))
         {
                INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(docount2),IARG_FAST_ANALYSIS_CALL,
                    IARG_END);

            }
        }
    }
}

VOID Fini(INT32 code, VOID *v)
{
    fprintf(direct,"Indirect control flow transfer instructions: %llu \n", bcount);
    fprintf(direct,"Indirect control flow transfer instructions: %llu \n", bcount2);
    fprintf(direct,"The program exited\n");
    fclose(direct);
}

INT32 Usage()
{
    cerr << "This tool counts the number of direct or indirect instructions executed" << endl;
    return -1;
}

int main(int argc, char * argv[])
{
    if (PIN_Init(argc, argv)) return Usage();

    direct = fopen("directcount.log", "w");
    
    TRACE_AddInstrumentFunction(Trace, 0);

    PIN_AddFiniFunction(Fini, 0);
    
    PIN_StartProgram();
    
    return 0;
}
