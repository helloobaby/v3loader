
#include<iostream>
#include<filesystem>
#include<fstream>
#include<Windows.h>

#include "./Blackbone/src/BlackBone/Process/Process.h"

using namespace std;

namespace fs = std::filesystem;

ifstream fix1;
ifstream fix2;
ifstream otc;

char* fix1_buffer;
char* fix2_buffer;
char* otc_buffer;



int main()
{
    NTSTATUS Status = STATUS_SUCCESS;

    fs::path current_path = fs::canonical(".\\");

    fix1.open((current_path / "fix1").c_str());
    fix2.open((current_path / "fix2").c_str());
    otc.open((current_path / "otc.dll").c_str());

    cout << hex;

    if (!fix1.is_open())
        return -1;

    if (!fix2.is_open())
        return -1;

    if (!otc.is_open())
        return -1;

    int fix1_size = fs::file_size(current_path / "fix1");
    int fix2_size = fs::file_size(current_path / "fix2");
    int otc_size = fs::file_size(current_path / "otc.dll");


#ifdef DBG
    cout << "fix1 size " << fix1_size << endl;
    cout << "fix2 size " << fix2_size << endl;
    cout << "otc size " << otc_size << endl;
#endif // DBG

    fix1_buffer = new char[fix1_size];
    fix2_buffer = new char[fix2_size];
    otc_buffer = new char[otc_size];

    fix1.read(fix1_buffer, fix1_size);
    fix2.read(fix2_buffer, fix2_size);
    otc.read(otc_buffer, otc_size);


    blackbone::Process csgo;
    Status = csgo.Attach(L"csgo.exe");

    if (!NT_SUCCESS(Status))
    {
        cout << "[ERROR]Game Not Run" << endl;
        getchar();
        return -1;
    }

    blackbone::ProcessMemory& mm = csgo.memory();
    blackbone::ProcessThreads& threads = csgo.threads();

    auto remote_fix1_memory_block = mm.Allocate(fix1_size);
    auto fix1_remote_address = remote_fix1_memory_block.result().ptr();

    auto remote_fix2_memory_block = mm.Allocate(fix2_size);
    auto fix2_remote_address = remote_fix2_memory_block.result().ptr();

    auto remote_otc_memory_block = mm.Allocate(otc_size);
    auto otc_remote_address = remote_otc_memory_block.result().ptr();


    if (NT_SUCCESS(remote_fix1_memory_block.success()))
        cout << "[INFO]fix1_remote_address 0x" << fix1_remote_address << endl;
    else
        return -1;

    if (NT_SUCCESS(remote_fix2_memory_block.success()))
        cout << "[INFO]fix2_remote_address 0x" << fix2_remote_address << endl;
    else
        return -1;
    
    if (NT_SUCCESS(remote_otc_memory_block.success()))
        cout << "[INFO]otc_remote_address 0x" << otc_remote_address << endl;
    else
        return -1;


    mm.Write(otc_remote_address, otc_size, otc_buffer);
    
    //DEBUG
    //getchar();

    mm.Write(otc_remote_address, fix1_size, fix1_buffer);
    
    //DEBUG
    //getchar();

    mm.Write(fix2_remote_address, fix2_size, fix2_buffer);

    getchar();

    threads.CreateNew(fix2_remote_address, otc_remote_address);
    


    mm.Free(fix1_remote_address);
    mm.Free(fix2_remote_address);
    mm.Free(otc_remote_address);

    getchar();

    fix1.close();
    fix2.close();
    otc.close();
    return 0;
}

