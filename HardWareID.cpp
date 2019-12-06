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




#ifdef __linux__
// linux

// ---- get os info ---- //
string getOsInfo()
{
    
    FILE *fp = popen("cat /proc/version", "r");
    if(!fp) return "-1";
    char osinfo[1024];
    string osinfo2="";
    fgets(osinfo, sizeof(osinfo), fp);
    osinfo2 += osinfo;
    
    pclose(fp);
    string::size_type index = osinfo2.find("(");
    string ret = osinfo2.substr(0,index);
    return ret;
}

// ---- get cpu info ---- //
string getCpuInfo()
{
    
    FILE *fp = popen("cat /proc/cpuinfo | grep flags", "r");
    if(!fp) return "-1";
    char cpuinfo[1024];
    string cpuinfo2="";
    fgets(cpuinfo, sizeof(cpuinfo), fp);
    cpuinfo2 += cpuinfo;
    
    pclose(fp);
    string::size_type index = cpuinfo2.find(":");
    string ret = cpuinfo2.substr(index+2);
    return ret;
}


// ---- get uuid info ---- //

string getUUIDInfo()
{
    // new way
    FILE *fp = popen("blkid -s UUID", "r");
    if(!fp) return "-1";
    char uuidinfo[1024];
    string uuidinfo2="";
    fgets(uuidinfo, sizeof(uuidinfo), fp);
    uuidinfo2 += uuidinfo;
    
    pclose(fp);
    string::size_type index = uuidinfo2.find(":");
    string ret = uuidinfo2.substr(index+2);
    return ret;
    
}

// ---- get mac info ---- //


char* getMACInfo()
{
    int i=0;
    int sockfd;
    struct ifconf ifc;
    unsigned char buf[512];
    struct ifreq *ifr0;
    char *netName;
    
    //初始化ifconf
    ifc.ifc_len = 512;
    ifc.ifc_buf = (char*)buf;
    
    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0))<0)
    {
        perror("socket");
        exit(1);
    }
    ioctl(sockfd, SIOCGIFCONF, &ifc);
    
    ifr0 = (struct ifreq*)buf;
    for(i=(ifc.ifc_len/sizeof(struct ifreq)); i>0; i--)
    {
        
        netName = ifr0->ifr_name;
        
        // printf("name = [%s]\n", netName);
        
        // printf("local addr = [%s]\n",  inet_ntoa(((struct sockaddr_in*)&(ifr->ifr_addr))->sin_addr));
        ifr0++;
    }
    
    
    
    // get ip, works for linux and mac os, best method
    struct ifaddrs *ifAddrStruct = NULL;
    struct ifaddrs *ifa = NULL;
    
    getifaddrs(&ifAddrStruct);
    
    for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next)
    {
        if (!ifa->ifa_addr)
            continue;
        
        // check if IP4
        if (ifa->ifa_addr->sa_family == AF_INET)
        {
            void *tmpAddrPtr = &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            char local_ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, local_ip, INET_ADDRSTRLEN);
            
            // actually only need external ip
            // printf("%s IP: %s\n", ifa->ifa_name, local_ip);
        }
    }
    
    if (ifAddrStruct)
        freeifaddrs(ifAddrStruct);
    
    
    // get mac, need to create socket first, may not work for mac os
    struct ifreq ifr;
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    
    
    char local_mac[128]={0};
    strcpy(ifr.ifr_name, netName); // only need ethernet card
    if (0 == ioctl(fd, SIOCGIFHWADDR, &ifr))
    {
        char temp_str[10] = { 0 };
        memcpy(temp_str, ifr.ifr_hwaddr.sa_data, 6);
        sprintf(local_mac, "%02x-%02x-%02x-%02x-%02x-%02x", temp_str[0]&0xff, temp_str[1]&0xff, temp_str[2]&0xff, temp_str[3]&0xff, temp_str[4]&0xff, temp_str[5]&0xff);
    }
    
    // printf("Local Mac: %s\n", local_mac);
    return local_mac;
}



void HardWareID()
{
    
    string Linux_HWID="";
    string OsInfo = getOsInfo();
    string cpuinfo = getCpuInfo();
    string uuidinfo = getUUIDInfo();
    string macinfo = getMACInfo();
    Linux_HWID.append(OsInfo);
    Linux_HWID.append(cpuinfo);
    Linux_HWID.append(uuidinfo);
    Linux_HWID.append(macinfo);
    
    //    cout<<Linux_HWID<<endl;
    for(int i=1;i<Linux_HWID.length();i++){
        int key = 0x07;
        Linux_HWID[0] ^= key;
        Linux_HWID[i] ^= Linux_HWID[i-1];
    }
    //    cout<<Linux_HWID<<endl;
    string out = base64_encode((unsigned char const*)Linux_HWID.c_str(), (unsigned int)Linux_HWID.length());
    
    cout<<endl<<"HARDWARE ID : "<<endl<<endl;
    cout<<out<<endl<<endl;
    
}

int main(int argc, const char **argv){
    HardWareID();
    return 0;
}


#endif


#ifdef _WIN32
//define something for Windows (32-bit and 64-bit, this part is common)

// ---- get os info ---- //
string getOsInfo()
{
    
}

// ---- get cpu info ---- //
string getCpuInfo()
{
    
    FILE *fp = popen("wmic cpu get processorid", "r");
    if(!fp) return "-1";
    char cpuinfo[1024];
    string cpuinfo2="";
    fgets(cpuinfo, sizeof(cpuinfo), fp);
    fgets(cpuinfo, sizeof(cpuinfo), fp);
    cpuinfo2 += cpuinfo;
    
    pclose(fp);
//    string::size_type index = cpuinfo2.find(":");
//    string ret = cpuinfo2.substr(index+2);
    return cpuinfo2;
}


// ---- get hw info ---- //

string getHWInfo()
{
    //wmic path Win32_DiskDrive get SerialNumber
    //wmic path win32_physicalmedia get SerialNumber
    FILE *fp = popen("wmic diskdrive get serialnumber", "r");
    if(!fp) return "-1";
    char hwinfo[1024];
    string hwinfo2="";
    fgets(hwinfo, sizeof(hwinfo), fp);
    fgets(hwinfo, sizeof(hwinfo), fp);
    hwinfo2 += hwinfo;
    
    pclose(fp);
//    string::size_type index = hwinfo2.find(":");
//    string ret = hwinfo2.substr(index+2);
    return hwinfo2;
    
    
}

// ---- get mac info ---- //


string getMACInfo()
{
    FILE *fp = popen("wmic nicconfig get macaddress", "r");
    if(!fp) return "-1";
    char macinfo[1024];
    string macinfo2="";
    fgets(macinfo, sizeof(macinfo), fp);
    while (fgets(macinfo, sizeof(macinfo), fp) != NULL) {
        macinfo2 += macinfo;
    }
    
    
    pclose(fp);
    
    return macinfo2;
}

// ---- get baseboard info ---- //


string getbaseboardInfo()
{
    FILE *fp = popen("wmic baseboard get serialnumber", "r");
    if(!fp) return "-1";
    char baseboardinfo[1024];
    string baseboardinfo2="";
    fgets(baseboardinfo, sizeof(baseboardinfo), fp);
    fgets(baseboardinfo, sizeof(baseboardinfo), fp);
    baseboardinfo2 += baseboardinfo;
    
    pclose(fp);
    
    return baseboardinfo2;
    
}

void HardWareID()
{
    
    string Windows_HWID="";
//    string OsInfo = getOsInfo();
    string cpuinfo = getCpuInfo();
    string hwinfo = getHWInfo();
    string macinfo = getMACInfo();
    string baseboardinfo = getbaseboardInfo();
    
//    Windows_HWID.append(OsInfo);
    Windows_HWID.append(cpuinfo);
    Windows_HWID.append(hwinfo);
    Windows_HWID.append(macinfo);
    Windows_HWID.append(baseboardinfo);
    
    
//        cout<<Windows_HWID<<endl;
    for(int i=1;i<Windows_HWID.length();i++){
        int key = 0x07;
        Windows_HWID[0] ^= key;
        Windows_HWID[i] ^= Windows_HWID[i-1];
    }
//    cout<<Windows_HWID<<endl;
    string out = base64_encode((unsigned char const*)Windows_HWID.c_str(), (unsigned int)Windows_HWID.length());
    
    cout<<endl<<"HARDWARE ID : "<<endl<<endl;
    cout<<out<<endl<<endl;
    
//    string deout = base64_decode(out);
//    cout<<deout<<endl<<endl;
    
}


int main(int argc, const char **argv){
    HardWareID();
    return 0;
}




#ifdef _WIN64
//define something for Windows (64-bit only)
#endif

#endif
