#pragma once

class ExampleObject
{
public:
	ExampleObject();
	~ExampleObject();

	void	setValue(int value);
	int		getValue();

	int		publicMember;

private:
	int mValue;
};
