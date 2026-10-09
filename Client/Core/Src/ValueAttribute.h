//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2006-8-21 22:29
//      File_base        : ValueAttribute
//      File_ext         : h
//      Author           : chenshanglin
//      Description      : 
//
//      <Change_list>
//      {
//      Change_datetime  : 
//      Change_by        : 
//      Change_purpose   : 
//      }
//////////////////////////////////////////////////////////////////////
#ifndef _ValueAttribute_h
#define _ValueAttribute_h

#include "KRandom.h"

enum enCompoundMemIdx
{
	idx_base_value,
	idx_append_value,
	idx_append_percent,
	idx_current_value,
};

enum enRangeMemberIdx
{
	idx_value_low,
	idx_value_hight,
};

class CompoundAttr
{
public:
	CompoundAttr()
	{
		memset(m_Val, 0, sizeof(m_Val));
	}

	inline int operator[] (int nIdx) const
	{
		return m_Val[nIdx];
	}

	inline void Set(int nIdx, int nVal)
	{
		m_Val[nIdx] = nVal;

		if(idx_current_value != nIdx)
		{
			m_Val[idx_current_value] = (m_Val[0] + m_Val[1]) * (100 + m_Val[2]) / 100;
		}
	}

	inline operator int() const
	{
		return m_Val[idx_current_value];
	}
	
private:
	// Base Value, Append Value, Append Percent
	int		m_Val[4];
};

class RangeAttr
{
public:
	inline const CompoundAttr& operator[](int nAttrIdx) const
	{
		return m_RangeVal[nAttrIdx];
	}

	inline void Set(int nAttrIdx, int nValIdx, int nVal)
	{
		m_RangeVal[nAttrIdx].Set(nValIdx, nVal);
	}

	inline operator int() const
	{
		int low = m_RangeVal[0];
		int hight = m_RangeVal[1];

		if(hight >= low)
		{	// 因为 g_Random 去尾, 无法返回 hight - low 的值，所以加1
			return low + g_Random(hight - low + 1 );
		}
		else
		{
			return hight + g_Random(low - hight + 1);
		}
	}

private:
	// Low value and hight value
	CompoundAttr	m_RangeVal[2];	
};

template<typename ValType, int ELEM_NUM>
class UnaryAttrMgr
{
public:
	UnaryAttrMgr()
	{
		memset(&m_Attr, 0, sizeof(m_Attr));
	}

	inline const ValType& operator[] (int nIdx) const
	{
		return m_Attr[nIdx];
	}

	inline void Set(int nIdx, const ValType &nVal)
	{
		m_Attr[nIdx] = nVal;
	}

private:
	ValType		m_Attr[ELEM_NUM];
};

template<int ELEM_NUM>
class CompoundAttrMgr
{
public:
	inline void Set(int nAttrIdx, int nValIdx, int nVal)
	{
		m_Attr[nAttrIdx].Set(nValIdx, nVal);
	}

	inline const CompoundAttr& operator[] (int nAttrIdx) const
	{
		return m_Attr[nAttrIdx];
	}

private:
	CompoundAttr	m_Attr[ELEM_NUM];
};

template<int ELEM_NUM>
class RangeAttrMgr
{
public:
	inline void Set(int nAttrIdx, int nSubAttrIdx, int nValIdx, int nVal)
	{
		m_Attr[nAttrIdx].Set(nSubAttrIdx, nValIdx, nVal);
	}

	inline const RangeAttr& operator[] (int nAttrIdx) const
	{
		return m_Attr[nAttrIdx];
	}
private:
	RangeAttr	m_Attr[ELEM_NUM];
};

#endif // _ValueAttribute_h
