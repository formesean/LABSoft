#ifndef LABSOFT_H
#define LABSOFT_H

#include "LAB/LAB.h"
#include "LABSoft_GUI/LABSoft_GUI.h"
#include "LABSoft_Presenter/LABSoft_Presenter.h"

class LABSoft
{
private:
  LAB m_LAB;
  LABSoft_GUI m_LABSoft_GUI;
  LABSoft_Presenter m_LABSoft_Presenter;

public:
  LABSoft();
  ~LABSoft();
};

#endif