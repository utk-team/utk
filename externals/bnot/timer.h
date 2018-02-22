#ifndef _TIMER_H_
#define _TIMER_H_

// STL
#include <ctime>
#include <vector>
#include <string>
#include <iostream>

// local
#include "console_color.h"

class Timer
{
public:
    static
    void start_timer(std::vector<double>& timer,
                     const int color, 
                     const std::string msg)
    {
        set_color(color);
        std::cout << msg << white << " ... ";
        timer.push_back(clock());
    }

    static
    void stop_timer(std::vector<double>& timer, 
                    const int color,
                    const std::string msg = std::string())
    {
        double duration = time_duration(timer.back());
        timer.pop_back();
        set_color(color);
        std::cout << "done" << white << " (" << duration << " s) " << msg << std::endl;
    }

    static
    void set_color(const int color)
    {
        switch (color)
        {
            case COLOR_WHITE:
                std::cout << white;
                break;
            case COLOR_RED:
                std::cout << red;
                break;
            case COLOR_BLUE:
                std::cout << blue;
                break;
            case COLOR_GREEN:
                std::cout << green;
                break;
            case COLOR_YELLOW:
                std::cout << yellow;
                break;
        }
    }

    static
    double time_duration(const double init)
    {
        return (clock() - init) / CLOCKS_PER_SEC;
    }
};

#endif
