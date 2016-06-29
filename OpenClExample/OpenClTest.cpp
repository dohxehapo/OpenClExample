#include "stdafx.h"
#include "OpenClTest.h"


void OpenClTest::Run()
{
	double time;

	std::cout << "Test started." << std::endl;

	GenerateTestData();
	std::cout << "Test data generated." << std::endl;

	time = CalculateCpu();
	std::cout << "Cpu: " << time << " sec." << std::endl;

	time = CalculateGpu();
	std::cout << "Gpu: " << time << " sec." << std::endl;

	time = CalculateGpuNoSend();
	std::cout << "Gpu: " << time << " sec. (no buffers exchanges)" << std::endl;

	std::cout << "Press enter to exit." << std::endl;
	std::getchar();
}


void OpenClTest::GenerateTestData()
{
	m_src1.resize(m_dataSize);
	m_src2.resize(m_dataSize);
	m_dest.resize(m_dataSize, 0);

	for (int i = 0; i < m_dataSize; ++i)
	{
		m_src1[i] = rand() % 256;
		m_src2[i] = rand() % 256;
	}
}


double OpenClTest::CalculateCpu()
{
	m_timer.Start();

	for (int i = 0; i < m_dataSize; ++i)
	{
		m_dest[i] = m_src1[i] + m_src2[i];
		if (m_dest[i] >= 256)
			m_dest[i] -= 256;
	}

	return m_timer.Check();
}


double OpenClTest::CalculateGpu()
{
	// Find the first available device to process on

	std::vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);
	if (platforms.size() < 1)
		throw "No CL platforms found.";
	std::vector<cl::Device> devices;
	platforms[0].getDevices(CL_DEVICE_TYPE_GPU, &devices);
	if (devices.size() < 1)
		throw "No CL devices found.";
	cl::Device device = devices[0];

	// Create a context for the selected device

	std::vector<cl::Device> contextDevices;
	contextDevices.push_back(device);
	cl::Context context = cl::Context(contextDevices);

	// Create a context and command queue for the selected device

	cl::CommandQueue queue = cl::CommandQueue(context, device);

	// Create buffers

	cl::Buffer clSrc1 = cl::Buffer(context, CL_MEM_READ_ONLY, m_dataSize * sizeof(int));
	cl::Buffer clSrc2 = cl::Buffer(context, CL_MEM_READ_ONLY, m_dataSize * sizeof(int));
	cl::Buffer clDest = cl::Buffer(context, CL_MEM_WRITE_ONLY, m_dataSize * sizeof(int));

	// Load OpenCL source code

	std::ifstream sourceFile("Calc.cl");
	std::string sourceCode(std::istreambuf_iterator<char>(sourceFile), (std::istreambuf_iterator<char>()));

	// Build OpenCL program and create the kernel

	cl::Program::Sources source(1, std::make_pair(sourceCode.c_str(), sourceCode.length() + 1));
	cl::Program program = cl::Program(context, source);
	program.build(contextDevices);
	cl::Kernel kernel = cl::Kernel(program, "Calculate");

	// Calculations

	m_timer.Start();

	// Fill input buffers

	queue.enqueueWriteBuffer(clSrc1, CL_FALSE, 0, m_dataSize * sizeof(int), m_src1.data());
	queue.enqueueWriteBuffer(clSrc2, CL_FALSE, 0, m_dataSize * sizeof(int), m_src2.data());

	// Set kernel args

	int arg = 0;
	kernel.setArg(arg++, clSrc1);
	kernel.setArg(arg++, clSrc2);
	kernel.setArg(arg++, clDest);
	kernel.setArg(arg++, m_dataSize);

	// Call calc function

	queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(m_dataSize), cl::NullRange);

	// Read output buffer

	queue.enqueueReadBuffer(clDest, CL_FALSE, 0, m_dataSize * sizeof(int), m_dest.data());

	// Wait for the queue to finish

	queue.finish();

	return m_timer.Check();
}


double OpenClTest::CalculateGpuNoSend()
{
	// Find the first available device to process on

	std::vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);
	if (platforms.size() < 1)
		throw "No CL platforms found.";
	std::vector<cl::Device> devices;
	platforms[0].getDevices(CL_DEVICE_TYPE_GPU, &devices);
	if (devices.size() < 1)
		throw "No CL devices found.";
	cl::Device device = devices[0];

	// Create a context for the selected device

	std::vector<cl::Device> contextDevices;
	contextDevices.push_back(device);
	cl::Context context = cl::Context(contextDevices);

	// Create a context and command queue for the selected device

	cl::CommandQueue queue = cl::CommandQueue(context, device);

	// Create buffers

	cl::Buffer clSrc1 = cl::Buffer(context, CL_MEM_READ_ONLY, m_dataSize * sizeof(int));
	cl::Buffer clSrc2 = cl::Buffer(context, CL_MEM_READ_ONLY, m_dataSize * sizeof(int));
	cl::Buffer clDest = cl::Buffer(context, CL_MEM_WRITE_ONLY, m_dataSize * sizeof(int));

	// Load OpenCL source code

	std::ifstream sourceFile("Calc.cl");
	std::string sourceCode(std::istreambuf_iterator<char>(sourceFile), (std::istreambuf_iterator<char>()));

	// Build OpenCL program and create the kernel

	cl::Program::Sources source(1, std::make_pair(sourceCode.c_str(), sourceCode.length() + 1));
	cl::Program program = cl::Program(context, source);
	program.build(contextDevices);
	cl::Kernel kernel = cl::Kernel(program, "Calculate");

	// Calculations

	m_timer.Start();

	// Fill input buffers

	queue.enqueueWriteBuffer(clSrc1, CL_TRUE, 0, m_dataSize * sizeof(int), m_src1.data());
	queue.enqueueWriteBuffer(clSrc2, CL_TRUE, 0, m_dataSize * sizeof(int), m_src2.data());

	// Set kernel args

	int arg = 0;
	kernel.setArg(arg++, clSrc1);
	kernel.setArg(arg++, clSrc2);
	kernel.setArg(arg++, clDest);
	kernel.setArg(arg++, m_dataSize);

	// Call calc function

	m_timer.Start();
	queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(m_dataSize), cl::NullRange);
	queue.finish();
	double time = m_timer.Check();

	// Read output buffer

	queue.enqueueReadBuffer(clDest, CL_TRUE, 0, m_dataSize * sizeof(int), m_dest.data());

	return time;
}
