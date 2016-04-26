/*
 * (c) 2015- Hourglass Resurrection Team
 * Hourglass Resurrection is licensed under GPL v2.
 * Refer to the file COPYING.txt in the project root.
 */

#pragma once

#include <Windows.h>

#include <map>
#include <memory>
#include <set>
#include <utility>
#include <vector>

#include <print.h>

/*
 * Memory Manager
 * --------------
 * This is a manager of dynamic allocations that allows us to have full control of the heap.
 * This manager uses a best fit allocation model to not waste more RAM than necessary, and it
 * allocates all the objects as Shared Memory Objects. Using Shared Memory Objects allows for easy
 * access from the executable, in order to have an effective RAM Search.
 * Another benefit to Shared Memory Objects is that it becomes easier to manage save states with
 * these over regular RAM pages as we can have full READ / WRITE access from the executable side,
 * as well as shared ownership.
 *
 * There is a downside to this approach, and that is if the games use any libc, libc++ or STL
 * contaners in it's own code, as there will be 2 layers doing the same thing, which means both the
 * game and we will be keeping a memory map, wasting a bit of RAM. This is unavoidable.
 * Microsoft also opted for enforcing an Allocation Granularity that may be much larger than the
 * page size in the system. This is to be fully compatible with the exotic Alpha AVX RISC CPU.
 * The compatibility with this CPU puts extra strain on this implementation, as it becomes extra
 * important to put small allocations in the same block, or code like
 * "char* ptr = malloc(sizeof(char));" is going to provide a very nice chock, instead of allocating
 * a small number of bytes, it will allocate at least 64 kilobytes.
 *
 * The memory manager is also responsible for the heap allocations done by the DLL itself. This is
 * done by replacing the regular methods of allocations with custom ones.
 * For the std-containers there are new types provided that replaces the standard ones, but keeping
 * all the same functionality.
 *
 * All of the above means that new, malloc and bare std-containers are entirely forbidden(!)
 * in the DLL.
 *
 * Reference material (including reference material linked by the webpage):
 * https://msdn.microsoft.com/en-us/library/windows/desktop/aa366912%28v=vs.85%29.aspx
 * https://blogs.msdn.microsoft.com/oldnewthing/20031008-00/?p=42223/
 *
 * -- Warepire
 */
namespace MemoryManager
{
    enum AllocationFlags : UINT
    {
        ALLOC_WRITE     = 0x00000001,
        ALLOC_READONLY  = 0x00000002,
        ALLOC_EXECUTE   = 0x00000004,
        ALLOC_ZEROINIT  = 0x00000008,
        REALLOC_NO_MOVE = 0x00000010,
    };

    void Init();

    LPVOID Allocate(UINT bytes, UINT flags);
    LPVOID Reallocate(LPVOID address, UINT bytes, UINT flags);
    void Deallocate(LPVOID address);

    SIZE_T GetSizeOfAllocation(LPCVOID address);
};

