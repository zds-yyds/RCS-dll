// src/HYSimulationExport.h
#pragma once

#ifdef _WIN32
    #ifdef HYSimulation_EXPORTS
        #define HYSimulation_API __declspec(dllexport)
    #else
        #define HYSimulation_API __declspec(dllimport)
    #endif
#else
    #define HYSimulation_API __attribute__((visibility("default")))
#endif