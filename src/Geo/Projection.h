#pragma once

struct DataSetPoint;

class MapProjection
{
public:
	virtual ~MapProjection(){};
	virtual void operator()(DataSetPoint& p) = 0;
};