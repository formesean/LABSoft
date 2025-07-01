#ifndef LABSOFT_PRESENTER_UNIT_H
#define LABSOFT_PRESENTER_UNIT_H

class LAB;
class LABSoft_GUI;
class LABSoft_Presenter;

class LABSoft_Presenter_Unit
{
  protected:
    LABSoft_Presenter& m_presenter;

  public:
    LABSoft_Presenter_Unit (LABSoft_Presenter& _LABSoft_Presenter);

    LAB&                lab       () const;
    LABSoft_GUI&        gui       () const;
    LABSoft_Presenter&  presenter () const;
};

#endif