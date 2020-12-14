#include <stdio.h>
#include <iostream>
#include <cstring>
#include <stdlib.h>
#include <inttypes.h>
#include <sstream>
#include <syscall.h>
#include <fstream>
#include<map>
#include <string> 
#include "pin.H"

using namespace std;

FILE * btrace;

int syscount=0; //to flag syscall happen
int sysnum=0;   //to record syscall number for return use

// used for parse the output of write syscall
string sanitize(char a) {
    switch(a){
    case '\t':
       return "\\t";
    case '\n':
        return "\\n";
     case '\r':
        return "\\r";
     case ' ':
        return " ";
     default: {
        stringstream ss;
        ss << a;
        string ret;
        ss >> ret;
        return ret;
     }}
}

//Build the syscall table at the beginging by reading table.txt
map<int, string> stable;

void setstable(){
    string stro;
    char str[100],*p,*q;
    ifstream tablemap;
    tablemap.open("table.txt");
    while(getline(tablemap,stro)){
    strcpy(str,stro.c_str());
    p=strtok(str," ");
    int sum = atoi(p);
    q=strtok(NULL, " ");
    string s2(q);
    stable.insert(pair<int,string>(sum,s2));
    }
}


VOID SyscallBefore(ADDRINT ip, ADDRINT num, ADDRINT arg0, ADDRINT arg1, ADDRINT arg2, ADDRINT arg3, ADDRINT arg4, ADDRINT arg5)
{
     syscount=1; //set syscall countered
     sysnum=num; // record syscall number
    // To deal with the different systems calls, since I just covered dozens system
    //calls, I set others to output like with systems call number and the long
    //unsigned int value for arguments.
    switch(num){
      case SYS_access:
      fprintf(btrace,"access(%s, %i)\n",(char *)arg0,(int)arg1);
            break;
        case SYS_read:{
      int len = static_cast<int>(arg2);
        fprintf(btrace,"read(%x, %p, %i)\n", (unsigned int)arg0, (char *)arg1, len);
        }
        break;
      case SYS_write:{
      int len = static_cast<int>(arg2);
      string written = "'";
      char *buf = (char *)arg1;
      for (int i = 0; i < len; i++) {
          if (buf[i] == '\0') {
              break;
          } else {
              char z = buf[i];
              written += sanitize(z);
          }
      }
      written += "'";
      fprintf(btrace,"write(%i, %s, %i)\n",
          (int)arg0,
          written.c_str(),
              len);}
          break;
      case SYS_open:
      fprintf(btrace,"open(%s, %i, %u)\n", (const char *)arg0, (int)arg1, (int)arg2);
          break;
     case SYS_close:
       fprintf(btrace,"close(%u)\n", (unsigned int)arg0);
       break;
     case SYS_creat:
      fprintf(btrace,"creat(%s, %i)\n", (const char *)arg0, (int)arg1);
    break;
     case SYS_chmod:
      fprintf(btrace,"chmod(%s, %u)\n", (const char *)arg0, (mode_t)arg1);
       break;
     case SYS_stat:
      fprintf(btrace,"stat(%s, 0x%p)\n", (const char *)arg0, (void *)arg1);
       break;
    case SYS_ptrace: //not sure{
      fprintf(btrace,"ptrace(%i, %i, 0x%p, 0x%p)\n", (int)arg0, (pid_t)arg1, (void *)arg2, (void *)arg3);
    break;
    case SYS_kill:
      fprintf(btrace,"kill(%i, %i)\n", (pid_t)arg0, (int)arg1);
       break;
     case SYS_ustat:
      fprintf(btrace,"ustat(%u, 0x%p)\n", (dev_t)arg0, (struct ustat *)arg1);
    break;
    case SYS_lstat:
      fprintf(btrace,"lstat(%s, 0x%p)\n", (const char *)arg0, (void *)arg1);
    break;
    case SYS_stat64:
      fprintf(btrace,"stat64(%s, 0x%p)\n", (const char *)arg0, (void *)arg1);
      break;
    case SYS_lstat64:
      fprintf(btrace,"lstat64(%s, 0x%p)\n", (const char *)arg0, (void *)arg1);
      break;
     case SYS_fstat:
      fprintf(btrace,"fstat(%i, 0x%p)\n", (int)arg0, (void *)arg1);
    break;
    case SYS_fstat64:
      fprintf(btrace,"fstat64(%lu, %p)\n", (unsigned long)arg0, (void *)arg1);
    break;
    case SYS_getcwd:
      fprintf(btrace,"getcwd(0x%p, %lu)\n", (unsigned *)arg0, (unsigned long)arg1);
    break;
    case SYS_exit:
      fprintf(btrace,"exit(%i)\n", (int)arg0);
    break;
    case SYS_lseek:
      fprintf(btrace,"lseek(%i, %lu, %i)\n", (int)arg0,(off_t)arg1, (int)arg2);
    break;
    case SYS_lsetxattr:
      fprintf(btrace,"lsetxattr(%s, %s, 0x%p, %u, %i)\n", (const char *)arg0, (const char *)arg1, (const void *)arg2, (size_t)arg3, (int)arg4);
    break;
    case SYS_mkdir:
      fprintf(btrace,"mkdir(%s, %i)\n", (const char *)arg0, (int)arg1);
    break;
    case 192:{
        fprintf(btrace,"mmap2(0x%lx, 0x%lx, 0x%lx, 0x%lx, 0x%lx, 0x%lx)\n", (unsigned long)arg0, (unsigned long)arg1, (unsigned long)arg2, (unsigned long)arg3, (unsigned long)arg4,
        (unsigned long)arg5);
       }
    break;
    default :{
      fprintf(btrace,"%s(0x%lx, 0x%lx, 0x%lx, 0x%lx, 0x%lx, 0x%lx)\n", stable[num].c_str(),(unsigned long)arg0, (unsigned long)arg1, (unsigned long)arg2, (unsigned long)arg3, (unsigned long)arg4,
      (unsigned long)arg5);}
    break;
    }
}

VOID SyscallAfter(const CONTEXT * ctxt)
{
    syscount=0;
    //fprintf(btrace,"%d\n", syscount);
    int regVal;
    regVal = PIN_GetContextReg(ctxt, REG_EAX);
    switch(sysnum){
        case 192:
         fprintf(btrace,"returns: %p\n", (void*)regVal);
      break;
    case 45:
      fprintf(btrace,"returns: %p\n", (void*)regVal);
      break;
    default:
        fprintf(btrace,"returns: %i\n", regVal);
      break;
    }
    fflush(btrace);
}

VOID Trace(TRACE trace, VOID *v)
{
    //for instrument a trace, starting at the first basic block, check BBLS one by
    //one, for each BBL, starting at the first INS, check INS one by one,
    //if
    for (BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl = BBL_Next(bbl))
    {
    
        for (INS ins = BBL_InsHead(bbl); INS_Valid(ins); ins = INS_Next(ins))
        {
           
           if (syscount==1){
               syscount=0;
              //read the eax to get reurn value and print it
              INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SyscallAfter),
                             IARG_CONTEXT,
                              IARG_END);
           }
         //if (INS_IsSyscall(ins) && INS_HasFallThrough(ins))
         else if (INS_IsSyscall(ins))
         {
                //syscount=1;
                //fprintf(btrace,"%d\n", syscount);
             // incert call to print the syscall and arguments
                INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SyscallBefore),
                               IARG_INST_PTR, IARG_SYSCALL_NUMBER,
                               IARG_SYSARG_VALUE, 0, IARG_SYSARG_VALUE, 1,
                               IARG_SYSARG_VALUE, 2, IARG_SYSARG_VALUE, 3,
                               IARG_SYSARG_VALUE, 4, IARG_SYSARG_VALUE, 5,
                               IARG_END);

            }
        }
    }
    
}

VOID Fini(INT32 code, VOID *v)
{
    fprintf(btrace,"The program exited\n");
    fclose(btrace);
}

INT32 Usage()
{
    cerr <<"This tool prints a log of system calls"<< endl;
    return -1;
}

int main(int argc, char *argv[])
{
    if (PIN_Init(argc, argv)) return Usage();

    btrace = fopen("btrace.log", "w");
    
    setstable();//set the syscall number to name map
    
    TRACE_AddInstrumentFunction(Trace, 0);
    
    PIN_AddFiniFunction(Fini, 0);

    PIN_StartProgram();
    
    return 0;
}
