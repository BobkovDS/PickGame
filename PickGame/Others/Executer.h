#pragma once
class ExecuterBase
{
public:
	virtual void execute() = 0;
	virtual void execute(int p1) = 0;
	virtual void execute(int p1, int p2) = 0;
	virtual ~ExecuterBase() {}
};

template<class ClassName>
class ExecuterVoidVoid : public ExecuterBase // executer with void excute(void) function
{
	using ptrVoidFnRA = void(ClassName::*)();
	ptrVoidFnRA ptr;
	ClassName* parent;

	void execute(int) {}; // not used
	void execute(int, int) {}; // not used
public:
	ExecuterVoidVoid(ClassName* iparent, ptrVoidFnRA iptr) : parent(iparent), ptr(iptr)
	{
		assert(parent);
		assert(ptr);
	}

	void execute()
	{
		(parent->*ptr)();
	}	
};

template<class ClassName>
class ExecuterVoidInt: public ExecuterBase // executer with void excute(UINT) function
{
	using ptrVoidFnRA = void(ClassName::*)(int p1);
	ptrVoidFnRA ptr;
	ClassName* parent;

	void execute() {}; // not used
	void execute(int, int) {}; // not used
public:
	ExecuterVoidInt(ClassName* iparent, ptrVoidFnRA iptr) : parent(iparent), ptr(iptr)
	{
		assert(parent);
		assert(ptr);
	}

	void execute(int p1)
	{
		(parent->*ptr)(p1);
	}
};

template<class ClassName>
class ExecuterVoidIntInt : public ExecuterBase // executer with void excute(UINT, UINT) function
{
	using ptrVoidFnRA = void(ClassName::*)(int p1, int p2);
	ptrVoidFnRA ptr;
	ClassName* parent;

	void execute() {}; // not used
	void execute(int) {}; // not used

public:
	ExecuterVoidIntInt(ClassName* iparent, ptrVoidFnRA iptr) : parent(iparent), ptr(iptr)
	{
		assert(parent);
		assert(ptr);
	}

	void execute(int p1, int p2)
	{
		(parent->*ptr)(p1, p2);
	}
};