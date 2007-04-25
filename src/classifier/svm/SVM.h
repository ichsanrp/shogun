/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Written (W) 1999-2007 Soeren Sonnenburg
 * Copyright (C) 1999-2007 Fraunhofer Institute FIRST and Max-Planck-Society
 */

#ifndef _SVM_H___
#define _SVM_H___

#include "lib/common.h"
#include "features/Features.h"
#include "kernel/Kernel.h"
#include "kernel/KernelMachine.h"

class CKernelMachine;

class CSVM : public CKernelMachine
{
	public:
		CSVM(INT num_sv=0);
		CSVM(DREAL C, CKernel* k, CLabels* lab);
		virtual ~CSVM();

		void set_defaults(INT num_sv=0);

		bool load(FILE* svm_file);
		bool save(FILE* svm_file);

		inline void set_nu(DREAL nue) { nu=nue; }
		inline void set_C(DREAL c1, DREAL c2) { C1=c1; C2=c2; }
		inline void set_weight_epsilon(DREAL eps) { weight_epsilon=eps; }
		inline void set_epsilon(DREAL eps) { epsilon=eps; }
		inline void set_tube_epsilon(DREAL eps) { tube_epsilon=eps; }
		inline void set_C_mkl(DREAL C) { C_mkl = C; }
		inline void set_qpsize(int qps) { qpsize=qps; }
		inline void set_max_train_time(DREAL s) { max_train_time=s; }

		inline DREAL get_weight_epsilon() { return weight_epsilon; }
		inline DREAL get_epsilon() { return epsilon; }
		inline DREAL get_nu() { return nu; }
		inline DREAL get_C1() { return C1; }
		inline DREAL get_C2() { return C2; }
		inline int get_qpsize() { return qpsize; }

		inline int get_support_vector(int idx)
		{
			ASSERT(svm_model.svs && idx<svm_model.num_svs);
			return svm_model.svs[idx];
		}

		inline DREAL get_alpha(int idx)
		{
			ASSERT(svm_model.alpha && idx<svm_model.num_svs);
			return svm_model.alpha[idx];
		}

		inline bool set_support_vector(int idx, INT val)
		{
			if (svm_model.svs && idx<svm_model.num_svs)
				svm_model.svs[idx]=val;
			else
				return false;

			return true;
		}

		inline bool set_alpha(int idx, DREAL val)
		{
			if (svm_model.alpha && idx<svm_model.num_svs)
				svm_model.alpha[idx]=val;
			else
				return false;

			return true;
		}

		inline DREAL get_bias()
		{
			return svm_model.b;
		}

		inline void set_bias(double bias)
		{
			svm_model.b=bias;
		}

		inline int get_num_support_vectors()
		{
			return svm_model.num_svs;
		}

      
        void set_alphas(DREAL* alphas, INT d)
        {
            ASSERT(alphas);
            ASSERT(d==svm_model.num_svs);

            for(int i=0; i<d; i++)
				svm_model.alpha[i]=alphas[i];
        }

        void set_support_vectors(INT* svs, INT d)
        {
            ASSERT(svs);
            ASSERT(d==svm_model.num_svs);

            for(int i=0; i<d; i++)
				svm_model.svs[i]=svs[i];
        }

        void get_support_vectors(INT** svs, INT* num)
        {
            int nsv = get_num_support_vectors();

            ASSERT(svs && num);
            *svs=NULL;
            *num=nsv;

            if (nsv>0)
            {
                *svs = new INT[nsv];
                for(int i=0; i<nsv; i++)
                    (*svs)[i] = get_support_vector(i);
            } 
        }

        void get_alphas(DREAL** alphas, INT* d1)
        {
            int nsv = get_num_support_vectors();

            ASSERT(alphas && d1);
            *alphas=NULL;
            *d1=nsv;

            if (nsv>0)
            {
                *alphas = new DREAL[nsv];
                for(int i=0; i<nsv; i++)
                    (*alphas)[i] = get_alpha(i);
            } 
        }

		inline bool create_new_model(int num)
		{
			delete[] svm_model.alpha;
			delete[] svm_model.svs;

			svm_model.b=0;
			svm_model.num_svs=num;

			if (num>0)
			{
				svm_model.alpha= new double[num];
				svm_model.svs= new int[num];
				return (svm_model.alpha!=NULL && svm_model.svs!=NULL);
			}
			else
			{
				svm_model.alpha= NULL;
				svm_model.svs=NULL;
				return true;
			}
		}

		inline void set_shrinking_enabled(bool enable)
		{
			use_shrinking=enable;
		}

		inline bool get_shrinking_enabled()
		{
			return use_shrinking;
		}

		inline void set_mkl_enabled(bool enable)
		{
			use_mkl=enable;
		}

		inline bool get_mkl_enabled()
		{
			return use_mkl;
		}

		inline void set_batch_computation_enabled(bool enable)
		{
			use_batch_computation=enable;
		}

		inline bool get_batch_computation_enabled()
		{
			return use_batch_computation;
		}

		inline void set_linadd_enabled(bool enable)
		{
			use_linadd=enable;
		}

		inline bool get_linadd_enabled()
		{
			return use_linadd ;
		}

		///compute and set objective
		DREAL compute_objective();

		inline void set_objective(DREAL v)
		{
			objective=v;
		}

		inline DREAL get_objective()
		{
			return objective ;
		}

		bool init_kernel_optimization();

		virtual CLabels* classify(CLabels* labels=NULL);
		virtual DREAL classify_example(INT num);
		static void* classify_example_helper(void* p);

		void set_precomputed_subkernels_enabled(bool flag)
		{
			use_precomputed_subkernels = flag;
		}
	protected:

		/// an SVM is defined by support vectors, their coefficients alpha
		/// and the bias b ( + CKernelMachine::get_kernel())
		struct TModel
		{
			DREAL b;

			DREAL* alpha;
			int* svs;

			int num_svs;
		};

		TModel svm_model;
		bool svm_loaded;

		DREAL weight_epsilon;
		DREAL epsilon;
		DREAL tube_epsilon;
		DREAL max_train_time;

		DREAL nu;
		DREAL C1;
		DREAL C2;
		DREAL C_mkl ;

		DREAL objective;

		int qpsize;
		bool use_shrinking;
		bool use_mkl;
		bool use_batch_computation;
		bool use_linadd;
		bool use_precomputed_subkernels;
};
#endif
