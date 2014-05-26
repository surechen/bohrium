#compiler-settings
directiveStartToken= %
#end compiler-settings
%slurp
#include "specializer.hpp"
//
//  NOTE: This file is autogenerated based on the tac-definition.
//        You should therefore not edit it manually.
//
using namespace std;
namespace bohrium {
namespace engine {
namespace cpu {

//
// NOTE: This function relies on the posix entension for positional arguments
// to print format string.
//
string Specializer::cexpression(SymbolTable& symbol_table, const Block& block, size_t tac_idx)
{
    tac_t& tac  = block.tac(tac_idx);
    ETYPE etype = symbol_table[tac.out].etype;

    string expr_text;

    char out_c = ' ';
    char in1_c = ' ';
    char in2_c = ' ';

    switch(core::tac_noperands(tac)) {
        case 3:
            if ((symbol_table[tac.in2].layout & ARRAY_LAYOUT) >0) {
                in2_c = '*';
            }
        case 2:
            if ((symbol_table[tac.in1].layout & ARRAY_LAYOUT) >0) {
                in1_c = '*';
            }
        case 1:
            if ((symbol_table[tac.out].layout & ARRAY_LAYOUT) >0) {
                out_c = '*';
            }
            break;
    }

    switch(tac.oper) {
    %for $oper, $op_and_etype_expr in $expressions
        case $oper:            
            %if len($op_and_etype_expr) > 1
            switch (tac.op) {
                %for $op, $etype_expr in $op_and_etype_expr
                case $op:
                    %if len($etype_expr) > 1
                    switch(etype) {
                        %for $etype, $expr in $etype_expr
                        %if $etype == "default"
                        %set $case = ""
                        %else
                        %set $case = "case "
                        %end if
                        $case$etype: expr_text = "$expr"; break;
                        %end for
                    }
                    %else 
                    expr_text = "$etype_expr[0][1]"; break;
                    %end if             
                %end for
                default:
                    expr_text = "__ERR_UNS_OPER__"; break;
            }            
            %else            
            %set $op, $etype_expr = $op_and_etype_expr[0]
            
            %if len($etype_expr) > 1
            switch(etype) {
                %for $etype, $expr in $etype_expr
                %if $etype == "default"
                %set $case = ""
                %else
                %set $case = "case "
                %end if
                $case$etype: expr_text = "$expr"; break;
                %end for
            }
            %else 
            expr_text = "$etype_expr[0][1]"; break;
            %end if
            %end if
            break;
    %end for
    }


    switch(core::tac_noperands(tac)) {
        case 3:
            return core::string_format(
                expr_text,
                out_c, block.global_to_local(tac.out), 
                in1_c, block.global_to_local(tac.in1),
                in2_c, block.global_to_local(tac.in2)
            );
        case 2:
            return core::string_format(
                expr_text, 
                out_c, block.global_to_local(tac.out),
                in1_c, block.global_to_local(tac.in1)
            );
        case 1:
            return core::string_format(
                expr_text,
                out_c, block.global_to_local(tac.out)
            );
        default:
            return expr_text;
    }
}    

}}}