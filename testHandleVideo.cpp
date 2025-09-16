// #include "./include/handleVideo.hpp"
// #include <iostream>
// #include <string>

// using std::cout;
// using std::endl;

// int main(int argc, char* argv[])
// {
//     std::string url = "rtsp://admin:admin@192.168.105.222/live/chn=0";
//     cout << "测试视频流URL: " << url << std::endl;
//     HandleVideo hv(url);
//     cout << "创建HandleVideo对象成功" << std::endl;
//     if (!hv.openVideo()) {
//         cout << "打开视频流失败: " << url << std::endl;
//         return 1;
//     }
//     cout << "打开视频流成功: " << url << std::endl;
//     int frameCount = 0;
//     while (frameCount < 10) { // 只读取10帧做测试
//         if (!hv.readFrame(nullptr)) {
//             cout << "读取帧失败或视频结束" << std::endl;
//             break;
//         }
//         std::cout << "读取到第 " << (frameCount+1) << " 帧" << std::endl;
//         frameCount++;
//     }
//     hv.closeVideo();
//     std::cout << "测试结束" << std::endl;
//     return 0;
// }
