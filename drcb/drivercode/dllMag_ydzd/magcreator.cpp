#include "StdAfx.h"
#include "driverMag.h"
#include "magcreator.h"

CDriverMag *IMagCreator::_pInst = NULL;

void IMagCreator::CoCreatIntance()
{
	_pInst = new CDriverMag();
}

void IMagCreator::ReleaseIntance()
{
	delete _pInst;
	_pInst = NULL;
}

CDriverMag *IMagCreator::getIntance()
{
	return _pInst;
}