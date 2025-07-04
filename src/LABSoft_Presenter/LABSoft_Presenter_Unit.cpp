#include "LABSoft_Presenter_Unit.h"

#include "LABSoft_Presenter.h"

LABSoft_Presenter_Unit:: 
LABSoft_Presenter_Unit (LABSoft_Presenter& _LABSoft_Presenter)
  : m_presenter (_LABSoft_Presenter)
{

}

LAB& LABSoft_Presenter_Unit:: 
lab () const
{
  return (m_presenter.lab ());
}

LABSoft_GUI& LABSoft_Presenter_Unit:: 
gui () const
{
  return (m_presenter.gui ());
}

LABSoft_Presenter& LABSoft_Presenter_Unit:: 
presenter () const
{
  return (m_presenter);
}