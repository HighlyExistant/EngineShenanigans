#include <iostream>
int main(int argc, char const *argv[])
{
    int currentFrameIndex = 0;

    while (true)
    {
        //currentFrameIndex = (currentFrameIndex + 1) % 2;
        currentFrameIndex = (currentFrameIndex == 0) ? 1 : 0;
        std::cout << "current frame:\t" << currentFrameIndex << '\n';
    }
    
    return 0;
}
