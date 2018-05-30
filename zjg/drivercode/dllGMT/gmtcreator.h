class CGmtDriver;

class IGMTCreator
{
protected:
	static CGmtDriver *_pInst;
public:
	static void CoCreatIntance();
	static void ReleaseIntance();
	static CGmtDriver *getIntance();
};