#ifndef _FRAME_ALLOCATOR_HPP_
#define _FRAME_ALLOCATOR_HPP_
#include <cstdint>
#include <string>
#include <map>
#include <stack>
/*

To use this allocator with the new and delete operators you must create derived classes
that overload the new and delete operators.

Structure of the stack:

+--------------+
| Unused stack |
| space.       |
|              |
|              |
|              |
|              |
|              |
|              |
+--------------+ <- Top
| Memory block |
| N-Bytes      |
+--------------+ <- Address returned by last call to getBlock
| Address of   |
| the previous |
| base pointer |
+--------------+ <- Base
| Memory block |
| N-Bytes      |
+--------------+
| Address of   |
| the previous |
| base pointer |
+--------------+
| Memory block |
| N-Bytes      |
+--------------+
| Address of   |
| the previous |
| base pointer |
+--------------+ <- Address returned by a previous call to getFrame
| Memory block |
| N-Bytes      |
+--------------+
| Address of   |
| the previous |
| base pointer |
+--------------+
| Memory block |
| N-Bytes      |
+--------------+
| Address of   |
| the previous |
| base pointer |
+--------------+ <- memory


*/
namespace frameallocator {
  typedef std::uint8_t byte;
  typedef byte* stackptr;
  typedef const stackptr frameptr;

  class FrameAllocator {
    public:
      FrameAllocator();
      FrameAllocator(size_t size);
      ~FrameAllocator();
      /**
       *  getBlock
       *  Advances the top pointer by size + 8 bytes.
       *  Returns a pointer to the 
       */
      stackptr getBlock(size_t size);
      /**
       *  freeBlock
       *  Frees the top block in the memory.
       */
      void freeBlock();
      /**
       *  getFrame
       *  Gets the address of a frame pointer to be used in a later
       *  fast free.
       */
      frameptr getFrame();
      /**
       * freeFrame
       * Resets the top pointer to the frame pointer
       */
      bool freeFrame(frameptr frame);
      /**
       *  getError
       *  Returns the string error message at the top of the stack
       *  of errors.
       */
      std::string getError();
    private:
      static std::stack<std::string> errors;
      stackptr memory;
      stackptr top;
      stackptr base;
      size_t unusedBytes;
      size_t usedBytes;
  };

  class FrameAllocatorManager {
    public:
      /**
       *  getInstance
       *  Gets a reference to the frame allocator or creates it if
       *  it already exists.
       *  @returns A reference to the FrameAllocatorManager.
       */
      static FrameAllocatorManager& getInstance();
      /**
       *  createAllocator
       *  Creates a FrameAllocator instance with the associated id
       *  @param[in]  id  A string id to identify the new allocator
       *  @returns  true if the allocator could be created.
       *  @returns  false if the allocator could not be created.
       */
      bool createAllocator(std::string id, size_t size);
      /**
       *  getAllocator
       *  Retrieves a reference to the 
       */
      FrameAllocator* getAllocator(std::string id);
      /**
       *  cleanupAllocator
       *  Deletes the memory and associated allocator associated
       *  with id.
       *  @param[in]  id  The id of the allocator to be deleted.
       */
      bool cleanupAllocator(std::string id);
      /**
       *  cleanupInstance
       *  Frees all memory associated with all allocators.
       */
      void cleanupInstance();
      /**
       *  getError
       *  Gets the error at the top of the stack.
       *  @returns  A std::string containing the latest error
       *            message.
       */
      std::string getError();
    private:
      std::stack<std::string> errors;
      std::map<std::string, FrameAllocator*> allocators;
      FrameAllocatorManager();
      ~FrameAllocatorManager();
      static FrameAllocatorManager *manager;
  };
}
#endif//_FRAME_ALLOCATOR_HPP_
