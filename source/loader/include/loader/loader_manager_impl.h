#ifndef LOADER_MANAGER_IMPL_H
#define LOADER_MANAGER_IMPL_H 1


#include <loader/loader_api.h>

#include <loader/loader_impl.h>

#include <plugin/plugin_manager.h>

#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>


struct loader_initialization_order_type
{
	uint64_t id;
	plugin p;
	int being_deleted;
};

struct loader_manager_impl_type
{
	plugin host;			
	vector initialization_order; 
	uint64_t init_thread_id;	 
	vector script_paths;		 
	set destroy_map;			 
	detour d;					 
};


typedef struct loader_initialization_order_type *loader_initialization_order;

typedef struct loader_manager_impl_type *loader_manager_impl;


LOADER_API loader_manager_impl loader_manager_impl_initialize(void);

LOADER_API plugin_manager_interface loader_manager_impl_iface(void);

LOADER_API void loader_manager_impl_set_destroyed(loader_manager_impl manager_impl, loader_impl impl);

LOADER_API int loader_manager_impl_is_destroyed(loader_manager_impl manager_impl, loader_impl impl);

LOADER_API void loader_manager_impl_destroy(loader_manager_impl manager_impl);

#ifdef __cplusplus
}
#endif

#endif
