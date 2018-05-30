class CDriverMag;

class IMagCreator
{
protected:
	static CDriverMag *_pInst;
public:
	static void CoCreatIntance();
	static void ReleaseIntance();
	static CDriverMag *getIntance();
};