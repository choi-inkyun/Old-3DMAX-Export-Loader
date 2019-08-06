#include "CIKExport.h"
/*
void CIKExport::CommaScan(TCHAR* buf)
{
    for(; *buf; buf++) if (*buf == ',') *buf = '.';
}

// ������ �ٲ��� //
TSTR CIKExport::Format(int value)
{
	TCHAR buf[50];
	
	memset(buf, NULL, 50);

	sprintf(buf, "%d", value);

	return buf;
}
// y,z ���� �ٲ���. �ƽ��� ������ ��ǥ�� �̱� ������..
TSTR CIKExport::Format(Point3 value)
{
	TCHAR buf[120];

	memset(buf, NULL, 120);
	
	sprintf(buf, "%f %f %f", value.x, value.z, value.y);

	return buf;
}

TSTR CIKExport::Format(Color value)
{
	TCHAR buf[120];

	memset(buf, NULL, 120);
	
	sprintf(buf, "%f %f %f", value.r, value.g, value.b);

	return buf;
}
// ���Լ��� ���� ������..�߸𸣰���.
// �ִϸ��̼� ������ �ʿ���..
TSTR CIKExport::Format(AngAxis value)
{
	TCHAR buf[160];
//	TCHAR fmt[160];
	
//	sprintf(fmt, "%s\t%s\t%s\t%s", szFmtStr, szFmtStr, szFmtStr, szFmtStr);
//	sprintf(buf, fmt, value.axis.x, value.axis.y, value.axis.z, value.angle);
//	sprintf(buf, "%s %s %s %s %f %f %f %f",szFmtStr,szFmtStr,szFmtStr,szFmtStr, value.axis.x,value.axis.y,value.axis.z,value.angle);
	sprintf(buf, "%f %f %f %f", value.axis.x,value.axis.y,value.axis.z,value.angle);

	CommaScan(buf);

	return buf;
}
TSTR CIKExport::Format(Quat value)
{
	// A Quat is converted to an AngAxis before output.
	
	Point3 axis;
	float angle;
	AngAxisFromQ(value, &angle, axis);
	
	return Format(AngAxis(axis, angle));
}

TSTR CIKExport::Format(ScaleValue value)
{
	TCHAR buf[280];
	
	sprintf(buf, "%s %s", Format(value.s), Format(value.q));
	CommaScan(buf);
	return buf;
}

TSTR CIKExport::Format(float value)
{
	TCHAR buf[40];
	
	memset(buf, NULL, 40);

	sprintf(buf, "%f", value);

	CommaScan(buf);
	return TSTR(buf);
}
*/

void CIKExport::CommaScan(TCHAR* buf)
{
    for(; *buf; buf++) if (*buf == ',') *buf = '.';
}

TSTR CIKExport::Format(int value)
{
	TCHAR buf[50];
	
	sprintf(buf, _T("%d"), value);
	return buf;
}


TSTR CIKExport::Format(float value)
{
	TCHAR buf[40];
	
	sprintf(buf, szFmtStr, value);
	CommaScan(buf);
	return TSTR(buf);
}

TSTR CIKExport::Format(Point3 value)
{
	TCHAR buf[120];
	TCHAR fmt[120];
	
	sprintf(fmt, "%s\t%s\t%s", szFmtStr, szFmtStr, szFmtStr);
	sprintf(buf, fmt, value.x, value.y, value.z);
	
	CommaScan(buf);
	return buf;
}

TSTR CIKExport::Format(Color value)
{
	TCHAR buf[120];
	TCHAR fmt[120];
	
	sprintf(fmt, "%s\t%s\t%s", szFmtStr, szFmtStr, szFmtStr);
	sprintf(buf, fmt, value.r, value.g, value.b);
	
	CommaScan(buf);
	return buf;
}

TSTR CIKExport::Format(AngAxis value)
{
	TCHAR buf[160];
	TCHAR fmt[160];
	
	sprintf(fmt, "%s\t%s\t%s\t%s", szFmtStr, szFmtStr, szFmtStr, szFmtStr);
	sprintf(buf, fmt, value.axis.x, value.axis.y, value.axis.z, value.angle);
	
	CommaScan(buf);
	return buf;
}


TSTR CIKExport::Format(Quat value)
{
	// A Quat is converted to an AngAxis before output.
	
	Point3 axis;
	float angle;
	AngAxisFromQ(value, &angle, axis);
	
	return Format(AngAxis(axis, angle));
}

TSTR CIKExport::Format(ScaleValue value)
{
	TCHAR buf[280];
	
	sprintf(buf, "%s %s", Format(value.s), Format(value.q));
	CommaScan(buf);
	return buf;
}
