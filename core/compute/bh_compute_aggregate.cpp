/*
This file is part of Bohrium and copyright (c) 2012 the Bohrium
team <http://www.bh107.org>.

Bohrium is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as 
published by the Free Software Foundation, either version 3 
of the License, or (at your option) any later version.

Bohrium is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the 
GNU Lesser General Public License along with Bohrium. 

If not, see <http://www.gnu.org/licenses/>.
*/
#include <bh.h>
#include <bh_compute.h>
#include <bh_compute.h>
#include "functors.hpp"
#include <complex>
#include "traverser.hpp"

typedef char BYTE;


/**
 *  A optimized implementation of executing an instruction.
 *
 *  @param opt_out The output scalar storage array
 *  @param opt_in The array to aggregate
 *  @return This function always returns BH_SUCCESS unless it raises an exception with assert.
 */
template <typename T0, typename T1, typename Instr>
bh_error traverse_sa( bh_array* op_out, bh_array* op_in ) {

    Instr opcode_func;                        	// Element-wise functor-pointer

    bh_index i, j;                        	// Traversal variables

    BYTE* d0;									// Pointer to start of data elements
	BYTE* d1;

    size_t elsize1 = sizeof(T1);				// We use the size for explicit multiplication

	T1 value;									// Local store with data
	
    d0 = (BYTE*) bh_base_array(op_out)->data;
    d1 = (BYTE*) bh_base_array(op_in)->data;

    assert(d0 != NULL);                         // Ensure that data is allocated
    assert(d1 != NULL);

	d1 += op_in->start * elsize1;				// Compute offsets
	value = *((T1*)d1);
		
	if (op_in->ndim == 1)
	{
		// Simple 1D loop
		bh_index stride1 = op_in->stride[0] * elsize1;
		
		bh_index total_ops = op_in->shape[0];
		
		d1 += stride1;
		total_ops--;

		bh_index remainder = total_ops % 4;
		bh_index fulls = total_ops / 4;

		//Macro magic time!
        INNER_LOOP_SSA(opcode_func, fulls, remainder, d1, stride1, &value);
	}
	else if(op_in->ndim == 2)
	{
		bh_index ops_outer = op_in->shape[0];
		bh_index ops_inner = op_in->shape[1];
		
		// Basic 2D loop with unrolling
		bh_index outer_stride1 = op_in->stride[0] * elsize1;
		bh_index inner_stride1 = op_in->stride[1] * elsize1;
		outer_stride1 -= inner_stride1 * op_in->shape[1];

		bh_index remainder = (ops_inner - 1) % 4;
		bh_index fulls = (ops_inner - 1) / 4;

		// Skip the first element, as that is copied
		d1 += inner_stride1;
		INNER_LOOP_SSA(opcode_func, fulls, remainder, d1, inner_stride1, &value);

		remainder = ops_inner % 4;
		fulls = ops_inner / 4;

		for (i = 1; i < ops_outer; i++)
		{
			//Macro magic time!
            INNER_LOOP_SSA(opcode_func, fulls, remainder, d1, inner_stride1, &value);
			d1 += outer_stride1;
		}
	}
	else
	{
		//General case, optimal up to 3D, and almost optimal for 4D
		bh_index n = op_in->ndim - 3;
		bh_index counters[BH_MAXDIM - 3];
		memset(&counters, 0, sizeof(bh_index) * n);		

		bh_index total_ops = 1;
		for(i = 0; i < n; i++)
			total_ops *= op_in->shape[i];
			
		//This chunk of variables prevents repeated calculations of offsets
		bh_index dim_index0 = n + 0;
		bh_index dim_index1 = n + 1;
		bh_index dim_index2 = n + 2;

		bh_index ops_outer = op_in->shape[dim_index0];
		bh_index ops_inner = op_in->shape[dim_index1];
		bh_index ops_inner_inner = op_in->shape[dim_index2];

		bh_index outer_stride1 = op_in->stride[dim_index0] * elsize1;
		bh_index inner_stride1 = op_in->stride[dim_index1] * elsize1;
		bh_index inner_inner_stride1 = op_in->stride[dim_index2] * elsize1;

		outer_stride1 -= inner_stride1 * op_in->shape[dim_index1];
		inner_stride1 -= inner_inner_stride1 * op_in->shape[dim_index2];

		bh_index remainder = (ops_inner_inner - 1) % 4;
		bh_index fulls = (ops_inner_inner - 1) / 4;

		BYTE* d1_orig = d1;

		// Run the first inner loop without the first element
		d1 += inner_inner_stride1;
		INNER_LOOP_SSA(opcode_func, fulls, remainder, d1, inner_inner_stride1, &value);
		d1 += inner_stride1; //Patch to start at next entry

		// Prepare for the following loops
		remainder = ops_inner_inner % 4;
		fulls = ops_inner_inner / 4;
		bh_index j_start = 1;

		while (total_ops-- > 0)
		{
			for (i = 0; i < ops_outer; i++)
			{
				for (j = j_start; j < ops_inner; j++)
				{
					//Macro magic time!
                    INNER_LOOP_SSA(opcode_func, fulls, remainder, d1, inner_inner_stride1, &value);
					d1 += inner_stride1;
				}

				d1 += outer_stride1;
				j_start = 0;
			}

			if (n > 0)
			{
				//Basically a ripple carry adder
				long p = n - 1;

				// Move one in current dimension
				d1_orig += (op_in->stride[p] * elsize1);

				while (++counters[p] == op_in->shape[p] && p > 0)
				{
					//Update to move in the outer dimension, on carry
					d1_orig += ((op_in->stride[p-1]) - (op_in->shape[p] * op_in->stride[p])) * elsize1;

					counters[p] = 0;
					p--;
				}
				
				d1 = d1_orig;
			}
		}		
	}

	*(((T0*)d0) + op_out->start) = (T1)value;

    return BH_SUCCESS;

}

bh_error bh_compute_aggregate(bh_userfunc *arg, void* ve_arg)
{
    bh_aggregate_type *a = (bh_aggregate_type *) arg;   // Grab function arguments

    bh_opcode opcode = a->opcode;                    // Opcode

    bh_array *op_out = a->operand[0];                // Output operand
    bh_array *op_in  = a->operand[1];                // Input operand

                                                        //  Sanity checks.
    if (bh_operands(opcode) != 3) {
        fprintf(stderr, "ERR: opcode: %lld is not a binary ufunc, hence it is not suitable for reduction.\n", (long long int)opcode);
        return BH_ERROR;
    }

	if (bh_base_array(a->operand[1])->data == NULL) {
        fprintf(stderr, "ERR: bh_compute_aggregate; input-operand ( op[1] ) is null.\n");
        return BH_ERROR;
	}

    if (op_in->type != op_out->type) {
        fprintf(stderr, "ERR: bh_compute_aggregate; input and output types are mixed."
                        "Probable causes include reducing over 'LESS', just dont...\n");
        return BH_ERROR;
    }
    
    if (bh_data_malloc(op_out) != BH_SUCCESS) {
        fprintf(stderr, "ERR: bh_compute_aggregate; No memory for reduction-result.\n");
        return BH_OUT_OF_MEMORY;
    }
    
    if (op_out->ndim != 1 || op_out->shape[0] != 1) {
        fprintf(stderr, "ERR: bh_compute_aggregate; output-operand ( op[0] ) is not a scalar.\n");
        return BH_ERROR;
    }

    long int poly = opcode + (op_in->type << 8);

    switch(poly) {
    
        case BH_ADD + (BH_BOOL << 8):
            return traverse_sa<bh_bool, bh_bool, add_functor<bh_bool, bh_bool, bh_bool > >( op_out, op_in );
        case BH_ADD + (BH_COMPLEX128 << 8):
            return traverse_sa<std::complex<double>, std::complex<double>, add_functor<std::complex<double>, std::complex<double>, std::complex<double> > >( op_out, op_in );
        case BH_ADD + (BH_COMPLEX64 << 8):
            return traverse_sa<std::complex<float>, std::complex<float>, add_functor<std::complex<float>, std::complex<float>, std::complex<float> > >( op_out, op_in );
        case BH_ADD + (BH_FLOAT32 << 8):
            return traverse_sa<bh_float32, bh_float32, add_functor<bh_float32, bh_float32, bh_float32 > >( op_out, op_in );
        case BH_ADD + (BH_FLOAT64 << 8):
            return traverse_sa<bh_float64, bh_float64, add_functor<bh_float64, bh_float64, bh_float64 > >( op_out, op_in );
        case BH_ADD + (BH_INT16 << 8):
            return traverse_sa<bh_int16, bh_int16, add_functor<bh_int16, bh_int16, bh_int16 > >( op_out, op_in );
        case BH_ADD + (BH_INT32 << 8):
            return traverse_sa<bh_int32, bh_int32, add_functor<bh_int32, bh_int32, bh_int32 > >( op_out, op_in );
        case BH_ADD + (BH_INT64 << 8):
            return traverse_sa<bh_int64, bh_int64, add_functor<bh_int64, bh_int64, bh_int64 > >( op_out, op_in );
        case BH_ADD + (BH_INT8 << 8):
            return traverse_sa<bh_int8, bh_int8, add_functor<bh_int8, bh_int8, bh_int8 > >( op_out, op_in );
        case BH_ADD + (BH_UINT16 << 8):
            return traverse_sa<bh_uint16, bh_uint16, add_functor<bh_uint16, bh_uint16, bh_uint16 > >( op_out, op_in );
        case BH_ADD + (BH_UINT32 << 8):
            return traverse_sa<bh_uint32, bh_uint32, add_functor<bh_uint32, bh_uint32, bh_uint32 > >( op_out, op_in );
        case BH_ADD + (BH_UINT64 << 8):
            return traverse_sa<bh_uint64, bh_uint64, add_functor<bh_uint64, bh_uint64, bh_uint64 > >( op_out, op_in );
        case BH_ADD + (BH_UINT8 << 8):
            return traverse_sa<bh_uint8, bh_uint8, add_functor<bh_uint8, bh_uint8, bh_uint8 > >( op_out, op_in );
        case BH_SUBTRACT + (BH_BOOL << 8):
            return traverse_sa<bh_bool, bh_bool, subtract_functor<bh_bool, bh_bool, bh_bool > >( op_out, op_in );
        case BH_SUBTRACT + (BH_COMPLEX128 << 8):
            return traverse_sa<std::complex<double>, std::complex<double>, subtract_functor<std::complex<double>, std::complex<double>, std::complex<double> > >( op_out, op_in );
        case BH_SUBTRACT + (BH_COMPLEX64 << 8):
            return traverse_sa<std::complex<float>, std::complex<float>, subtract_functor<std::complex<float>, std::complex<float>, std::complex<float> > >( op_out, op_in );
        case BH_SUBTRACT + (BH_FLOAT32 << 8):
            return traverse_sa<bh_float32, bh_float32, subtract_functor<bh_float32, bh_float32, bh_float32 > >( op_out, op_in );
        case BH_SUBTRACT + (BH_FLOAT64 << 8):
            return traverse_sa<bh_float64, bh_float64, subtract_functor<bh_float64, bh_float64, bh_float64 > >( op_out, op_in );
        case BH_SUBTRACT + (BH_INT16 << 8):
            return traverse_sa<bh_int16, bh_int16, subtract_functor<bh_int16, bh_int16, bh_int16 > >( op_out, op_in );
        case BH_SUBTRACT + (BH_INT32 << 8):
            return traverse_sa<bh_int32, bh_int32, subtract_functor<bh_int32, bh_int32, bh_int32 > >( op_out, op_in );
        case BH_SUBTRACT + (BH_INT64 << 8):
            return traverse_sa<bh_int64, bh_int64, subtract_functor<bh_int64, bh_int64, bh_int64 > >( op_out, op_in );
        case BH_SUBTRACT + (BH_INT8 << 8):
            return traverse_sa<bh_int8, bh_int8, subtract_functor<bh_int8, bh_int8, bh_int8 > >( op_out, op_in );
        case BH_SUBTRACT + (BH_UINT16 << 8):
            return traverse_sa<bh_uint16, bh_uint16, subtract_functor<bh_uint16, bh_uint16, bh_uint16 > >( op_out, op_in );
        case BH_SUBTRACT + (BH_UINT32 << 8):
            return traverse_sa<bh_uint32, bh_uint32, subtract_functor<bh_uint32, bh_uint32, bh_uint32 > >( op_out, op_in );
        case BH_SUBTRACT + (BH_UINT64 << 8):
            return traverse_sa<bh_uint64, bh_uint64, subtract_functor<bh_uint64, bh_uint64, bh_uint64 > >( op_out, op_in );
        case BH_SUBTRACT + (BH_UINT8 << 8):
            return traverse_sa<bh_uint8, bh_uint8, subtract_functor<bh_uint8, bh_uint8, bh_uint8 > >( op_out, op_in );
        case BH_MULTIPLY + (BH_BOOL << 8):
            return traverse_sa<bh_bool, bh_bool, multiply_functor<bh_bool, bh_bool, bh_bool > >( op_out, op_in );
        case BH_MULTIPLY + (BH_COMPLEX128 << 8):
            return traverse_sa<std::complex<double>, std::complex<double>, multiply_functor<std::complex<double>, std::complex<double>, std::complex<double> > >( op_out, op_in );
        case BH_MULTIPLY + (BH_COMPLEX64 << 8):
            return traverse_sa<std::complex<float>, std::complex<float>, multiply_functor<std::complex<float>, std::complex<float>, std::complex<float> > >( op_out, op_in );
        case BH_MULTIPLY + (BH_FLOAT32 << 8):
            return traverse_sa<bh_float32, bh_float32, multiply_functor<bh_float32, bh_float32, bh_float32 > >( op_out, op_in );
        case BH_MULTIPLY + (BH_FLOAT64 << 8):
            return traverse_sa<bh_float64, bh_float64, multiply_functor<bh_float64, bh_float64, bh_float64 > >( op_out, op_in );
        case BH_MULTIPLY + (BH_INT16 << 8):
            return traverse_sa<bh_int16, bh_int16, multiply_functor<bh_int16, bh_int16, bh_int16 > >( op_out, op_in );
        case BH_MULTIPLY + (BH_INT32 << 8):
            return traverse_sa<bh_int32, bh_int32, multiply_functor<bh_int32, bh_int32, bh_int32 > >( op_out, op_in );
        case BH_MULTIPLY + (BH_INT64 << 8):
            return traverse_sa<bh_int64, bh_int64, multiply_functor<bh_int64, bh_int64, bh_int64 > >( op_out, op_in );
        case BH_MULTIPLY + (BH_INT8 << 8):
            return traverse_sa<bh_int8, bh_int8, multiply_functor<bh_int8, bh_int8, bh_int8 > >( op_out, op_in );
        case BH_MULTIPLY + (BH_UINT16 << 8):
            return traverse_sa<bh_uint16, bh_uint16, multiply_functor<bh_uint16, bh_uint16, bh_uint16 > >( op_out, op_in );
        case BH_MULTIPLY + (BH_UINT32 << 8):
            return traverse_sa<bh_uint32, bh_uint32, multiply_functor<bh_uint32, bh_uint32, bh_uint32 > >( op_out, op_in );
        case BH_MULTIPLY + (BH_UINT64 << 8):
            return traverse_sa<bh_uint64, bh_uint64, multiply_functor<bh_uint64, bh_uint64, bh_uint64 > >( op_out, op_in );
        case BH_MULTIPLY + (BH_UINT8 << 8):
            return traverse_sa<bh_uint8, bh_uint8, multiply_functor<bh_uint8, bh_uint8, bh_uint8 > >( op_out, op_in );
        case BH_DIVIDE + (BH_COMPLEX128 << 8):
            return traverse_sa<std::complex<double>, std::complex<double>, divide_functor<std::complex<double>, std::complex<double>, std::complex<double> > >( op_out, op_in );
        case BH_DIVIDE + (BH_COMPLEX64 << 8):
            return traverse_sa<std::complex<float>, std::complex<float>, divide_functor<std::complex<float>, std::complex<float>, std::complex<float> > >( op_out, op_in );
        case BH_DIVIDE + (BH_FLOAT32 << 8):
            return traverse_sa<bh_float32, bh_float32, divide_functor<bh_float32, bh_float32, bh_float32 > >( op_out, op_in );
        case BH_DIVIDE + (BH_FLOAT64 << 8):
            return traverse_sa<bh_float64, bh_float64, divide_functor<bh_float64, bh_float64, bh_float64 > >( op_out, op_in );
        case BH_DIVIDE + (BH_INT16 << 8):
            return traverse_sa<bh_int16, bh_int16, divide_functor<bh_int16, bh_int16, bh_int16 > >( op_out, op_in );
        case BH_DIVIDE + (BH_INT32 << 8):
            return traverse_sa<bh_int32, bh_int32, divide_functor<bh_int32, bh_int32, bh_int32 > >( op_out, op_in );
        case BH_DIVIDE + (BH_INT64 << 8):
            return traverse_sa<bh_int64, bh_int64, divide_functor<bh_int64, bh_int64, bh_int64 > >( op_out, op_in );
        case BH_DIVIDE + (BH_INT8 << 8):
            return traverse_sa<bh_int8, bh_int8, divide_functor<bh_int8, bh_int8, bh_int8 > >( op_out, op_in );
        case BH_DIVIDE + (BH_UINT16 << 8):
            return traverse_sa<bh_uint16, bh_uint16, divide_functor<bh_uint16, bh_uint16, bh_uint16 > >( op_out, op_in );
        case BH_DIVIDE + (BH_UINT32 << 8):
            return traverse_sa<bh_uint32, bh_uint32, divide_functor<bh_uint32, bh_uint32, bh_uint32 > >( op_out, op_in );
        case BH_DIVIDE + (BH_UINT64 << 8):
            return traverse_sa<bh_uint64, bh_uint64, divide_functor<bh_uint64, bh_uint64, bh_uint64 > >( op_out, op_in );
        case BH_DIVIDE + (BH_UINT8 << 8):
            return traverse_sa<bh_uint8, bh_uint8, divide_functor<bh_uint8, bh_uint8, bh_uint8 > >( op_out, op_in );
        case BH_POWER + (BH_FLOAT32 << 8):
            return traverse_sa<bh_float32, bh_float32, power_functor<bh_float32, bh_float32, bh_float32 > >( op_out, op_in );
        case BH_POWER + (BH_FLOAT64 << 8):
            return traverse_sa<bh_float64, bh_float64, power_functor<bh_float64, bh_float64, bh_float64 > >( op_out, op_in );
        case BH_POWER + (BH_INT16 << 8):
            return traverse_sa<bh_int16, bh_int16, power_functor<bh_int16, bh_int16, bh_int16 > >( op_out, op_in );
        case BH_POWER + (BH_INT32 << 8):
            return traverse_sa<bh_int32, bh_int32, power_functor<bh_int32, bh_int32, bh_int32 > >( op_out, op_in );
        case BH_POWER + (BH_INT64 << 8):
            return traverse_sa<bh_int64, bh_int64, power_functor<bh_int64, bh_int64, bh_int64 > >( op_out, op_in );
        case BH_POWER + (BH_INT8 << 8):
            return traverse_sa<bh_int8, bh_int8, power_functor<bh_int8, bh_int8, bh_int8 > >( op_out, op_in );
        case BH_POWER + (BH_UINT16 << 8):
            return traverse_sa<bh_uint16, bh_uint16, power_functor<bh_uint16, bh_uint16, bh_uint16 > >( op_out, op_in );
        case BH_POWER + (BH_UINT32 << 8):
            return traverse_sa<bh_uint32, bh_uint32, power_functor<bh_uint32, bh_uint32, bh_uint32 > >( op_out, op_in );
        case BH_POWER + (BH_UINT64 << 8):
            return traverse_sa<bh_uint64, bh_uint64, power_functor<bh_uint64, bh_uint64, bh_uint64 > >( op_out, op_in );
        case BH_POWER + (BH_UINT8 << 8):
            return traverse_sa<bh_uint8, bh_uint8, power_functor<bh_uint8, bh_uint8, bh_uint8 > >( op_out, op_in );
        case BH_GREATER + (BH_BOOL << 8):
            return traverse_sa<bh_bool, bh_bool, greater_functor<bh_bool, bh_bool, bh_bool > >( op_out, op_in );
        case BH_GREATER + (BH_FLOAT32 << 8):
            return traverse_sa<bh_float32, bh_float32, greater_functor<bh_float32, bh_float32, bh_float32 > >( op_out, op_in );
        case BH_GREATER + (BH_FLOAT64 << 8):
            return traverse_sa<bh_float64, bh_float64, greater_functor<bh_float64, bh_float64, bh_float64 > >( op_out, op_in );
        case BH_GREATER + (BH_INT16 << 8):
            return traverse_sa<bh_int16, bh_int16, greater_functor<bh_int16, bh_int16, bh_int16 > >( op_out, op_in );
        case BH_GREATER + (BH_INT32 << 8):
            return traverse_sa<bh_int32, bh_int32, greater_functor<bh_int32, bh_int32, bh_int32 > >( op_out, op_in );
        case BH_GREATER + (BH_INT64 << 8):
            return traverse_sa<bh_int64, bh_int64, greater_functor<bh_int64, bh_int64, bh_int64 > >( op_out, op_in );
        case BH_GREATER + (BH_INT8 << 8):
            return traverse_sa<bh_int8, bh_int8, greater_functor<bh_int8, bh_int8, bh_int8 > >( op_out, op_in );
        case BH_GREATER + (BH_UINT16 << 8):
            return traverse_sa<bh_uint16, bh_uint16, greater_functor<bh_uint16, bh_uint16, bh_uint16 > >( op_out, op_in );
        case BH_GREATER + (BH_UINT32 << 8):
            return traverse_sa<bh_uint32, bh_uint32, greater_functor<bh_uint32, bh_uint32, bh_uint32 > >( op_out, op_in );
        case BH_GREATER + (BH_UINT64 << 8):
            return traverse_sa<bh_uint64, bh_uint64, greater_functor<bh_uint64, bh_uint64, bh_uint64 > >( op_out, op_in );
        case BH_GREATER + (BH_UINT8 << 8):
            return traverse_sa<bh_uint8, bh_uint8, greater_functor<bh_uint8, bh_uint8, bh_uint8 > >( op_out, op_in );
        case BH_GREATER_EQUAL + (BH_BOOL << 8):
            return traverse_sa<bh_bool, bh_bool, greater_equal_functor<bh_bool, bh_bool, bh_bool > >( op_out, op_in );
        case BH_GREATER_EQUAL + (BH_FLOAT32 << 8):
            return traverse_sa<bh_float32, bh_float32, greater_equal_functor<bh_float32, bh_float32, bh_float32 > >( op_out, op_in );
        case BH_GREATER_EQUAL + (BH_FLOAT64 << 8):
            return traverse_sa<bh_float64, bh_float64, greater_equal_functor<bh_float64, bh_float64, bh_float64 > >( op_out, op_in );
        case BH_GREATER_EQUAL + (BH_INT16 << 8):
            return traverse_sa<bh_int16, bh_int16, greater_equal_functor<bh_int16, bh_int16, bh_int16 > >( op_out, op_in );
        case BH_GREATER_EQUAL + (BH_INT32 << 8):
            return traverse_sa<bh_int32, bh_int32, greater_equal_functor<bh_int32, bh_int32, bh_int32 > >( op_out, op_in );
        case BH_GREATER_EQUAL + (BH_INT64 << 8):
            return traverse_sa<bh_int64, bh_int64, greater_equal_functor<bh_int64, bh_int64, bh_int64 > >( op_out, op_in );
        case BH_GREATER_EQUAL + (BH_INT8 << 8):
            return traverse_sa<bh_int8, bh_int8, greater_equal_functor<bh_int8, bh_int8, bh_int8 > >( op_out, op_in );
        case BH_GREATER_EQUAL + (BH_UINT16 << 8):
            return traverse_sa<bh_uint16, bh_uint16, greater_equal_functor<bh_uint16, bh_uint16, bh_uint16 > >( op_out, op_in );
        case BH_GREATER_EQUAL + (BH_UINT32 << 8):
            return traverse_sa<bh_uint32, bh_uint32, greater_equal_functor<bh_uint32, bh_uint32, bh_uint32 > >( op_out, op_in );
        case BH_GREATER_EQUAL + (BH_UINT64 << 8):
            return traverse_sa<bh_uint64, bh_uint64, greater_equal_functor<bh_uint64, bh_uint64, bh_uint64 > >( op_out, op_in );
        case BH_GREATER_EQUAL + (BH_UINT8 << 8):
            return traverse_sa<bh_uint8, bh_uint8, greater_equal_functor<bh_uint8, bh_uint8, bh_uint8 > >( op_out, op_in );
        case BH_LESS + (BH_BOOL << 8):
            return traverse_sa<bh_bool, bh_bool, less_functor<bh_bool, bh_bool, bh_bool > >( op_out, op_in );
        case BH_LESS + (BH_FLOAT32 << 8):
            return traverse_sa<bh_float32, bh_float32, less_functor<bh_float32, bh_float32, bh_float32 > >( op_out, op_in );
        case BH_LESS + (BH_FLOAT64 << 8):
            return traverse_sa<bh_float64, bh_float64, less_functor<bh_float64, bh_float64, bh_float64 > >( op_out, op_in );
        case BH_LESS + (BH_INT16 << 8):
            return traverse_sa<bh_int16, bh_int16, less_functor<bh_int16, bh_int16, bh_int16 > >( op_out, op_in );
        case BH_LESS + (BH_INT32 << 8):
            return traverse_sa<bh_int32, bh_int32, less_functor<bh_int32, bh_int32, bh_int32 > >( op_out, op_in );
        case BH_LESS + (BH_INT64 << 8):
            return traverse_sa<bh_int64, bh_int64, less_functor<bh_int64, bh_int64, bh_int64 > >( op_out, op_in );
        case BH_LESS + (BH_INT8 << 8):
            return traverse_sa<bh_int8, bh_int8, less_functor<bh_int8, bh_int8, bh_int8 > >( op_out, op_in );
        case BH_LESS + (BH_UINT16 << 8):
            return traverse_sa<bh_uint16, bh_uint16, less_functor<bh_uint16, bh_uint16, bh_uint16 > >( op_out, op_in );
        case BH_LESS + (BH_UINT32 << 8):
            return traverse_sa<bh_uint32, bh_uint32, less_functor<bh_uint32, bh_uint32, bh_uint32 > >( op_out, op_in );
        case BH_LESS + (BH_UINT64 << 8):
            return traverse_sa<bh_uint64, bh_uint64, less_functor<bh_uint64, bh_uint64, bh_uint64 > >( op_out, op_in );
        case BH_LESS + (BH_UINT8 << 8):
            return traverse_sa<bh_uint8, bh_uint8, less_functor<bh_uint8, bh_uint8, bh_uint8 > >( op_out, op_in );
        case BH_LESS_EQUAL + (BH_BOOL << 8):
            return traverse_sa<bh_bool, bh_bool, less_equal_functor<bh_bool, bh_bool, bh_bool > >( op_out, op_in );
        case BH_LESS_EQUAL + (BH_FLOAT32 << 8):
            return traverse_sa<bh_float32, bh_float32, less_equal_functor<bh_float32, bh_float32, bh_float32 > >( op_out, op_in );
        case BH_LESS_EQUAL + (BH_FLOAT64 << 8):
            return traverse_sa<bh_float64, bh_float64, less_equal_functor<bh_float64, bh_float64, bh_float64 > >( op_out, op_in );
        case BH_LESS_EQUAL + (BH_INT16 << 8):
            return traverse_sa<bh_int16, bh_int16, less_equal_functor<bh_int16, bh_int16, bh_int16 > >( op_out, op_in );
        case BH_LESS_EQUAL + (BH_INT32 << 8):
            return traverse_sa<bh_int32, bh_int32, less_equal_functor<bh_int32, bh_int32, bh_int32 > >( op_out, op_in );
        case BH_LESS_EQUAL + (BH_INT64 << 8):
            return traverse_sa<bh_int64, bh_int64, less_equal_functor<bh_int64, bh_int64, bh_int64 > >( op_out, op_in );
        case BH_LESS_EQUAL + (BH_INT8 << 8):
            return traverse_sa<bh_int8, bh_int8, less_equal_functor<bh_int8, bh_int8, bh_int8 > >( op_out, op_in );
        case BH_LESS_EQUAL + (BH_UINT16 << 8):
            return traverse_sa<bh_uint16, bh_uint16, less_equal_functor<bh_uint16, bh_uint16, bh_uint16 > >( op_out, op_in );
        case BH_LESS_EQUAL + (BH_UINT32 << 8):
            return traverse_sa<bh_uint32, bh_uint32, less_equal_functor<bh_uint32, bh_uint32, bh_uint32 > >( op_out, op_in );
        case BH_LESS_EQUAL + (BH_UINT64 << 8):
            return traverse_sa<bh_uint64, bh_uint64, less_equal_functor<bh_uint64, bh_uint64, bh_uint64 > >( op_out, op_in );
        case BH_LESS_EQUAL + (BH_UINT8 << 8):
            return traverse_sa<bh_uint8, bh_uint8, less_equal_functor<bh_uint8, bh_uint8, bh_uint8 > >( op_out, op_in );
        case BH_EQUAL + (BH_BOOL << 8):
            return traverse_sa<bh_bool, bh_bool, equal_functor<bh_bool, bh_bool, bh_bool > >( op_out, op_in );
        case BH_EQUAL + (BH_COMPLEX128 << 8):
            return traverse_sa<std::complex<double>, std::complex<double>, equal_functor<std::complex<double>, std::complex<double>, std::complex<double> > >( op_out, op_in );
        case BH_EQUAL + (BH_COMPLEX64 << 8):
            return traverse_sa<std::complex<float>, std::complex<float>, equal_functor<std::complex<float>, std::complex<float>, std::complex<float> > >( op_out, op_in );
        case BH_EQUAL + (BH_FLOAT32 << 8):
            return traverse_sa<bh_float32, bh_float32, equal_functor<bh_float32, bh_float32, bh_float32 > >( op_out, op_in );
        case BH_EQUAL + (BH_FLOAT64 << 8):
            return traverse_sa<bh_float64, bh_float64, equal_functor<bh_float64, bh_float64, bh_float64 > >( op_out, op_in );
        case BH_EQUAL + (BH_INT16 << 8):
            return traverse_sa<bh_int16, bh_int16, equal_functor<bh_int16, bh_int16, bh_int16 > >( op_out, op_in );
        case BH_EQUAL + (BH_INT32 << 8):
            return traverse_sa<bh_int32, bh_int32, equal_functor<bh_int32, bh_int32, bh_int32 > >( op_out, op_in );
        case BH_EQUAL + (BH_INT64 << 8):
            return traverse_sa<bh_int64, bh_int64, equal_functor<bh_int64, bh_int64, bh_int64 > >( op_out, op_in );
        case BH_EQUAL + (BH_INT8 << 8):
            return traverse_sa<bh_int8, bh_int8, equal_functor<bh_int8, bh_int8, bh_int8 > >( op_out, op_in );
        case BH_EQUAL + (BH_UINT16 << 8):
            return traverse_sa<bh_uint16, bh_uint16, equal_functor<bh_uint16, bh_uint16, bh_uint16 > >( op_out, op_in );
        case BH_EQUAL + (BH_UINT32 << 8):
            return traverse_sa<bh_uint32, bh_uint32, equal_functor<bh_uint32, bh_uint32, bh_uint32 > >( op_out, op_in );
        case BH_EQUAL + (BH_UINT64 << 8):
            return traverse_sa<bh_uint64, bh_uint64, equal_functor<bh_uint64, bh_uint64, bh_uint64 > >( op_out, op_in );
        case BH_EQUAL + (BH_UINT8 << 8):
            return traverse_sa<bh_uint8, bh_uint8, equal_functor<bh_uint8, bh_uint8, bh_uint8 > >( op_out, op_in );
        case BH_NOT_EQUAL + (BH_BOOL << 8):
            return traverse_sa<bh_bool, bh_bool, not_equal_functor<bh_bool, bh_bool, bh_bool > >( op_out, op_in );
        case BH_NOT_EQUAL + (BH_COMPLEX128 << 8):
            return traverse_sa<std::complex<double>, std::complex<double>, not_equal_functor<std::complex<double>, std::complex<double>, std::complex<double> > >( op_out, op_in );
        case BH_NOT_EQUAL + (BH_COMPLEX64 << 8):
            return traverse_sa<std::complex<float>, std::complex<float>, not_equal_functor<std::complex<float>, std::complex<float>, std::complex<float> > >( op_out, op_in );
        case BH_NOT_EQUAL + (BH_FLOAT32 << 8):
            return traverse_sa<bh_float32, bh_float32, not_equal_functor<bh_float32, bh_float32, bh_float32 > >( op_out, op_in );
        case BH_NOT_EQUAL + (BH_FLOAT64 << 8):
            return traverse_sa<bh_float64, bh_float64, not_equal_functor<bh_float64, bh_float64, bh_float64 > >( op_out, op_in );
        case BH_NOT_EQUAL + (BH_INT16 << 8):
            return traverse_sa<bh_int16, bh_int16, not_equal_functor<bh_int16, bh_int16, bh_int16 > >( op_out, op_in );
        case BH_NOT_EQUAL + (BH_INT32 << 8):
            return traverse_sa<bh_int32, bh_int32, not_equal_functor<bh_int32, bh_int32, bh_int32 > >( op_out, op_in );
        case BH_NOT_EQUAL + (BH_INT64 << 8):
            return traverse_sa<bh_int64, bh_int64, not_equal_functor<bh_int64, bh_int64, bh_int64 > >( op_out, op_in );
        case BH_NOT_EQUAL + (BH_INT8 << 8):
            return traverse_sa<bh_int8, bh_int8, not_equal_functor<bh_int8, bh_int8, bh_int8 > >( op_out, op_in );
        case BH_NOT_EQUAL + (BH_UINT16 << 8):
            return traverse_sa<bh_uint16, bh_uint16, not_equal_functor<bh_uint16, bh_uint16, bh_uint16 > >( op_out, op_in );
        case BH_NOT_EQUAL + (BH_UINT32 << 8):
            return traverse_sa<bh_uint32, bh_uint32, not_equal_functor<bh_uint32, bh_uint32, bh_uint32 > >( op_out, op_in );
        case BH_NOT_EQUAL + (BH_UINT64 << 8):
            return traverse_sa<bh_uint64, bh_uint64, not_equal_functor<bh_uint64, bh_uint64, bh_uint64 > >( op_out, op_in );
        case BH_NOT_EQUAL + (BH_UINT8 << 8):
            return traverse_sa<bh_uint8, bh_uint8, not_equal_functor<bh_uint8, bh_uint8, bh_uint8 > >( op_out, op_in );
        case BH_LOGICAL_AND + (BH_BOOL << 8):
            return traverse_sa<bh_bool, bh_bool, logical_and_functor<bh_bool, bh_bool, bh_bool > >( op_out, op_in );
        case BH_LOGICAL_AND + (BH_FLOAT32 << 8):
            return traverse_sa<bh_float32, bh_float32, logical_and_functor<bh_float32, bh_float32, bh_float32 > >( op_out, op_in );
        case BH_LOGICAL_AND + (BH_FLOAT64 << 8):
            return traverse_sa<bh_float64, bh_float64, logical_and_functor<bh_float64, bh_float64, bh_float64 > >( op_out, op_in );
        case BH_LOGICAL_AND + (BH_INT16 << 8):
            return traverse_sa<bh_int16, bh_int16, logical_and_functor<bh_int16, bh_int16, bh_int16 > >( op_out, op_in );
        case BH_LOGICAL_AND + (BH_INT32 << 8):
            return traverse_sa<bh_int32, bh_int32, logical_and_functor<bh_int32, bh_int32, bh_int32 > >( op_out, op_in );
        case BH_LOGICAL_AND + (BH_INT64 << 8):
            return traverse_sa<bh_int64, bh_int64, logical_and_functor<bh_int64, bh_int64, bh_int64 > >( op_out, op_in );
        case BH_LOGICAL_AND + (BH_INT8 << 8):
            return traverse_sa<bh_int8, bh_int8, logical_and_functor<bh_int8, bh_int8, bh_int8 > >( op_out, op_in );
        case BH_LOGICAL_AND + (BH_UINT16 << 8):
            return traverse_sa<bh_uint16, bh_uint16, logical_and_functor<bh_uint16, bh_uint16, bh_uint16 > >( op_out, op_in );
        case BH_LOGICAL_AND + (BH_UINT32 << 8):
            return traverse_sa<bh_uint32, bh_uint32, logical_and_functor<bh_uint32, bh_uint32, bh_uint32 > >( op_out, op_in );
        case BH_LOGICAL_AND + (BH_UINT64 << 8):
            return traverse_sa<bh_uint64, bh_uint64, logical_and_functor<bh_uint64, bh_uint64, bh_uint64 > >( op_out, op_in );
        case BH_LOGICAL_AND + (BH_UINT8 << 8):
            return traverse_sa<bh_uint8, bh_uint8, logical_and_functor<bh_uint8, bh_uint8, bh_uint8 > >( op_out, op_in );
        case BH_LOGICAL_OR + (BH_BOOL << 8):
            return traverse_sa<bh_bool, bh_bool, logical_or_functor<bh_bool, bh_bool, bh_bool > >( op_out, op_in );
        case BH_LOGICAL_OR + (BH_FLOAT32 << 8):
            return traverse_sa<bh_float32, bh_float32, logical_or_functor<bh_float32, bh_float32, bh_float32 > >( op_out, op_in );
        case BH_LOGICAL_OR + (BH_FLOAT64 << 8):
            return traverse_sa<bh_float64, bh_float64, logical_or_functor<bh_float64, bh_float64, bh_float64 > >( op_out, op_in );
        case BH_LOGICAL_OR + (BH_INT16 << 8):
            return traverse_sa<bh_int16, bh_int16, logical_or_functor<bh_int16, bh_int16, bh_int16 > >( op_out, op_in );
        case BH_LOGICAL_OR + (BH_INT32 << 8):
            return traverse_sa<bh_int32, bh_int32, logical_or_functor<bh_int32, bh_int32, bh_int32 > >( op_out, op_in );
        case BH_LOGICAL_OR + (BH_INT64 << 8):
            return traverse_sa<bh_int64, bh_int64, logical_or_functor<bh_int64, bh_int64, bh_int64 > >( op_out, op_in );
        case BH_LOGICAL_OR + (BH_INT8 << 8):
            return traverse_sa<bh_int8, bh_int8, logical_or_functor<bh_int8, bh_int8, bh_int8 > >( op_out, op_in );
        case BH_LOGICAL_OR + (BH_UINT16 << 8):
            return traverse_sa<bh_uint16, bh_uint16, logical_or_functor<bh_uint16, bh_uint16, bh_uint16 > >( op_out, op_in );
        case BH_LOGICAL_OR + (BH_UINT32 << 8):
            return traverse_sa<bh_uint32, bh_uint32, logical_or_functor<bh_uint32, bh_uint32, bh_uint32 > >( op_out, op_in );
        case BH_LOGICAL_OR + (BH_UINT64 << 8):
            return traverse_sa<bh_uint64, bh_uint64, logical_or_functor<bh_uint64, bh_uint64, bh_uint64 > >( op_out, op_in );
        case BH_LOGICAL_OR + (BH_UINT8 << 8):
            return traverse_sa<bh_uint8, bh_uint8, logical_or_functor<bh_uint8, bh_uint8, bh_uint8 > >( op_out, op_in );
        case BH_LOGICAL_XOR + (BH_BOOL << 8):
            return traverse_sa<bh_bool, bh_bool, logical_xor_functor<bh_bool, bh_bool, bh_bool > >( op_out, op_in );
        case BH_LOGICAL_XOR + (BH_FLOAT32 << 8):
            return traverse_sa<bh_float32, bh_float32, logical_xor_functor<bh_float32, bh_float32, bh_float32 > >( op_out, op_in );
        case BH_LOGICAL_XOR + (BH_FLOAT64 << 8):
            return traverse_sa<bh_float64, bh_float64, logical_xor_functor<bh_float64, bh_float64, bh_float64 > >( op_out, op_in );
        case BH_LOGICAL_XOR + (BH_INT16 << 8):
            return traverse_sa<bh_int16, bh_int16, logical_xor_functor<bh_int16, bh_int16, bh_int16 > >( op_out, op_in );
        case BH_LOGICAL_XOR + (BH_INT32 << 8):
            return traverse_sa<bh_int32, bh_int32, logical_xor_functor<bh_int32, bh_int32, bh_int32 > >( op_out, op_in );
        case BH_LOGICAL_XOR + (BH_INT64 << 8):
            return traverse_sa<bh_int64, bh_int64, logical_xor_functor<bh_int64, bh_int64, bh_int64 > >( op_out, op_in );
        case BH_LOGICAL_XOR + (BH_INT8 << 8):
            return traverse_sa<bh_int8, bh_int8, logical_xor_functor<bh_int8, bh_int8, bh_int8 > >( op_out, op_in );
        case BH_LOGICAL_XOR + (BH_UINT16 << 8):
            return traverse_sa<bh_uint16, bh_uint16, logical_xor_functor<bh_uint16, bh_uint16, bh_uint16 > >( op_out, op_in );
        case BH_LOGICAL_XOR + (BH_UINT32 << 8):
            return traverse_sa<bh_uint32, bh_uint32, logical_xor_functor<bh_uint32, bh_uint32, bh_uint32 > >( op_out, op_in );
        case BH_LOGICAL_XOR + (BH_UINT64 << 8):
            return traverse_sa<bh_uint64, bh_uint64, logical_xor_functor<bh_uint64, bh_uint64, bh_uint64 > >( op_out, op_in );
        case BH_LOGICAL_XOR + (BH_UINT8 << 8):
            return traverse_sa<bh_uint8, bh_uint8, logical_xor_functor<bh_uint8, bh_uint8, bh_uint8 > >( op_out, op_in );
        case BH_MAXIMUM + (BH_BOOL << 8):
            return traverse_sa<bh_bool, bh_bool, maximum_functor<bh_bool, bh_bool, bh_bool > >( op_out, op_in );
        case BH_MAXIMUM + (BH_FLOAT32 << 8):
            return traverse_sa<bh_float32, bh_float32, maximum_functor<bh_float32, bh_float32, bh_float32 > >( op_out, op_in );
        case BH_MAXIMUM + (BH_FLOAT64 << 8):
            return traverse_sa<bh_float64, bh_float64, maximum_functor<bh_float64, bh_float64, bh_float64 > >( op_out, op_in );
        case BH_MAXIMUM + (BH_INT16 << 8):
            return traverse_sa<bh_int16, bh_int16, maximum_functor<bh_int16, bh_int16, bh_int16 > >( op_out, op_in );
        case BH_MAXIMUM + (BH_INT32 << 8):
            return traverse_sa<bh_int32, bh_int32, maximum_functor<bh_int32, bh_int32, bh_int32 > >( op_out, op_in );
        case BH_MAXIMUM + (BH_INT64 << 8):
            return traverse_sa<bh_int64, bh_int64, maximum_functor<bh_int64, bh_int64, bh_int64 > >( op_out, op_in );
        case BH_MAXIMUM + (BH_INT8 << 8):
            return traverse_sa<bh_int8, bh_int8, maximum_functor<bh_int8, bh_int8, bh_int8 > >( op_out, op_in );
        case BH_MAXIMUM + (BH_UINT16 << 8):
            return traverse_sa<bh_uint16, bh_uint16, maximum_functor<bh_uint16, bh_uint16, bh_uint16 > >( op_out, op_in );
        case BH_MAXIMUM + (BH_UINT32 << 8):
            return traverse_sa<bh_uint32, bh_uint32, maximum_functor<bh_uint32, bh_uint32, bh_uint32 > >( op_out, op_in );
        case BH_MAXIMUM + (BH_UINT64 << 8):
            return traverse_sa<bh_uint64, bh_uint64, maximum_functor<bh_uint64, bh_uint64, bh_uint64 > >( op_out, op_in );
        case BH_MAXIMUM + (BH_UINT8 << 8):
            return traverse_sa<bh_uint8, bh_uint8, maximum_functor<bh_uint8, bh_uint8, bh_uint8 > >( op_out, op_in );
        case BH_MINIMUM + (BH_BOOL << 8):
            return traverse_sa<bh_bool, bh_bool, minimum_functor<bh_bool, bh_bool, bh_bool > >( op_out, op_in );
        case BH_MINIMUM + (BH_FLOAT32 << 8):
            return traverse_sa<bh_float32, bh_float32, minimum_functor<bh_float32, bh_float32, bh_float32 > >( op_out, op_in );
        case BH_MINIMUM + (BH_FLOAT64 << 8):
            return traverse_sa<bh_float64, bh_float64, minimum_functor<bh_float64, bh_float64, bh_float64 > >( op_out, op_in );
        case BH_MINIMUM + (BH_INT16 << 8):
            return traverse_sa<bh_int16, bh_int16, minimum_functor<bh_int16, bh_int16, bh_int16 > >( op_out, op_in );
        case BH_MINIMUM + (BH_INT32 << 8):
            return traverse_sa<bh_int32, bh_int32, minimum_functor<bh_int32, bh_int32, bh_int32 > >( op_out, op_in );
        case BH_MINIMUM + (BH_INT64 << 8):
            return traverse_sa<bh_int64, bh_int64, minimum_functor<bh_int64, bh_int64, bh_int64 > >( op_out, op_in );
        case BH_MINIMUM + (BH_INT8 << 8):
            return traverse_sa<bh_int8, bh_int8, minimum_functor<bh_int8, bh_int8, bh_int8 > >( op_out, op_in );
        case BH_MINIMUM + (BH_UINT16 << 8):
            return traverse_sa<bh_uint16, bh_uint16, minimum_functor<bh_uint16, bh_uint16, bh_uint16 > >( op_out, op_in );
        case BH_MINIMUM + (BH_UINT32 << 8):
            return traverse_sa<bh_uint32, bh_uint32, minimum_functor<bh_uint32, bh_uint32, bh_uint32 > >( op_out, op_in );
        case BH_MINIMUM + (BH_UINT64 << 8):
            return traverse_sa<bh_uint64, bh_uint64, minimum_functor<bh_uint64, bh_uint64, bh_uint64 > >( op_out, op_in );
        case BH_MINIMUM + (BH_UINT8 << 8):
            return traverse_sa<bh_uint8, bh_uint8, minimum_functor<bh_uint8, bh_uint8, bh_uint8 > >( op_out, op_in );
        case BH_BITWISE_AND + (BH_BOOL << 8):
            return traverse_sa<bh_bool, bh_bool, bitwise_and_functor<bh_bool, bh_bool, bh_bool > >( op_out, op_in );
        case BH_BITWISE_AND + (BH_INT16 << 8):
            return traverse_sa<bh_int16, bh_int16, bitwise_and_functor<bh_int16, bh_int16, bh_int16 > >( op_out, op_in );
        case BH_BITWISE_AND + (BH_INT32 << 8):
            return traverse_sa<bh_int32, bh_int32, bitwise_and_functor<bh_int32, bh_int32, bh_int32 > >( op_out, op_in );
        case BH_BITWISE_AND + (BH_INT64 << 8):
            return traverse_sa<bh_int64, bh_int64, bitwise_and_functor<bh_int64, bh_int64, bh_int64 > >( op_out, op_in );
        case BH_BITWISE_AND + (BH_INT8 << 8):
            return traverse_sa<bh_int8, bh_int8, bitwise_and_functor<bh_int8, bh_int8, bh_int8 > >( op_out, op_in );
        case BH_BITWISE_AND + (BH_UINT16 << 8):
            return traverse_sa<bh_uint16, bh_uint16, bitwise_and_functor<bh_uint16, bh_uint16, bh_uint16 > >( op_out, op_in );
        case BH_BITWISE_AND + (BH_UINT32 << 8):
            return traverse_sa<bh_uint32, bh_uint32, bitwise_and_functor<bh_uint32, bh_uint32, bh_uint32 > >( op_out, op_in );
        case BH_BITWISE_AND + (BH_UINT64 << 8):
            return traverse_sa<bh_uint64, bh_uint64, bitwise_and_functor<bh_uint64, bh_uint64, bh_uint64 > >( op_out, op_in );
        case BH_BITWISE_AND + (BH_UINT8 << 8):
            return traverse_sa<bh_uint8, bh_uint8, bitwise_and_functor<bh_uint8, bh_uint8, bh_uint8 > >( op_out, op_in );
        case BH_BITWISE_OR + (BH_BOOL << 8):
            return traverse_sa<bh_bool, bh_bool, bitwise_or_functor<bh_bool, bh_bool, bh_bool > >( op_out, op_in );
        case BH_BITWISE_OR + (BH_INT16 << 8):
            return traverse_sa<bh_int16, bh_int16, bitwise_or_functor<bh_int16, bh_int16, bh_int16 > >( op_out, op_in );
        case BH_BITWISE_OR + (BH_INT32 << 8):
            return traverse_sa<bh_int32, bh_int32, bitwise_or_functor<bh_int32, bh_int32, bh_int32 > >( op_out, op_in );
        case BH_BITWISE_OR + (BH_INT64 << 8):
            return traverse_sa<bh_int64, bh_int64, bitwise_or_functor<bh_int64, bh_int64, bh_int64 > >( op_out, op_in );
        case BH_BITWISE_OR + (BH_INT8 << 8):
            return traverse_sa<bh_int8, bh_int8, bitwise_or_functor<bh_int8, bh_int8, bh_int8 > >( op_out, op_in );
        case BH_BITWISE_OR + (BH_UINT16 << 8):
            return traverse_sa<bh_uint16, bh_uint16, bitwise_or_functor<bh_uint16, bh_uint16, bh_uint16 > >( op_out, op_in );
        case BH_BITWISE_OR + (BH_UINT32 << 8):
            return traverse_sa<bh_uint32, bh_uint32, bitwise_or_functor<bh_uint32, bh_uint32, bh_uint32 > >( op_out, op_in );
        case BH_BITWISE_OR + (BH_UINT64 << 8):
            return traverse_sa<bh_uint64, bh_uint64, bitwise_or_functor<bh_uint64, bh_uint64, bh_uint64 > >( op_out, op_in );
        case BH_BITWISE_OR + (BH_UINT8 << 8):
            return traverse_sa<bh_uint8, bh_uint8, bitwise_or_functor<bh_uint8, bh_uint8, bh_uint8 > >( op_out, op_in );
        case BH_BITWISE_XOR + (BH_BOOL << 8):
            return traverse_sa<bh_bool, bh_bool, bitwise_xor_functor<bh_bool, bh_bool, bh_bool > >( op_out, op_in );
        case BH_BITWISE_XOR + (BH_INT16 << 8):
            return traverse_sa<bh_int16, bh_int16, bitwise_xor_functor<bh_int16, bh_int16, bh_int16 > >( op_out, op_in );
        case BH_BITWISE_XOR + (BH_INT32 << 8):
            return traverse_sa<bh_int32, bh_int32, bitwise_xor_functor<bh_int32, bh_int32, bh_int32 > >( op_out, op_in );
        case BH_BITWISE_XOR + (BH_INT64 << 8):
            return traverse_sa<bh_int64, bh_int64, bitwise_xor_functor<bh_int64, bh_int64, bh_int64 > >( op_out, op_in );
        case BH_BITWISE_XOR + (BH_INT8 << 8):
            return traverse_sa<bh_int8, bh_int8, bitwise_xor_functor<bh_int8, bh_int8, bh_int8 > >( op_out, op_in );
        case BH_BITWISE_XOR + (BH_UINT16 << 8):
            return traverse_sa<bh_uint16, bh_uint16, bitwise_xor_functor<bh_uint16, bh_uint16, bh_uint16 > >( op_out, op_in );
        case BH_BITWISE_XOR + (BH_UINT32 << 8):
            return traverse_sa<bh_uint32, bh_uint32, bitwise_xor_functor<bh_uint32, bh_uint32, bh_uint32 > >( op_out, op_in );
        case BH_BITWISE_XOR + (BH_UINT64 << 8):
            return traverse_sa<bh_uint64, bh_uint64, bitwise_xor_functor<bh_uint64, bh_uint64, bh_uint64 > >( op_out, op_in );
        case BH_BITWISE_XOR + (BH_UINT8 << 8):
            return traverse_sa<bh_uint8, bh_uint8, bitwise_xor_functor<bh_uint8, bh_uint8, bh_uint8 > >( op_out, op_in );
        case BH_LEFT_SHIFT + (BH_INT16 << 8):
            return traverse_sa<bh_int16, bh_int16, left_shift_functor<bh_int16, bh_int16, bh_int16 > >( op_out, op_in );
        case BH_LEFT_SHIFT + (BH_INT32 << 8):
            return traverse_sa<bh_int32, bh_int32, left_shift_functor<bh_int32, bh_int32, bh_int32 > >( op_out, op_in );
        case BH_LEFT_SHIFT + (BH_INT64 << 8):
            return traverse_sa<bh_int64, bh_int64, left_shift_functor<bh_int64, bh_int64, bh_int64 > >( op_out, op_in );
        case BH_LEFT_SHIFT + (BH_INT8 << 8):
            return traverse_sa<bh_int8, bh_int8, left_shift_functor<bh_int8, bh_int8, bh_int8 > >( op_out, op_in );
        case BH_LEFT_SHIFT + (BH_UINT16 << 8):
            return traverse_sa<bh_uint16, bh_uint16, left_shift_functor<bh_uint16, bh_uint16, bh_uint16 > >( op_out, op_in );
        case BH_LEFT_SHIFT + (BH_UINT32 << 8):
            return traverse_sa<bh_uint32, bh_uint32, left_shift_functor<bh_uint32, bh_uint32, bh_uint32 > >( op_out, op_in );
        case BH_LEFT_SHIFT + (BH_UINT64 << 8):
            return traverse_sa<bh_uint64, bh_uint64, left_shift_functor<bh_uint64, bh_uint64, bh_uint64 > >( op_out, op_in );
        case BH_LEFT_SHIFT + (BH_UINT8 << 8):
            return traverse_sa<bh_uint8, bh_uint8, left_shift_functor<bh_uint8, bh_uint8, bh_uint8 > >( op_out, op_in );
        case BH_RIGHT_SHIFT + (BH_INT16 << 8):
            return traverse_sa<bh_int16, bh_int16, right_shift_functor<bh_int16, bh_int16, bh_int16 > >( op_out, op_in );
        case BH_RIGHT_SHIFT + (BH_INT32 << 8):
            return traverse_sa<bh_int32, bh_int32, right_shift_functor<bh_int32, bh_int32, bh_int32 > >( op_out, op_in );
        case BH_RIGHT_SHIFT + (BH_INT64 << 8):
            return traverse_sa<bh_int64, bh_int64, right_shift_functor<bh_int64, bh_int64, bh_int64 > >( op_out, op_in );
        case BH_RIGHT_SHIFT + (BH_INT8 << 8):
            return traverse_sa<bh_int8, bh_int8, right_shift_functor<bh_int8, bh_int8, bh_int8 > >( op_out, op_in );
        case BH_RIGHT_SHIFT + (BH_UINT16 << 8):
            return traverse_sa<bh_uint16, bh_uint16, right_shift_functor<bh_uint16, bh_uint16, bh_uint16 > >( op_out, op_in );
        case BH_RIGHT_SHIFT + (BH_UINT32 << 8):
            return traverse_sa<bh_uint32, bh_uint32, right_shift_functor<bh_uint32, bh_uint32, bh_uint32 > >( op_out, op_in );
        case BH_RIGHT_SHIFT + (BH_UINT64 << 8):
            return traverse_sa<bh_uint64, bh_uint64, right_shift_functor<bh_uint64, bh_uint64, bh_uint64 > >( op_out, op_in );
        case BH_RIGHT_SHIFT + (BH_UINT8 << 8):
            return traverse_sa<bh_uint8, bh_uint8, right_shift_functor<bh_uint8, bh_uint8, bh_uint8 > >( op_out, op_in );
        case BH_ARCTAN2 + (BH_FLOAT32 << 8):
            return traverse_sa<bh_float32, bh_float32, arctan2_functor<bh_float32, bh_float32, bh_float32 > >( op_out, op_in );
        case BH_ARCTAN2 + (BH_FLOAT64 << 8):
            return traverse_sa<bh_float64, bh_float64, arctan2_functor<bh_float64, bh_float64, bh_float64 > >( op_out, op_in );
        case BH_MOD + (BH_FLOAT32 << 8):
            return traverse_sa<bh_float32, bh_float32, mod_functor<bh_float32, bh_float32, bh_float32 > >( op_out, op_in );
        case BH_MOD + (BH_FLOAT64 << 8):
            return traverse_sa<bh_float64, bh_float64, mod_functor<bh_float64, bh_float64, bh_float64 > >( op_out, op_in );
        case BH_MOD + (BH_INT16 << 8):
            return traverse_sa<bh_int16, bh_int16, mod_functor<bh_int16, bh_int16, bh_int16 > >( op_out, op_in );
        case BH_MOD + (BH_INT32 << 8):
            return traverse_sa<bh_int32, bh_int32, mod_functor<bh_int32, bh_int32, bh_int32 > >( op_out, op_in );
        case BH_MOD + (BH_INT64 << 8):
            return traverse_sa<bh_int64, bh_int64, mod_functor<bh_int64, bh_int64, bh_int64 > >( op_out, op_in );
        case BH_MOD + (BH_INT8 << 8):
            return traverse_sa<bh_int8, bh_int8, mod_functor<bh_int8, bh_int8, bh_int8 > >( op_out, op_in );
        case BH_MOD + (BH_UINT16 << 8):
            return traverse_sa<bh_uint16, bh_uint16, mod_functor<bh_uint16, bh_uint16, bh_uint16 > >( op_out, op_in );
        case BH_MOD + (BH_UINT32 << 8):
            return traverse_sa<bh_uint32, bh_uint32, mod_functor<bh_uint32, bh_uint32, bh_uint32 > >( op_out, op_in );
        case BH_MOD + (BH_UINT64 << 8):
            return traverse_sa<bh_uint64, bh_uint64, mod_functor<bh_uint64, bh_uint64, bh_uint64 > >( op_out, op_in );
        case BH_MOD + (BH_UINT8 << 8):
            return traverse_sa<bh_uint8, bh_uint8, mod_functor<bh_uint8, bh_uint8, bh_uint8 > >( op_out, op_in );

        default:
            
            return BH_ERROR;

    }

}
