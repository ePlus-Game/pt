//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 12/11/2006 14:00
//      File_base        : KSimulation
//      File_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#ifndef KPROTOCOLSIMULATION_H
#define KPROTOCOLSIMULATION_H

#ifndef _SERVER
#include "UiMDLInterface.h"

#include <map>
#include <string>

class IProtocolSimulation : public IUIMDLEvent
{
public:
	IProtocolSimulation()
	{
		GetMDLPtr( &m_pMDL );
	}
	virtual ~IProtocolSimulation() {};
	virtual	void Breathe() = 0;
protected:
	IUIMDL*	m_pMDL;
};



class KProtocolSimulationSet
{
	typedef std::map<std::string, IProtocolSimulation*> _SimulationSet;
public:
	KProtocolSimulationSet() {};
	~KProtocolSimulationSet();
public:
	bool registerSimulation( const std::string& strKey, IProtocolSimulation* iSimulation  );
	IProtocolSimulation* getSimulation( const std::string& strKey );
	bool IsExisting( const std::string& strKey );
	void Breathe();

	void Clear();

private:
	_SimulationSet m_SimulationSet;
};

extern KProtocolSimulationSet g_ProtocolSimulationSet;


class KItemGroupCDSimulation : public IProtocolSimulation
{
	static bool _findGroupCD( void* pParam0, void* pParam1 ); 
public:
	KItemGroupCDSimulation( const std::string& strDatasetName );
	virtual ~KItemGroupCDSimulation();
public:
	virtual void	onCreate	( UIMDLEvent& rEvent				);
	virtual void	onRelease	( UIMDLEvent& rEvent				);
	virtual void	onChange	( UIMDLEvent& rEvent				);
	virtual	void	Breathe		( void								);
public:
	void			AddGroupCD	( KItemGroupCD_C& rCD				);
	void			DelGroupCD	( KItemGroupCD_C& rCD				);
private:
	void			ClearAllGroupCD( void );
private:
	std::string		m_strDatasetName;
	IUIMDLDataset*	m_pDataset;	
};


#endif

#endif	