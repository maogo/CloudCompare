//##########################################################################
//#                                                                        #
//#                               CCLIB                                    #
//#                                                                        #
//#  This program is free software; you can redistribute it and/or modify  #
//#  it under the terms of the GNU Library General Public License as       #
//#  published by the Free Software Foundation; version 2 of the License.  #
//#                                                                        #
//#  This program is distributed in the hope that it will be useful,       #
//#  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
//#  GNU General Public License for more details.                          #
//#                                                                        #
//#          COPYRIGHT: EDF R&D / TELECOM ParisTech (ENST-TSI)             #
//#                                                                        #
//##########################################################################

#include "ScalarField.h"

//system
#include <assert.h>
#include <string.h>

using namespace CCLib;

ScalarField::ScalarField(const char* name/*=0*/, bool positive /*=false*/)
	: GenericChunkedArray<1,ScalarType>()
	, m_onlyPositiveValues(positive)
{
	setName(name);
}

void ScalarField::setName(const char* name)
{
	if (name)
		strncpy(m_name,name,255);
	else
		strcpy(m_name,"Undefined");
}

void ScalarField::computeMeanAndVariance(ScalarType &mean, ScalarType* variance) const
{
	double _mean=0.0, _std2=0.0;
	unsigned count=0;

	for (unsigned i=0;i<m_maxCount;++i)
	{
		const ScalarType& val = getValue(i);
		if (validValue(val))
		{
			_mean += (double)val;
			_std2 += (double)val * (double)val;
			++count;
		}
	}

	if (count)
	{
		_mean /= (double)count;
		mean = (ScalarType)_mean;

		if (variance)
		{
			_std2 = fabs(_std2/(double)count - _mean*_mean);
			*variance = (ScalarType)_std2;
		}
	}
	else
	{
		mean = 0;
		if (variance)
			*variance = 0;
	}
}

void ScalarField::computeMinAndMax()
{
	if (m_maxCount!=0)
	{
		bool minMaxInitialized = false;
		for (unsigned i=0;i<m_maxCount;++i)
		{
			const ScalarType& val = getValue(i);
			if (validValue(val))
			{
				if (minMaxInitialized)
				{
					if (val<m_minVal)
						m_minVal=val;
					else if (val>m_maxVal)
						m_maxVal=val;
				}
				else
				{
					//first valid value is used to init min and max
					m_minVal = m_maxVal = val;
					minMaxInitialized = true;
				}
			}
		}
	}
	else //particular case: no value
	{
		m_minVal = m_maxVal = 0;
	}
}

bool ScalarField::setPositiveAuto()
{
	for (unsigned i=0;i<m_maxCount;++i)
	{
		const ScalarType& val = getValue(i);

		if (!PositiveSfValue(val))
		{
			setPositive(false);
			return false;
		}
	}

	setPositive(true);
	return true;
}