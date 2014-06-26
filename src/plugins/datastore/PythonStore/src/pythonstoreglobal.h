#ifndef PYTHONSTOREGLOBAL_H
#define PYTHONSTOREGLOBAL_H

#ifdef _WIN32
    #ifdef PythonStore_PLUGIN
        #define PYTHONSTORE_EXPORT __declspec(dllexport)
    #else
        #define PYTHONSTORE_EXPORT __declspec(dllimport)
    #endif
#else
    #define PYTHONSTORE_EXPORT
#endif

#endif
