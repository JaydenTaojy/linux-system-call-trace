// This program is mostly based on the examples provided in the turtorial.
#include <iostream>
#include <fstream>
#include "pin.H"
using std::cerr;
using std::ofstream;
using std::ios;
using std::string;
using std::endl;


FILE * trace;
static UINT64 bcount = 0;

VOID PIN_FAST_ANALYSIS_CALL docount() { bcount ++; }
    

VOID Trace(TRACE trace, VOID *v)
{
    // Visit every basic block  in the trace
    for (BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl = BBL_Next(bbl))
    {
        // Insert a call to docount for every bbl, passing the number of instructions.
        // IPOINT_ANYWHERE allows Pin to schedule the call anywhere in the bbl to obtain best performance.
    
       // BBL_InsertCall(bbl, IPOINT_ANYWHERE, AFUNPTR(docount), IARG_FAST_ANALYSIS_CALL, IARG_UINT32, BBL_NumIns(bbl), IARG_END);
      BBL_InsertCall(bbl, IPOINT_ANYWHERE, AFUNPTR(docount), IARG_FAST_ANALYSIS_CALL, IARG_END);
    }
}

// This function is called when the application exits
VOID Fini(INT32 code, VOID *v)
{
    fprintf(trace,"Basic block Count: %llu \n", bcount);
    fclose(trace);
}

INT32 Usage()
{
    cerr << "This tool counts the number of dynamic instructions executed" << endl;
    //cerr << endl << KNOB_BASE::StringKnobSummary() << endl;
    return -1;
}

int main(int argc, char * argv[])
{
    // Initialize pin
    if (PIN_Init(argc, argv)) return Usage();

    //OutFile.open(KnobOutputFile.Value().c_str());
    trace = fopen("bblcount.log", "w");
    // Register Instruction to be called to instrument instructions
    TRACE_AddInstrumentFunction(Trace, 0);

    // Register Fini to be called when the application exits
    PIN_AddFiniFunction(Fini, 0);
    
    // Start the program, never returns
    PIN_StartProgram();
    
    return 0;
}
