#pragma once

#include "BoatCommand.h"

class MainsheetCommand : public BoatCommand
{
  Real m_value;

public:
  MainsheetCommand(Boat* pBoat, Real value)
    : BoatCommand(pBoat),
      m_value(value)
  {
  }

  void Execute();
};