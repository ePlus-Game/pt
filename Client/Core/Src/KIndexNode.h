#ifndef KIndexNodeH
#define	KIndexNodeH

// lixuewu 2004.04.16
class KIndexNode : public KNode
{
public:
	int		m_nIndex;
public:
	KIndexNode():m_nIndex(0),m_Ref(0){};
	inline void AddRef(void) { m_Ref++; };
	inline void Release(void) { _ASSERT(m_Ref > 0); m_Ref--; };
	unsigned int m_Ref;
};

#endif
