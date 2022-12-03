#pragma once

#include "BoatCommand.h"

class RudderCommand : public BoatCommand
{
  Real m_value;

public:
  RudderCommand(Boat* pBoat, Real value)
    : BoatCommand(pBoat),
      m_value(value)
  {
  }

  void Execute();
};