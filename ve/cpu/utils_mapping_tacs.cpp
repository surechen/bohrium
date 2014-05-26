#include "utils.hpp"
//
//  NOTE: This file is autogenerated based on the tac-definition.
//        You should therefore not edit it manually.
//
using namespace std;
namespace bohrium{
namespace core{

void instrs_to_tacs(bh_instruction* instrs, bh_intp ninstrs, vector<tac_t>& tacs, SymbolTable& symbol_table)
{
    // Reset metadata
    int omask = 0;        // And the operation mask
    
    for(bh_intp idx=0; idx<ninstrs; ++idx) {

        bh_instruction& instr = instrs[idx];        

        uint32_t out=0, in1=0, in2=0;

        //
        // Program packing: output argument
        // NOTE: All but BH_NONE has an output which is an array
        if (instr.opcode != BH_NONE) {
            out = symbol_table.map_operand(instr, 0);
        }

        //
        // Program packing; operator, operand and input argument(s).
        switch (instr.opcode) {    // [OPCODE_SWITCH]

            case BH_ABSOLUTE:
                in1 = symbol_table.map_operand(instr, 1);

                tacs[idx].op    = MAP;  // TAC
                tacs[idx].oper  = ABSOLUTE;
                tacs[idx].ext   = NULL;
                tacs[idx].out   = out;
                tacs[idx].in1   = in1;
                tacs[idx].in2   = in2;
            
                omask |= MAP;    // Operationmask
                break;
            case BH_ARCCOS:
                in1 = symbol_table.map_operand(instr, 1);

                tacs[idx].op    = MAP;  // TAC
                tacs[idx].oper  = ARCCOS;
                tacs[idx].ext   = NULL;
                tacs[idx].out   = out;
                tacs[idx].in1   = in1;
                tacs[idx].in2   = in2;
            
                omask |= MAP;    // Operationmask
                break;
            case BH_ARCCOSH:
                in1 = symbol_table.map_operand(instr, 1);

                tacs[idx].op    = MAP;  // TAC
                tacs[idx].oper  = ARCCOSH;
                tacs[idx].ext   = NULL;
                tacs[idx].out   = out;
                tacs[idx].in1   = in1;
                tacs[idx].in2   = in2;
            
                omask |= MAP;    // Operationmask
                break;
            case BH_ARCSIN:
                in1 = symbol_table.map_operand(instr, 1);

                tacs[idx].op    = MAP;  // TAC
                tacs[idx].oper  = ARCSIN;
                tacs[idx].ext   = NULL;
                tacs[idx].out   = out;
                tacs[idx].in1   = in1;
                tacs[idx].in2   = in2;
            
                omask |= MAP;    // Operationmask
                break;
            case BH_ARCSINH:
                in1 = symbol_table.map_operand(instr, 1);

                tacs[idx].op    = MAP;  // TAC
                tacs[idx].oper  = ARCSINH;
                tacs[idx].ext   = NULL;
                tacs[idx].out   = out;
                tacs[idx].in1   = in1;
                tacs[idx].in2   = in2;
            
                omask |= MAP;    // Operationmask
                break;
            case BH_ARCTAN:
                in1 = symbol_table.map_operand(instr, 1);

                tacs[idx].op    = MAP;  // TAC
                tacs[idx].oper  = ARCTAN;
                tacs[idx].ext   = NULL;
                tacs[idx].out   = out;
                tacs[idx].in1   = in1;
                tacs[idx].in2   = in2;
            
                omask |= MAP;    // Operationmask
                break;
            case BH_ARCTANH:
                in1 = symbol_table.map_operand(instr, 1);

                tacs[idx].op    = MAP;  // TAC
                tacs[idx].oper  = ARCTANH;
                tacs[idx].ext   = NULL;
                tacs[idx].out   = out;
                tacs[idx].in1   = in1;
                tacs[idx].in2   = in2;
            
                omask |= MAP;    // Operationmask
                break;
            case BH_CEIL:
                in1 = symbol_table.map_operand(instr, 1);

                tacs[idx].op    = MAP;  // TAC
                tacs[idx].oper  = CEIL;
                tacs[idx].ext   = NULL;
                tacs[idx].out   = out;
                tacs[idx].in1   = in1;
                tacs[idx].in2   = in2;
            
                omask |= MAP;    // Operationmask
                break;
            case BH_COS:
                in1 = symbol_table.map_operand(instr, 1);

                tacs[idx].op    = MAP;  // TAC
                tacs[idx].oper  = COS;
                tacs[idx].ext   = NULL;
                tacs[idx].out   = out;
                tacs[idx].in1   = in1;
                tacs[idx].in2   = in2;
            
                omask |= MAP;    // Operationmask
                break;
            case BH_COSH:
                in1 = symbol_table.map_operand(instr, 1);

                tacs[idx].op    = MAP;  // TAC
                tacs[idx].oper  = COSH;
                tacs[idx].ext   = NULL;
                tacs[idx].out   = out;
                tacs[idx].in1   = in1;
                tacs[idx].in2   = in2;
            
                omask |= MAP;    // Operationmask
                break;
            case BH_EXP:
                in1 = symbol_table.map_operand(instr, 1);

                tacs[idx].op    = MAP;  // TAC
                tacs[idx].oper  = EXP;
                tacs[idx].ext   = NULL;
                tacs[idx].out   = out;
                tacs[idx].in1   = in1;
                tacs[idx].in2   = in2;
            
                omask |= MAP;    // Operationmask
                break;
            case BH_EXP2:
                in1 = symbol_table.map_operand(instr, 1);

                tacs[idx].op    = MAP;  // TAC
                tacs[idx].oper  = EXP2;
                tacs[idx].ext   = NULL;
                tacs[idx].out   = out;
                tacs[idx].in1   = in1;
                tacs[idx].in2   = in2;
            
                omask |= MAP;    // Operationmask
                break;
            case BH_EXPM1:
                in1 = symbol_table.map_operand(instr, 1);

                tacs[idx].op    = MAP;  // TAC
                tacs[idx].oper  = EXPM1;
                tacs[idx].ext   = NULL;
                tacs[idx].out   = out;
                tacs[idx].in1   = in1;
                tacs[idx].in2   = in2;
            
                omask |= MAP;    // Operationmask
                break;
            case BH_FLOOR:
                in1 = symbol_table.map_operand(instr, 1);

                tacs[idx].op    = MAP;  // TAC
                tacs[idx].oper  = FLOOR;
                tacs[idx].ext   = NULL;
                tacs[idx].out   = out;
                tacs[idx].in1   = in1;
                tacs[idx].in2   = in2;
            
                omask |= MAP;    // Operationmask
                break;
            case BH_IDENTITY:
                in1 = symbol_table.map_operand(instr, 1);

                tacs[idx].op    = MAP;  // TAC
                tacs[idx].oper  = IDENTITY;
                tacs[idx].ext   = NULL;
                tacs[idx].out   = out;
                tacs[idx].in1   = in1;
                tacs[idx].in2   = in2;
            
                omask |= MAP;    // Operationmask
                break;
            case BH_IMAG:
                in1 = symbol_table.map_operand(instr, 1);

                tacs[idx].op    = MAP;  // TAC
                tacs[idx].oper  = IMAG;
                tacs[idx].ext   = NULL;
                tacs[idx].out   = out;
                tacs[idx].in1   = in1;
                tacs[idx].in2   = in2;
            
                omask |= MAP;    // Operationmask
                break;
            case BH_INVERT:
                in1 = symbol_table.map_operand(instr, 1);

                tacs[idx].op    = MAP;  // TAC
                tacs[idx].oper  = INVERT;
                tacs[idx].ext   = NULL;
                tacs[idx].out   = out;
                tacs[idx].in1   = in1;
                tacs[idx].in2   = in2;
            
                omask |= MAP;    // Operationmask
                break;
            case BH_ISINF:
                in1 = symbol_table.map_operand(instr, 1);

                tacs[idx].op    = MAP;  // TAC
                tacs[idx].oper  = ISINF;
                tacs[idx].ext   = NULL;
                tacs[idx].out   = out;
                tacs[idx].in1   = in1;
                tacs[idx].in2   = in2;
            
                omask |= MAP;    // Operationmask
                break;
            case BH_ISNAN:
                in1 = symbol_table.map_operand(instr, 1);

                tacs[idx].op    = MAP;  // TAC
                tacs[idx].oper  = ISNAN;
                tacs[idx].ext   = NULL;
                tacs[idx].out   = out;
                tacs[idx].in1   = in1;
                tacs[idx].in2   = in2;
            
                omask |= MAP;    // Operationmask
                break;
            case BH_LOG:
                in1 = symbol_table.map_operand(instr, 1);

                tacs[idx].op    = MAP;  // TAC
                tacs[idx].oper  = LOG;
                tacs[idx].ext   = NULL;
                tacs[idx].out   = out;
                tacs[idx].in1   = in1;
                tacs[idx].in2   = in2;
            
                omask |= MAP;    // Operationmask
                break;
            case BH_LOG10:
                in1 = symbol_table.map_operand(instr, 1);

                tacs[idx].op    = MAP;  // TAC
                tacs[idx].oper  = LOG10;
                tacs[idx].ext   = NULL;
                tacs[idx].out   = out;
                tacs[idx].in1   = in1;
                tacs[idx].in2   = in2;
            
                omask |= MAP;    // Operationmask
                break;
            case BH_LOG1P:
                in1 = symbol_table.map_operand(instr, 1);

                tacs[idx].op    = MAP;  // TAC
                tacs[idx].oper  = LOG1P;
                tacs[idx].ext   = NULL;
                tacs[idx].out   = out;
                tacs[idx].in1   = in1;
                tacs[idx].in2   = in2;
            
                omask |= MAP;    // Operationmask
                break;
            case BH_LOG2:
                in1 = symbol_table.map_operand(instr, 1);

                tacs[idx].op    = MAP;  // TAC
                tacs[idx].oper  = LOG2;
                tacs[idx].ext   = NULL;
                tacs[idx].out   = out;
                tacs[idx].in1   = in1;
                tacs[idx].in2   = in2;
            
                omask |= MAP;    // Operationmask
                break;
            case BH_LOGICAL_NOT:
                in1 = symbol_table.map_operand(instr, 1);

                tacs[idx].op    = MAP;  // TAC
                tacs[idx].oper  = LOGICAL_NOT;
                tacs[idx].ext   = NULL;
                tacs[idx].out   = out;
                tacs[idx].in1   = in1;
                tacs[idx].in2   = in2;
            
                omask |= MAP;    // Operationmask
                break;
            case BH_REAL:
                in1 = symbol_table.map_operand(instr, 1);

                tacs[idx].op    = MAP;  // TAC
                tacs[idx].oper  = REAL;
                tacs[idx].ext   = NULL;
                tacs[idx].out   = out;
                tacs[idx].in1   = in1;
                tacs[idx].in2   = in2;
            
                omask |= MAP;    // Operationmask
                break;
            case BH_RINT:
                in1 = symbol_table.map_operand(instr, 1);

                tacs[idx].op    = MAP;  // TAC
                tacs[idx].oper  = RINT;
                tacs[idx].ext   = NULL;
                tacs[idx].out   = out;
                tacs[idx].in1   = in1;
                tacs[idx].in2   = in2;
            
                omask |= MAP;    // Operationmask
                break;
            case BH_SIN:
                in1 = symbol_table.map_operand(instr, 1);

                tacs[idx].op    = MAP;  // TAC
                tacs[idx].oper  = SIN;
                tacs[idx].ext   = NULL;
                tacs[idx].out   = out;
                tacs[idx].in1   = in1;
                tacs[idx].in2   = in2;
            
                omask |= MAP;    // Operationmask
                break;
            case BH_SINH:
                in1 = symbol_table.map_operand(instr, 1);

                tacs[idx].op    = MAP;  // TAC
                tacs[idx].oper  = SINH;
                tacs[idx].ext   = NULL;
                tacs[idx].out   = out;
                tacs[idx].in1   = in1;
                tacs[idx].in2   = in2;
            
                omask |= MAP;    // Operationmask
                break;
            case BH_SQRT:
                in1 = symbol_table.map_operand(instr, 1);

                tacs[idx].op    = MAP;  // TAC
                tacs[idx].oper  = SQRT;
                tacs[idx].ext   = NULL;
                tacs[idx].out   = out;
                tacs[idx].in1   = in1;
                tacs[idx].in2   = in2;
            
                omask |= MAP;    // Operationmask
                break;
            case BH_TAN:
                in1 = symbol_table.map_operand(instr, 1);

                tacs[idx].op    = MAP;  // TAC
                tacs[idx].oper  = TAN;
                tacs[idx].ext   = NULL;
                tacs[idx].out   = out;
                tacs[idx].in1   = in1;
                tacs[idx].in2   = in2;
            
                omask |= MAP;    // Operationmask
                break;
            case BH_TANH:
                in1 = symbol_table.map_operand(instr, 1);

                tacs[idx].op    = MAP;  // TAC
                tacs[idx].oper  = TANH;
                tacs[idx].ext   = NULL;
                tacs[idx].out   = out;
                tacs[idx].in1   = in1;
                tacs[idx].in2   = in2;
            
                omask |= MAP;    // Operationmask
                break;
            case BH_TRUNC:
                in1 = symbol_table.map_operand(instr, 1);

                tacs[idx].op    = MAP;  // TAC
                tacs[idx].oper  = TRUNC;
                tacs[idx].ext   = NULL;
                tacs[idx].out   = out;
                tacs[idx].in1   = in1;
                tacs[idx].in2   = in2;
            
                omask |= MAP;    // Operationmask
                break;
            case BH_ADD:
                in1 = symbol_table.map_operand(instr, 1);
                in2 = symbol_table.map_operand(instr, 2);

                tacs[idx].op    = ZIP;  // TAC
                tacs[idx].oper  = ADD;
                tacs[idx].ext   = NULL;
                tacs[idx].out   = out;
                tacs[idx].in1   = in1;
                tacs[idx].in2   = in2;
            
                omask |= ZIP;    // Operationmask
                break;
            case BH_ARCTAN2:
                in1 = symbol_table.map_operand(instr, 1);
                in2 = symbol_table.map_operand(instr, 2);

                tacs[idx].op    = ZIP;  // TAC
                tacs[idx].oper  = ARCTAN2;
                tacs[idx].ext   = NULL;
                tacs[idx].out   = out;
                tacs[idx].in1   = in1;
                tacs[idx].in2   = in2;
            
                omask |= ZIP;    // Operationmask
                break;
            case BH_BITWISE_AND:
                in1 = symbol_table.map_operand(instr, 1);
                in2 = symbol_table.map_operand(instr, 2);

                tacs[idx].op    = ZIP;  // TAC
                tacs[idx].oper  = BITWISE_AND;
                tacs[idx].ext   = NULL;
                tacs[idx].out   = out;
                tacs[idx].in1   = in1;
                tacs[idx].in2   = in2;
            
                omask |= ZIP;    // Operationmask
                break;
            case BH_BITWISE_OR:
                in1 = symbol_table.map_operand(instr, 1);
                in2 = symbol_table.map_operand(instr, 2);

                tacs[idx].op    = ZIP;  // TAC
                tacs[idx].oper  = BITWISE_OR;
                tacs[idx].ext   = NULL;
                tacs[idx].out   = out;
                tacs[idx].in1   = in1;
                tacs[idx].in2   = in2;
            
                omask |= ZIP;    // Operationmask
                break;
            case BH_BITWISE_XOR:
                in1 = symbol_table.map_operand(instr, 1);
                in2 = symbol_table.map_operand(instr, 2);

                tacs[idx].op    = ZIP;  // TAC
                tacs[idx].oper  = BITWISE_XOR;
                tacs[idx].ext   = NULL;
                tacs[idx].out   = out;
                tacs[idx].in1   = in1;
                tacs[idx].in2   = in2;
            
                omask |= ZIP;    // Operationmask
                break;
            case BH_DIVIDE:
                in1 = symbol_table.map_operand(instr, 1);
                in2 = symbol_table.map_operand(instr, 2);

                tacs[idx].op    = ZIP;  // TAC
                tacs[idx].oper  = DIVIDE;
                tacs[idx].ext   = NULL;
                tacs[idx].out   = out;
                tacs[idx].in1   = in1;
                tacs[idx].in2   = in2;
            
                omask |= ZIP;    // Operationmask
                break;
            case BH_EQUAL:
                in1 = symbol_table.map_operand(instr, 1);
                in2 = symbol_table.map_operand(instr, 2);

                tacs[idx].op    = ZIP;  // TAC
                tacs[idx].oper  = EQUAL;
                tacs[idx].ext   = NULL;
                tacs[idx].out   = out;
                tacs[idx].in1   = in1;
                tacs[idx].in2   = in2;
            
                omask |= ZIP;    // Operationmask
                break;
            case BH_GREATER:
                in1 = symbol_table.map_operand(instr, 1);
                in2 = symbol_table.map_operand(instr, 2);

                tacs[idx].op    = ZIP;  // TAC
                tacs[idx].oper  = GREATER;
                tacs[idx].ext   = NULL;
                tacs[idx].out   = out;
                tacs[idx].in1   = in1;
                tacs[idx].in2   = in2;
            
                omask |= ZIP;    // Operationmask
                break;
            case BH_GREATER_EQUAL:
                in1 = symbol_table.map_operand(instr, 1);
                in2 = symbol_table.map_operand(instr, 2);

                tacs[idx].op    = ZIP;  // TAC
                tacs[idx].oper  = GREATER_EQUAL;
                tacs[idx].ext   = NULL;
                tacs[idx].out   = out;
                tacs[idx].in1   = in1;
                tacs[idx].in2   = in2;
            
                omask |= ZIP;    // Operationmask
                break;
            case BH_LEFT_SHIFT:
                in1 = symbol_table.map_operand(instr, 1);
                in2 = symbol_table.map_operand(instr, 2);

                tacs[idx].op    = ZIP;  // TAC
                tacs[idx].oper  = LEFT_SHIFT;
                tacs[idx].ext   = NULL;
                tacs[idx].out   = out;
                tacs[idx].in1   = in1;
                tacs[idx].in2   = in2;
            
                omask |= ZIP;    // Operationmask
                break;
            case BH_LESS:
                in1 = symbol_table.map_operand(instr, 1);
                in2 = symbol_table.map_operand(instr, 2);

                tacs[idx].op    = ZIP;  // TAC
                tacs[idx].oper  = LESS;
                tacs[idx].ext   = NULL;
                tacs[idx].out   = out;
                tacs[idx].in1   = in1;
                tacs[idx].in2   = in2;
            
                omask |= ZIP;    // Operationmask
                break;
            case BH_LESS_EQUAL:
                in1 = symbol_table.map_operand(instr, 1);
                in2 = symbol_table.map_operand(instr, 2);

                tacs[idx].op    = ZIP;  // TAC
                tacs[idx].oper  = LESS_EQUAL;
                tacs[idx].ext   = NULL;
                tacs[idx].out   = out;
                tacs[idx].in1   = in1;
                tacs[idx].in2   = in2;
            
                omask |= ZIP;    // Operationmask
                break;
            case BH_LOGICAL_AND:
                in1 = symbol_table.map_operand(instr, 1);
                in2 = symbol_table.map_operand(instr, 2);

                tacs[idx].op    = ZIP;  // TAC
                tacs[idx].oper  = LOGICAL_AND;
                tacs[idx].ext   = NULL;
                tacs[idx].out   = out;
                tacs[idx].in1   = in1;
                tacs[idx].in2   = in2;
            
                omask |= ZIP;    // Operationmask
                break;
            case BH_LOGICAL_OR:
                in1 = symbol_table.map_operand(instr, 1);
                in2 = symbol_table.map_operand(instr, 2);

                tacs[idx].op    = ZIP;  // TAC
                tacs[idx].oper  = LOGICAL_OR;
                tacs[idx].ext   = NULL;
                tacs[idx].out   = out;
                tacs[idx].in1   = in1;
                tacs[idx].in2   = in2;
            
                omask |= ZIP;    // Operationmask
                break;
            case BH_LOGICAL_XOR:
                in1 = symbol_table.map_operand(instr, 1);
                in2 = symbol_table.map_operand(instr, 2);

                tacs[idx].op    = ZIP;  // TAC
                tacs[idx].oper  = LOGICAL_XOR;
                tacs[idx].ext   = NULL;
                tacs[idx].out   = out;
                tacs[idx].in1   = in1;
                tacs[idx].in2   = in2;
            
                omask |= ZIP;    // Operationmask
                break;
            case BH_MAXIMUM:
                in1 = symbol_table.map_operand(instr, 1);
                in2 = symbol_table.map_operand(instr, 2);

                tacs[idx].op    = ZIP;  // TAC
                tacs[idx].oper  = MAXIMUM;
                tacs[idx].ext   = NULL;
                tacs[idx].out   = out;
                tacs[idx].in1   = in1;
                tacs[idx].in2   = in2;
            
                omask |= ZIP;    // Operationmask
                break;
            case BH_MINIMUM:
                in1 = symbol_table.map_operand(instr, 1);
                in2 = symbol_table.map_operand(instr, 2);

                tacs[idx].op    = ZIP;  // TAC
                tacs[idx].oper  = MINIMUM;
                tacs[idx].ext   = NULL;
                tacs[idx].out   = out;
                tacs[idx].in1   = in1;
                tacs[idx].in2   = in2;
            
                omask |= ZIP;    // Operationmask
                break;
            case BH_MOD:
                in1 = symbol_table.map_operand(instr, 1);
                in2 = symbol_table.map_operand(instr, 2);

                tacs[idx].op    = ZIP;  // TAC
                tacs[idx].oper  = MOD;
                tacs[idx].ext   = NULL;
                tacs[idx].out   = out;
                tacs[idx].in1   = in1;
                tacs[idx].in2   = in2;
            
                omask |= ZIP;    // Operationmask
                break;
            case BH_MULTIPLY:
                in1 = symbol_table.map_operand(instr, 1);
                in2 = symbol_table.map_operand(instr, 2);

                tacs[idx].op    = ZIP;  // TAC
                tacs[idx].oper  = MULTIPLY;
                tacs[idx].ext   = NULL;
                tacs[idx].out   = out;
                tacs[idx].in1   = in1;
                tacs[idx].in2   = in2;
            
                omask |= ZIP;    // Operationmask
                break;
            case BH_NOT_EQUAL:
                in1 = symbol_table.map_operand(instr, 1);
                in2 = symbol_table.map_operand(instr, 2);

                tacs[idx].op    = ZIP;  // TAC
                tacs[idx].oper  = NOT_EQUAL;
                tacs[idx].ext   = NULL;
                tacs[idx].out   = out;
                tacs[idx].in1   = in1;
                tacs[idx].in2   = in2;
            
                omask |= ZIP;    // Operationmask
                break;
            case BH_POWER:
                in1 = symbol_table.map_operand(instr, 1);
                in2 = symbol_table.map_operand(instr, 2);

                tacs[idx].op    = ZIP;  // TAC
                tacs[idx].oper  = POWER;
                tacs[idx].ext   = NULL;
                tacs[idx].out   = out;
                tacs[idx].in1   = in1;
                tacs[idx].in2   = in2;
            
                omask |= ZIP;    // Operationmask
                break;
            case BH_RIGHT_SHIFT:
                in1 = symbol_table.map_operand(instr, 1);
                in2 = symbol_table.map_operand(instr, 2);

                tacs[idx].op    = ZIP;  // TAC
                tacs[idx].oper  = RIGHT_SHIFT;
                tacs[idx].ext   = NULL;
                tacs[idx].out   = out;
                tacs[idx].in1   = in1;
                tacs[idx].in2   = in2;
            
                omask |= ZIP;    // Operationmask
                break;
            case BH_SUBTRACT:
                in1 = symbol_table.map_operand(instr, 1);
                in2 = symbol_table.map_operand(instr, 2);

                tacs[idx].op    = ZIP;  // TAC
                tacs[idx].oper  = SUBTRACT;
                tacs[idx].ext   = NULL;
                tacs[idx].out   = out;
                tacs[idx].in1   = in1;
                tacs[idx].in2   = in2;
            
                omask |= ZIP;    // Operationmask
                break;
            case BH_ADD_REDUCE:
                in1 = symbol_table.map_operand(instr, 1);
                in2 = symbol_table.map_operand(instr, 2);

                tacs[idx].op    = REDUCE;  // TAC
                tacs[idx].oper  = ADD;
                tacs[idx].ext   = NULL;
                tacs[idx].out   = out;
                tacs[idx].in1   = in1;
                tacs[idx].in2   = in2;
            
                omask |= REDUCE;    // Operationmask
                break;
            case BH_BITWISE_AND_REDUCE:
                in1 = symbol_table.map_operand(instr, 1);
                in2 = symbol_table.map_operand(instr, 2);

                tacs[idx].op    = REDUCE;  // TAC
                tacs[idx].oper  = BITWISE_AND;
                tacs[idx].ext   = NULL;
                tacs[idx].out   = out;
                tacs[idx].in1   = in1;
                tacs[idx].in2   = in2;
            
                omask |= REDUCE;    // Operationmask
                break;
            case BH_BITWISE_OR_REDUCE:
                in1 = symbol_table.map_operand(instr, 1);
                in2 = symbol_table.map_operand(instr, 2);

                tacs[idx].op    = REDUCE;  // TAC
                tacs[idx].oper  = BITWISE_OR;
                tacs[idx].ext   = NULL;
                tacs[idx].out   = out;
                tacs[idx].in1   = in1;
                tacs[idx].in2   = in2;
            
                omask |= REDUCE;    // Operationmask
                break;
            case BH_BITWISE_XOR_REDUCE:
                in1 = symbol_table.map_operand(instr, 1);
                in2 = symbol_table.map_operand(instr, 2);

                tacs[idx].op    = REDUCE;  // TAC
                tacs[idx].oper  = BITWISE_XOR;
                tacs[idx].ext   = NULL;
                tacs[idx].out   = out;
                tacs[idx].in1   = in1;
                tacs[idx].in2   = in2;
            
                omask |= REDUCE;    // Operationmask
                break;
            case BH_LOGICAL_AND_REDUCE:
                in1 = symbol_table.map_operand(instr, 1);
                in2 = symbol_table.map_operand(instr, 2);

                tacs[idx].op    = REDUCE;  // TAC
                tacs[idx].oper  = LOGICAL_AND;
                tacs[idx].ext   = NULL;
                tacs[idx].out   = out;
                tacs[idx].in1   = in1;
                tacs[idx].in2   = in2;
            
                omask |= REDUCE;    // Operationmask
                break;
            case BH_LOGICAL_OR_REDUCE:
                in1 = symbol_table.map_operand(instr, 1);
                in2 = symbol_table.map_operand(instr, 2);

                tacs[idx].op    = REDUCE;  // TAC
                tacs[idx].oper  = LOGICAL_OR;
                tacs[idx].ext   = NULL;
                tacs[idx].out   = out;
                tacs[idx].in1   = in1;
                tacs[idx].in2   = in2;
            
                omask |= REDUCE;    // Operationmask
                break;
            case BH_LOGICAL_XOR_REDUCE:
                in1 = symbol_table.map_operand(instr, 1);
                in2 = symbol_table.map_operand(instr, 2);

                tacs[idx].op    = REDUCE;  // TAC
                tacs[idx].oper  = LOGICAL_XOR;
                tacs[idx].ext   = NULL;
                tacs[idx].out   = out;
                tacs[idx].in1   = in1;
                tacs[idx].in2   = in2;
            
                omask |= REDUCE;    // Operationmask
                break;
            case BH_MAXIMUM_REDUCE:
                in1 = symbol_table.map_operand(instr, 1);
                in2 = symbol_table.map_operand(instr, 2);

                tacs[idx].op    = REDUCE;  // TAC
                tacs[idx].oper  = MAXIMUM;
                tacs[idx].ext   = NULL;
                tacs[idx].out   = out;
                tacs[idx].in1   = in1;
                tacs[idx].in2   = in2;
            
                omask |= REDUCE;    // Operationmask
                break;
            case BH_MINIMUM_REDUCE:
                in1 = symbol_table.map_operand(instr, 1);
                in2 = symbol_table.map_operand(instr, 2);

                tacs[idx].op    = REDUCE;  // TAC
                tacs[idx].oper  = MINIMUM;
                tacs[idx].ext   = NULL;
                tacs[idx].out   = out;
                tacs[idx].in1   = in1;
                tacs[idx].in2   = in2;
            
                omask |= REDUCE;    // Operationmask
                break;
            case BH_MULTIPLY_REDUCE:
                in1 = symbol_table.map_operand(instr, 1);
                in2 = symbol_table.map_operand(instr, 2);

                tacs[idx].op    = REDUCE;  // TAC
                tacs[idx].oper  = MULTIPLY;
                tacs[idx].ext   = NULL;
                tacs[idx].out   = out;
                tacs[idx].in1   = in1;
                tacs[idx].in2   = in2;
            
                omask |= REDUCE;    // Operationmask
                break;
            case BH_ADD_ACCUMULATE:
                in1 = symbol_table.map_operand(instr, 1);
                in2 = symbol_table.map_operand(instr, 2);

                tacs[idx].op    = SCAN;  // TAC
                tacs[idx].oper  = ADD;
                tacs[idx].ext   = NULL;
                tacs[idx].out   = out;
                tacs[idx].in1   = in1;
                tacs[idx].in2   = in2;
            
                omask |= SCAN;    // Operationmask
                break;
            case BH_MULTIPLY_ACCUMULATE:
                in1 = symbol_table.map_operand(instr, 1);
                in2 = symbol_table.map_operand(instr, 2);

                tacs[idx].op    = SCAN;  // TAC
                tacs[idx].oper  = MULTIPLY;
                tacs[idx].ext   = NULL;
                tacs[idx].out   = out;
                tacs[idx].in1   = in1;
                tacs[idx].in2   = in2;
            
                omask |= SCAN;    // Operationmask
                break;
            case BH_RANDOM:
                in1 = symbol_table.map_operand(instr, 1);

                tacs[idx].op    = GENERATE;  // TAC
                tacs[idx].oper  = RANDOM;
                tacs[idx].ext   = NULL;
                tacs[idx].out   = out;
                tacs[idx].in1   = in1;
                tacs[idx].in2   = in2;
            
                omask |= GENERATE;    // Operationmask
                break;
            case BH_RANGE:

                tacs[idx].op    = GENERATE;  // TAC
                tacs[idx].oper  = RANGE;
                tacs[idx].ext   = NULL;
                tacs[idx].out   = out;
                tacs[idx].in1   = in1;
                tacs[idx].in2   = in2;
            
                omask |= GENERATE;    // Operationmask
                break;
            case BH_DISCARD:

                tacs[idx].op    = SYSTEM;  // TAC
                tacs[idx].oper  = DISCARD;
                tacs[idx].ext   = NULL;
                tacs[idx].out   = out;
                tacs[idx].in1   = in1;
                tacs[idx].in2   = in2;
            
                omask |= SYSTEM;    // Operationmask
                break;
            case BH_FREE:

                tacs[idx].op    = SYSTEM;  // TAC
                tacs[idx].oper  = FREE;
                tacs[idx].ext   = NULL;
                tacs[idx].out   = out;
                tacs[idx].in1   = in1;
                tacs[idx].in2   = in2;
            
                omask |= SYSTEM;    // Operationmask
                break;
            case BH_NONE:

                tacs[idx].op    = SYSTEM;  // TAC
                tacs[idx].oper  = NONE;
                tacs[idx].ext   = NULL;
                tacs[idx].out   = out;
                tacs[idx].in1   = in1;
                tacs[idx].in2   = in2;
            
                omask |= SYSTEM;    // Operationmask
                break;
            case BH_SYNC:

                tacs[idx].op    = SYSTEM;  // TAC
                tacs[idx].oper  = SYNC;
                tacs[idx].ext   = NULL;
                tacs[idx].out   = out;
                tacs[idx].in1   = in1;
                tacs[idx].in2   = in2;
            
                omask |= SYSTEM;    // Operationmask
                break;

            default:
                if (instr.opcode>=BH_MAX_OPCODE_ID) {   // Handle extensions here

                    in1 = symbol_table.map_operand(instr, 1);
                    in2 = symbol_table.map_operand(instr, 2);

                    tacs[idx].op   = EXTENSION;
                    tacs[idx].oper = EXTENSION_OPERATOR;
                    tacs[idx].ext  = &instr;
                    tacs[idx].out  = out;
                    tacs[idx].in1  = in1;
                    tacs[idx].in2  = in2;

                    omask |= EXTENSION;
                    break;

                } else {
                    fprintf(stderr, "Block::compose: Err=[Unsupported instruction] {\n");
                    bh_pprint_instr(&instr);
                    fprintf(stderr, "}\n");
                }
        }

        //
        // Update the ref count
        symbol_table.count_rw(tacs[idx]);
    }
}

}}
