#include "StdAfx.h"
#include "Driverfinger.h"
#include "fingercreator.h"

CDriverFinger *IFingerCreator::_pInst = NULL;

void IFingerCreator::CoCreatIntance()
{
	_pInst = new CDriverFinger();
}

void IFingerCreator::ReleaseIntance()
{
	delete _pInst;
	_pInst = NULL;
}

CDriverFinger *IFingerCreator::getIntance()
{
	return _pInst;
}