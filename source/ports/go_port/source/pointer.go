package metacall

// #include <stdlib.h>
import "C"

import (
	"sync"
	"unsafe"
)

// Implements a way to store Go data and make it accessible from C
// Based on: https://github.com/mattn/go-pointer
var (
	mutex sync.RWMutex
	store = map[unsafe.Pointer]interface{}{}
)

func pointerSave(v interface{}) unsafe.Pointer {
	if v == nil {
		return nil
	}

	var ptr unsafe.Pointer = C.malloc(C.size_t(1))

	if ptr == nil {
		return nil
	}

	mutex.Lock()
	store[ptr] = v
	mutex.Unlock()

	return ptr
}

func pointerGet(ptr unsafe.Pointer) interface{} {
	if ptr == nil {
		return nil
	}

	mutex.RLock()
	v := store[ptr]
	mutex.RUnlock()

	return v
}

func pointerDelete(ptr unsafe.Pointer) {
	if ptr == nil {
		return
	}

	mutex.Lock()
	delete(store, ptr)
	mutex.Unlock()

	C.free(ptr)
}
