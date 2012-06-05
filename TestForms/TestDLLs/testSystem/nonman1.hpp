//#include <cstring>
#include <iostream>
using namespace std;

	// TODO: Add your methods for this class here.
//#pragma unmanaged
//extern "C" __declspec(dllexport) int plugin_main(char *event, void *data)
extern "C" __declspec(dllexport)  UINT DLLFunc1( DWORD one, UINT two )
{
	cout << "dllfunc1";
	return 0;
};
