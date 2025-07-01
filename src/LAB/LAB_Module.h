#ifndef LAB_MODULE_H
#define LAB_MODULE_H

class LAB;
class AikaPi;

class LAB_Module
{
  protected:
    LAB& m_LAB;

  public:
    LAB_Module (LAB& _LAB);

    AikaPi& rpi () const;
    LAB&    lab () const;
};

#endif