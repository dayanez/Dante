#include <windows.h> 

double GetCpuUsage(){
	FILETIME ftCreation, ftExit, ftKernel, ftUser;
	GetProcessTimes(GetCurrentProcess(), &ftCreation, &ftExit, &ftKernel, &ftUser);



	ULARGE_INTEGER ulKernel, ulUser;
	ulKernel.LowPart = ftKernel.dwLowDateTime;
	ulKernel.HighPart = ftKernel.dwHighDateTime;
	ulUser.LowPart = ftUser.dwLowDateTime; 
	ulUser.HighPart = ftUser.dwHighDateTime;


	//total cpu time 
	double processTime = (ulKernel.QuadPart + ulUser.QuadPart) / 10000.0;



	FILETIME ftIdle, ftKernelSys, ftUserSys;
	GetSystemTimes(&ftIdle, &ftKernelSys, &ftUserSys);


	ULARGE_INTEGER ulKernelSys, ulUserSys; 
	ulKernelSys.LowPart = ftKernelSys.dwLowDateTime; 
	ulKernelSys.HighPart = ftKernelSys.dwHighDatetime;
	ulUserSys.LowPart = ftUserSys.dwLowDateTime; 
	ulUserSys.HighPart = ftUserSys.dwHighDateTime;

	//Total CPU time used by the system
	double systemTime = (ulKernelSys.QuadPart + ulUserSys.QuadPart) / 10000.0;

	return (processTime / systemTime) * 100; //cpu usage percent
	
	
} 
