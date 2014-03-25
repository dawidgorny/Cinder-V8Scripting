#include "ExampleObject.h"


ExampleObject::ExampleObject()
{

}

ExampleObject::~ExampleObject()
{

}

void ExampleObject::setValue(int value)
{
	mValue = value;
}

int ExampleObject::getValue()
{
	return mValue;
}