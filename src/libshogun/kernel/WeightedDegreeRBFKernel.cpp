/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Written (W) 1999-2009 Soeren Sonnenburg
 * Copyright (C) 1999-2009 Fraunhofer Institute FIRST and Max-Planck-Society
 */

#include "lib/common.h"
#include "kernel/WeightedDegreeRBFKernel.h"
#include "features/Features.h"
#include "features/SimpleFeatures.h"
#include "lib/io.h"

#ifdef HAVE_BOOST_SERIALIZATION
#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT(shogun::CWeightedDegreeRBFKernel);
#endif //HAVE_BOOST_SERIALIZATION

using namespace shogun;

CWeightedDegreeRBFKernel::CWeightedDegreeRBFKernel()
: CSimpleKernel<float64_t>(), width(1)
{
	init_wd_weights();
}


CWeightedDegreeRBFKernel::CWeightedDegreeRBFKernel(int32_t size, float64_t w, int32_t d, int32_t nof_prop)
: CSimpleKernel<float64_t>(size), width(w), degree(d), nof_properties(nof_prop)
{
	init_wd_weights();
}

CWeightedDegreeRBFKernel::CWeightedDegreeRBFKernel(
	CSimpleFeatures<float64_t>* l, CSimpleFeatures<float64_t>* r, float64_t w, int32_t d, int32_t nof_prop, int32_t size)
: CSimpleKernel<float64_t>(size), width(w), degree(d), nof_properties(nof_prop)
{
	init_wd_weights();
	init(l,r);
}

CWeightedDegreeRBFKernel::~CWeightedDegreeRBFKernel()
{
	delete[] weights;
	weights=NULL;
}

bool CWeightedDegreeRBFKernel::init(CFeatures* l, CFeatures* r)
{
	CSimpleKernel<float64_t>::init(l, r);
	return init_normalizer();
}

bool CWeightedDegreeRBFKernel::init_wd_weights()
{
	ASSERT(degree>0);

	delete[] weights;
	weights=new float64_t[degree];
	if (weights)
	{
		int32_t i;
		float64_t sum=0;
		for (i=0; i<degree; i++)
		{
			weights[i]=degree-i;
			sum+=weights[i];
		}
		for (i=0; i<degree; i++)
			weights[i]/=sum;

		return true;
	}
	else
		return false;
}


float64_t CWeightedDegreeRBFKernel::compute(int32_t idx_a, int32_t idx_b)
{
	int32_t alen, blen;
	bool afree, bfree;

	float64_t* avec=((CSimpleFeatures<float64_t>*) lhs)->get_feature_vector(idx_a, alen, afree);
	float64_t* bvec=((CSimpleFeatures<float64_t>*) rhs)->get_feature_vector(idx_b, blen, bfree);
	ASSERT(alen==blen);
	ASSERT(alen%nof_properties == 0);

	float64_t result=0;

	for (int32_t i=0; i<alen; i+=nof_properties)
	{
		float64_t resulti = 0.0;

		for (int32_t d=0; (i+(d*nof_properties)<alen) && (d<degree); d++)
		{
			float64_t resultid = 0.0;
			int32_t limit = (d + 1 ) * nof_properties;
			for (int32_t k=0; k < limit; k++)
			{
				resultid+=CMath::sq(avec[i+k]-bvec[i+k]);
			}
			
			resulti += weights[d] * exp(-resultid/width);
		}
	
		result+=resulti ;
	}

	return result;
}
