//
//  main.cpp
//  HWID_ALL
//
//  Created by 秦耀 on 2019/12/4.
//  Copyright © 2019 秦耀. All rights reserved.
//

#include "HardWareID.h"

#ifdef __APPLE__
// mac can use cmdline 'system_profiler -listDataTypes' to get everything

string getOSInfo(){
    FILE *fp = popen("sw_vers", "r");
    if(!fp) return "-1";
    char osinfo[1024];
    string osinfo2="";
    while (fgets(osinfo, sizeof(osinfo), fp) != NULL) {
//        cout<<osinfo<<endl;
        osinfo2 += osinfo;
    }
    pclose(fp);
    return osinfo2;
    
    
}

// get disk uuid
string getDISKInfo(){
    
    FILE *fp = popen("system_profiler SPStorageDataType |grep UUID", "r");
    if(!fp) return "-1";
    char diskinfo[1024];
    string diskinfo2="";
    fgets(diskinfo, sizeof(diskinfo), fp);
    diskinfo2 += diskinfo;
//    while (fgets(diskinfo, sizeof(diskinfo), fp) != NULL) {
////        cout<<diskinfo<<endl;
//        diskinfo2 += diskinfo;
//    }
    pclose(fp);
    string::size_type index = diskinfo2.find(":");
    string ret = diskinfo2.substr(index+2);
    return ret;
}

// get mac
string getMACInfo(){
    
    FILE *fp = popen("system_profiler SPNetworkDataType | grep MAC", "r");
    if(!fp) return "-1";
    char macinfo[1024];
    string macinfo2="";
    fgets(macinfo, sizeof(macinfo), fp);
    macinfo2 += macinfo;
    //    while (fgets(macinfo, sizeof(macinfo), fp) != NULL) {
    ////        cout<<macinfo<<endl;
    //        macinfo2 += macinfo;
    //    }
    pclose(fp);
    string::size_type index = macinfo2.find(":");
    string ret = macinfo2.substr(index+2);
    return ret;
}

// get Camera ID
string getCameraInfo(){
    
    FILE *fp = popen("system_profiler SPCameraDataType | grep Unique", "r");
    if(!fp) return "-1";
    char Camera[1024];
    string Camera2="";
    fgets(Camera, sizeof(Camera), fp);
    Camera2 += Camera;
    //    while (fgets(Camera, sizeof(Camera), fp) != NULL) {
    ////        cout<<Camera<<endl;
    //        Camera2 += Camera;
    //    }
    pclose(fp);
    string::size_type index = Camera2.find(":");
    string ret = Camera2.substr(index+2);
    return ret;
}

// get cpu
string getCPUInfo(){
    
    FILE *fp = popen("sysctl -n machdep.cpu.features", "r");
    if(!fp) return "-1";
    char cpu[1024];
    string cpu2="";
    fgets(cpu, sizeof(cpu), fp);
    cpu2 += cpu;

    pclose(fp);
    return cpu2;
}

void HardWareID() {
    // insert code here...
//    string osinfo = getOSInfo();
//    cout<<osinfo<<endl;
    string MAC_HWID = "";
    string diskinfo = getDISKInfo();
    string macinfo = getMACInfo();
    string cpuinfo = getCPUInfo();
    string camerainfo = getCameraInfo();
    
    MAC_HWID.append(diskinfo);
    MAC_HWID.append(macinfo);
    MAC_HWID.append(cpuinfo);
    MAC_HWID.append(camerainfo);
//    cout<<MAC_HWID<<endl;
    for(int i=1;i<MAC_HWID.length();i++){
        int key = 0x07;
        MAC_HWID[0] ^= key;
        MAC_HWID[i] ^= MAC_HWID[i-1];
    }
//    cout<<MAC_HWID<<endl;
    string out = base64_encode((unsigned char const*)MAC_HWID.c_str(), (unsigned int)MAC_HWID.length());
    cout<<"HARDWARE ID : "<<endl<<endl;
    cout<<out<<endl<<endl;

}

int main(int argc, const char * argv[]){
    HardWareID();
    return 0;
}
#endif



#ifdef _WIN32
    //define something for Windows (32-bit and 64-bit, this part is common)
    #ifdef _WIN64
    //define something for Windows (64-bit only)
    #endif

#endif

#ifdef __linux__
    // linux


#endif
