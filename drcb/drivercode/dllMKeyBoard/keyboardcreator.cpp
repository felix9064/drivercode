#include "StdAfx.h"
#include "DriverMKeyBoard.h"
#include "keyboardcreator.h"

CDriverMKeyBd *IMKeyBdCreator::_pInst = NULL;

void IMKeyBdCreator::CoCreatIntance()
{
	_pInst = new CDriverMKeyBd();
}

void IMKeyBdCreator::ReleaseIntance()
{
	delete _pInst;
	_pInst = NULL;
}

CDriverMKeyBd *IMKeyBdCreator::getIntance()
{
	return _pInst;
}