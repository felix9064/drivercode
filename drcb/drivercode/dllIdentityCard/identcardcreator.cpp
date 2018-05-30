#include "StdAfx.h"
#include "DriverIdentCard.h"
#include "identcardcreator.h"

CDriverIdentCard *IIdentCardCreator::_pInst = NULL;

void IIdentCardCreator::CoCreatIntance()
{
	_pInst = new CDriverIdentCard();
}

void IIdentCardCreator::ReleaseIntance()
{
	delete _pInst;
	_pInst = NULL;
}

CDriverIdentCard *IIdentCardCreator::getIntance()
{
	return _pInst;
}