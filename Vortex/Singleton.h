#pragma once

class Singleton
{
public:
	static Singleton& instance()
	{
		// Lazy initialize.
		static Singleton *instance = new Singleton();
		return *instance;
	}

private:
	Singleton() {}
};