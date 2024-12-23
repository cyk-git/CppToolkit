/*
 * qt_compatibility.h
 *
 * Created on 20240530
 *   by Yukun Cheng
 *   cyk_phy@mail.ustc.edu.cn
 *
 * This header manages Qt compatibility by ensuring that the 'slots' macro,
 * which can potentially conflict with third-party libraries, is handled correctly.
 * It provides mechanisms to temporarily undefine and restore the 'slots' macro,
 * thus avoiding conflicts when including headers from libraries such as Python.
 *
 * Protected Macros:
 * - 'slots': This macro is temporarily undefined and then restored to prevent
 *   conflicts with Qt's signal-slot mechanism.
 *
 * Unprotected Macros:
 * - This file does not handle other Qt-specific macros like 'signals' or
 *   any macros from third-party libraries that might conflict with Qt or
 *   other libraries. Users should handle other potential macro conflicts
 *   separately.
 */

//#ifndef CPPTOOLKIT_QT_COMPATIBILITY_H_
//#define CPPTOOLKIT_QT_COMPATIBILITY_H_
// Macro to save the slots status and undefine if necessary
#ifdef slots
#define CPPTOOLKIT_END_QT_COMPATIBILITY \
    #define slots Q_SLOTS
#define CPPTOOLKIT_BEGIN_QT_COMPATIBILITY \
    #undef slots 
#else 
#define CPPTOOLKIT_BEGIN_QT_COMPATIBILITY
#define CPPTOOLKIT_END_QT_COMPATIBILITY
#endif

//// At the end of the translation unit, check if the block was closed
//#ifdef CPPTOOLKIT_MUST_USE_END_QT_COMPATIBILITY
//#error "Missing CPPTOOLKIT_END_QT_COMPATIBILITY after CPPTOOLKIT_BEGIN_QT_COMPATIBILITY"
//#endif

//#endif // CPPTOOLKIT_QT_COMPATIBILITY_H_
