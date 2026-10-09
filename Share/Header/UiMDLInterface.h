#ifndef UIMDLINTERFACE_H
#define UIMDLINTERFACE_H



namespace UIMDL
{
	class IUIMDLEvent;
	class IUIMDLDataset;
	class IUIMDL;

	//UI数据集接口
	typedef	bool ( *findFunc )( void*, void* ); 

	struct UIMDLDatasetRecord 
	{
		UIMDLDatasetRecord()
		{
			nIndex			= 0;
			pRecordData		= 0;
			nRecordLength	= 0;
		}
		UIMDLDatasetRecord( const UIMDLDatasetRecord& rDatasetRecord )
		{
			nIndex			= rDatasetRecord.nIndex;
			pRecordData		= rDatasetRecord.pRecordData;
			nRecordLength	= rDatasetRecord.nRecordLength;
		}
		int		nIndex;
		void*	pRecordData;
		int		nRecordLength;
	};

	class IUIMDLDataset 
	{
	public:
		IUIMDLDataset() {}
		virtual ~IUIMDLDataset() {} 
	public:
		virtual UIMDLDatasetRecord&	getDataRecord	( int nIndex										) = 0;
		virtual void				updateRecord	( int nIndex, void* pDataRecord, int nDataRecordLen ) = 0;
		virtual void				addDataRecord	( void* pDataRecord, int nDataRecordLen				) = 0;
		virtual void				setEventHandle	( IUIMDLEvent* pEventHandler						) = 0;
		virtual	void				delDataRecord	( int nIndex										) = 0;
		virtual	void				delAllDataRecord( void												) = 0;
		virtual UIMDLDatasetRecord&	findDataRecord	( findFunc pFunc, void* pFindParam					) = 0;	
		virtual int					getRecordCount	( void												) = 0;	
	};

	//UI数据集事件接口
	enum UIMDLDetailEvent
	{
		insert_umdl,
		delete_umdl,
		update_umdl,
	};

	struct UIMDLEvent 
	{
		IUIMDLDataset*		pDataSet;
		UIMDLDetailEvent	nOperation;
		int					nRecordIndex;
	};

	class IUIMDLEvent
	{
	public:
		IUIMDLEvent() {}
		virtual ~IUIMDLEvent() {}
	public:
		virtual void				onCreate		( UIMDLEvent& rEvent				) = 0;
		virtual void				onRelease		( UIMDLEvent& rEvent				) = 0;
		virtual void				onChange		( UIMDLEvent& rEvent				) = 0;
	};

	//UI数据中间层接口
	class IUIMDL
	{
	public:
		virtual int					createDataSet	( const char* szKey									) = 0;
		virtual int					releaseDataSet	( const char* szKey									) = 0;
		virtual int					queryDataSet	( const char* szKey, IUIMDLDataset** iDataset		) = 0;
	};

	//全局函数
	int CreateMDL ( void						);
	int GetMDLPtr ( IUIMDL** pUIMDLInterface	);
	int ReleaseMDL( void						);

	//错误代码
	enum UIMDLError
	{
		success_errorcode,
		unknown_errorcode,
		no_create_datasetmgr_errorcode,
		create_dataset_errorcode,
		not_find_dataset_errorcode,
		dataset_areadycreated_errorcode,
	};
}

using namespace UIMDL;

#endif