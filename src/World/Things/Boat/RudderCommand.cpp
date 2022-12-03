#include "stdafx.h"
#include "Boat.h"
#include "Rudder.h"
#include "RudderCommand.h"

void RudderCommand::Execute()
{
  m_pBoat->Rudder.AngularSpeed = m_value;
}