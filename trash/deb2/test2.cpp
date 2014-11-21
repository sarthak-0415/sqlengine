#include <iostream>
#include <string>
#include <utility>
 
int main(int argc, char* argv[])
{
        std::string s1 = "lowertoupper";
        std::string s2 = "UPPERTOLOWER";
 
        std::cout << s1 << " : ";
        std::transform(s1.begin(), s1.end(), s1.begin(), ::toupper);
        std::cout << s1 << std::endl;
 
        std::cout << s2 << " : ";
        std::transform(s2.begin(), s2.end(), s2.begin(), ::tolower);
        std::cout << s2 << std::endl;
 
        return 0;
}