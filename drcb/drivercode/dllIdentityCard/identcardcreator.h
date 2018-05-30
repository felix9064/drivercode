class CDriverIdentCard;

class IIdentCardCreator
{
protected:
	static CDriverIdentCard *_pInst;
public:
	static void CoCreatIntance();
	static void ReleaseIntance();
	static CDriverIdentCard *getIntance();
};