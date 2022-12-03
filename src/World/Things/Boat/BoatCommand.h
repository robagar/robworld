#pragma once

#include "Commands.h"

class BoatCommand : public Command
{
protected:
  class Boat* m_pBoat;

public:

  BoatCommand(Boat* pBoat)
    : m_pBoat(pBoat)
  {
  }

};