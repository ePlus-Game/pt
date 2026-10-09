/*******************************************************************************
File        : PosterIncise.cpp
Creator     : Fyt(Fan Zhanpeng)
create data : 02-23-2004(mm-dd-yyyy)
Description : 把一幅图形分CELL切割开来，并在Represent模块创建图形资源
            : 并且可以在指定位置绘画出某部分，或某CELL
********************************************************************************/

#include "KCore.h"

#ifndef _SERVER

#include "iRepresentShell.h"

#include "imgref.h"

#include ".\posterincise.h"

static unsigned long StringToHash(const char *pString, BOOL bIsCaseSensitive = FALSE);

KPosterIncise::KPosterIncise(void)
{
	m_bIsLoaded = FALSE;
	m_pCell     = NULL;

	m_nCellCount = 0;
	m_nHCount = 0;
	m_nVCount = 0;

	m_nCurrentCellX = 0;
	m_nCurrentCellY = 0;
	m_nCurrentCell = 0;


	int nBits = (int)enumCS_CELL_WIDTH;

	m_nDivBit = -1;
	while(nBits)
	{
		nBits >>= 1;
		m_nDivBit ++;
	}
	m_nResidueBit = (int)enumCS_CELL_WIDTH - 1;

	m_Img.bRenderStyle = IMAGE_RENDER_STYLE_OPACITY;
	m_Img.bRenderFlag = 0;
	m_Img.Color.Color_dw = 0;
	m_Img.nFrame = 0;
	m_Img.nType = ISI_T_BITMAP16;
}

KPosterIncise::~KPosterIncise(void)
{
	ReleaseCell();
}



/*******************************************************************************************/
//载入JPG图片
/*******************************************************************************************/
int KPosterIncise::LoadJPG(char *szPicFileName)
{
	KSGImageContent* pViewFile = NULL;
	KBitmapDataBuffInfo Info;
	int nColorByte;
	int nRet = 0;
	void *pBuff;

	m_eType = ISI_T_BITMAP16;

	if(ConstructCell(1) && g_pRepresent)
	{
		pBuff = g_pRepresent->GetBitmapDataBuffer(m_pCell[0].szImageName, &Info);
		if(pBuff)
		{
			g_pRepresent->ReleaseBitmapDataBuffer(m_pCell[0].szImageName, pBuff);
			unsigned int uMask16 = -1;
			if (Info.eFormat == BDBF_16BIT_555)
			{
				uMask16 = RGB_555;
			}
			else if (Info.eFormat == BDBF_16BIT_565)
			{
				uMask16 = RGB_565;
			}
			nColorByte = Info.nPitch / enumCS_CELL_WIDTH;
			if (uMask16 >= 0)
			{
				pViewFile = get_jpg_image(szPicFileName, uMask16);
			}
		}
	}
	if(pViewFile)
	{
		//释放图形资源
		ReleaseCell();

		m_nNameHash = StringToHash(szPicFileName);
		m_nHeight = pViewFile->nHeight;
		m_nWidth  = pViewFile->nWidth;

		WorkoutCellInfo();

		//构建图形资源
		if(ConstructCell())
		{
			int nTemp;

			for(int i = 0;i < m_nVCount;i++)
			{
				for(int j = 0;j < m_nHCount;j++)
				{
					nTemp = i + j * m_nVCount;
					pBuff = g_pRepresent->GetBitmapDataBuffer(m_pCell[nTemp].szImageName, &Info);
					_ASSERT(pBuff);
					if(pBuff)
					{
						CopyPicToBuff(pBuff, pViewFile, nColorByte, i * enumCS_CELL_WIDTH, j * enumCS_CELL_HEIGHT, (int)(m_pCell[nTemp].nWidth), (int)(m_pCell[nTemp].nHeight));
						g_pRepresent->ReleaseBitmapDataBuffer(m_pCell[nTemp].szImageName, pBuff);
					}
					//
				}
				//
			}
			//
			m_bIsLoaded = TRUE;
			nRet = 1;
		}
		//
	}
	//
	return nRet;
}


/*******************************************************************************************/
//根据m_nWidth和m_nHeight来运算出Cell相关数值
/*******************************************************************************************/
void KPosterIncise::WorkoutCellInfo()
{
	m_nVCount = (m_nWidth >> m_nDivBit)  + ((m_nWidth & m_nResidueBit) > 0);
	m_nHCount = (m_nHeight >> m_nDivBit) + ((m_nHeight & m_nResidueBit) > 0);
	m_nCellCount = m_nVCount * m_nHCount;
}


/*******************************************************************************************/
//构造CELL，传入要构造的CELL的数量，-1(默认值)的话就根据m_nCellCount来构造
/*******************************************************************************************/
int KPosterIncise::ConstructCell(int nCount)
{
	int nCellCount = nCount;
	int nRet = 0;

	if(nCellCount < 0)
	{
		nCellCount = m_nCellCount;
	}

	if(m_pCell)
	{
		ReleaseCell();
	}
	if(!m_pCell)
	{
		m_pCell = new MAP_CELL[nCellCount];
		if(m_pCell)
		{
			if(g_pRepresent)
			{
				char szBuff[32];
				char szName[32];
				DWORD nTimeTick = IR_GetCurrentTime();

				nTimeTick += m_nNameHash;
				itoa(nTimeTick, szBuff, 16);

				int j = 0;
				for(int i = 0;j < nCellCount && i < 10240;i++)
				{
					sprintf(szName, "%4d#%s#%4d", i, szBuff, i);
					m_pCell[j].uImageId = g_pRepresent->CreateImage(szName, enumCS_CELL_WIDTH, enumCS_CELL_HEIGHT, m_eType);

					if(!m_pCell[j].uImageId)
					{
						continue;
					}
					else
					{
						strcpy(m_pCell[j].szImageName, szName);
						j++;
					}
				}
				if(j < nCellCount)
				{
					ReleaseCell();
				}
				else
				{
					m_nCellCount = nCellCount;
					nRet = 1;
				}
			}
			else
			{
				ReleaseCell();
			}
			//
		}
		//
	}
	//
	return nRet;
}


/*******************************************************************************************/
//释放CELL资源
/*******************************************************************************************/
void KPosterIncise::ReleaseCell()
{
	if(m_pCell)
	{
		if(g_pRepresent)
		{
			for(int i = 0;i < m_nCellCount;i++)
			{
				g_pRepresent->FreeImage(m_pCell[i].szImageName);
			}
		}
		delete m_pCell;
	}
	m_pCell = NULL;
	m_bIsLoaded = FALSE;
	m_nCurrentCell = 0;
	m_nCurrentCellX = 0;
	m_nCurrentCellY = 0;
	m_nCellCount = 0;
	m_nHCount = 0;
	m_nVCount = 0;
}


/*******************************************************************************************/
//从整张图片上攫取一部分，拷贝到指定的缓冲区里，并且把实际拷贝的长宽
//告诉调用者
/*******************************************************************************************/
void KPosterIncise::CopyPicToBuff(void* pDest, void* pSrc, int nColorByte, int nSrcX, int nSrcY, int &nCopyWidth, int &nCopyHeight)
{
	char *pBuff, *pSrcBuff;

	if(nSrcX + enumCS_CELL_WIDTH > m_nWidth)
	{
		nCopyWidth = m_nWidth - nSrcX;
	}
	else
	{
		nCopyWidth = enumCS_CELL_WIDTH;
	}

	if(nSrcY + enumCS_CELL_HEIGHT > m_nHeight)
	{
		nCopyHeight = m_nHeight - nSrcY;
	}
	else
	{
		nCopyHeight = enumCS_CELL_HEIGHT;
	}

	pBuff    = (char *)pDest;
	pSrcBuff = (char *)pSrc;
	pSrcBuff += (nSrcX + nSrcY * m_nWidth) * nColorByte;
	for(int i = 0;i < nCopyHeight;i++)
	{
		memcpy(pBuff, pSrcBuff, nCopyWidth * nColorByte);
		pBuff += nColorByte << m_nDivBit;
		pSrcBuff += m_nWidth * nColorByte;
	}
}


/*******************************************************************************************/
//绘制当前CELL的某部分
/*******************************************************************************************/
void KPosterIncise::PaintCurrentCell(int nPaintX, int nPaintY, int nCellX, int nCellY, int nCellWidth, int nCellHeight, int /*[out]*/&nPaintWidth, int /*[out]*/&nPaintHeight)
{
	PaintCell(nPaintX, nPaintY, m_nCurrentCell, nCellX, nCellY, nCellWidth, nCellHeight, nPaintWidth, nPaintHeight);
}


/*******************************************************************************************/
//绘制由X索引和Y索引所指定的某个CELL的某部分
/*******************************************************************************************/
void KPosterIncise::PaintCell(int nPaintX, int nPaintY, int nCellIndexX, int nCellIndexY,int nCellX, int nCellY, int nCellWidth, int nCellHeight, int /*[out]*/&nPaintWidth, int /*[out]*/&nPaintHeight)
{
	int nCellIndex = nCellIndexX + nCellIndexY * m_nVCount;
	PaintCell(nPaintX, nPaintY, nCellIndex, nCellX, nCellY, nCellWidth, nCellHeight, nPaintWidth, nPaintHeight);
}


/*******************************************************************************************/
//绘制由总索引所指定的某个CELL的某部分
/*******************************************************************************************/
void KPosterIncise::PaintCell(int nPaintX, int nPaintY, int nCellIndex, int nCellX, int nCellY, int nCellWidth, int nCellHeight, int /*[out]*/&nPaintWidth, int /*[out]*/&nPaintHeight)
{
	//return;
	_ASSERT(nCellIndex >= 0 && nCellIndex < m_nCellCount);
	if(!m_pCell)
	{
		return;
	}

	MAP_CELL &Cell = m_pCell[nCellIndex];

	//如果超出宽度，就不画
	if(nCellX >= Cell.nWidth)
	{
		nPaintWidth = 0;
		nPaintHeight = 0;
		return;
	}
	//如果超出高度，就不画
	if(nCellY >= Cell.nHeight)
	{
		nPaintWidth = nCellWidth;
		nPaintHeight = 0;
		return;
	}
	//计算实际绘画宽度
	if((nCellX + nCellWidth) > Cell.nWidth)
	{
		nPaintWidth = Cell.nWidth - nCellX;
	}
	else
	{
		nPaintWidth = nCellWidth;
	}
	//计算实际绘画高度
	if((nCellY + nCellHeight) > Cell.nHeight)
	{
		nPaintHeight = Cell.nHeight - nCellY;
	}
	else
	{
		nPaintHeight = nCellHeight;
	}
	//画图实施
	switch((int)m_eType)
	{
	case ISI_T_SPR:
		{
		}
		break;

	case ISI_T_BITMAP16:
		{
			m_Img.oPosition.nX = nPaintX;
			m_Img.oPosition.nY = nPaintY;
			m_Img.oImgLTPos.nX = nCellX;
			m_Img.oImgLTPos.nY = nCellY;
			m_Img.oImgRBPos.nX = nCellX + nPaintWidth;
			m_Img.oImgRBPos.nY = nCellY + nPaintHeight;
			m_Img.nISPosition  =  Cell.sISPosition;
			m_Img.uImage       =  Cell.uImageId;
			strcpy(m_Img.szImage, Cell.szImageName);

			g_pRepresent->DrawPrimitives(1, &m_Img, RU_T_IMAGE_PART, true);
			Cell.sISPosition = m_Img.nISPosition;
			Cell.uImageId    = m_Img.uImage;
		}
		break;
	}
}


/*******************************************************************************************/
//绘制整张图的某部分
/*******************************************************************************************/
void KPosterIncise::Paint(int nPaintX, int nPaintY, int nX, int nY, int nWidth, int nHeight, int /*[out]*/&nPaintWidth, int /*[out]*/&nPaintHeight)
{
	//如果超过宽度或者高度，则返回
	if(nX >= m_nWidth || nY >= m_nHeight)
	{
		nPaintWidth = nPaintHeight = 0;
		return;
	}
	//计算实际绘画宽度
	if((nX + nWidth) > m_nWidth)
	{
		nPaintWidth = m_nWidth - nX;
	}
	else
	{
		nPaintWidth = nWidth;
	}
	//计算实际绘画高度
	if((nY + nHeight) > m_nHeight)
	{
		nPaintHeight = m_nHeight - nY;
	}
	else
	{
		nPaintHeight = nHeight;
	}
	//进行计算的前期准备
	int nPaintXInc, nPaintYInc, nLeft, nTop, nRight, nBottom, nBeginCellIndexX, nBeginCellIndexY;
	int nCellBeginX, nCellBeginY, nCellEndX, nCellEndY, nCellWidth, nCellHeight, nPaintEndX, nPaintEndY;

	nLeft = nX & m_nResidueBit;
	nTop = nY & m_nResidueBit;
	nRight = (nX + nPaintWidth) & m_nResidueBit;
	nBottom = (nY + nPaintHeight) & m_nResidueBit;

	nBeginCellIndexX = (nX >> m_nDivBit);
	nBeginCellIndexY = (nY >> m_nDivBit);

	SetCurrentCell(nBeginCellIndexX, nBeginCellIndexY);

	nPaintXInc = nPaintX;
	nPaintYInc = nPaintY;

	nPaintEndX = nPaintX + nPaintWidth;
	nPaintEndY = nPaintY + nPaintHeight;
	//绘画实施
	while(nPaintYInc < nPaintEndY)
	{
		if(nPaintYInc == nPaintY)
		{
			nCellBeginY = nTop;
		}
		else
		{
			nCellBeginY = 0;
		}
		if((nPaintEndY - nPaintYInc) < (enumCS_CELL_HEIGHT - nCellBeginY))
		{
			nCellEndY = nPaintEndY - nPaintYInc + nCellBeginY;
		}
		else
		{
			nCellEndY = enumCS_CELL_HEIGHT - 1;
		}
		while(nPaintXInc < nPaintEndX)
		{
			if(nPaintXInc == nPaintX)
			{
				nCellBeginX = nLeft;
			}
			else
			{
				nCellBeginX = 0;
			}
			if((nPaintEndX - nPaintXInc) < (enumCS_CELL_WIDTH - nCellBeginX))
			{
				nCellEndX = nPaintEndX - nPaintXInc + nCellBeginX;
			}
			else
			{
				nCellEndX = enumCS_CELL_WIDTH - 1;
			}
			PaintCurrentCell(nPaintXInc, nPaintYInc, nCellBeginX, nCellBeginY, nCellEndX - nCellBeginX + 1, nCellEndY - nCellBeginY + 1, nCellWidth, nCellHeight);
			nPaintXInc += nCellWidth;
			NextVCell();
		}
		nPaintYInc += nCellHeight;
		nPaintXInc  = nPaintX;
		SetV(nBeginCellIndexX);
		NextHCell();
	}
}


/*******************************************************************************************/
//根据X和Y索引设置当前CELL，返回总索引
/*******************************************************************************************/
int KPosterIncise::SetCurrentCell(int nX, int nY)
{
	if(nX >= 0 && nX < m_nVCount && nY >= 0 && nY < m_nHCount)
	{
		m_nCurrentCellX = nX;
		m_nCurrentCellY = nY;
		m_nCurrentCell = nX + nY * m_nVCount;
	}
	return m_nCurrentCell;
}


/*******************************************************************************************/
//根据总索引设置当前CELL，返回总索引
/*******************************************************************************************/
int KPosterIncise::SetCurrentCell(int nIndex)
{
	if(nIndex >= 0 && nIndex < m_nCellCount)
	{
		m_nCurrentCell = nIndex;
		m_nCurrentCellX = nIndex % m_nVCount;
		m_nCurrentCellY = nIndex / m_nVCount;
	}
	return m_nCurrentCell;
}


/*******************************************************************************************/
//把当前CELL移去下一列，返回总索引
/*******************************************************************************************/
int KPosterIncise::NextVCell()
{
	m_nCurrentCellX ++;
	m_nCurrentCell ++;
	if(m_nCurrentCellX >= m_nVCount)
	{
		m_nCurrentCellX = 0;
		m_nCurrentCell -= m_nVCount;
	}
	return m_nCurrentCell;
}


/*******************************************************************************************/
//把当前CELL移去下一行，返回总索引
/*******************************************************************************************/
int KPosterIncise::NextHCell()
{
	m_nCurrentCellY ++;
	m_nCurrentCell  += m_nVCount;
	if(m_nCurrentCellY >= m_nHCount)
	{
		m_nCurrentCellY = 0;
		m_nCurrentCell -= m_nCellCount;
	}
	return m_nCurrentCell;
}


/*******************************************************************************************/
//设置当前CELL去指定的列，返回总索引
/*******************************************************************************************/
int KPosterIncise::SetV(int nVertical)
{
	if(nVertical >= 0 && nVertical < m_nVCount)
	{
		int nInterval = nVertical - m_nCurrentCellX;
		m_nCurrentCellX += nInterval;
		m_nCurrentCell  += nInterval;
	}
	return m_nCurrentCell;
}


/*******************************************************************************************/
//设置当前CELL去指定的行，返回总索引
/*******************************************************************************************/
int KPosterIncise::SetH(int nHorizontal)
{
	if(nHorizontal >= 0 && nHorizontal < m_nHCount)
	{
		int nInterval = nHorizontal - m_nCurrentCellY;
		m_nCurrentCellY += nInterval;
		m_nCurrentCell  += (nInterval * m_nVCount);
	}
	return m_nCurrentCell;
}




unsigned long StringToHash(const char *pString, BOOL bIsCaseSensitive)
{
	if(pString && pString[0])
	{
		unsigned long id = 0;
		const char *ptr;
		int index = 0;

		if(bIsCaseSensitive)
		{
			ptr = pString;

			while(*ptr)
			{
    			id = (id + (++index) * (*ptr)) % 0x8000000b * 0xffffffef;
		        ptr++;
	        }
		}
		else
		{
			char Buff[256];
			strcpy(Buff, pString);
			strlwr(Buff);
			ptr = Buff;

        	while(*ptr)
		    {
    			id = (id + (++index) * (*ptr)) % 0x8000000b * 0xffffffef;
		        ptr++;
	        }
		}
		return (id ^ 0x12345678);
	}

	return 0;
}

#endif
