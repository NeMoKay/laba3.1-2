#include "VirtualFS.hpp"





// int main(){

//     VirtualFS finder;
//     finder.mkdir("n1");
//     finder.cd("n1");
//     finder.mkdir("n2");
//     finder.cd("..");
//     std::vector<VirtualFS::FSNode> bob = finder.ls();
//     for(auto i : bob){
//         std::cout << i.name << '\n';
//     }

//     return 0;
// }

int main(){
    VirtualFS fs;

    fs.mkdir("/directory");
    fs.mkdir("/directory/director2");

    fs.touch("/directory/text.txt");
    fs.touch("/directory/text1.txt");
    fs.touch("/directory/director2/text3.txt");

    std::cout << fs.cat("/directory/director2/text3.txt");

    return 0;
}