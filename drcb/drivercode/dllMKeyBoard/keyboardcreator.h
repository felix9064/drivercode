class CDriverMKeyBd;

class IMKeyBdCreator
{
protected:
	static CDriverMKeyBd *_pInst;
public:
	static void CoCreatIntance();
	static void ReleaseIntance();
	static CDriverMKeyBd *getIntance();
};

