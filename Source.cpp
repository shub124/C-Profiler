#pragma once

#include<iostream>
#include <string>
#include <chrono>
#include <fstream>
#include <algorithm>


struct ProfileResult
{
	const char* Name;
	long long start, end;
};

struct InstrumentationSession
{
	const char* name;
};

class Instrumentor
{
private:
	InstrumentationSession* session;
	bool Profile_start;
	std::ofstream m_OutputStream;
public:
	Instrumentor()
		:session(nullptr), Profile_start(false)
	{

	}
	
	void BeginSession(const char* name, const std::string& filepath = "benchmark.json")
	{
		m_OutputStream.open(filepath);
		WriteHeader();
		session = new InstrumentationSession{ name };


	}

	void EndSession()
	{
		WriteFooter();
		m_OutputStream.close();
		delete session;
		session = nullptr;
		
	}

	void WriteHeader()
	{
		m_OutputStream << "{\"otherData\": {},\"traceEvents\":[";
		m_OutputStream.flush();
	}

	void WriteFooter()
	{
		m_OutputStream << "]}";
		m_OutputStream.flush();
	}

	static Instrumentor& Get()
	{
		static Instrumentor instance;
		return instance;
	}

	void WriteProfile(const ProfileResult & result)
	{
		if (Profile_start ==true)
		{
			m_OutputStream << ',';
		}
		else if (Profile_start == false)
		{
			Profile_start = true;
		}
		std::string name = result.Name;
		std::replace(name.begin(), name.end(), '"', '\'');
		m_OutputStream << "{";
		m_OutputStream << "\"cat\":\"function\",";
		m_OutputStream << "\"dur\":" << ((long long)result.end -(long long) result.start) << ',';
		m_OutputStream << "\"name\":\"" << name << "\",";
		m_OutputStream << "\"ph\":\"X\",";
		m_OutputStream << "\"pid\":0,";
		m_OutputStream << "\"ts\":" << result.start<< ',';
		m_OutputStream << "\"es\":" << result.end;
		m_OutputStream << "}";

		m_OutputStream.flush();
	}

};


class Timer
{
public:
	Timer(const char* name)
		:m_Stopped(false), m_Name(name)
	{
		start = std::chrono::high_resolution_clock::now();

	}

	~Timer()
	{
		if (!m_Stopped)
			Stop();
	}

	void Stop()
	{
		end = std::chrono::high_resolution_clock::now();
		long long st = std::chrono::time_point_cast<std::chrono::microseconds>(start).time_since_epoch().count();
		long long  en = std::chrono::time_point_cast<std::chrono::microseconds>(end).time_since_epoch().count();
		Instrumentor::Get().WriteProfile({ m_Name, st, en});
		m_Stopped = true;


	}

private:
	bool m_Stopped;
	const char* m_Name;
	std::chrono::time_point<std::chrono::high_resolution_clock> start;
	std::chrono::time_point<std::chrono::high_resolution_clock> end;
	
};

#define PROFILING 1
#if PROFILING
#define FUNC(name) Timer timer(name)
#define SCOPE() FUNC(__FUNCTION__)
#else
#define FUNC(name) 
#endif




void func1()
{
	SCOPE();
	long long  p=0;
	for (int i = 0; i < 1000; i++)
	{
		std::cout << "Hello Worls" <<std:: endl;
	}
}
void func2()
{
	SCOPE();
	long long p = 2;
	for (int i = 0; i < 1003; i++)
	{
		std::cout << "Hello Dear" << std:: endl;
		p++; p++;
	}

}

int main()
{
	Instrumentor::Get().BeginSession("Profiling","Hello_9.json");
	func1();
	func2();
	Instrumentor::Get().EndSession();

}

