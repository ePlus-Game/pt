//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 12/10/2006 17:36
//      fire_base        : UiMDLDataset
//      fire_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#include "UiMDLInterface.h"
#include <vector>
#include <list>


namespace UIMDL
{

typedef	std::vector<UIMDLDatasetRecord> DataRecordSet;
typedef std::list<IUIMDLEvent*>	EventHandlerList;

class Dataset : public IUIMDLDataset
{
	friend class DatasetManager;
private:
	Dataset();
	~Dataset();

public:
	UIMDLDatasetRecord&	getDataRecord	( int nIndex										);
	void				updateRecord	( int nIndex, void* pDataRecord, int nDataRecordLen );
	void				addDataRecord	( void* pDataRecord, int nDataRecordLen				);
	void				setEventHandle	( IUIMDLEvent* pEventHandler						);
	int					getRecordCount	( void												);
	void				delDataRecord	( int nIndex										);
	void				delAllDataRecord( void												);
	UIMDLDatasetRecord&	findDataRecord	( findFunc pFunc, void* pFindParam					);

protected:
	bool				createDataSet	( void												);
	bool				releaseDataSet	( void												);
	void				fireCreateEvent	( UIMDLEvent& rEvent								);
	void				fireChangeEvent	( UIMDLEvent& rEvent								);
	void				fireReleaseEvent( UIMDLEvent& rEvent								);

protected:
	DataRecordSet		m_dataSet;
	EventHandlerList	m_eventHandlerList;
};

}