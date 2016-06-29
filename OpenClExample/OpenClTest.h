#pragma once

#ifndef OPENCLTEST_H
#define OPENCLTEST_H


#include "Timer.h"


class OpenClTest
{
public:

	void Run();

private:

	const int m_dataSize = 2 << 25; // ~32 MB

	std::vector<int> m_src1;
	std::vector<int> m_src2;
	std::vector<int> m_dest;

	Timer m_timer;

	void GenerateTestData();

	double CalculateCpu();
	double CalculateGpu();
	double CalculateGpuNoSend();

};


#endif // OPENCLTEST_H
