class CDriverFinger;

class IFingerCreator
{
protected:
	static CDriverFinger *_pInst;
public:
	static void CoCreatIntance();
	static void ReleaseIntance();
	static CDriverFinger *getIntance();
};
