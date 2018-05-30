#include "StdAfx.h"
#include "GmtDriver.h"
#include "gmtcreator.h"

CGmtDriver *IGMTCreator::_pInst = NULL;

void IGMTCreator::CoCreatIntance()
{
	_pInst = new CGmtDriver();
}

void IGMTCreator::ReleaseIntance()
{
	delete _pInst;
	_pInst = NULL;
}

CGmtDriver *IGMTCreator::getIntance()
{
	return _pInst;
}