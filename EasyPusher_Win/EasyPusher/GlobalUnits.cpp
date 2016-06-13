/*
	Copyright (c) 2013-2014 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.EasyDarwin.org
*/
#include "StdAfx.h"
#include "GlobalUnits.h"

CGlobalUnits::CGlobalUnits(void)
{
	m_szDefaultSkin[0]=0;
	m_bLeftDrawExtrude = false;
}

CGlobalUnits::~CGlobalUnits(void)
{

}

CGlobalUnits* CGlobalUnits::GetInstance()
{
	static CGlobalUnits _Instance;

	return &_Instance;
}
