// FilterText.cpp : Defines the entry point for the DLL application.
// Linux transplant by Cooler 2004-01-02 liuyujun@263.net
//////////////////////////////////////////////////////////////////////

#include "KCore.h"
#include "FilterText.h"

CTextFilter g_TextFilter;
CTextFilter g_ChatTxtFilter;
#ifndef _SERVER
CTextFilter g_UserChatFilter;
CTextFilter g_ChatRecvFilter;
#endif

BOOL CTextFilter::AddExpression(LPCTSTR szExp)
{
	try
	{
		static const TCHAR CH_ESC = '\\';

		if (szExp == NULL || szExp[0] == 0)
			return TRUE;

		if (szExp[0] == leadchar_ignore)
			return TRUE;

		BOOL bInsensitive = szExp[0] == leadchar_insensitive;
		if (bInsensitive)
			szExp ++;
	
		if (szExp[0] != leadchar_common
			&& szExp[0] != leadchar_advance)
			return FALSE;

		BOOL bCommon = szExp[0] == leadchar_common;
		LPCTSTR szRegExp = szExp + 1;

		if (szRegExp[0] == 0)
			return TRUE;

		TCHAR chLead = szRegExp[0];
		if (chLead == CH_ESC)
		{
			bCommon = TRUE;
			chLead = szExp[2];
			if (chLead == 0)
				return FALSE;
			if (isalpha(chLead))
				szRegExp ++;
		}

		if (bCommon && bInsensitive && isalpha(chLead))
			bCommon = FALSE;


		Regexp expr(szRegExp, bInsensitive);
		if (!expr.CompiledOK())
			return FALSE;

		if (bCommon)
		{
			const HASHINDEXTYPE hashkey = Char2HashIdx(chLead);
			m_HashEntry[hashkey].push_back(expr);
			m_nCommon ++;
		}
		else
		{
			m_AdvExps.push_back(expr);
		}


		return TRUE;
	}
	catch (...)
	{
//		ASSERT(FALSE);
	}

	return FALSE;
}

BOOL CTextFilter::Clearup()
{
	try
	{
		for (int i = 0; i < HASHSIZE; i++)
			m_HashEntry[i].clear();
		m_nCommon = 0;

		m_AdvExps.clear();

		return TRUE;
	}
	catch (...)
	{
//		ASSERT(FALSE);
	}

	return FALSE;
}


#ifndef _SERVER
BOOL CTextFilter::ReplaceInvalidText(LPCTSTR text, TCHAR saver) //Notice just replace the common regexp
{
	try
	{
		if (text == NULL || text[0] == 0)
			return FALSE;
		
		int nTextLen = strlen(text);

		for (const TCHAR* pos = text; *pos != 0; pos = NextChar(pos))
		{
			const size_t cntAdvance = m_AdvExps.size();
			for (size_t i = 0; i < cntAdvance; i++)
			{
				if (m_AdvExps[i].Match(pos))
				{
					int   nMatchStart = m_AdvExps[i].SubStart(0);
		        	int   nMatchLen   = m_AdvExps[i].SubLength(0);

					if (nMatchLen > 0 && (pos - text) + nMatchStart < nTextLen)
					{
						int     nCurReplace =  0;
						char *  pChanger    = (char *)(pos + nMatchStart);
						
						while (nCurReplace < nMatchLen && *pChanger != 0 )
						{
							*pChanger = saver;
							nCurReplace ++;
							pChanger    ++;
						}//end for while
						
					}//endif

				}//endif
				
			}//end for i

			const HASHINDEXTYPE hashkey = Char2HashIdx(*pos);
			
			CExpArray& rExpVec = m_HashEntry[hashkey];
			
			const size_t expcount = rExpVec.size();
			for (size_t k = 0; k < expcount; k++)
			{
				if (rExpVec[k].Match(pos))
				{
					int nMatchStart = rExpVec[k].SubStart(0);
					int nMatchLen   = rExpVec[k].SubLength(0);
					
					if (nMatchLen > 0 && (pos - text) + nMatchStart < nTextLen)
					{
						int     nCurReplace =  0;
						char *  pChanger    = (char *)(pos + nMatchStart);
						
						while (nCurReplace < nMatchLen && *pChanger != 0 )
						{
							*pChanger = saver;
							nCurReplace ++;
							pChanger ++;
						}//end for while
						
						if (nMatchStart == 0)
						{
							pos += nMatchLen - 1;
 							break;
						}//endif
						
					}//endif

				}//endif
				
			}//end for k
				

		}//end for pos
		
		
	}//end for try
	
	catch (...)
	{
		//ASSERT(FALSE);
	}

	return TRUE;
}

#endif

BOOL CTextFilter::IsTextPass(LPCTSTR text)
{
	try
	{
		if (text == NULL || text[0] == 0)
			return TRUE;

		const size_t cntAdvance = m_AdvExps.size();
		const BOOL bAdvPrior = cntAdvance < m_nCommon;

		for (int loop = 0; loop < 2; loop++)
		{
			if ((loop == 0 && bAdvPrior)
				|| (loop != 0 && !bAdvPrior))
			{//advance
				for (size_t i = 0; i < cntAdvance; i++)
				{
					if (m_AdvExps[i].Match(text))
						return FALSE;
				}
			}
			else
			{//common
//				ASSERT(HASHSIZE % sizeof(BYTE) == 0);
				BYTE occur[HASHSIZE / sizeof(BYTE)] = {0};

				for (const TCHAR* pos = text; *pos != 0; pos = NextChar(pos))
				{
					const HASHINDEXTYPE hashkey = Char2HashIdx(*pos);

					BYTE& rByte = occur[hashkey / sizeof(BYTE)];
					const BYTE bitmask = 0x01 << (hashkey % sizeof(BYTE));

					if (!(rByte & bitmask))
					{
						CExpArray& rExpVec = m_HashEntry[hashkey];
						const size_t expcount = rExpVec.size();
						for (size_t k = 0; k < expcount; k++)
						{
							if (rExpVec[k].Match(pos))
								return FALSE;
						}

						rByte |= bitmask;
					}
				}
			}
		}

		return TRUE;
	}
	catch (...)
	{
//		ASSERT(FALSE);
	}

	return FALSE;
}
