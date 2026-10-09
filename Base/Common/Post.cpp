#include "KWin32.h"
#include "Post.h"

ptrdiff_t			
Post::Load(const unsigned char* p)
{
	if (p == 0)
	{
//		assert(0);
		return 0;
	}

	Id(*reinterpret_cast<const unsigned long*>(p));
	p += sizeof(unsigned long);

	SetState(static_cast<State>(*p));
	p += sizeof(unsigned char);	

	Duration(*p);
	p += sizeof(unsigned char);

	RewardPrize(*reinterpret_cast<const unsigned long*>(p));
	p += sizeof(unsigned long);

	HunterCount(*p);
	p += sizeof(unsigned char);

	MissionLevel(*reinterpret_cast<const unsigned short*>(p));
	p += sizeof(unsigned short);

	MissionCount(*p);
	p += sizeof(unsigned char);

	size_t infoLen = *p;
	p += 1;

	MissionInfo(reinterpret_cast<const char*>(p), infoLen);

	const size_t unchange = 
		   sizeof(unsigned long)  + // Id.
		   sizeof(unsigned char)  + // State.
		   sizeof(unsigned char)  + // Duration.

   		   sizeof(unsigned long)  + // Reward prize.

		   sizeof(unsigned char)  + // Hunter count.

		   sizeof(unsigned short) + // Target level.
		   sizeof(unsigned char)  + // Kill count.

		   sizeof(unsigned char)  ; // Target name length.

	return static_cast<ptrdiff_t>(unchange + infoLen);
}

ptrdiff_t			
Post::Save(unsigned char* p) const
{
	if (p == 0)
	{
//		assert(0);
		return 0;
	}
	
	*reinterpret_cast<unsigned long*>(p)  = Id();
	p += sizeof(unsigned long);

	*p									  = GetState();
	p += sizeof(unsigned char);

	*p									  = Duration();
	p += sizeof(unsigned char);

	*reinterpret_cast<unsigned long*>(p)  = RewardPrize();
	p += sizeof(unsigned long);

	*p									  = HunterCount();
	p += sizeof(unsigned char);

	*reinterpret_cast<unsigned short*>(p) = MissionLevel();
	p += sizeof(unsigned short);

	*reinterpret_cast<unsigned char*>(p)  = MissionCount();
	p += sizeof(unsigned char);

	size_t infoLen = ::strlen(MissionInfo());
	*p = static_cast<unsigned char>(infoLen);
	p += 1;

	::memcpy(reinterpret_cast<char*>(p), MissionInfo(), infoLen);

 	const size_t unchange = 
		   sizeof(unsigned long)  + // Id.
		   sizeof(unsigned char)  + // State.
		   sizeof(unsigned char)  + // Duration.

   		   sizeof(unsigned long)  + // Reward prize.

		   sizeof(unsigned char)  + // Hunter count.

		   sizeof(unsigned short) + // Target level.
		   sizeof(unsigned char)  + // Kill count.

		   sizeof(unsigned char)  ; // Target name length.

	return static_cast<ptrdiff_t>(unchange + infoLen);
}
